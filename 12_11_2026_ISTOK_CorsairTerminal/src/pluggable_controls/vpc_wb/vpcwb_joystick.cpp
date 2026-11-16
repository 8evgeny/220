#include "vpcwb_joystick.h"
#include "vpcwb_joystick_internal.h"

#include <QMetaEnum>
#include <QDebug>

namespace VpcWB {

static Joystick * s_inst = nullptr;

Joystick::Joystick(QObject *parent)
    : QObject{parent}
    , m_d{new Internal{this}}
{
    if (s_inst != nullptr) {
        // qWarning() << Q_FUNC_INFO << "!!!!!!!!! duplicate instancing. first one will be deleted";
        s_inst->deleteLater();
    }

    s_inst = this;
}

Joystick::~Joystick()
{
    s_inst = nullptr;
}

const QString &Joystick::lastError() const
{
    return m_d->lastError();
}

bool Joystick::init()
{
    return m_d->init();
}

void Joystick::deinit()
{
    return m_d->deinit();
}

bool Joystick::isReady() const
{
    return m_d->ready();
}

bool Joystick::isPollingActive() const
{
    return m_d->isPollingActive();
}

int Joystick::axisNum() const
{
    return m_d->m_axisNum;
}

int Joystick::buttonNum() const
{
    return m_d->m_btnNum;
}

int Joystick::axisMapValue(int origId) const
{
    return (origId >= 0 && origId < Internal::AXIS_NUM) ? m_d->m_axisMap[origId] : origId;
}

int Joystick::buttonMapValue(int origId) const
{
    return (origId >= 0 && origId < Internal::BTN_NUM) ? m_d->m_btnMap[origId] : origId;
}

void Joystick::mapAxis(int origId, int newId)
{
    if (origId >= 0 && origId < Internal::AXIS_NUM)
        m_d->m_axisMap[origId] = newId;
}

void Joystick::mapButton(int origId, int newId)
{
    if (origId >= 0 && origId < Internal::BTN_NUM)
        m_d->m_btnMap[origId] = newId;
}

int Joystick::maxAxisNum()
{
    return Internal::AXIS_NUM;
}

int Joystick::maxButtonNum()
{
    return Internal::BTN_NUM;
}

Joystick *Joystick::inst()
{
    return s_inst;
}

const Joystick::EventData &Joystick::lastEventData() const
{
    return m_d->lastEventData();
}

void Joystick::startPolling(qint64 ms)
{
    m_d->startPolling(ms);
}

void Joystick::stopPolling()
{
    m_d->stopPolling();
}

const Joystick::EventData &Joystick::poll()
{
    return m_d->poll();
}

QString Joystick::EventData::toString() const
{
    static auto me = QMetaEnum::fromType<VpcWB::Joystick::EventType>();

    QString out;

    out = QString("type: ") + me.valueToKey((int) type);
    out += QString(", id: %1").arg(id);
    out += (type == EventType::Axis1D) ? QString(", value: %1").arg(value)
           : (type == EventType::Button)
               ? (QString(", button ") + (pressed ? QString("pressed") : QString("released")))
               : "_";
    return out;
}

bool Joystick::EventData::valid() const
{
    return type != EventType::None;
}

} // namespace VpcWB
