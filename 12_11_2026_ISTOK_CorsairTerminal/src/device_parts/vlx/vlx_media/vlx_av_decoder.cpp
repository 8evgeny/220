#include "vlx_av_decoder.h"

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

//===========================================================================================


const QStringList &InputStream::supportedHwAccelList()
{
    Decoder::updateSupportedHwAccelList();
    return Decoder::supportedHwAccelNameList();
}

Decoder::StreamImpl::StreamImpl(int pid)
    : InputStream()
    , m_pid {pid}
{}

void Decoder::StreamImpl::pushFrame(cv::Mat &img)
{
    if (!m_enabled)
        return;

    QMutexLocker l(&m_mtx);
    bool notify = (m_frmNum == 0);

    if (m_frames.size() > 16) {
        _warn << "input stream output buffer overflow. dropping" << m_frames.size() << "frames." << QThread::currentThread()->objectName();
        m_frames.clear();
        m_frmNum = 0;
        notify = true;
    }

    m_frames.push_back(std::move(img));
    m_frmNum++;
    l.unlock();

    if (notify) {
        if (QThread::currentThread() == thread()) {
            emit hasPendingFrames();
        } else {
            QMetaObject::invokeMethod(this, &InputStream::hasPendingFrames);
        }
    }
}

void Decoder::StreamImpl::clearFrames()
{
    QMutexLocker l(&m_mtx);
    m_frames.clear();
    m_frmNum = 0;
}

cv::Mat Decoder::StreamImpl::popFrame()
{
    QMutexLocker l(&m_mtx);
    if (m_frames.empty())
        return {};
    m_frmNum--;
    auto out = std::move(m_frames.front());
    m_frames.pop_front();
    return out;
}

//===========================================================================================

Decoder::Thread::Thread(Decoder *decoder)
    : m_decoder {decoder}
{}

void Decoder::Thread::run()
{
    while (!isInterruptionRequested()) {
        m_decoder->process();
        QCoreApplication::processEvents();
    };

    m_decoder->postProcess();
}

//===========================================================================================
//===========================================================================================
//===========================================================================================
//===========================================================================================

QList<AVHWDeviceType> Decoder::s_supportedHwAccels;
QStringList Decoder::s_supportedHwAccelNames;

static void _updateSuportedHwAccelList(QList<AVHWDeviceType> &list, QStringList &names)
{
    list.clear();
    names.clear();

    // Перечисление всех доступных типов аппаратного ускорения
    enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;

    // qDebug() << "Available hardware device types:";
    while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
        const char *type_name = av_hwdevice_get_type_name(type);
        if (type_name) {
            _log << " - found HW ACCEL:" << type_name;

            // Попытка перечислить устройства для данного типа
            AVBufferRef *device_ref = nullptr;
            int ret = av_hwdevice_ctx_create(&device_ref, type, nullptr, nullptr, 0);
            if (ret == 0 && device_ref) {
                list.append(type);
                names.append(QString(type_name));
                // Здесь можно добавить дополнительную информацию о устройстве, если она доступна
                av_buffer_unref(&device_ref);
            } else {
                // qDebug() << "  Failed to create device for type: " << type_name;
            }
        }
    }
}

const QStringList &Decoder::supportedHwAccelNameList()
{
    return s_supportedHwAccelNames;
}

void Decoder::updateSupportedHwAccelList()
{
    if (s_supportedHwAccels.empty())
        _updateSuportedHwAccelList(s_supportedHwAccels, s_supportedHwAccelNames);
}

Decoder::Decoder(ErrorHandler *eh)
    : m_err {eh}
{
    QObject::connect(&m_thread, &QThread::started, &m_thread, [this]{
        _log << "Started:" << m_thread.objectName();
    }, Qt::QueuedConnection);
    updateSupportedHwAccelList();
}

Decoder::~Decoder()
{
    deinit();
}

bool Decoder::init(AVStream *stream, const Params &params)
{
    if (stream == nullptr)
        return false;

    m_streamPid = stream->id;
    m_streamIndex = stream->index;

    QScopeGuard g([this]{deinit();});

    if (!openDecoder(stream, params))
        return false;
    _log << "Decoder ready: PID:" << stream->id;
    if (!createSwsUnit())
        return false;
    _log << "Sws Unit ready: PID:" << stream->id;

    auto w = stream->codecpar->width;
    auto h = stream->codecpar->height;
    auto codec_str = (stream->codecpar->codec_id == AV_CODEC_ID_HEVC) ? "HEVC" : "not_HEVC";

    auto accelName = (m_hwAccelName.isEmpty()) ? QString("software") : m_hwAccelName;
    QString threadName = QString("AV_Decoder_Thread_%1_%2x%3_%4_(%5)").arg(m_streamPid).arg(w).arg(h).arg(codec_str).arg(accelName);
    m_thread.setObjectName(threadName);

    g.dismiss();

    frame = av_frame_alloc();

    if (m_stream == nullptr)
        m_stream = QSharedPointer<StreamImpl>::create(m_streamPid);

    m_readyToDecode = true;

    startThread();
    return true;
}

