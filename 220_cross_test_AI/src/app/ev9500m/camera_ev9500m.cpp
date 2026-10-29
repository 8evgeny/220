#include "camera_ev9500m.h"
#include <QDebug>
#include <QThread>

const QHash<quint32, quint32> baudrateRegHash{{9600, 0}, {19200, 1}, {38400, 2}, {115200, 3}};

TVCamera::TVCamera(QObject *parent)
    : QObject{parent}, bIsConnected(false), bIsBusy(false),
      errViscaCount(0) // FIX: инициализация счётчика ошибок
{

    QObject::connect(&timer, &QTimer::timeout, this, &TVCamera::timeout);

    baudrateHash.insert(9600, Br9600);
    baudrateHash.insert(19200, Br19200);
    baudrateHash.insert(38400, Br38400);
    baudrateHash.insert(115200, Br115200);
} /* TVCamera::TVCamera */

TVCamera::~TVCamera()
{
    qDebug() << Q_FUNC_INFO;
    timer.stop(); // FIX: останавливаем таймер в деструкторе
    qDebug() << Q_FUNC_INFO << "exit success";
} /* TVCamera::~TVCamera */

void TVCamera::timeout()
{
} /* TVCamera::timeout */
int TVCamera::connect(QString portName, quint32 baudrate)
{
    if (!baudrateHash.contains(baudrate))
    {
        return VISCA_FAILURE;
    } /* if */

    int camera_num = 0;
    int err;

    // FIX: передаём РЕАЛЬНОЕ число (115200), а не termios-константу!
    if (VISCA_open_serial(&interface, portName.toStdString().c_str(), baudrate) != VISCA_SUCCESS)
    {
        qWarning() << "camera_ui: unable to open serial device" << portName;
        return VISCA_FAILURE;
    } /* if */

    m_portName = portName; // сохраняем для ChangeBaudrate

    interface.broadcast = 0;
    err = VISCA_set_address(&interface, &camera_num);
    if (err != VISCA_SUCCESS)
    {
        qWarning() << Q_FUNC_INFO << "Invalid address";
        VISCA_close_serial(&interface);
        return VISCA_FAILURE;
    } /* if */

    camera.address = 1;
    err = VISCA_clear(&interface, &camera);

    err = VISCA_get_camera_info(&interface, &camera);
    if (err != VISCA_SUCCESS)
    {
        qWarning() << Q_FUNC_INFO << "Cannot get camera info";
        VISCA_close_serial(&interface);
        return VISCA_FAILURE;
    } /* if */

    if (camera.vendor != VISCA_VENDOR_SONY)
    {
        qWarning() << "Invalid Vendor:" << Qt::hex << camera.vendor;
        VISCA_close_serial(&interface);
        return VISCA_FAILURE;
    } /* if */

    qDebug() << "Camera connected successfully. Model:" << Qt::hex << camera.model;
    bIsConnected = true;
    timer.start();

    return VISCA_SUCCESS;
} /* TVCamera::connect */

void TVCamera::disconnect()
{
    timer.stop();
    // m_buff.clear();  // FIX: очищаем буфер при отключении (если есть доступ)
    if (VISCA_close_serial(&interface) != VISCA_SUCCESS)
    {
        qWarning() << "camera_ui: unable to close serial device";
    } /* if */

    bIsConnected = false;
} /* TVCamera::disconnect */

/* ========================================================================
 * FIX #5: bIsBusy для speed-команд zoom/focus
 * ======================================================================== */
