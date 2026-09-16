#include "camera_ev9500m.h"
#include <QDebug>
#include <QThread>

const QHash<quint32, quint32> baudrateRegHash {
    {9600, 0},
    {19200, 1},
    {38400, 2},
    {115200, 3}
};

TVCamera::TVCamera(QObject *parent)
    : QObject{parent},
    bIsConnected(false),
    bIsBusy(false)
{
    QObject::connect(&timer, &QTimer::timeout, this, &TVCamera::timeout);

    // Init baudrate hash
    baudrateHash.insert(9600, Br9600);
    baudrateHash.insert(19200, Br19200);
    baudrateHash.insert(38400, Br38400);
    baudrateHash.insert(115200, Br115200);
}

TVCamera::~TVCamera()
{
    qDebug() << Q_FUNC_INFO;
    timer.stop();
    qDebug() << Q_FUNC_INFO<<"exit success";

}

void TVCamera::timeout()
{
 //   qDebug()<<Q_FUNC_INFO;
}

int TVCamera::connect(QString portName, quint32 baudrate)
{
    // qDebug()<<Q_FUNC_INFO<<QThread::currentThread() << " portName TV: "<<portName<<"baudrate TV:"<< baudrate ;
    if (!baudrateHash.contains(baudrate)) {
        return VISCA_FAILURE;
    }
    Baudrate rate = baudrateHash.value(baudrate);

    int camera_num;
    int err;

    if (VISCA_open_serial(&interface, portName.toStdString().c_str(), rate)!=VISCA_SUCCESS) {
        qDebug()<<"camera_ui: unable to open serial device %s\n"<<portName.toStdString().c_str();
        return VISCA_FAILURE;
    }

    interface.broadcast=0;
    err = VISCA_set_address(&interface, &camera_num);
    if (err != VISCA_SUCCESS) {
        qWarning() << Q_FUNC_INFO << "Invalid address";
        return VISCA_FAILURE;
    }
    camera.address=1;
    err = VISCA_clear(&interface, &camera);
    err = VISCA_get_camera_info(&interface, &camera);

    if (camera.vendor != VISCA_VENDOR_SONY) {
        qWarning() << "Invalid Vendor";
        return VISCA_FAILURE;
    }

    qDebug()<<"Camera connected successfully";
    bIsConnected = true;
    timer.start();

    return VISCA_SUCCESS;
}

void TVCamera::disconnect()
{
  //  qDebug()<<Q_FUNC_INFO<<QThread::currentThread();
    int camera_num;
    int err;
    timer.stop();
    if (VISCA_close_serial(&interface) !=VISCA_SUCCESS) {
        fprintf(stderr,"camera_ui: unable to close serial device\n");
        exit(1);
    }

    bIsConnected = false;
}

