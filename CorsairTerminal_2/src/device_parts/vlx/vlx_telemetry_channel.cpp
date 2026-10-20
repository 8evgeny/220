#include "vlx_telemetry_channel.h"
#include "device_parts/cmd/telemetry.h"
#include "vlx_settings.h"

#include <QTimer>
#include <QDebug>

#include <device_parts/cmd/telemetry.h>

struct VlxTelemetryChannel::Internal
{
    Vlx::Av::InputDemuxerPtr idmx;
    Vlx::Av::InputDataPtr stream;
};

VlxTelemetryChannel::VlxTelemetryChannel()
    : QObject {nullptr}
    , m_i {std::make_unique<VlxTelemetryChannel::Internal>()}
{
    QTimer::singleShot(100, this, &VlxTelemetryChannel::init);
}

VlxTelemetryChannel::~VlxTelemetryChannel() = default;

VlxTelemetryChannel &VlxTelemetryChannel::i()
{
    static VlxTelemetryChannel inst;
    return inst;
}

bool VlxTelemetryChannel::getNext(Telemetry &tm)
{
    auto i = m_i->stream.toStrongRef();
    if (i == nullptr)
        return false;

    if (i->pendingPackets() == 0)
        return false;

    auto d = i->popPacket();
    if (d.size() < 100)
        return false;

    auto bytes = (const uint8_t*)d.constData();
    auto offset = bytes[0];
    if (offset >= d.size())
        return false;

    // удаляем всякие преамбулы и т.п.
    return tm.from_buffer(bytes + offset, d.size() - offset);
}

void VlxTelemetryChannel::init()
{
    // m_i->idmx = Vlx::Av::inputDemuxer(Vlx::Av::Transport::RtpMpegTs, VlxSettings::i().rtpMuxPort());
    m_i->idmx = Vlx::Av::inputDemuxer(Vlx::Av::StreamType::SrtMpegTs, VlxSettings::i().networkPort());
    auto idmx = m_i->idmx.toStrongRef();
    if (idmx == nullptr) {
        qWarning() << "Unable to get an Vlx::Av::inputDemuxer";
        return;
    }
    connect(idmx.data(), &Vlx::Av::InputDemuxer::stateChanged, this, &VlxTelemetryChannel::onInputDemuxerStateChanged);
}

void VlxTelemetryChannel::onInputDemuxerStateChanged()
{
    if(m_i->idmx == nullptr)
        return;

    auto idmx = m_i->idmx.toStrongRef();
    if (idmx->state() != Vlx::Av::InputDemuxer::State::Receiving)
        return;

    m_i->stream = idmx->getInputDataByPid(VlxSettings::i().dstreamPid());
    if (auto s = m_i->stream.toStrongRef(); s != nullptr) {
        qDebug() << "=== telemetry stream.pid:" << s->pid();
    } else {
        qDebug() << "=ERROR=: no telemetry stream avaliable";
    }
}