quint32 TVCamera::executeCommand(Command cmd, quint32 arg)
{
    quint32 err;
    switch (cmd)
    {
    case TVCamera::FocusFar:
        bIsBusy = true; // FIX: ставим busy
        qDebug() << Q_FUNC_INFO << "focus far";
        err = VISCA_set_focus_far_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::FocusNear:
        bIsBusy = true; // FIX: ставим busy
        qDebug() << Q_FUNC_INFO << "focus near";
        err = VISCA_set_focus_near_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::FocusMode:
        qDebug() << Q_FUNC_INFO << "focus mode";
        err = VISCA_set_focus_auto(&interface, &camera, VISCA_FOCUS_CHANGE);
        break;
    case TVCamera::FocusPos:
        bIsBusy = true; // FIX: позиционирование тоже busy
        qDebug() << Q_FUNC_INFO << "focus pos";
        err = VISCA_set_focus_value(&interface, &camera, (quint32)arg);
        if (err != VISCA_SUCCESS)
            bIsBusy = false; // FIX: сброс при ошибке
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
        bIsBusy = true; // FIX: ставим busy
        qDebug() << Q_FUNC_INFO << "zoom tele";
        err = VISCA_set_zoom_tele_speed(&interface, &camera, (quint8)arg);
        if (err != VISCA_SUCCESS)
            bIsBusy = false;
        break;
    case TVCamera::ZoomWide:
        bIsBusy = true; // FIX: ставим busy
        qDebug() << Q_FUNC_INFO << "zoom wide";
        err = VISCA_set_zoom_wide_speed(&interface, &camera, (quint8)arg);
        if (err != VISCA_SUCCESS)
            bIsBusy = false;
        break;
    case TVCamera::ZoomPos:
        bIsBusy = true;
        qDebug() << Q_FUNC_INFO << "zoom pos" << arg;
        err = VISCA_set_zoom_value(&interface, &camera, (quint16)arg);
        if (err != VISCA_SUCCESS)
            bIsBusy = false; // FIX: сброс при ошибке
        break;

    case AutoExpManual:
        err = VISCA_set_auto_exp_mode(&interface, &camera, VISCA_AUTO_EXP_MANUAL);
        break;
    case AutoExpAuto:
        err = VISCA_set_auto_exp_mode(&interface, &camera, VISCA_AUTO_EXP_FULL_AUTO);
        break;
    case IrisOpen:
        qDebug() << Q_FUNC_INFO << "iris up";
        err = VISCA_set_iris_up(&interface, &camera);
        break;
    case IrisClose:
        qDebug() << Q_FUNC_INFO << "iris down";
        err = VISCA_set_iris_down(&interface, &camera);
        break;
    case ShutterUp:
        qDebug() << Q_FUNC_INFO << "shutter up";
        err = VISCA_set_shutter_up(&interface, &camera);
        break;
    case ShutterDown:
        qDebug() << Q_FUNC_INFO << "shutter down";
        err = VISCA_set_shutter_down(&interface, &camera);
        break;
    case GainUp:
        qDebug() << Q_FUNC_INFO << "gain up";
        err = VISCA_set_gain_up(&interface, &camera);
        break;
    case GainDown:
        qDebug() << Q_FUNC_INFO << "gain down";
        err = VISCA_set_gain_down(&interface, &camera);
        break;
    case ContrastValue:
        qDebug() << Q_FUNC_INFO << "contrast value";
        err = VISCA_set_contrast_value(&interface, &camera, arg);
        break;
    case DefogOn:
        qDebug() << Q_FUNC_INFO << "defog on. lvl:" << arg;
        err = VISCA_set_on_defog_level(&interface, &camera, arg);
        break;
    case DefogOff:
        qDebug() << Q_FUNC_INFO << "defog off";
        err = VISCA_set_off_defog(&interface, &camera);
        break;
    case SetNR:
        qDebug() << Q_FUNC_INFO << "NR. lvl:" << arg;
        err = VISCA_set_nr(&interface, &camera, arg);
        break;
    case Set23DNR:
        qDebug() << Q_FUNC_INFO << "23 DNR. lvl:" << arg;
        err = VISCA_set_23dnr(&interface, &camera, arg);
        break;

    /* ====================================================================
     * FIX #6: ChangeBaudrate — закрыть порт, сменить регистр, переоткрыть
     * ==================================================================== */
    case ChangeBaudrate:
    {
        qDebug() << Q_FUNC_INFO << "change baudrate" << arg;

        if (!baudrateRegHash.contains(arg))
        {
            qWarning() << Q_FUNC_INFO << "Unsupported baudrate:" << arg;
            err = VISCA_FAILURE;
            break;
        } /* if */

        quint32 newBaudrate = arg; // реальное число: 9600, 19200, 38400, 115200
        quint8 regVal = (quint8)baudrateRegHash.value(arg);

        err = VISCA_set_register(&interface, &camera, 0x00, regVal);
        if (err != VISCA_SUCCESS)
        {
            qWarning() << Q_FUNC_INFO << "Failed to set baudrate register";
            break;
        } /* if */

        // Закрываем и переоткрываем с НОВЫМ реальным числом
        VISCA_close_serial(&interface);

        if (VISCA_open_serial(&interface, m_portName.toStdString().c_str(), newBaudrate) !=
            VISCA_SUCCESS)
        {
            qWarning() << Q_FUNC_INFO << "Failed to reopen port at" << newBaudrate;
            bIsConnected = false;
            err = VISCA_FAILURE;
            break;
        } /* if */

        // Верификация
        quint8 checkVal = 0xFF;
        err = VISCA_get_register(&interface, &camera, 0x00, &checkVal);
        if (err != VISCA_SUCCESS || checkVal != regVal)
        {
            qWarning() << Q_FUNC_INFO << "Verification failed. Got:" << checkVal
                       << "expected:" << regVal;
            bIsConnected = false;
            err = VISCA_FAILURE;
            break;
        } /* if */

        qDebug() << Q_FUNC_INFO << "Baudrate changed successfully to" << newBaudrate;
        err = VISCA_SUCCESS;
        break;
    } /* case */

    default:
        err = VISCA_SUCCESS;
        break;
    } /* switch */

    return err;
} /* TVCamera::executeCommand */

