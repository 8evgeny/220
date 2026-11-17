#ifndef PIDCOEFS_HPP
#define PIDCOEFS_HPP
#include "vdb_vrt_tracker_adapter_impl.hpp"



class pidCoefs{
public:
    TrackerAdapter::ChannelsPidMultiplexors coefs;
    pidCoefs() = default;
    TrackerAdapter::ChannelsPidMultiplexors getCoefs();
};

inline TrackerAdapter::ChannelsPidMultiplexors pidCoefs::getCoefs(){
    //__________________________________TV_____________________________________//
    //Base PID X
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::x)].Kp = 1;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::x)].Ki = 0.001;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::x)].Kd = 0;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::x)].iLim = 1;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::x)].outLim = 10;
    //Base PID Y
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::y)].Kp = 1;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::y)].Ki = 0.001;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::y)].Kd = 0;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::y)].iLim = 1;
    coefs.oneChannelK[int(Channel::TV)].basePidK[int(Axis::y)].outLim = 10;
    //WIDE FOV
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].fovVal = 20;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKp = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKi = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKd = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKiLim = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKp = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKi = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKd = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKiLim = 1;
    //TELE FOV
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].fovVal = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKp = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKi = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKd = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKiLim = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKp = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKi = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKd = 1;
    coefs.oneChannelK[int(Channel::TV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKiLim = 1;

    //__________________________________TPV_____________________________________//
    //Base PID X
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::x)].Kp = 1;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::x)].Ki = 0.001;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::x)].Kd = 0;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::x)].iLim = 1;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::x)].outLim = 10;
    //Base PID Y
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::y)].Kp = 1;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::y)].Ki = 0.001;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::y)].Kd = 0;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::y)].iLim = 1;
    coefs.oneChannelK[int(Channel::TPV)].basePidK[int(Axis::y)].outLim = 10;
    //WIDE FOV
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].fovVal = 20;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKp = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKi = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKd = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKiLim = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKp = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKi = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKd = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::y)].kKiLim = 1;
    //TELE FOV
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].fovVal = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKp = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKi = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKd = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKiLim = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKp = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKi = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKd = 1;
    coefs.oneChannelK[int(Channel::TPV)].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::y)].kKiLim = 1;
}










#endif // PIDCOEFS_HPP
