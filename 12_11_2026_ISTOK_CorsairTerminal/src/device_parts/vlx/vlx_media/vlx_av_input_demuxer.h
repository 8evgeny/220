#pragma once

#include <QObject>

#include "vlx_av_common.h"

namespace Vlx
{
namespace Av
{

class InputDemuxer : public QObject
{
    Q_OBJECT
protected:
    class InternalImpl;
    explicit InputDemuxer(InternalImpl *impl);
public:

    enum class State {
        NotInitialized,
        Initializing,
        Receiving,
        Deinitiliazing
    };

    Q_ENUM(State)

    virtual ~InputDemuxer();

    quint16 port() const;
    StreamType transport() const;
    State state() const;
    const QString& lastErrorString() const;

    const Params &params() const;
    void setParams(const Params &p);

    int streamsAvailable() const;

    int streamPid(int index) const;
    bool isVideoStream(int index) const;
    bool isDataStream(int index) const;

    InputStreamPtr getStreamByIdx(int index);
    InputStreamPtr getStreamByPid(int pid);

    InputDataPtr getInputDataByIdx(int index);
    InputDataPtr getInputDataByPid(int pid);

signals:
    void errorOccured();
    void stateChanged(Vlx::Av::InputDemuxer::State state);

protected:
    virtual bool openInput() = 0;
protected:
    InternalImpl *m_i;
};

InputDemuxerPtr inputDemuxer(StreamType tr, quint16 port = 0);


} // namespace Av
} // namespace Vlx