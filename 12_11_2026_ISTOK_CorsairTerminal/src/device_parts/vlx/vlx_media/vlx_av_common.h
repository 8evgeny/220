#pragma once
#include <QObject>
#include <QWeakPointer>

namespace Vlx
{
Q_NAMESPACE
namespace Av
{
Q_NAMESPACE

class InputStream;
using InputStreamPtr = QWeakPointer<InputStream>;

class InputData;
using InputDataPtr = QWeakPointer<InputData>;

class InputDemuxer;
using InputDemuxerPtr = QWeakPointer<InputDemuxer>;

struct Params {
    struct D {
        static constexpr int hw_accel_index = -1;
        static constexpr int hw_accel_dev_id = -1;
        static constexpr int avLogLevel = 8; // AV_LOG_FATAL
    };

    int hw_accel_index = D::hw_accel_index;
    int hw_accel_dev_id = D::hw_accel_dev_id;
    int av_log_level = D::avLogLevel;
};

enum class StreamType
{
    Rtp,
    MpegTs,
    RtpMpegTs,
    SrtMpegTs
};

Q_ENUM_NS(StreamType)


class ErrorHandler
{
public:
    virtual ~ErrorHandler() = default;
    virtual void setError(const QString &err) = 0;
    virtual const QString &errorString() const = 0;
};

QString avErrStr(int errNum);



} // namespace Av
} // namespace Vlx
