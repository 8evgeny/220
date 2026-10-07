
#ifndef RESULT_GET_STATUS_COMMAND_H
#define RESULT_GET_STATUS_COMMAND_H



#include "ReadResponseCommand.h"



class ResultGetStatusCommand : public ReadResponseCommand
{
public:

    struct MemsErrors
    {
        bool    errorAllMems;
    };

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

    struct MemsFlags
    {
        MemsFlagsBitFields      notConfigured;
        MemsFlagsBitFields      illegalData;
        MemsFlagsBitFields      noResponseOnSpi;
        MemsFlagsBitFields      reserve;
    };

    struct MemsBoardStatus
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
    };


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


    struct MotorStatus
    {
        uint8_t     mode;

        MotorErrors errors;
        MotorFlags  flags;

        float       angle;
        float       speed;
    };

    ResultGetStatusCommand(     const uint8_t srcDevice, const uint8_t dstDevice,
                                const uint8_t mode,
                                const ResultGetStatusCommand::MemsBoardStatus *const memsBoardStatus,
                                const ResultGetStatusCommand::MotorStatus *const motorYaw,
                                const ResultGetStatusCommand::MotorStatus *const motorPitch,
                                CommandExecutor *const commandExecutor = nullptr);

    ResultGetStatusCommand(     const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;

};


//struct ResultGetStatusCommand::MemsBoardStatus
//{
//    uint8_t     mode;

//    MemsErrors  errors;
//    MemsFlags   flags;

//    uint8_t     axisSwitchMode;
//    uint8_t     memsProcessMode;
//    float       speedYaw;
//    float       speedPitch;
//    float       angleYaw;
//    float       anglePitch;
//};







#endif

