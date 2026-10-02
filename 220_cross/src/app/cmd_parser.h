#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <QObject>
#include "ev9500m/camera_ev9500m.h"
#include "ldc20i/ldc20i_controller.h"
#include "mini640/mini640_worker.h"
#include "telemetry.h"

#define ucp const Telemetry::udp_command_pack
#define usm const Telemetry::udp_command_msg

class CMD_Parser: public QObject
{
    QThread thread;
    Q_OBJECT
public:
    CMD_Parser();
    ~CMD_Parser();


 private:
   ucp* cm;
   usm* msg;

   qint32 processCmd();
   qint32 processCmdGowinMini640();

   //IMG_corr_str ics;


   qint32 proc_CH_ENABLE();
   qint32 proc_CONTR_ENHANCEMENT();
   qint32 proc_PROCESSING_BRIGHTNESS();
   qint32 proc_PROCESSING_CONTRAST();
   qint32 proc_NEGATIVE_MW();
   qint32 proc_AUTOFOCUS();
   qint32 proc_MAN_ZOOM_IN();
   qint32 proc_MAN_ZOOM_OUT();
   qint32 proc_MAN_ZOOM_STOP();
   qint32 proc_MAN_ZOOM_POSITION();
   qint32 proc_MAN_FOCUS_FAR();
   qint32 proc_MAN_FOCUS_NEAR();
   qint32 proc_MAN_FOCUS_STOP();
   qint32 proc_MAN_FOCUS_POSITION();
   qint32 proc_MAN_IRIS_OPEN();
   qint32 proc_MAN_IRIS_CLOSE();
   qint32 proc_MAN_IRIS_STOP();
   qint32 proc_FIELDS_SYNC();
   qint32 proc_REGULAR_FSYNC();
   qint32 proc_SYNC_NARROW_FEILDS();
   qint32 proc_SWIR_GAIN_CONTROL();
   qint32 proc_LFR_RADIATION();
   qint32 proc_LFR_POWER_ON();
   qint32 proc_LFR_SET_FREQ();
   qint32 proc_ARR_SET();
   qint32 proc_STAB_EN();
   qint32 proc_PARK();
   qint32 proc_SET_DEF_POS();
   qint32 proc_SET_SPEED();
   qint32 proc_CHANGE_MODE();
   qint32 proc_SET_RELATIVE_ANGLE();
   qint32 proc_ARR_REMOVE();
   qint32 proc_SET_ABSOLUTE_ANGLE();
   qint32 proc_TRACKING_EN_SINGLE();
   qint32 proc_TRACKING_DIS_SINGLE();
   qint32 proc_TRACKING_CHS();
   qint32 proc_TRACKING_EN_4();
   qint32 proc_TRACKING_DIS();
   qint32 proc_ALIGNMENT();
   qint32 proc_AUTO_INSPECTION();

   qint32 proc_SET_SPEED_DEG();
   qint32 proc_SET_ABSOLUTE_ANGLE_DEG();
   qint32 proc_TURN_OFF_MOTORS();

   signals:
         void sig_VISCA_CMD(TVCamera::Command cmd, quint32 value);
         void sig_J200_CMD(J200_CMD_enum cmd, const quint32 par1 = 0, const quint32 par2 = 0);
         void sig_LDC_CMD(LDC_CMD_enum cmd, quint32 value);
         void sig_MINI640_CMD(MINI640_CMD_enum cmd, quint32 uvalue=0, float fvalue=0);

         void sig_Switch_Channel(quint32 channel, quint8 en);
         void sig_TrackingSetTargetObject(uint cx, uint cy, uint w, uint h);
         void sig_TrackingSetStopTracking();


         // void sig_setControlPosition( const float yaw, const float pitch);
         void sig_setDrivePositionMode( const bool zero, const bool pohod, const bool pilot, const bool park);
         void sig_setStabilization( const bool enable);
         void sig_turnOffMotors();
         void sig_setControlSpeed( const float z, const float x);
         void sig_setControlPositionRel( const float z, const float x);
         void sig_setControlPositionAbs( const float z, const float x);


public slots:
        qint32 proc_checkCmd(QByteArray ba);
        void proc_Thread_Finished();

};


#endif // CMD_PARSER_H
