#include "vlx_av_input_demuxer.h"
#include "vlx_av_decoder.h"
#include "vlx_av_data_manager.h"

#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>
#include <QHash>
#include <QMetaEnum>
#include <QThread>
#include <QTimer>
#include <QScopeGuard>
#include <QDir>
#include <QCoreApplication>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

Q_LOGGING_CATEGORY(VLX_AV, "[VLX::AV]")

#define _log qCDebug(VLX_AV)
#define _warn qCWarning(VLX_AV)

namespace Vlx
{
namespace  Av
{

//=========================================================

QString avErrStr(int errNum)
{
    static char errBuf[AV_ERROR_MAX_STRING_SIZE];
    return {av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, errNum)};
}

//----------------------------------------------------------

struct InterruptCtx
{
    qint64 deadline_ms = std::numeric_limits<qint64>::max();
    bool timeout = false;
    quint64 timeout_cntr = 0;


    void restart(qint64 timeout_ms = 4000)
    {
        timeout = false;
        tmo_event_trigger = false;
        deadline_ms = QDateTime::currentMSecsSinceEpoch() + timeout_ms;
    }

    bool isOver()
    {
        timeout = (QDateTime::currentMSecsSinceEpoch() >= deadline_ms);
        if (timeout && !tmo_event_trigger) {
            tmo_event_trigger = true;
            timeout_cntr++;
        }
        return timeout;
    }

    void reset()
    {
        timeout = false;
        timeout_cntr = 0;
        deadline_ms = std::numeric_limits<qint64>::max();
    }

    static int callback(void *ctx)
    {
        InterruptCtx *ic = static_cast<InterruptCtx*>(ctx);
        return ic->isOver() ? 1 : 0;
    }

private:
    bool tmo_event_trigger = false;
};


//=========================================================

class InputDemuxer::InternalImpl : public ErrorHandler
{
    friend class InputDemuxer;

    class Thread : public QThread
    {
    public:
        Thread (InputDemuxer::InternalImpl *i)
            : QThread(nullptr)
            , m_i {i}
        {}

    private:
        InputDemuxer::InternalImpl *m_i;

        // QThread interface
    protected:
        void run() override
        {
            QThread::msleep(100);
            m_i->init();
            while (!isInterruptionRequested()) {
                if (m_i->state() == State::Receiving)
                    m_i->receiveAll();
                QCoreApplication::processEvents();
            }
            m_i->deinit();
        }
    };

public:

    InternalImpl(StreamType tr, quint16 port, InputDemuxer *o)
        : m_idmx {o}
        , m_tr {tr}
        , m_port {port}
    {
        static bool runOnce = true;
        static auto me = QMetaEnum::fromType<StreamType>();

        if (runOnce) {
            if (auto ret = avformat_network_init(); ret != 0) {
                setError("avformat_network_init failed: " + avErrStr(ret));
                return;
            }
            av_log_set_level(Params::D::avLogLevel);
            runOnce = false;
        }

        QString threadName = QString("AV_IDmx_Thread_%1:%2").arg(me.key((int)m_tr)).arg(m_port);
        m_thread.setObjectName(threadName);
    }

    virtual ~InternalImpl()
    {
        stopThread();
    }

    void startThread()
    {
        m_thread.start();
        m_threadProxyObject.moveToThread(&m_thread);
    }

    const QString &errorString() const override
    {
        return m_lastErrorStr;
    }

    void setError(const QString &str) override
    {
        static QMutex _mtx;
        QMutexLocker _l(&_mtx);

        m_lastErrorStr = str;
        _warn << "Error:" << str << QThread::currentThread()->objectName();
        notifyErrorOccured();
    }

    void notifyErrorOccured()
    {
        if (QThread::currentThread() == m_idmx->thread())
            emit m_idmx->errorOccured();
        else
            QMetaObject::invokeMethod(m_idmx, [this]{emit m_idmx->errorOccured();});
    }

