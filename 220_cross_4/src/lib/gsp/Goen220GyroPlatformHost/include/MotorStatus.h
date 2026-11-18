#ifndef MOTOR_STATUS_H
#define MOTOR_STATUS_H


#include <QObject>


struct MotorErrors
{
    uint8_t overcurrent         : 1;    //  Lo
    uint8_t overtemperature     : 1;
    uint8_t overspeed           : 1;
    uint8_t weakMagneticField   : 1;
    uint8_t lowVoltage          : 1;

    uint8_t reserved            : 3;
};


struct MotorFlags
{
    uint8_t error               : 1;    //  Lo
    uint8_t sectorLimitEnable   : 1;
    uint8_t calibrationEnable   : 1;
    uint8_t unsavedData         : 1;

    uint8_t reserved            : 4;
};




struct MotorStatus
{
    Q_GADGET

    Q_PROPERTY( uint8_t mode MEMBER mode)

    Q_PROPERTY( uint8_t errors MEMBER errors)	//	MotorErrors
    Q_PROPERTY( uint8_t flags MEMBER flags)		//	MotorFlags

    Q_PROPERTY( float angle MEMBER angle)
    Q_PROPERTY( float speed MEMBER speed)

public:

    uint8_t  	mode = 0x00;

    uint8_t 	errors = 0x00;
    uint8_t  	flags = 0x00;

    float       angle = 0.0f;
    float       speed = 0.0f;
};


Q_DECLARE_METATYPE(MotorStatus)



#endif