void TVCamera::process()
{
    qDebug() << "thread started";
} /* TVCamera::process */

void TVCamera::setBrightness(quint32 value)
{
    VISCA_set_bright_value(&interface, &camera, value);
} /* TVCamera::setBrightness */

void TVCamera::getBrightness()
{
    if (bIsBusy)
        return;
    uint16_t value = 0;
    VISCA_get_bright_value(&interface, &camera, &value);
    qDebug() << Q_FUNC_INFO << value;
} /* TVCamera::getBrightness */

/* ========================================================================
 * FIX #7: Корректная семантика bool (VISCA_SUCCESS=0 → true)
 * ======================================================================== */

void TVCamera::setVideoFormat(quint8 format)
{
    if (bIsBusy)
        return;
    quint8 val = 0;
    VISCA_get_register(&interface, &camera, 0x60, &val);
    qDebug() << Q_FUNC_INFO << "reg_val=" << val;

    VISCA_get_register(&interface, &camera, 72, &val);
    qDebug() << Q_FUNC_INFO << "rate_val=" << val;
} /* TVCamera::setVideoFormat */

bool TVCamera::getIris()
{
    quint16 value = 0;
    uint32_t res = VISCA_get_iris_value(&interface, &camera, &value);
    m_status.iris = value;
    return (res == VISCA_SUCCESS); // FIX: true = успех
} /* TVCamera::getIris */

bool TVCamera::getTemperature()
{
    uint32_t res = VISCA_get_temperature(&interface, &camera, &m_status.temperature);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::getTemperature */

bool TVCamera::getShutter()
{
    if (bIsBusy)
        return false;
    uint32_t res = VISCA_get_shutter_value(&interface, &camera, &m_status.shutter);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::getShutter */

bool TVCamera::getGain()
{
    uint32_t res = VISCA_get_gain_value(&interface, &camera, &m_status.gain);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::getGain */

bool TVCamera::getContrast()
{
    uint32_t res = VISCA_get_contrast(&interface, &camera, &m_status.contrast);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::getContrast */

bool TVCamera::setZoomValue(quint32 value)
{
    uint32_t res = VISCA_set_zoom_value(&interface, &camera, value);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::setZoomValue */

bool TVCamera::setFocusValue(quint32 value)
{
    uint32_t res = VISCA_set_focus_value(&interface, &camera, value);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::setFocusValue */

bool TVCamera::getFocusValue()
{
    uint32_t res = VISCA_get_focus_value(&interface, &camera, &m_status.focus_val);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::getFocusValue */

bool TVCamera::getZoomValue()
{
    uint32_t res = VISCA_get_zoom_value(&interface, &camera, &m_status.zoom_val);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::getZoomValue */

bool TVCamera::getAWBDisplayMode()
{
    uint32_t res = VISCA_get_spot_awb_display_mode(&interface, &camera, &m_status.awb_display_mode);
    return (res == VISCA_SUCCESS); // FIX
} /* TVCamera::getAWBDisplayMode */