    void notifyStateChanged()
    {
        if (QThread::currentThread() == m_idmx->thread())
            emit m_idmx->stateChanged(m_state);
        else
            QMetaObject::invokeMethod(m_idmx, [this]{emit m_idmx->stateChanged(m_state);});
    }

    bool open()
    {
        QScopeGuard g([this]{ m_ic.reset(); });

        m_ic.restart();

        if (m_formatCtx == nullptr) {
            m_formatCtx = avformat_alloc_context();
            m_formatCtx->interrupt_callback.callback = InterruptCtx::callback;
            m_formatCtx->interrupt_callback.opaque = &m_ic;
        }

        if (!m_idmx->openInput()) {
            if (m_formatCtx != nullptr) {
                avformat_close_input(&m_formatCtx);
                m_formatCtx = nullptr;
            }
            return false;
        }

        m_ic.restart();

        if (auto ret = avformat_find_stream_info(m_formatCtx, nullptr); ret != 0) {
            setError("avformat_find_stream_info failed: " + avErrStr(ret));
            avformat_close_input(&m_formatCtx);
            m_formatCtx = nullptr;
            return false;
        }

        return true;
    }

    void init()
    {
        _log << "Opening input..." << QThread::currentThread()->objectName();

        set(State::Initializing);

        QElapsedTimer et;
        et.start();
        int sec = 0;
        int tmo_sec = 30;
        while(!open()) {
            deinit(true);
            if (QThread::currentThread()->isInterruptionRequested())
                return;
            auto now_sec = et.elapsed() / 1000;

            if (now_sec != sec) {
                sec = now_sec;
                _log << "Opening input..." << QString("%1/%2").arg(sec).arg(tmo_sec) << QThread::currentThread()->objectName();
            }

            if (et.elapsed() > (tmo_sec * 1000)) {
                setError("Timeout opening stream. Retrying...");
                QTimer::singleShot(1000, &m_threadProxyObject, [this](){init();});
                return;
            }
        }

        _log << "Input opened:" << QThread::currentThread()->objectName();

        auto sn = m_formatCtx->nb_streams;
        _log << "Number of streams:" << sn;
        QMutexLocker _l(&m_mtx);
        for (decltype(sn) i = 0; i < sn; i++) {
            auto st = m_formatCtx->streams[i];
            auto cp = st->codecpar;
            QString typeStr;
            if (cp->codec_type == AVMEDIA_TYPE_VIDEO) {
                typeStr = "Video";
            } else if (cp->codec_type == AVMEDIA_TYPE_DATA) {
                typeStr = "Data";
            } else {
                typeStr = "?";
            }
            _log << "idx:" << st->index << "PID:" << st->id << "type:" << typeStr;
        }
        _l.unlock();
        set(State::Receiving);
    }

    void stopThread()
    {
        for(auto &d : m_decoders)
            d->deinit();
        m_decoders.clear();

        for(auto &d : m_dataMgr)
            d->deinit();
        m_dataMgr.clear();

        _log << "Stopping..." << m_thread.objectName();
        if (m_thread.isRunning()) {
            m_thread.requestInterruption();
            m_thread.quit();
            if (!m_thread.wait(4000))
                m_thread.terminate();
        }
        _log << "Stopped:" << m_thread.objectName();

        deinit();
    }

    void deinit(bool skipStateChanging = false)
    {
        if (!skipStateChanging)
            set(State::Deinitiliazing);
        if (m_formatCtx != nullptr) {
            if (m_formatCtx->pb != nullptr) {
                if (m_formatCtx->pb->opaque != nullptr) {
                    // auto opaque = static_cast<SdpOpaque *>(m_formatContext->pb->opaque);
                    // delete opaque;
                    // m_formatContext->pb->opaque = nullptr;
                }
                avio_context_free(&m_formatCtx->pb);
                m_formatCtx->pb = nullptr;
            }
            avformat_close_input(&m_formatCtx);
            m_formatCtx = nullptr;
            _log << "Input closed:" << m_thread.objectName();
        }
        if (!skipStateChanging)
            set(State::NotInitialized);
    }

