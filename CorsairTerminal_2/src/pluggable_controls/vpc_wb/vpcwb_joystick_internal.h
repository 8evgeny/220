#pragma once

#include <SDL2/SDL.h>

#include "vpcwb_joystick.h"

#include <QHash>
#include <QObject>

#include <QTimer>

namespace VpcWB {

class Internal : public QObject
{
    Q_OBJECT

    constexpr static const int AXIS_NUM = 8;
    constexpr static const int BTN_NUM = 72;

    friend class Joystick;

    explicit Internal(Joystick *parent);

    virtual ~Internal();

    const QString &lastError() const { return m_lastErrorStr; }

    bool ready() const { return m_jHash.size() > 0; }

    bool init();

    void deinit();

    void setError(const QString &msgPrefix, const QString &errorStr, bool silent = false);

    bool isPollingActive() const;

    void startPolling(qint64 ms);

    void stopPolling();

    const Joystick::EventData &lastEventData() const;

    void setEventAxisData(int id, qint16 value);

    void setEventButtonData(int id, bool isPressed);

private slots:

    const Joystick::EventData &poll();

private:
    Joystick *m_p;

    QTimer m_pollingTimer;

    QString m_lastErrorStr;

    QHash<int, SDL_Joystick *> m_jHash;
    QHash<int, int> m_axisIdOffset;
    QHash<int, int> m_btnIdOffset;

    int m_axisNum = 0;
    int m_btnNum = 0;

    Joystick::EventData m_ed;

    int m_axisMap[AXIS_NUM];
    int m_btnMap[BTN_NUM];
};

} // namespace VpcWB
