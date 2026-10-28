#ifndef GSP_INITIALIZATION_H
#define GSP_INITIALIZATION_H

#include <QObject>
#include <QTime>

class GSP_Initialization : public QObject
{
    Q_OBJECT
public:
    GSP_Initialization();
    ~GSP_Initialization() {  };

    typedef enum
    {

    } t_currState;

    void turnON() { this->start(); m_ON = true; };
    void turnOFF() { m_ON = false; };
    bool isProcessing() { return m_en; };
    QString runtime(const bool mechArrEn, const bool elArrFine, const bool elArrCoarse, const bool zeroEn, const bool stabEn);

signals:
    void sg_turnOffMotors();
    void sg_blockAllGSPInterface(const bool block);
    void sg_turnOffMechArretires();
    void sg_turnOnFineCircuit();
    void sg_turnOnCoarseCircuit();
    void sg_setZero();
    void sg_turnOnStabilization();

private slots:

private:
    bool isTimeOver(const int32_t timeout);
    void start();
    void stop();


private:
    bool m_ON { false };
    bool m_en { false };
    bool m_initComplete { false };
    int16_t m_currStep { 0 };
    QString m_currState { "Off" };

    int8_t m_attemptsCount { 0 };

    int32_t m_timeoutTurnOffMotors_msec { 2000 };
    int32_t m_timeoutTurnOffMechArr_msec { 30000 };
    int32_t m_timeoutTurnOnFineCircuit_msec { 2000 };
    int32_t m_timeoutTurnOnCoarseCircuit_msec { 2000 };
    int32_t m_timeoutSetZero_msec { 7000 };

    QTime m_time;

};

#endif // GSP_INITIALIZATION_H
