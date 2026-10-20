#include "vpcwb_joystick_internal.h"

#include <QDebug>

namespace VpcWB {

Internal::Internal(Joystick *parent)
    : QObject{parent}
    , m_p{parent}
{
    connect(&m_pollingTimer, &QTimer::timeout, this, &Internal::poll);

    for (int i = 0; i < AXIS_NUM; i++)
        m_axisMap[i] = i;

    for (int i = 0; i < BTN_NUM; i++)
        m_btnMap[i] = i;
}

Internal::~Internal()
{
    deinit();
}

bool Internal::init()
{
    if (ready()) {
        setError(Q_FUNC_INFO, "double initialization. refused");
        return false;
    }

    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        setError(Q_FUNC_INFO, QString("SDL INIT ERROR. ") + SDL_GetError());
        return false;
    }

    int numJoysticks = SDL_NumJoysticks();

    for (int i = 0; i < numJoysticks; i++) {
        QString name = SDL_JoystickNameForIndex(i);

        qInfo() << Q_FUNC_INFO << " joystick" << i << "name:" << name;

        bool auxMode = name.contains("VPC Stick WarBRD-D");
        bool sepMode = (name.contains("VPC Stick WarBRD") || name.contains("VPC Panel #1"));

        if (!auxMode && !sepMode)
            continue;

        auto *j = SDL_JoystickOpen(i);
        if (j == nullptr) {
            setError(Q_FUNC_INFO,
                     "Unable to open joystick. index=" + QString::number(i) + ", name=" + name
                         + ", error: " + SDL_GetError());
            return false;
        }

        auto axs = SDL_JoystickNumAxes(j);
        auto btns = SDL_JoystickNumButtons(j);

        int id = SDL_JoystickInstanceID(j);

        qInfo() << "Joystick found:" << name << "id: " << id << " axis num:" << axs
                << " btns num:" << btns;

        m_jHash[id] = j;
    }

    if (!ready()) {
        setError(Q_FUNC_INFO, "No <VPC Stick WarBRD> joystick found", true);
        deinit();
        return false;
    }

    auto il = m_jHash.keys();

    std::sort(il.begin(), il.end());

    m_axisNum = 0;
    m_btnNum = 0;

    for (int i = 0; i < il.size(); i++) {
        auto j = m_jHash[il.at(i)];
        int id = SDL_JoystickInstanceID(j);
        auto an = SDL_JoystickNumAxes(j);
        auto bn = SDL_JoystickNumButtons(j);

        m_axisIdOffset[id] = m_axisNum;
        m_btnIdOffset[id] = m_btnNum;

        m_axisNum += an;
        m_btnNum += bn;
    }

    emit m_p->readyStateChanged(ready());

    return true;
}

void Internal::deinit()
{
    if (!ready())
        return;

    m_pollingTimer.stop();

    for (auto &j : m_jHash)
        if (SDL_JoystickGetAttached(j) == SDL_TRUE)
            SDL_JoystickClose(j);

    m_jHash.clear();
    m_axisIdOffset.clear();
    m_btnIdOffset.clear();
    m_axisNum = 0;
    m_btnNum = 0;

    if(SDL_WasInit(SDL_INIT_JOYSTICK))
        SDL_JoystickEventState(SDL_IGNORE);

    SDL_Quit();

    emit m_p->readyStateChanged(false);
}

void Internal::setError(const QString &msgPrefix, const QString &errorStr, bool silent)
{
    m_lastErrorStr = errorStr;

    if (!silent)
        qCritical() << msgPrefix << errorStr;

    emit m_p->errorOccured(m_lastErrorStr);
}

bool Internal::isPollingActive() const
{
    return m_pollingTimer.isActive();
}

void Internal::startPolling(qint64 ms)
{
    m_pollingTimer.start(ms);
}

void Internal::stopPolling()
{
    m_pollingTimer.stop();
}

const Joystick::EventData &Internal::lastEventData() const
{
    return m_ed;
}

void Internal::setEventAxisData(int id, qint16 value)
{
    m_ed.id = m_axisMap[id];
    m_ed.type = Joystick::EventType::Axis1D;
    m_ed.value = value;

    emit m_p->eventOccured(m_ed);
}

void Internal::setEventButtonData(int id, bool isPressed)
{
    m_ed.id = m_btnMap[id];
    m_ed.type = Joystick::EventType::Button;
    m_ed.pressed = isPressed;
    m_ed.released = !isPressed;

    emit m_p->eventOccured(m_ed);
}

const Joystick::EventData &Internal::poll()
{
    static const Joystick::EventData _stub;

    SDL_Event event;
    if (!SDL_PollEvent(&event))
        return _stub;

    switch (event.type) {
    case SDL_JOYAXISMOTION: {
        auto jid = event.jaxis.which;
        if (!m_jHash.contains(jid))
            break;
        auto axis = m_axisIdOffset[jid] + event.jaxis.axis;
        setEventAxisData(axis, event.jaxis.value);
    }
        return lastEventData();

    case SDL_JOYBUTTONDOWN: {
        auto jid = event.jbutton.which;
        if (!m_jHash.contains(jid))
            break;
        auto btn = m_btnIdOffset[jid] + event.jbutton.button;
        setEventButtonData(btn, true);
    }
        return lastEventData();

    case SDL_JOYBUTTONUP: {
        auto jid = event.jbutton.which;
        if (!m_jHash.contains(jid))
            break;
        auto btn = m_btnIdOffset[jid] + event.jbutton.button;
        setEventButtonData(btn, false);
    }
        return lastEventData();

    case SDL_JOYDEVICEREMOVED:
        if (m_jHash.contains(event.jdevice.which))
            deinit();
        break;
    }

    return _stub;
}

} // namespace VpcWB
