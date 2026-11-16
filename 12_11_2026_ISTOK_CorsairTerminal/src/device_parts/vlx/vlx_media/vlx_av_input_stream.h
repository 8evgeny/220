#pragma once
#include <QObject>

#include <opencv2/opencv.hpp>

namespace Vlx
{
namespace Av
{

class InputStream : public QObject
{
    Q_OBJECT

protected:
    explicit InputStream()
        : QObject{nullptr}
    {}

    virtual ~InputStream() = default;

public:
    static const QStringList& supportedHwAccelList();


    virtual bool enabled() const = 0;
    virtual void setEnabled(bool state)  = 0;

    virtual int pid() const = 0;
    virtual int pendingFrames() const = 0;
    virtual cv::Mat popFrame() = 0;


signals:
    void hasPendingFrames();
};

} // namespace Av
} // namespace Vlx