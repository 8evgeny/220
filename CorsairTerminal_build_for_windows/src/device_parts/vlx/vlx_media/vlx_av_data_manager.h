#pragma once

#include "vlx_av_common.h"
#include "vlx_av_input_data.h"

#include <QMutex>
#include <QObject>
#include <QSharedPointer>
#include <QThread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace Vlx
{
namespace Av
{

class DataManager;
using DataManagerPtr = QSharedPointer<DataManager>;

class DataManager
{
    class InputDataImpl : public InputData
    {
        friend DataManager;
        friend QSharedPointer<InputDataImpl>;


        explicit InputDataImpl(int pid);

        void pushPacket(QByteArray && packet);

        void clearPackets();

        // InputData interface
    public:
        int pid() const override { return m_pid; }
        int pendingPackets() const override { return m_pktNum; }
        QByteArray popPacket() override;

    private:
        int m_pid;
        std::list<QByteArray> m_packets;
        int m_pktNum = 0;
        QMutex m_mtx;
    };


public:
    DataManager(ErrorHandler *eh);

    virtual ~DataManager();

    bool init(AVStream* stream, const Params &params);
    void deinit();
    void push(AVPacket*pkt);
    InputDataPtr stream();
    int streamPid() const {return m_streamPid;}
    int streamIndex() const {return m_streamIndex;}

private:
    ErrorHandler *m_err = nullptr;
    int m_streamPid = -1;
    int m_streamIndex = -1;

    QSharedPointer<InputDataImpl> m_stream;
};

} // namespace Av
} // namespace Vl
