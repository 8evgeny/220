
#include "Goen220GyroPlatformHost.h"

#include "Goen220GyroPlatformHostPrivate.h"

#include <QDebug>
#include <QMetaEnum>

Goen220GyroPlatformHost::Goen220GyroPlatformHost(QObject *parent)
    : QObject{parent}, d_ptr(new Goen220GyroPlatformHostPrivate())
{

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::stateConnectToSerialPortChanged, this,
                     &Goen220GyroPlatformHost::stateConnectToSerialPortChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::serialPortNameChanged, this,
                     &Goen220GyroPlatformHost::serialPortNameChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::serialPortErrorStringChanged, this,
                     &Goen220GyroPlatformHost::serialPortErrorStringChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::memsBoardStatusCommonChanged, this,
                     &Goen220GyroPlatformHost::memsBoardStatusCommonChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::versionChanged, this,
                     &Goen220GyroPlatformHost::versionChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::modeChanged, this,
                     [this](const uint8_t mode)
                     {
                         emit modeChanged((Mode)mode);
                     } /* lambda */
    );

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::memsBoardStatusSpeedAndAngleChanged,
                     this, &Goen220GyroPlatformHost::memsBoardStatusSpeedAndAngleChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::yawMotorStatusChanged, this,
                     &Goen220GyroPlatformHost::yawMotorStatusChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::pitchMotorStatusChanged, this,
                     &Goen220GyroPlatformHost::pitchMotorStatusChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::setModeCommandCompleted, this,
                     &Goen220GyroPlatformHost::setModeCommandCompleted);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::setControlPositionCommandCompleted,
                     this, &Goen220GyroPlatformHost::setControlPositionCommandCompleted);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::setControlSpeedCommandCompleted, this,
                     &Goen220GyroPlatformHost::setControlSpeedCommandCompleted);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::setControlTrackingCommandCompleted,
                     this, &Goen220GyroPlatformHost::setControlTrackingCommandCompleted);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::txRawDataSerialPortText, this,
                     &Goen220GyroPlatformHost::txRawDataSerialPortText);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::rxRawDataSerialPortText, this,
                     &Goen220GyroPlatformHost::rxRawDataSerialPortText);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::printTxRawDataSerialPortChanged, this,
                     &Goen220GyroPlatformHost::printTxRawDataSerialPortChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::printRxRawDataSerialPortChanged, this,
                     &Goen220GyroPlatformHost::printRxRawDataSerialPortChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::stateConnectToRemoteDeviceChanged,
                     this, &Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged);

    QObject::connect(d_ptr, &Goen220GyroPlatformHostPrivate::restart, this,
                     &Goen220GyroPlatformHost::restart);

} /* Goen220GyroPlatformHost::Goen220GyroPlatformHost */

Goen220GyroPlatformHost::~Goen220GyroPlatformHost()
{
    d_ptr->deleteLater();
    d_ptr = nullptr;
} /* Goen220GyroPlatformHost::~Goen220GyroPlatformHost */

QStringList Goen220GyroPlatformHost::getSerialPorts() const
{
    return Goen220GyroPlatformHostPrivate::getSerialPorts();
} /* Goen220GyroPlatformHost::getSerialPorts */
void Goen220GyroPlatformHost::connect(const bool con)
{
    d_ptr->connect(con);
} /* Goen220GyroPlatformHost::connect */

QString Goen220GyroPlatformHost::getSerialPortName() const
{
    return d_ptr->getSerialPortName();
} /* Goen220GyroPlatformHost::getSerialPortName */

void Goen220GyroPlatformHost::setSerialPortName(const QString &portName)
{
    d_ptr->setSerialPortName(portName);
} /* Goen220GyroPlatformHost::setSerialPortName */

QString Goen220GyroPlatformHost::getSerialPortErrorString() const
{
    return d_ptr->getSerialPortErrorString();
} /* Goen220GyroPlatformHost::getSerialPortErrorString */

bool Goen220GyroPlatformHost::getPrintTxRawDataSerialPort() const
{
    return d_ptr->getPrintTxRawDataSerialPort();
} /* Goen220GyroPlatformHost::getPrintTxRawDataSerialPort */

void Goen220GyroPlatformHost::setPrintTxRawDataSerialPort(const bool enable)
{
    d_ptr->setPrintTxRawDataSerialPort(enable);
} /* Goen220GyroPlatformHost::setPrintTxRawDataSerialPort */

bool Goen220GyroPlatformHost::getPrintRxRawDataSerialPort() const
{
    return d_ptr->getPrintRxRawDataSerialPort();
} /* Goen220GyroPlatformHost::getPrintRxRawDataSerialPort */

void Goen220GyroPlatformHost::setPrintRxRawDataSerialPort(const bool enable)
{
    d_ptr->setPrintRxRawDataSerialPort(enable);
} /* Goen220GyroPlatformHost::setPrintRxRawDataSerialPort */

void Goen220GyroPlatformHost::setHostId(const uint8_t hostId)
{
    d_ptr->setHostId(hostId);
} /* Goen220GyroPlatformHost::setHostId */

void Goen220GyroPlatformHost::setDeviceId(const uint8_t deviceId)
{
    d_ptr->setDeviceId(deviceId);
} /* Goen220GyroPlatformHost::setDeviceId */

void Goen220GyroPlatformHost::setTimeoutWaitingResponse(const uint32_t msec)
{
    d_ptr->setTimeoutWaitingResponse(msec);
} /* Goen220GyroPlatformHost::setTimeoutWaitingResponse */

void Goen220GyroPlatformHost::setTimeBetweenStatusRequests(const uint32_t msec)
{
    d_ptr->setTimeBetweenStatusRequests(msec);
} /* Goen220GyroPlatformHost::setTimeBetweenStatusRequests */

void Goen220GyroPlatformHost::setMode(const Mode mode)
{
    d_ptr->setMode((uint8_t)mode);
} /* Goen220GyroPlatformHost::setMode */

QStringList Goen220GyroPlatformHost::getModeNames() const
{
    QStringList names;
    const QMetaObject *meta = metaObject();

    int enumIndex = meta->indexOfEnumerator("Mode");
    QMetaEnum metaEnum = meta->enumerator(enumIndex);

    for (int i = 0; i < metaEnum.keyCount(); i++)
        names.append(metaEnum.key(i));

    return names;
} /* Goen220GyroPlatformHost::getModeNames */

void Goen220GyroPlatformHost::setControlPosition(const float yaw, const float pitch)
{
    d_ptr->setControlPosition(yaw, pitch);
} /* Goen220GyroPlatformHost::setControlPosition */

void Goen220GyroPlatformHost::setControlSpeed(const float yaw, const float pitch)
{
    d_ptr->setControlSpeed(yaw, pitch);
} /* Goen220GyroPlatformHost::setControlSpeed */

void Goen220GyroPlatformHost::setControlTracking(const float yawMisalignment,
                                                 const float pitchMisalignment,
                                                 const float yawAccelLimit,
                                                 const float pitchAccelLimit)
{
    d_ptr->setControlTracking(yawMisalignment, pitchMisalignment, yawAccelLimit, pitchAccelLimit);
} /* Goen220GyroPlatformHost::setControlTracking */
