#pragma once

#include <QObject>

namespace Vlx
{
namespace Av
{

class InputData : public QObject
{
    Q_OBJECT

protected:
    explicit InputData()
        : QObject{nullptr}
    {}

    virtual ~InputData() = default;

public:
    static const QStringList& supportedHwAccelList();

    virtual int pid() const = 0;
    virtual int pendingPackets() const = 0;
    virtual QByteArray popPacket() = 0;

signals:
    void hasPendingPackets();
};

} // namespace Av
} // namespace Vlx