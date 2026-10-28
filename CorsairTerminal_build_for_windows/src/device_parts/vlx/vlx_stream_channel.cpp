#include "vlx_stream_channel.h"

#include "device_parts/cmd/cmdmanager.h"

#include "vlx_settings.h"
#include "vlx_media_parameters_dialog.h"

#include <QDateTime>
#include <QDebug>
#include <QTimer>

#include <QPointer>
#include <QThread>


//==========================================

static Vlx::Av::Params settingsToAvParams()
{
    return Vlx::Av::Params {
        .hw_accel_index = VlxSettings::i().hwIndex(),
        .hw_accel_dev_id = VlxSettings::i().devId(),
        .av_log_level = VlxSettings::i().avLogLevel()
    };
}

//==========================================

struct VlxStreamChannel::Internal
{
    Vlx::Av::InputDemuxerPtr idmx;
    Vlx::Av::InputStreamPtr stream;
    int pid = -1;
    bool selected = false;
};

//==========================================

VlxStreamChannel::VlxStreamChannel(quint16 networkPort,
                       const QString &_name,
                       const QString &_show_name,
                       const uint32_t &_id,
                       const int &w,
                       const int &h,
                       QObject *parent)
    : AbstractChannel(parent, _name, _show_name, _id, w, h)
    , m_i {new Internal}
{
    Q_UNUSED(networkPort);

    connect(VlxMediaParametersDialog::inst(), &VlxMediaParametersDialog::settingsSaved, this, &VlxStreamChannel::onSettingsSaved);

    QTimer::singleShot(50, this, &VlxStreamChannel::initialization);
}

VlxStreamChannel::~VlxStreamChannel()
{
    delete m_i;
    // s_av.deinit();
}

uint8_t *VlxStreamChannel::fake_rcv(int &w, int &h, int &id, int &num)
{
    id = this->id();

    m_frame.release();

    if (auto s = m_i->stream.toStrongRef())
        m_frame = s->popFrame();
    else
        return nullptr;

    if (m_frame.empty())
        return nullptr;

    num = ++m_framesCnt;

    auto ms = m_fcElapsedCntr.elapsed();
    m_fc++;

    if (ms > 1000) {
        m_fps = (qreal)m_fc * 1000.0 / (qreal)ms;
        m_fcElapsedCntr.restart();
        m_fc = 0;
    }

    w = m_frame.cols;
    h = m_frame.rows;

    m_lastFrameMs = QDateTime::currentMSecsSinceEpoch();

    return m_frame.data;
}

void VlxStreamChannel::parse_raw_img(cv::Mat_<uint8_t> raw_img)
{
    Q_UNUSED(raw_img)

    if (m_frame.empty())
        return;

    auto &f = m_frame;

    if (f.rows == 0 || f.cols == 0)
        return;
    _origin_img = std::move(f);
}

Vlx::Av::InputDemuxerPtr VlxStreamChannel::demuxer()
{

#if defined(VLX_STREAM_TYPE_RTP_MPEG_TS)
    return Vlx::Av::inputDemuxer(Vlx::Av::StreamType::RtpMpegTs, VlxSettings::i().networkPort());
#elif defined(VLX_STREAM_TYPE_SRT_MPEG_TS)
    return Vlx::Av::inputDemuxer(Vlx::Av::StreamType::SrtMpegTs, VlxSettings::i().networkPort());
#endif

}

void VlxStreamChannel::initialization()
{
    m_i->idmx = demuxer();
    auto idmx = m_i->idmx.toStrongRef();
    if (idmx == nullptr) {
        qWarning() << "Unable to get an Vlx::Av::inputDemuxer";
        return;
    }
    idmx->setParams( settingsToAvParams() );

    connect(idmx.data(), &Vlx::Av::InputDemuxer::stateChanged, this, &VlxStreamChannel::onInputDemuxerStateChanged);
    connect(idmx.data(), &Vlx::Av::InputDemuxer::errorOccured, this, &VlxStreamChannel::onAvErrorOccured);
}