    void receiveAll()
    {
        AVPacket* pkt = nullptr;
        while (!m_thread.isInterruptionRequested()) {
            pkt = av_packet_alloc();
            m_ic.restart(500);
            if (auto ret = av_read_frame(m_formatCtx, pkt); ret < 0) {
                setError("av_read_frame err: " + avErrStr(ret));

                if (ret == AVERROR_EXIT || ret == AVERROR(EIO)) {
                    _log << "trying to restart.." << QThread::currentThread();
                    m_thread.requestInterruption();
                    QMetaObject::invokeMethod(m_idmx, [this]{restartAll();});
                }

                if (m_ic.timeout_cntr == 16) { // примерно 8 сек
                    _log << "trying to restart.." << QThread::currentThread();
                    QMetaObject::invokeMethod(m_idmx, [this]{restartAll();});
                }

                break;
            }
            m_ic.reset();
            auto si = pkt->stream_index;
            auto st = m_formatCtx->streams[si];
            auto pid = st->id;
            if (st->codecpar->codec_type == AVMEDIA_TYPE_DATA) {
                QMutexLocker _l(&m_mtx);
                if (!m_dataMgr.contains(pid))
                    break;
                auto &dm = m_dataMgr[pid];
                _l.unlock();
                dm->push(pkt);
            } else if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                QMutexLocker _l(&m_mtx);
                if (!m_decoders.contains(pid))
                    break;
                auto &dec = m_decoders[pid];
                _l.unlock();
                dec->push(pkt);
            } else {
                av_packet_free(&pkt);
            }
        }

        if (pkt != nullptr)
            av_packet_free(&pkt);
    }

    void restartAll()
    {
        _log << "restarting..." << QThread::currentThread();
        stopThread();
        startThread();
    }

    State state() const { return m_state; }
    AVFormatContext ** _avCtx() { return &m_formatCtx; }

    AVStream* avStreamByIndex(int idx)
    {
        if (m_formatCtx == nullptr)
            return nullptr;
        if (idx < 0 || idx >= (int)m_formatCtx->nb_streams)
            return nullptr;
        return m_formatCtx->streams[idx];
    }
    AVStream* avStreamByPid(int pid)
    {
        if (m_formatCtx == nullptr)
            return nullptr;
        auto n = m_formatCtx->nb_streams;
        for (decltype(n) i = 0; i < n; i++) {
            auto st = m_formatCtx->streams[i];
            if (st->id == pid)
                return st;
        }
        return nullptr;
    }

    InputStreamPtr videoStreamByPid(int pid)
    {
        if (m_decoders.contains(pid))
            return m_decoders[pid]->stream();

        if (auto st = avStreamByPid(pid); st != nullptr)
            return videoStreamByIndex(st->index);
        return {};
    }

    InputStreamPtr videoStreamByIndex(int index)
    {
        auto st = avStreamByIndex(index);
        if (st == nullptr || st->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
            return {};

        auto pid = st->id;

        if (m_decoders.contains(pid))
            return m_decoders[pid]->stream();

        m_decoders[pid] = DecoderPtr::create(this);
        if (!m_decoders[pid]->init(st, m_params))
            return {};
        return m_decoders[pid]->stream();
    }

    InputDataPtr inputDataByPid(int pid)
    {
        if (m_dataMgr.contains(pid))
            return m_dataMgr[pid]->stream();

        if (auto st = avStreamByPid(pid); st != nullptr)
            return inputDataByIndex(st->index);
        return {};
    }

    InputDataPtr inputDataByIndex(int index)
    {
        auto st = avStreamByIndex(index);
        if (st == nullptr || st->codecpar->codec_type != AVMEDIA_TYPE_DATA)
            return {};

        auto pid = st->id;

        if (m_dataMgr.contains(pid))
            return m_dataMgr[pid]->stream();

        m_dataMgr[pid] = DataManagerPtr::create(this);
        if (!m_dataMgr[pid]->init(st, m_params))
            return {};
        return m_dataMgr[pid]->stream();
    }


protected:

    void set(State state)
    {
        if (state != m_state) {
            m_state = state;
            // _log << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> new state:" << state << QThread::currentThread()->objectName();
            notifyStateChanged();
        }
    }

private:
    InputDemuxer *m_idmx;
    StreamType m_tr;
    quint16 m_port;
    Thread m_thread = Thread(this);
    QObject m_threadProxyObject;
    AVFormatContext *m_formatCtx = nullptr;
    QString m_lastErrorStr;
    State m_state = State::NotInitialized;
    InterruptCtx m_ic;

    QMutex m_mtx;
    // pid : shared_ptr
    QHash<int, DecoderPtr> m_decoders;
    QHash<int, DataManagerPtr> m_dataMgr;

    Params m_params;
};


//==================================================================================================================
//==================================================================================================================
//==================================================================================================================
//==================================================================================================================

class RtpObject : public InputDemuxer
{

public:
    RtpObject(quint16 port)
        : Av::InputDemuxer {new InternalImpl{StreamType::Rtp, port, this}}
    {}

