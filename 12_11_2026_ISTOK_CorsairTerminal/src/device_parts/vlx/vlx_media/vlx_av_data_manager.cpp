#include "vlx_av_data_manager.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QScopeGuard>

Q_DECLARE_LOGGING_CATEGORY(VLX_AV)

#define _log qCDebug(VLX_AV)
#define _warn qCWarning(VLX_AV)

namespace Vlx
{
namespace Av
{

DataManager::InputDataImpl::InputDataImpl(int pid)
    : InputData()
    , m_pid {pid}
{}

void DataManager::InputDataImpl::pushPacket(QByteArray &&packet)
{
    QMutexLocker l(&m_mtx);
    bool notify = (m_pktNum == 0);

    if (m_packets.size() > 16) {
        _warn << "input stream output buffer overflow. dropping" << m_packets.size() << "frames." << QThread::currentThread()->objectName();
        m_packets.clear();
        m_pktNum = 0;
        notify = true;
    }

    m_packets.push_back(std::move(packet));
    m_pktNum++;
    l.unlock();

    if (notify) {
        if (QThread::currentThread() == thread()) {
            emit hasPendingPackets();
        } else {
            QMetaObject::invokeMethod(this, &InputData::hasPendingPackets);
        }
    }
}

void DataManager::InputDataImpl::clearPackets()
{
    QMutexLocker _l(&m_mtx);
    m_packets.clear();
    m_pktNum = 0;
}

QByteArray DataManager::InputDataImpl::popPacket()
{
    QMutexLocker l(&m_mtx);
    if (m_packets.empty())
        return {};
    m_pktNum--;
    auto out = std::move(m_packets.front());
    m_packets.pop_front();
    return out;
}

DataManager::DataManager(ErrorHandler *eh)
    : m_err {eh}
{}

DataManager::~DataManager()
{
    deinit();
}

bool DataManager::init(AVStream *stream, const Params &params)
{
    Q_UNUSED(params);
    if (stream == nullptr)
        return false;

    m_streamPid = stream->id;
    m_streamIndex = stream->index;

    if (m_stream == nullptr)
        m_stream = QSharedPointer<InputDataImpl>::create(m_streamPid);

    _log << "Data manager for pid:" << m_streamPid << "is ready";
    return true;
}

void DataManager::deinit()
{
    if (m_stream != nullptr)
        m_stream->clearPackets();
}

void DataManager::push(AVPacket *pkt)
{
    if (m_stream != nullptr)
        m_stream->pushPacket({(const char*)pkt->data, pkt->size});
    av_packet_free(&pkt);
}

InputDataPtr DataManager::stream()
{
    return m_stream.toWeakRef();
}


} // namespace Av
} // namespace Vlx