void Decoder::deinit()
{
    m_readyToDecode = false;

    stopThread();

    if (m_stream != nullptr)
        m_stream->clearFrames();

    if (m_swsYuvToBgrCtx)
        sws_freeContext(m_swsYuvToBgrCtx);
    if (m_codecCtx != nullptr) {
        m_codecCtx->hw_device_ctx = nullptr;
        m_codecCtx->hw_frames_ctx = nullptr;
        avcodec_free_context(&m_codecCtx);
    }

    if (frame != nullptr)
        av_frame_free(&frame);

    m_codecCtx = nullptr;
    m_swsYuvToBgrCtx = nullptr;
    frame = nullptr;
    m_hwAccelName.clear();
}

void Decoder::push(AVPacket *pkt)
{
    if (!m_readyToDecode) {
        av_packet_free(&pkt);
        return;
    }
    QMutexLocker _l(&m_inputMtx);
    m_packets.push_back(pkt);
}

InputStreamPtr Decoder::stream()
{
    return m_stream.toWeakRef();
}

bool Decoder::openDecoder(AVStream *stream, const Params &params)
{
    AVCodecParameters *codecParameters = stream->codecpar;
    if (codecParameters->width == 0 || codecParameters->height == 0) {
        m_err->setError("codec parameters: zero picture size");
    }

    const AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
    if (!codec) {
        m_err->setError("Unsupported codec");
        return false;
    }

    m_codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codecCtx, codecParameters);

    if (params.hw_accel_index >= 0 && params.hw_accel_index < s_supportedHwAccels.count()) {
        auto type = s_supportedHwAccels.at(params.hw_accel_index);
        m_hwAccelName = s_supportedHwAccelNames.at(params.hw_accel_index);

        auto devIdStr = ((params.hw_accel_dev_id >= 0) ? QString::number(params.hw_accel_dev_id) : QString())
                            .toLatin1();

        _log << "av_hwdevice_ctx_create:" << m_hwAccelName << "dev_id:" << params.hw_accel_dev_id;

        const char *devId_cstr = (params.hw_accel_dev_id >= 0) ? devIdStr.constData() : nullptr;
        AVBufferRef* hwDeviceCtx = nullptr;
        if (auto ret = av_hwdevice_ctx_create(&hwDeviceCtx, type, devId_cstr, nullptr, 0); ret != 0) {
            m_err->setError("Failed to create hardware device context: " + avErrStr(ret));
            return false;
        }
        m_codecCtx->hw_device_ctx = av_buffer_ref(hwDeviceCtx);
        av_buffer_unref(&hwDeviceCtx);
    }
    // m_codecCtx->err_recognition = AV_EF_EXPLODE | AV_EF_CAREFUL | AV_EF_CRCCHECK | AV_EF_BITSTREAM;
    m_codecCtx->err_recognition = AV_EF_CAREFUL | AV_EF_CRCCHECK | AV_EF_BITSTREAM;
    m_codecCtx->error_concealment = 0;
    m_codecCtx->flags  |= AV_CODEC_FLAG_LOW_DELAY;
    m_codecCtx->flags2 |= AV_CODEC_FLAG2_FAST;
    m_codecCtx->thread_count = 0;  // auto
    m_codecCtx->thread_type = FF_THREAD_SLICE;  // только слайсы, без frame threading

    if (auto ret = avcodec_open2(m_codecCtx, codec, nullptr); ret < 0) {
        m_err->setError("Could not open codec: " + avErrStr(ret));
        return false;
    }

    if (m_codecCtx->width == 0 || m_codecCtx->height == 0) {
        qCritical() << "Invalid codecContext picture size:" << m_codecCtx->width << m_codecCtx->height;
        return false;
    }
    return true;
}

bool Decoder::createSwsUnit()
{
    m_swsYuvToBgrCtx = sws_getContext(
        m_codecCtx->width, m_codecCtx->height, AV_PIX_FMT_YUV420P, // Исходный формат
        m_codecCtx->width, m_codecCtx->height, AV_PIX_FMT_BGR24,   // Целевой формат
        SWS_BILINEAR,                      // Метод интерполяции
        NULL, NULL, NULL
        );

    if (m_swsYuvToBgrCtx == nullptr) {
        m_err->setError("Failed to create SwsContext");
        return false;
    }
    return true;
}

