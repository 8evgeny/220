#pragma once

#include <QObject>



namespace VpcWB {

class Internal;

class Joystick : public QObject
{
    Q_OBJECT

public:
    enum class EventType { None, Axis1D, Button };

    Q_ENUM(EventType)

    struct EventData
    {
        int id = -1;
        union {
            qint16 value = 0;
            struct
            {
                bool pressed;
                bool released;
            };
        };
        EventType type = EventType::None;

        QString toString() const;

        bool valid() const;
    };

    explicit Joystick(QObject *parent = nullptr);

    virtual ~Joystick();

    const QString &lastError() const;

    bool init();

    void deinit();

    bool isReady() const;

    bool isPollingActive() const;

    const EventData &lastEventData() const;

    int axisNum() const;

    int buttonNum() const;

    int axisMapValue(int origId) const;

    int buttonMapValue(int origId) const;

    void mapAxis(int origId, int newId);

    void mapButton(int origId, int newId);

    static int maxAxisNum();

    static int maxButtonNum();

    static Joystick *inst();

public slots:

    void startPolling(qint64 ms = 0);

    void stopPolling();

    const EventData &poll();

signals:

    void errorOccured(const QString &errorString);
    void readyStateChanged(bool state);
    void eventOccured(EventData ed);

private:
    Internal *m_d;
};

} // namespace VpcWB
