#include "gsp_initialization.h"

GSP_Initialization::GSP_Initialization()
    : QObject()
{

}

void GSP_Initialization::start()
{
    if(m_en) {
        this->stop();
    }

    m_currStep = 0;
    m_en = true;
    m_attemptsCount = 0;
    m_initComplete = false;
}

void GSP_Initialization::stop()
{
    m_en = false;
    m_currStep = 0;
    m_ON = false;
    emit sg_blockAllGSPInterface(false);

}

QString GSP_Initialization::runtime(const bool mechArrEn, const bool elArrFine, const bool elArrCoarse, const bool zeroEn, const bool stabEn)
{
    if( !m_ON ) {
        return "";
    }
/*
    static bool mechArrEnPrev = false;

    if( mechArrEnPrev != mechArrEn)
    {
        if(mechArrEn == true) {
            //Block any gsp control
            emit sg_blockAllGSPInterface(true);
    //        blockAllInterface(true);
            this->start();
//            return 1;
        }
        mechArrEnPrev = mechArrEn;
    }

    if( !m_en ) {
        m_currState = "";
        return 0;
    }
*/

    switch(m_currStep)
    {
    case 0: /** Turning off motors/ */
    {
        emit sg_turnOffMotors();
        m_currState = "Выключение двигателей";
        m_time = QTime::currentTime();
        m_currStep++;
    } break;
    case 1: /** Waiting command will be done. */
    {
        if(isTimeOver(m_timeoutTurnOffMotors_msec)) {
            m_currStep++;
        }
    } break;

    case 2: /** Send command to turn off mechanical arretires. */
    {
        emit sg_turnOffMechArretires();
        m_currState = "Снятие механических арретиров";
        m_time = QTime::currentTime();
        m_currStep++;
    } break;
    case 3: /** Waiting command will be done. */
    {
        if(mechArrEn == true) {
            if(isTimeOver(m_timeoutTurnOffMechArr_msec)) {
                if(m_attemptsCount > 2) {
                    /** FAIL */
                    this->stop();
                    m_currState = "Ошибка !!!";
                } else {
                    emit sg_turnOffMechArretires();
                    m_time = QTime::currentTime();
                    m_attemptsCount++;
                }
            }
        } else {
            m_currStep++;
            m_attemptsCount = 0;
        }
    } break;

    case 4: /** Turning ON fine circuit. */
    {
        emit sg_turnOnFineCircuit();
        m_currState = "Включение точного контура";
        m_time = QTime::currentTime();
        m_currStep++;
    } break;
    case 5: /** Waiting command will be done. */
    {
        if(isTimeOver(m_timeoutTurnOnFineCircuit_msec)) {
            m_currStep++;
        }
    } break;

    case 6: /** Turning ON coarse circuit. */
    {
        emit sg_turnOnCoarseCircuit();
        m_currState = "Включение грубого контура";
        m_time = QTime::currentTime();
        m_currStep++;
    } break;
    case 7: /** Waiting command will be done. */
    {
        if( (elArrFine && elArrCoarse) != true ) {
            if(isTimeOver(m_timeoutTurnOnCoarseCircuit_msec)) {
                if(m_attemptsCount > 2) {
                    /** FAIL */
                    this->stop();
                    m_currState = "Ошибка !!!";
                } else {
                    m_currStep = 4;
//                    emit sg_turnOffMechArretires();
//                    m_time = QTime::currentTime();
                    m_attemptsCount++;
                }
            }
        } else {
            m_currStep++;
            m_attemptsCount = 0;
        }
    } break;

    case 8: /** Set ZERO. */
    {
        emit sg_setZero();
        m_currState = "Установка в НОЛЬ";
        m_time = QTime::currentTime();
        m_currStep++;
    } break;
    case 9: /** Waiting command will be done. */
    {
        if(zeroEn) {
            m_attemptsCount = 0;
            m_currStep++;
        } else {
            if(isTimeOver(m_timeoutSetZero_msec)) {
                if(m_attemptsCount > 2) {
                    /** FAIL */
                    m_currState = "Ошибка !!!";
                    this->stop();
                } else {
                    emit sg_setZero();
                    m_time = QTime::currentTime();
                    m_attemptsCount++;
                }
            }
        }
    } break;

    case 10: /** Turning ON stabilization. */
    {
        emit sg_turnOnStabilization();
        m_currState = "Завершено";
        m_initComplete = true;
        this->stop();
    } break;
    default:
    {
        this->stop();
        m_currState = "Ошибка !!!";
        /** Status:  FAILED */
    } break;
    }

    return m_currState;

}

bool GSP_Initialization::isTimeOver(const int32_t timeout)
{
    QTime currTime = QTime::currentTime();
    if( m_time.msecsSinceStartOfDay() + timeout < currTime.msecsSinceStartOfDay()) {
        return true;
    } else {
        return false;
    }
}