void Decoder::startThread()
{
    if (m_thread.isRunning())
        return;
    m_thread.start();
}

void Decoder::stopThread()
{
    if (!m_thread.isRunning())
        return;

    m_thread.requestInterruption();
    m_thread.quit();
    if (!m_thread.wait(3000)) {
        m_thread.terminate();
    }

    _log << "Stopped:" << m_thread.objectName();

}

void Decoder::process()
{
    AVPacket *pkt = nullptr;
    while(!m_thread.isInterruptionRequested() && m_readyToDecode) {
        {
            QMutexLocker _l(&m_inputMtx);
            if (m_packets.empty())
                return;
            pkt = m_packets.front();
            m_packets.pop_front();
        }
        if (auto ret = avcodec_send_packet(m_codecCtx, pkt); ret != 0) {
            // m_err->setError("codec error: " + avErrStr(ret));
            break;
        }
        av_packet_free(&pkt);
        while (!m_thread.isInterruptionRequested() && avcodec_receive_frame(m_codecCtx, frame) == 0) {
            cv::Mat img = convertAvToOpenCv(frame);
            if (!img.empty() && m_stream != nullptr)
                m_stream->pushFrame(img);
        };
    }
    if (pkt != nullptr)
        av_packet_free(&pkt);
}

void Decoder::postProcess()
{
    avcodec_send_packet(m_codecCtx, nullptr);
    while (avcodec_receive_frame(m_codecCtx, frame) == 0) {}
}

cv::Mat Decoder::convertAvToOpenCv(AVFrame *frame)
{
    if (frame->format == AV_PIX_FMT_CUDA
        || frame->format == AV_PIX_FMT_VULKAN
        || frame->format == AV_PIX_FMT_DXVA2_VLD
        || frame->format == AV_PIX_FMT_D3D11
        || frame->format == AV_PIX_FMT_OPENCL
        || frame->format == AV_PIX_FMT_VDPAU
        || frame->format == AV_PIX_FMT_VAAPI
        // || frame->format == AV_PIX_FMT_D3D12
        )
        return convFromGpu(frame);
    else if (frame->format == AV_PIX_FMT_YUV420P)
        return convFromYuv420p(frame);

    return {};
}

cv::Mat Decoder::convFromGpu(AVFrame *frame)
{
    AVFrame* sw_frame = av_frame_alloc();
    Q_ASSERT(sw_frame != NULL);

    int ret = av_hwframe_transfer_data(sw_frame, frame, 0);
    if (ret != 0) {
        m_err->setError("av_hwframe_transfer_data failed: " + avErrStr(ret));
        av_free(sw_frame);
        return {};
    }

    int width = sw_frame->width;
    int height = sw_frame->height;

    // // Создаем матрицу для Y-компонента
    cv::Mat yPlane(height, width, CV_8UC1, sw_frame->data[0], sw_frame->linesize[0]);

    // Создаем матрицу для UV-компонент (чересстрочная упаковка U и V)
    cv::Mat uvPlane(height / 2, width / 2, CV_8UC2, sw_frame->data[1], sw_frame->linesize[1]);

    // Объединяем Y и UV плоскости в одну матрицу NV12
    cv::Mat nv12Frame;
    cv::vconcat(yPlane, uvPlane.reshape(1, height / 2), nv12Frame);

    // Преобразуем NV12 в BGR
    cv::Mat bgrFrame;
    cv::cvtColor(nv12Frame, bgrFrame, cv::COLOR_YUV2BGR_NV12);

    av_freep(sw_frame);
    av_free(sw_frame);

    return bgrFrame;
}

cv::Mat Decoder::convFromYuv420p(AVFrame *frame)
{
    // Создаем выходной кадр для BGR
    cv::Mat bgrMat(frame->height, frame->width, CV_8UC3);

    // Настраиваем выходные параметры
    uint8_t* dstData[1] = { bgrMat.data };
    int dstLinesize[1] = { static_cast<int>(bgrMat.step) };

    // Выполняем конвертацию
    sws_scale(
        m_swsYuvToBgrCtx,
        frame->data,     // Входные данные (YUV)
        frame->linesize, // Размеры строк
        0, frame->height,          // Область обработки
        dstData,            // Выходные данные (BGR)
        dstLinesize         // Размеры строк выходного кадра
        );

    return bgrMat;
}


} // namespace Av
} // namespace Vlx
