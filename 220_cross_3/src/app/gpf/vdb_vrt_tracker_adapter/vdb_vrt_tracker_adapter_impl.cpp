#include "vdb_vrt_tracker_adapter_impl.hpp"
#include "math.h"
#include <thread>
#include "QDebug"


TrackerAdapter::TrackerAdapter(ChannelsPidMultiplexors multiplexor)
{
    //m_pixSize = (m_params.resX/m_params.angleField)*180/3.14f; //размер пикселя в радианах
    m_expFiltAlfa.setAlpha(0.85);
    m_expFiltBetta.setAlpha(0.85);
    m_slewRateLimitterAlfa = new SlewRateLimiter(10.0f);
    m_slewRateLimitterBetta = new SlewRateLimiter(10.0f);
    setNewKoefs(multiplexor);
    //запуск runtime в потоке, можно убрат<<m_speeedYь и вызывать runtime извне, тогда надо убрать while(1) и td::this_thread::sleep_for(std::chrono::milliseconds(1));
    //std::thread processCoefs(&TrackerAdapter::runtime, this);
    //processCoefs.detach();
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::runtime()
{
    // while(1)
    // {
        if(tracking)
        {
            if(m_FOVCur.newVal || m_trackerFrameCur.newValDist || m_trackerFrameCur.newValSize)
            {
                calcAngles();  
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
                    m_speeedX = m_slewRateLimitterAlfa->limit(m_pidAlfa.calc(m_angErrors.alfa))*squarekoef;
                    m_speeedY  = m_slewRateLimitterBetta->limit(m_pidBetta.calc(m_angErrors.betta))*squarekoef;
                } else {
                m_speeedX = m_slewRateLimitterAlfa->limit(m_pidAlfa.calc(m_angErrors.alfa));
                m_speeedY  = m_slewRateLimitterBetta->limit(m_pidBetta.calc(m_angErrors.betta));
                qDebug()<<"m_speeedY="<<m_speeedY;
                }
                m_FOVCur.newVal = false;
                m_trackerFrameCur.newValDist = false;
                m_trackerFrameCur.newValSize = false;
                newSpeedReady = true;
                //send_speed_to_gsp(-speed_beta, -speed_alpha);
            }
        }
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // }
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::calcAngles()
{
    // m_angErrors.alfa = atan(m_trackerFrameCur.distX * tan(m_FOVCur.x/2)) * 2;
    // m_angErrors.betta =  atan(m_trackerFrameCur.distY * tan(m_FOVCur.y/2)) * 2;
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
    setCurFOV(v_params.fov, v_params.fov);
    setCurTrackerFrameSize(v_params.sizeX, v_params.sizeY);
    setCurTrackerFrameDist(v_params.distX, v_params.distY);
    m_trackerPrepareScoreNorm = v_params.trackerPrepareScoreNorm;
    m_enableSlowdownByPrepareScore = v_params.enableSlowdownByPrepareScore; 
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setCurFOV(float v_xFov, float v_yFOV)
{
    //v_xFov = v_xFov / 180.f * M_PI;
    //v_yFOV = v_yFOV / 180.f * M_PI;
    if(m_FOVCur.x != v_xFov || m_FOVCur.y != v_yFOV) {
        m_FOVCur.x = v_xFov;
        m_FOVCur.y = v_yFOV; 
        interpolateAndpreparePIDKoefs();
        m_FOVCur.newVal = true;
    }
    //m_pixSize = (m_params.resX/m_params.angleField)*180/3.14; //размер пикселя в радианах
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
    if( m_trackerFrameCur.distX != v_distX ||  m_trackerFrameCur.distY != v_distY){
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
    m_speeedX = 0;
    m_speeedY = 0;
} /* handle_f_stop_wr_norsp */
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::interpolateAndpreparePIDKoefs()
{
        //FOV
        float fovMaxX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].fovVal;
        float fovMinX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].fovVal;
        float fovMaxY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].fovVal;
        float fovMinY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].fovVal;
        //KKp
        float kMaxKKpX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKp;
        float kMinKKpX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKp;
        float kMaxKKpY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKp;
        float kMinKKpY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKp;
        float baseKpX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Kp;
        float baseKpY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Kp;
        float newKpX   = baseKpX * (kMinKKpX + (kMaxKKpX - kMinKKpX)*(m_FOVCur.x - fovMinX)/(fovMaxX - fovMinX));
        float newKpY   = baseKpY * (kMinKKpY + (kMaxKKpY - kMinKKpY)*(m_FOVCur.y - fovMinY)/(fovMaxY - fovMinY));
        //KKD
        float kMaxKKdX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKd;
        float kMinKKdX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKd;
        float kMaxKKdY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKd;
        float kMinKKdY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKd;
        float baseKdX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Kd;
        float baseKdY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Kd;
        float newKdX   = baseKdX * (kMinKKdX + (kMaxKKdX - kMinKKdX)*(m_FOVCur.x - fovMinX)/(fovMaxX - fovMinX));
        float newKdY   = baseKdY * (kMinKKdY + (kMaxKKdY - kMinKKdY)*(m_FOVCur.y - fovMinY)/(fovMaxY - fovMinY));
        //KKI
        float kMaxKKiX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKi;
        float kMinKKiX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKi;
        float kMaxKKiY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKi;
        float kMinKKiY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKi;
        float baseKiX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].Ki;
        float baseKiY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].Ki;
        float newKiX   = baseKiX * (kMinKKiX + (kMaxKKiX - kMinKKiX)*(m_FOVCur.x - fovMinX)/(fovMaxX - fovMinX));
        float newKiY   = baseKiY * (kMinKKiY + (kMaxKKiY - kMinKKiY)*(m_FOVCur.y - fovMinY)/(fovMaxY - fovMinY));
        //KiLim
        float kMaxKKilimX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKiLim;
        float kMinKKilimX = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKiLim;
        float kMaxKKilimY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKiLim;
        float kMinKKilimY = m_pidKoefs.oneChannelK[int(m_curChannel)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKiLim;
        float baseKilimX  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].iLim;
        float baseKilimY  = m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].iLim;
        float newilimX   = baseKilimX * (kMinKKilimX + (kMaxKKilimX - kMinKKilimX)*(m_FOVCur.x - fovMinX)/(fovMaxX - fovMinX));
        float newilimY   = baseKilimY * (kMinKKilimY + (kMaxKKilimY - kMinKKilimY)*(m_FOVCur.y - fovMinY)/(fovMaxY - fovMinY));

        m_pidAlfa.set_coefs(newKpX, newKiX, newKdX,m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::x)].outLim, newilimX);
        m_pidBetta.set_coefs(newKpY, newKiY, newKdY,m_pidKoefs.oneChannelK[int(m_curChannel)].basePidK[int(Axis::y)].outLim, newilimY);
        qDebug()<<"KpY="<<newKpY<<"KiY="<<newKiY<<"KdY="<<newKdY;
        // m_k_pd_fov_x = k_min_x + (k_max - k_min_x)*(m_FOVCur.x - fov_min_x)/(fov_max_x-fov_min_x);
        // m_k_pd_fov_y = k_min_y + (k_max - k_min_y)*(m_FOVCur.y - fov_min_y)/(fov_max_y-fov_min_y);
        // m_k_lim_fov_x = k_min_xy_lim + (k_max - k_min_xy_lim)*(m_FOVCur.x - fov_min_x)/(fov_max_x-fov_min_x);
        // m_k_lim_fov_y = k_min_xy_lim + (k_max - k_min_xy_lim)*(m_FOVCur.y - fov_min_y)/(fov_max_y-fov_min_y);
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setNewKoefs(ChannelsPidMultiplexors multiplexor)
{
    m_pidKoefs = multiplexor;
    interpolateAndpreparePIDKoefs();
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::send_speed_to_gsp(float pitch, float yaw)
{
    //отправка скоростей на гироплатформу
} /* send_speed_to_gsp */