void VlxStreamChannel::onAvErrorOccured()
{
    //TODO: use m_av->lastError(), in a case of error: also use m_av->isInterruptionRequested()

    // m_lblInfo->setText("ERROR: " + m_av->lastError());
    // if (m_av->isInterruptionRequested()) {
    //     ui->statusbar->showMessage("Failed");
    // }
}

void VlxStreamChannel::onAvStarted()
{
    m_fc = 0;
    m_framesCnt = 0;


    if (m_i->idmx.isNull())
        return;

    auto idmx = m_i->idmx.toStrongRef();
    auto stream = m_i->stream.toStrongRef();

    auto hw_idx = idmx->params().hw_accel_index;
    auto dev_id = idmx->params().hw_accel_dev_id;

    VlxSettings::i().setHwIndex(hw_idx);
    VlxSettings::i().setDevId(dev_id);

    QString hwName;

    auto &nlist = stream->supportedHwAccelList();

    if (hw_idx >= 0 && hw_idx < nlist.size()) {
        auto n = nlist.at(hw_idx);
        hwName = "HW: " + n;
    } else {
        hwName = "SOFTWARE";
    }

    QString devIdStr;

    if (dev_id >= 0)
        devIdStr = "<" + QString::number(dev_id) + ">";
    else
        devIdStr = "<auto>";

    qDebug() << show_name() << "decoder param. accel:" << hwName << "dev id:" << devIdStr;

    m_fcElapsedCntr.restart();
}

void VlxStreamChannel::onAvStopped()
{
    m_fcElapsedCntr.invalidate();
    if (m_autoRestart) {
        // startAv();
        m_autoRestart = false;
    }
}

void VlxStreamChannel::onInputDemuxerStateChanged()
{
    Q_ASSERT(m_i->idmx != nullptr);

    auto idmx = m_i->idmx.toStrongRef();

    qDebug() << show_name()<< "Input demuxer state changed:" << idmx->state();

    if (idmx->state() == Vlx::Av::InputDemuxer::State::Receiving) {
        qDebug() << show_name() << "Streams available:" << idmx->streamsAvailable();

        if (name() == CHANNEL_NAME_VLX_STREAM_1) {
            m_i->stream = idmx->getStreamByPid(VlxSettings::i().vstream1Pid());
        } else {
            m_i->stream = idmx->getStreamByPid(VlxSettings::i().vstream2Pid());
        }

        if (auto s = m_i->stream.toStrongRef(); s != nullptr) {
            m_i->pid = s->pid();
            qDebug() << show_name() << "=== stream.pid:" << m_i->pid;
            if (m_i->selected)
                s->setEnabled(true);
            onAvStarted();
        } else {
            qDebug() << show_name() << "=ERROR=: no stream avaliable";
        }
    } else if (idmx->state() == Vlx::Av::InputDemuxer::State::NotInitialized) {
        onAvStopped();
    }

    // ParametersDialog::inst()->uploadAvParams(m_av);

    //autostart
    // QTimer::singleShot(100, this, &RtpChannel::startAv);
}

void VlxStreamChannel::onSettingsSaved()
{
    if (auto idmx = m_i->idmx.toStrongRef(); idmx != nullptr)
        idmx->setParams(settingsToAvParams());
}

void VlxStreamChannel::startAv()
{
    // s_av.start();

    // ui->statusbar->showMessage("Connecting...");
}

void VlxStreamChannel::selected()
{
    m_fcElapsedCntr.invalidate();
    m_fc = 0;
    m_i->selected = true;
    auto s = m_i->stream.toStrongRef();
    if (s != nullptr)
        s->setEnabled(true);
}

void VlxStreamChannel::deselected()
{
    m_i->selected = false;
    auto s = m_i->stream.toStrongRef();
    if (s != nullptr)
        s->setEnabled(false);
}