    // Object interface
protected:
    bool openInput() override
    {
        static const char *sdp_tmpl =
            "v=0\n"
            "m=video %1 RTP/AVP 96\n"
            "c=IN IP4 0.0.0.0\n"
            "a=rtpmap:96 H265/90000";
        auto sdp_content = QString(sdp_tmpl).arg(port());
        QFile f(QDir::tempPath() + QDir::separator() + QString("av_sdp_%1.sdp").arg(port()));
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            m_i->setError("Unable to create tmp sdp file: " + f.fileName() + " due to error: " + f.errorString());
            return false;
        }
        f.write(sdp_content.toLatin1());
        f.close();

        _log << "tmp sdp file: " << f.fileName();

        AVDictionary* opts = nullptr;
        QScopeGuard g([&opts]{av_dict_free(&opts);});
        av_dict_set(&opts, "protocol_whitelist", "file,rtp,udp", 0);
        av_dict_set(&opts, "overrun_nonfatal", "1", 0);
        av_dict_set(&opts, "payload_type", "96", 0);
        av_dict_set(&opts, "buffer_size", "1000000", 0);
        av_dict_set(&opts, "probesize", "10000000", 0);
        av_dict_set(&opts, "analyzeduration", "5000000", 0);

        if (auto ret = avformat_open_input(m_i->_avCtx(), f.fileName().toLatin1(), nullptr, &opts); ret != 0) {
            m_i->setError("avformat_open_input failed: " + avErrStr(ret));
            return false;
        }
        return true;
    }
};

//=========================================================

class MpegTsObject : public InputDemuxer
{
public:
    MpegTsObject(quint16 port)
        : InputDemuxer {new InternalImpl{StreamType::MpegTs, port, this}}
    {}

    // Object interface
protected:
    bool openInput() override
    {
        static const char *url_tmpl = "udp://0.0.0.0:%1?fifo_size=8000000&overrun_nonfatal=1&buffer_size=4000000&reuse=1";
        AVDictionary* opts = nullptr;
        QScopeGuard g([&opts]{av_dict_free(&opts);});
        av_dict_set(&opts, "probesize", "500000", 0);
        av_dict_set(&opts, "analyzeduration", "1000000", 0);
        av_dict_set(&opts, "fflags", "nobuffer", 0);
        av_dict_set(&opts, "flags", "low_delay", 0);

        auto url = QString(url_tmpl).arg(port());
        if (auto ret = avformat_open_input(m_i->_avCtx(), url.toLatin1(), nullptr, &opts); ret != 0) {
            m_i->setError("avformat_open_input failed: " + avErrStr(ret));
            return false;
        }
        return true;
    }
};


//=========================================================