quint32 TVCamera::executeCommand(Command cmd, quint32 arg)
{
    quint32 err;
    switch (cmd) {
    case TVCamera::FocusFar:
       // bIsBusy = true;
        qDebug()<<Q_FUNC_INFO<<"focus far";
        err = VISCA_set_focus_far_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::FocusNear:
        //bIsBusy = true;
        qDebug()<<Q_FUNC_INFO<<"focus near";
        err = VISCA_set_focus_near_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::FocusMode:
        qDebug()<<Q_FUNC_INFO<<"focus mode";
        err = VISCA_set_focus_auto(&interface, &camera, VISCA_FOCUS_CHANGE);  // Change focus mode to auto/manual
        break;
    case TVCamera::FocusPos:
        qDebug()<<Q_FUNC_INFO<<"focus pos";
        err = VISCA_set_focus_value(&interface, &camera, (quint32)arg);
        break;
    case TVCamera::FocusStop:
        bIsBusy = false;
        err = VISCA_set_focus_stop(&interface, &camera);
        break;
    case TVCamera::ZoomStop:
        bIsBusy = false;
        err = VISCA_set_zoom_stop(&interface, &camera);
        break;
    case TVCamera::ZoomTele:
        //bIsBusy = true;
        qDebug()<<Q_FUNC_INFO<<"zoom tele";
        err = VISCA_set_zoom_tele_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::ZoomWide:
        //bIsBusy = true;
        qDebug()<<Q_FUNC_INFO<<"zoom wide";
        err = VISCA_set_zoom_wide_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::ZoomPos:
        bIsBusy = true;
        qDebug()<<Q_FUNC_INFO<<"zoom pos" << arg;

             err = VISCA_set_zoom_value(&interface, &camera, (quint16)arg);
        bIsBusy = false;
         //err =VISCA_set_zoom_and_focus_value(&interface, &camera, (quint32)arg, 0);
        break;
    case AutoExpManual:
        err = VISCA_set_auto_exp_mode(&interface, &camera, VISCA_AUTO_EXP_MANUAL);
        break;
    case AutoExpAuto:
        err = VISCA_set_auto_exp_mode(&interface, &camera, VISCA_AUTO_EXP_FULL_AUTO);
        break;
    case IrisOpen:
        qDebug()<<Q_FUNC_INFO<<"iris up";
        err = VISCA_set_iris_up(&interface, &camera);
        break;
    case IrisClose:
        qDebug()<<Q_FUNC_INFO<<"iris down";
        err = VISCA_set_iris_down(&interface, &camera);
        break;
    case ShutterUp:
        qDebug()<<Q_FUNC_INFO<<"shutter up";
        err = VISCA_set_shutter_up(&interface, &camera);
        break;
    case ShutterDown:
        qDebug()<<Q_FUNC_INFO<<"shutter down";
        err = VISCA_set_shutter_down(&interface, &camera);
        break;
    case GainUp:
        qDebug()<<Q_FUNC_INFO<<"gain up";
        err = VISCA_set_gain_up(&interface, &camera);
        break;
    case GainDown:
        qDebug()<<Q_FUNC_INFO<<"gain down";
        err = VISCA_set_gain_down(&interface, &camera);
        break;
    case ContrastValue:
        qDebug()<<Q_FUNC_INFO<<"contrast value";
        err = VISCA_set_contrast_value(&interface, &camera, arg);
        break;
    case DefogOn:
        qDebug()<<Q_FUNC_INFO<<"defog on. lvl:"<<arg;
        err = VISCA_set_on_defog_level(&interface, &camera, arg);
        break;
    case DefogOff:
        qDebug()<<Q_FUNC_INFO<<"defog off";
        err = VISCA_set_off_defog(&interface, &camera);
        break;
    case SetNR:
        qDebug()<<Q_FUNC_INFO<<"NR. lvl:"<< arg;
        err = VISCA_set_nr(&interface, &camera,arg);
        break;
    case Set23DNR:
        qDebug()<<Q_FUNC_INFO<<"23 DNR. lvl:"<< arg;
        err = VISCA_set_23dnr(&interface, &camera,arg);
        break;
    case ChangeBaudrate:
        qDebug()<<Q_FUNC_INFO<<"change baudrate"<<arg;

        if (!baudrateRegHash.contains(arg)) {
            break;
        }
       // quint32 regVal = baudrateRegHash.value(arg);

        quint8 val;
        err = VISCA_get_register(&interface, &camera, 0x00, &val);
        qDebug()<<Q_FUNC_INFO<<"reg_val="<<val;
        err = VISCA_set_register(&interface, &camera, 0x00, baudrateRegHash.value(arg));

        err = VISCA_get_register(&interface, &camera, 0x00, &val);
        qDebug()<<Q_FUNC_INFO<<"reg_val="<<val;
        break;

    default:
        err = VISCA_SUCCESS;
        break;
    }

    return err;
}

void TVCamera::process()
{
    qDebug()<<"thread started";
}


void TVCamera::setBrightness(quint32 value)
{
    VISCA_set_bright_value(&interface, &camera, value);
}



void TVCamera::getBrightness()
{
    if(bIsBusy)
        return;
    uint16_t value;
    VISCA_get_bright_value(&interface, &camera, &value);
    qDebug()<<Q_FUNC_INFO<<value;
}

void TVCamera::setVideoFormat(quint8 format)
{
    if(bIsBusy)
        return;
    quint8 val;
    VISCA_get_register(&interface, &camera, 0x60, &val);
    qDebug()<<Q_FUNC_INFO<<"reg_val="<<val;
   //VISCA_set_register(&interface, &camera, 0x60, 1);
   //VISCA_get_register(&interface, &camera, 0x60, &val);
   //qDebug()<<Q_FUNC_INFO<<"reg_val="<<val;

    VISCA_get_register(&interface, &camera, 72, &val);
    qDebug()<<Q_FUNC_INFO<<"rate_val="<<val;
}

bool TVCamera::getIris()
{

    quint16 value;

    bool res = VISCA_get_iris_value(&interface, &camera, &value);
    m_status.iris = value;
    return res;
}

bool TVCamera::getTemperature()
{
    return VISCA_get_temperature(&interface, &camera, &m_status.temperature);
}

bool TVCamera::getShutter()
{
    if(bIsBusy)
        return false;
    return VISCA_get_shutter_value(&interface, &camera, &m_status.shutter);
}

bool TVCamera::getGain()
{

    return VISCA_get_gain_value(&interface, &camera, &m_status.gain);
}

bool TVCamera::getContrast()
{

    return VISCA_get_contrast(&interface, &camera, &m_status.contrast);
}

bool TVCamera::setZoomValue(quint32 value)
{
     return VISCA_set_zoom_value(&interface, &camera, value);
}

bool TVCamera::setFocusValue(quint32 value)
{

    return VISCA_set_focus_value(&interface, &camera, value);
}

bool TVCamera::getFocusValue()
{

    return VISCA_get_focus_value(&interface, &camera, &m_status.focus_val);
}

bool TVCamera::getZoomValue()
{

    return VISCA_get_zoom_value(&interface, &camera, &m_status.zoom_val);
}


bool TVCamera::getAWBDisplayMode()
{

    return VISCA_get_spot_awb_display_mode(&interface, &camera, &m_status.awb_display_mode);
}


// void TVCamera::getZoomFocusValue()
// {
//     VISCA_get__value(&interface, &camera, &m_status.zoom_val, &m_status.focus_val);
// }
