#ifndef MEMS_BOARD_STATUS_H
#define MEMS_BOARD_STATUS_H


#include <QObject>





struct MemsBoardStatusCommon
{
    Q_GADGET

    Q_PROPERTY( uint8_t mode MEMBER mode)

    Q_PROPERTY( bool errorAllMems MEMBER errorAllMems)

    Q_PROPERTY( uint8_t notConfiguredFlags MEMBER notConfiguredFlags)
    Q_PROPERTY( uint8_t illegalDataFlags MEMBER illegalDataFlags)
    Q_PROPERTY( uint8_t noResponseOnSpiFlags MEMBER noResponseOnSpiFlags)
    Q_PROPERTY( uint8_t reserveFlags MEMBER reserveFlags)


    Q_PROPERTY( uint8_t axisSwitchMode MEMBER axisSwitchMode)
    Q_PROPERTY( uint8_t memsProcessMode MEMBER memsProcessMode)

public:

    uint8_t     mode = 0x00;

    bool        errorAllMems = false;

    uint8_t     notConfiguredFlags = 0x00;
    uint8_t     illegalDataFlags = 0x00;
    uint8_t     noResponseOnSpiFlags = 0x00;
    uint8_t     reserveFlags = 0x00;

    uint8_t     axisSwitchMode = 0x00;
    uint8_t     memsProcessMode = 0x00;
};




struct MemsBoardStatusSpeedAndAngle
{
    Q_GADGET

    Q_PROPERTY( float speedYaw MEMBER speedYaw)
    Q_PROPERTY( float speedPitch MEMBER speedPitch)
    Q_PROPERTY( float angleYaw MEMBER angleYaw)
    Q_PROPERTY( float anglePitch MEMBER anglePitch)

public:

    float       speedYaw = 0.0f;
    float       speedPitch = 0.0f;
    float       angleYaw = 0.0f;
    float       anglePitch = 0.0f;
};



Q_DECLARE_METATYPE(MemsBoardStatusCommon)
Q_DECLARE_METATYPE(MemsBoardStatusSpeedAndAngle)



#endif