class RtpMpegTsObject : public InputDemuxer
{
public:
    RtpMpegTsObject(quint16 port)
        : InputDemuxer {new InternalImpl{StreamType::RtpMpegTs, port, this}}
    {}
    // Object interface
protected:
    bool openInput() override
    {
        static const char *sdp_tmpl =
            "v=0\n"
            "c=IN IP4 0.0.0.0\n"
            "m=video %1 RTP/AVP 33\n"
            "a=rtpmap:33 MP2T/90000";
        auto sdp_content = QString(sdp_tmpl).arg(port());
        QFile f(QDir::tempPath() + QDir::separator() + QString("av_sdp_%1.sdp").arg(port()));
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            m_i->setError("Unable to create tmp sdp file: " + f.fileName() + " due to error: " + f.errorString());
            return false;
        }
        f.write(sdp_content.toLatin1());
        f.close();

        _log << "tmp sdp file: " << f.fileName();

        AVDictionary* opts = nullptr;
        QScopeGuard g([&opts]{av_dict_free(&opts);});
        av_dict_set(&opts, "protocol_whitelist", "file,rtp,udp", 0);
        av_dict_set(&opts, "overrun_nonfatal", "1", 0);
        av_dict_set(&opts, "payload_type", "96", 0);
        av_dict_set(&opts, "buffer_size", "1000000", 0);
        av_dict_set(&opts, "probesize", "10000000", 0);
        av_dict_set(&opts, "analyzeduration", "5000000", 0);

        if (auto ret = avformat_open_input(m_i->_avCtx(), f.fileName().toLatin1(), nullptr, &opts); ret != 0) {
            m_i->setError("avformat_open_input failed: " + avErrStr(ret));
            return false;
        }
        return true;
    }
};

//=========================================================

class SrtMpegTsObject : public InputDemuxer
{
public:
    SrtMpegTsObject(quint16 port)
        : InputDemuxer {new InternalImpl{StreamType::SrtMpegTs, port, this}}
    {}
    // Object interface
protected:
    bool openInput() override
    {
        // 1. Формируем SRT URL вместо создания SDP-файла.

        QString srtUrl = QString("srt://0.0.0.0:%1?mode=listener&latency=200000&tlpktdrop=1").arg(port());
        _log << "SRT URL: " << srtUrl;

        // 2. Настраиваем опции.
        AVDictionary* opts = nullptr;
        QScopeGuard g([&opts]{ av_dict_free(&opts); });
        av_dict_set(&opts, "protocol_whitelist", "file,srt,udp", 0);
        av_dict_set(&opts, "overrun_nonfatal", "1", 0);
        av_dict_set(&opts, "fflags", "nobuffer", 0); // Эмуляция -fflags nobuffer
        av_dict_set(&opts, "buffer_size", "1000000", 0);
        av_dict_set(&opts, "probesize", "10000000", 0);
        av_dict_set(&opts, "analyzeduration", "5000000", 0);

        // 3. Пытаемся открыть поток.
        // Явно указываем формат mpegts, так как SRT обычно несёт именно его.
        auto inputFormat = av_find_input_format("mpegts");
        if (auto ret = avformat_open_input(m_i->_avCtx(), srtUrl.toUtf8().constData(), inputFormat, &opts); ret != 0) {
            m_i->setError("avformat_open_input failed: " + avErrStr(ret));
            return false;
        }
        return true;
    }
};

//=========================================================

struct _Storage
{
    using Ptr = QSharedPointer<InputDemuxer>;
    QHash<quint16, Ptr> map;
    virtual Ptr create(quint16 port)
    {
        Q_UNUSED(port)
        return nullptr;
    };
};

//---------------------------------------------------------

template<StreamType t>
struct Storage : public _Storage
{
    _Storage::Ptr create(quint16 port) override
    {
        if constexpr (t == StreamType::Rtp)
            return  QSharedPointer<RtpObject>::create(port);
        else if constexpr (t == StreamType::MpegTs)
            return QSharedPointer<MpegTsObject>::create(port);
        else if constexpr (t == StreamType::RtpMpegTs)
            return QSharedPointer<RtpMpegTsObject>::create(port);
        else if constexpr (t == StreamType::SrtMpegTs)
            return QSharedPointer<SrtMpegTsObject>::create(port);
        else
            return nullptr;
    }
};

