#include "vdb_vrt_tracker_adapter_impl.hpp"
#include "math.h"
#include <algorithm>    // std::clamp

//-----------------------------------------------------------------------------------------------------------------
TrackerAdapter::TrackerAdapter(ChannelsPidMultiplexors multiplexor)
{
    m_expFiltAlfa.setAlpha(0.85);
    m_expFiltBetta.setAlpha(0.85);
    m_slewRateLimitterAlfa = new SlewRateLimiter(10.0f);
    m_slewRateLimitterBetta = new SlewRateLimiter(10.0f);
    setNewKoefs(multiplexor);
    // Поток из исходника убран: периодический вызов step() выполняет владелец
    // (GyroPlatformHandler) из таймера Qt в своём потоке.
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::step()
{
    if (!tracking)
        return;

    if (!(m_FOVCur.newVal || m_trackerFrameCur.newValDist || m_trackerFrameCur.newValSize))
        return;

    calcAngles();
    float speed_alpha = 0;
    float speed_beta  = 0;
    if(m_enableSlowdownByPrepareScore)
    {
        float squarekoef = m_trackerPrepareScoreNorm*m_trackerPrepareScoreNorm;
        m_pidAlfa.set_coefs(m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Kp * squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Ki * squarekoef* squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Kd * squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].outLim * squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].iLim * squarekoef* squarekoef);
        m_pidBetta.set_coefs(m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Kp * squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Ki * squarekoef* squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Kd * squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].outLim * squarekoef,
                            m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].iLim * squarekoef* squarekoef);
        speed_alpha = m_slewRateLimitterAlfa->limit(m_pidAlfa.calc(m_angErrors.alfa))*squarekoef;
        speed_beta  = m_slewRateLimitterBetta->limit(m_pidBetta.calc(m_angErrors.betta))*squarekoef;
    } else {
    speed_alpha = m_slewRateLimitterAlfa->limit(m_pidAlfa.calc(m_angErrors.alfa));
    speed_beta  = m_slewRateLimitterBetta->limit(m_pidBetta.calc(m_angErrors.betta));
    }
    m_FOVCur.newVal = false;
    m_trackerFrameCur.newValDist = false;
    m_trackerFrameCur.newValSize = false;
    send_speed_to_gsp(-speed_beta, -speed_alpha);
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::calcAngles()
{
    m_angErrors.alfa = m_trackerFrameCur.distX * m_FOVCur.x;
    m_angErrors.betta = m_trackerFrameCur.distY * m_FOVCur.y;
    m_angErrors.alfa = m_expFiltAlfa.update(m_angErrors.alfa);
    m_angErrors.betta = m_expFiltBetta.update(m_angErrors.betta);
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setTrackerStatusUpdate(PldTrackerStatus v_params)
{
    if(m_curChannel != v_params.curChannel){
        m_pidAlfa.reset();
        m_pidBetta.reset();
        m_curChannel = v_params.curChannel;
    }
    setCurFOV(v_params.fov,
              (v_params.fovY > 0.0f) ? v_params.fovY : v_params.fov);
    setCurTrackerFrameSize(v_params.sizeX, v_params.sizeY);
    setCurTrackerFrameDist(v_params.distX, v_params.distY);
    m_trackerPrepareScoreNorm = v_params.trackerPrepareScoreNorm;
    m_enableSlowdownByPrepareScore = v_params.enableSlowdownByPrepareScore != 0;
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setCurFOV(float v_xFov, float v_yFOV)
{
    if(m_FOVCur.x != v_xFov || m_FOVCur.y != v_yFOV) {
        m_FOVCur.x = v_xFov;
        m_FOVCur.y = v_yFOV;
        interpolateAndpreparePIDKoefs();
        m_FOVCur.newVal = true;
    }
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setCurTrackerFrameSize(float v_sizeX,float v_sizeY)
{
    if( m_trackerFrameCur.sizeX != v_sizeX ||  m_trackerFrameCur.sizeY != v_sizeY){
    m_trackerFrameCur.sizeX = v_sizeX;
    m_trackerFrameCur.sizeY = v_sizeY;
    m_trackerFrameCur.area = m_trackerFrameCur.sizeX*m_trackerFrameCur.sizeY;
    m_trackerFrameCur.newValSize = true;
    }
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setCurTrackerFrameDist(float v_distX, float v_distY)
{
    // Конвенция: v_distX/v_distY — ЛЕВЫЙ ВЕРХНИЙ угол рамки в [0;1];
    // внутри dist = смещение ЦЕНТРА рамки от центра кадра.
    // Исправление исходника: сравнение сырого входа с сырым (исходник сравнивал
    // сырое значение с уже преобразованным sizeX/2+v-0.5 — флаг взводился хаотично).
    if( m_trackerFrameCur.rawDistX != v_distX ||  m_trackerFrameCur.rawDistY != v_distY){
    m_trackerFrameCur.rawDistX = v_distX;
    m_trackerFrameCur.rawDistY = v_distY;
    m_trackerFrameCur.distX = m_trackerFrameCur.sizeX/2 + v_distX - 0.5f;
    m_trackerFrameCur.distY = m_trackerFrameCur.sizeY/2 + v_distY - 0.5f;
    m_trackerFrameCur.distTotal = sqrt((m_trackerFrameCur.distX*m_trackerFrameCur.distX)+
                                      (m_trackerFrameCur.distY*m_trackerFrameCur.distY));
    m_trackerFrameCur.newValDist = true;
    }
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::startTracking()
{
    tracking = true;
} /* handle_f_start_wr_norsp */
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::stopTracking()
{
    tracking = false;
    m_pidAlfa.reset();
    m_pidBetta.reset();
    // Исправление исходника: сброс экспоненциальных фильтров и slew-лимитеров,
    // иначе при повторном захвате в контур попадает «протухшее» состояние.
    m_expFiltAlfa.reset();
    m_expFiltBetta.reset();
    m_slewRateLimitterAlfa->reset();
    m_slewRateLimitterBetta->reset();
    send_speed_to_gsp(0,0);
} /* handle_f_stop_wr_norsp */
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::interpolateAndpreparePIDKoefs()
{
        //FOV
        float fovMaxX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].fovVal;
        float fovMinX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].fovVal;
        float fovMaxY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].fovVal;
        float fovMinY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].fovVal;
        // Защита от деления на ноль при совпадении wide/tele FOV в конфиге
        if (std::fabs(fovMaxX - fovMinX) < 1e-6f) fovMinX = fovMaxX - 1e-6f;
        if (std::fabs(fovMaxY - fovMinY) < 1e-6f) fovMinY = fovMaxY - 1e-6f;
        // Нормированная позиция FOV в диапазоне Tele(0)..Wide(1), кламп в [0;1]
        // (исходник экстраполировал за пределы опорных точек, k мог стать <0)
        const float tx = std::clamp((m_FOVCur.x - fovMinX) / (fovMaxX - fovMinX), 0.0f, 1.0f);
        const float ty = std::clamp((m_FOVCur.y - fovMinY) / (fovMaxY - fovMinY), 0.0f, 1.0f);
        //KKp
        float kMaxKKpX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKp;
        float kMinKKpX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKp;
        float kMaxKKpY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKp;
        float kMinKKpY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKp;
        float baseKpX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Kp;
        float baseKpY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Kp;
        float newKpX   = baseKpX * (kMinKKpX + (kMaxKKpX - kMinKKpX)*tx);
        float newKpY   = baseKpY * (kMinKKpY + (kMaxKKpY - kMinKKpY)*ty);
        //KKD
        float kMaxKKdX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKd;
        float kMinKKdX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKd;
        float kMaxKKdY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKd;
        float kMinKKdY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKd;
        float baseKdX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Kd;
        float baseKdY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Kd;
        float newKdX   = baseKdX * (kMinKKdX + (kMaxKKdX - kMinKKdX)*tx);
        float newKdY   = baseKdY * (kMinKKdY + (kMaxKKdY - kMinKKdY)*ty);
        //KKI
        float kMaxKKiX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKi;
        float kMinKKiX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKi;
        float kMaxKKiY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKi;
        float kMinKKiY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKi;
        float baseKiX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Ki;
        float baseKiY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Ki;
        float newKiX   = baseKiX * (kMinKKiX + (kMaxKKiX - kMinKKiX)*tx);
        float newKiY   = baseKiY * (kMinKKiY + (kMaxKKiY - kMinKKiY)*ty);
        //KiLim
        float kMaxKKilimX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKiLim;
        float kMinKKilimX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKiLim;
        float kMaxKKilimY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKiLim;
        float kMinKKilimY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKiLim;
        float baseKilimX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].iLim;
        float baseKilimY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].iLim;
        float newilimX   = baseKilimX * (kMinKKilimX + (kMaxKKilimX - kMinKKilimX)*tx);
        float newilimY   = baseKilimY * (kMinKKilimY + (kMaxKKilimY - kMinKKilimY)*ty);

        m_pidAlfa.set_coefs(newKpX, newKiX, newKdX,m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].outLim, newilimX);
        m_pidBetta.set_coefs(newKpY, newKiY, newKdY,m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].outLim, newilimY);
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setNewKoefs(ChannelsPidMultiplexors multiplexor)
{
    m_pidKoefs = multiplexor;
    interpolateAndpreparePIDKoefs();
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setSpeedSink(ITrackerSpeedSink *sink)
{
    m_pSpeedSink = sink;
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::send_speed_to_gsp(float pitch, float yaw)
{
    //отправка скоростей на гироплатформу — через sink (GyroPlatformHandler)
    if (m_pSpeedSink)
        m_pSpeedSink->onTrackerSpeed(pitch, yaw);
} /* send_speed_to_gsp */
