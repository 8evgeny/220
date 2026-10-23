
#ifndef GOEN_220_PROTOCOL_H
#define GOEN_220_PROTOCOL_H


#include <stdint.h>


#define GOEN_220__GYRO_PLATFORM__CMD__MODE                  (0x01)
#define GOEN_220__GYRO_PLATFORM__CMD__CONTROL_POSITION      (0x02)
#define GOEN_220__GYRO_PLATFORM__CMD__CONTROL_SPEED         (0x03)
#define GOEN_220__GYRO_PLATFORM__CMD__CONTROL_TRACKING      (0x04)
#define GOEN_220__GYRO_PLATFORM__CMD__VERSION               (0x00)
#define GOEN_220__GYRO_PLATFORM__CMD__STATUS                (0x05)





typedef struct
{
    uint8_t     mode;
}
__attribute__((packed)) ModeCmd;


typedef struct
{
    float       yaw;
    float       pitch;
}
__attribute__((packed)) ControlPosition;


typedef struct
{
    float       yaw;
    float       pitch;
}
__attribute__((packed)) ControlSpeed;


typedef struct
{
    float       yawMisalignment;
    float       pitchMisalignment;
    float       yawAccelLimit;
    float       pitchAccelLimit;
}
__attribute__((packed)) ControlTracking;




typedef struct
{
    uint32_t    versionProtocolVdb;
    uint32_t    versionProtocol;
    uint32_t    versionDevice;
}
__attribute__((packed)) VersionResultCmd;


typedef struct
{
    uint8_t     errorAllMems    : 1;
    uint8_t     reserved        : 7;
}
__attribute__((packed)) MemsErrors;



union MemsFlagsBitFields
{
    uint8_t     flags;

    struct
    {
        uint8_t mems_1  : 1;    //  Lo
        uint8_t mems_2  : 1;
        uint8_t mems_3  : 1;
        uint8_t mems_4  : 1;
        uint8_t mems_5  : 1;
        uint8_t mems_6  : 1;
        uint8_t mems_7  : 1;
        uint8_t mems_8  : 1;
    };
};


typedef struct
{
    MemsFlagsBitFields      notConfigured;
    MemsFlagsBitFields      illegalData;
    MemsFlagsBitFields      noResponseOnSpi;
    MemsFlagsBitFields      reserve;
}
__attribute__((packed)) MemsFlags;


typedef struct
{
    uint8_t     mode;

    MemsErrors  errors;
    MemsFlags   flags;

    uint8_t     axisSwitchMode;
    uint8_t     memsProcessMode;
    float       speedYaw;
    float       speedPitch;
    float       angleYaw;
    float       anglePitch;
}
__attribute__((packed)) MemsBoardStatus;




union MotorErrors
{
    uint8_t     flags;

    struct
    {
        uint8_t overcurrent         : 1;    //  Lo
        uint8_t overtemperature     : 1;
        uint8_t overspeed           : 1;
        uint8_t weakMagneticField   : 1;
        uint8_t lowVoltage          : 1;

        uint8_t reserved            : 3;
    };
};


union MotorFlags
{
    uint8_t     flags;

    struct
    {
        uint8_t error               : 1;    //  Lo
        uint8_t sectorLimitEnable   : 1;
        uint8_t calibrationEnable   : 1;
        uint8_t unsavedData         : 1;

        uint8_t reserved            : 4;
    };
};


typedef struct
{
    uint8_t     mode;

    MotorErrors errors;
    MotorFlags  flags;

    float       angle;
    float       speed;
}
__attribute__((packed)) MotorStatus;





typedef struct
{
    uint8_t             mode;

    MemsBoardStatus     mems;

    MotorStatus         motorYaw;
    MotorStatus         motorPitch;
}
__attribute__((packed)) TelemetryUART;


#endif

