#pragma once

#include "vlx_av_common.h"
#include "vlx_av_input_stream.h"

#include <QMutex>
#include <QObject>
#include <QSharedPointer>
#include <QThread>

#include <opencv2/opencv.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace Vlx
{
namespace Av
{

class Decoder;
using DecoderPtr = QSharedPointer<Decoder>;

class Decoder
{
    class Thread : public QThread
    {
    public:
        Thread(Decoder *decoder);
    protected:
        void run() override;
    private:
        Decoder *m_decoder;
    };

    class StreamImpl : public InputStream
    {
        friend Decoder;
        friend QSharedPointer<StreamImpl>;

    protected:

        explicit StreamImpl(int pid);

        void pushFrame(cv::Mat &img);

        void clearFrames();

        // InputStream interface
    public:
        bool enabled() const override { return m_enabled; }
        void setEnabled(bool state) override { m_enabled = state; }
        int pid() const override { return m_pid; }
        int pendingFrames() const override { return m_frmNum; }
        cv::Mat popFrame() override;

    private:
        int m_pid;
        std::list<cv::Mat> m_frames;
        int m_frmNum = 0;
        QMutex m_mtx;
        bool m_enabled = false;
    };


public:

    static const QStringList &supportedHwAccelNameList();

    static void updateSupportedHwAccelList();

    Decoder(ErrorHandler *eh);

    virtual ~Decoder();

    bool init(AVStream* stream, const Params &params);

    void deinit();

    void push(AVPacket*pkt);

    InputStreamPtr stream();

    int streamPid() const {return m_streamPid;}
    int streamIndex() const {return m_streamIndex;}

    bool isReadyToDecode() const {return m_readyToDecode;}

protected:

    bool openDecoder(AVStream* stream, const Params &params);

    bool createSwsUnit();

    void startThread();

    void stopThread();

    void process();

    void postProcess();

    cv::Mat convertAvToOpenCv(AVFrame *frame);
    cv::Mat convFromGpu(AVFrame *frame);
    cv::Mat convFromYuv420p(AVFrame *frame);

private:
    static QList<AVHWDeviceType> s_supportedHwAccels;
    static QStringList s_supportedHwAccelNames;

    ErrorHandler *m_err = nullptr;
    int m_streamPid = -1;
    int m_streamIndex = -1;

    AVCodecContext *m_codecCtx = nullptr;
    SwsContext* m_swsYuvToBgrCtx = nullptr;
    QString m_hwAccelName;

    Thread m_thread = Thread(this);

    QMutex m_inputMtx;
    std::list<AVPacket*> m_packets;

    AVFrame *frame = nullptr;

    bool m_readyToDecode = false;

    QSharedPointer<StreamImpl> m_stream;
};

} // namespace Av
} // namespace Vlx