//=========================================================

InputDemuxer::InputDemuxer(InternalImpl *impl)
    : QObject {nullptr}
    , m_i {impl}
{
    m_i->startThread();
}

InputDemuxer::~InputDemuxer()
{
    delete m_i;
}

quint16 InputDemuxer::port() const
{
    return m_i->m_port;
}

StreamType InputDemuxer::transport() const
{
    return m_i->m_tr;
}

InputDemuxer::State InputDemuxer::state() const
{
    return m_i->m_state;
}

const QString &InputDemuxer::lastErrorString() const
{
    return m_i->m_lastErrorStr;
}

const Params &InputDemuxer::params() const
{
    return m_i->m_params;
}

void InputDemuxer::setParams(const Params &p)
{
    if (m_i->m_params.hw_accel_index != p.hw_accel_index
        || m_i->m_params.hw_accel_dev_id != p.hw_accel_dev_id) {

        m_i->m_params.hw_accel_index = p.hw_accel_index;
        m_i->m_params.hw_accel_dev_id = p.hw_accel_dev_id;

        for (auto &d : m_i->m_decoders) {
            auto st = m_i->avStreamByIndex(d->streamIndex());
            if (st == nullptr)
                continue;
            d->deinit();
            d->init(st, m_i->m_params);
        }
    }

    if (m_i->m_params.av_log_level != p.av_log_level) {
        m_i->m_params.av_log_level = p.av_log_level;
        av_log_set_level(p.av_log_level);
    }
}

int InputDemuxer::streamsAvailable() const
{
    if (m_i->m_state != State::Receiving)
        return 0;
    return m_i->m_formatCtx->nb_streams;
}

int InputDemuxer::streamPid(int index) const
{
    if (auto st = m_i->avStreamByIndex(index); st != nullptr)
        return (int)st->id;
    return -1;
}

bool InputDemuxer::isVideoStream(int index) const
{
    if (auto st = m_i->avStreamByIndex(index); st != nullptr)
        return st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO;
    return false;
}

bool InputDemuxer::isDataStream(int index) const
{
    if (auto st = m_i->avStreamByIndex(index); st != nullptr)
        return st->codecpar->codec_type == AVMEDIA_TYPE_DATA;
    return false;
}

InputStreamPtr InputDemuxer::getStreamByIdx(int index)
{
    return m_i->videoStreamByIndex(index);
}

InputStreamPtr InputDemuxer::getStreamByPid(int pid)
{
    return m_i->videoStreamByPid(pid);
}

InputDataPtr InputDemuxer::getInputDataByIdx(int index)
{
    return m_i->inputDataByIndex(index);
}

InputDataPtr InputDemuxer::getInputDataByPid(int pid)
{
    return m_i->inputDataByPid(pid);
}


//=========================================================

InputDemuxerPtr inputDemuxer(StreamType tr, quint16 port)
{
    static _Storage _dumb;
    static Storage<StreamType::Rtp> _rtpStorage;
    static Storage<StreamType::MpegTs> _mpegTsStorage;
    static Storage<StreamType::RtpMpegTs> _rtpMpegTsStorage;
    static Storage<StreamType::SrtMpegTs> _srtMpegTsStorage;

    _Storage &st = (tr == StreamType::Rtp)
                       ? _rtpStorage
                       : (tr == StreamType::MpegTs)
                             ? _mpegTsStorage
                             : (tr == StreamType::RtpMpegTs)
                                   ? _rtpMpegTsStorage
                                   : (tr == StreamType::SrtMpegTs)
                                     ? _srtMpegTsStorage
                                     : _dumb;

    if (st.map.contains(port))
        return st.map[port];

    auto out = st.create(port);
    if (out != nullptr)
        st.map[port] = out;

    return out;
}

} // namespace Av
} // namespace Vlx
