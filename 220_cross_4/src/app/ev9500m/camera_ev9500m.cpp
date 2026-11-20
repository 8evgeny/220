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
    bIsBusy(false),
    errViscaCount(0)   // FIX: инициализация счётчика ошибок
{
    QObject::connect(&timer, &QTimer::timeout, this, &TVCamera::timeout);

    baudrateHash.insert(9600, Br9600);
    baudrateHash.insert(19200, Br19200);
    baudrateHash.insert(38400, Br38400);
    baudrateHash.insert(115200, Br115200);
}

TVCamera::~TVCamera()
{
    qDebug() << Q_FUNC_INFO;
    timer.stop();         // FIX: останавливаем таймер в деструкторе
    qDebug() << Q_FUNC_INFO << "exit success";
}

void TVCamera::timeout()
{
    if (!bIsConnected) return;

    // COMPLETION для команд движения ждём ограниченное время. Если камера
    // его не прислала — снимаем busy, иначе новые команды движения встанут.
    if (bIsBusy && m_busyTimer.elapsed() > TV_VISCA_COMPLETION_TIMEOUT_MS) {
        qWarning() << Q_FUNC_INFO << "VISCA completion timeout — reset busy";
        setBusy(false);
        ++errViscaCount;
    }
}
int TVCamera::connect(QString portName, quint32 baudrate)
{
    if (!baudrateHash.contains(baudrate)) {
        return VISCA_FAILURE;
    }

    int camera_num = 0;
    int err;

    // FIX: передаём РЕАЛЬНОЕ число (115200), а не termios-константу!
    if (VISCA_open_serial(&interface, portName.toStdString().c_str(), baudrate) != VISCA_SUCCESS) {
        qWarning() << "camera_ui: unable to open serial device" << portName;
        return VISCA_FAILURE;
    }

    m_portName = portName;  // сохраняем для ChangeBaudrate

    interface.broadcast = 0;
    err = VISCA_set_address(&interface, &camera_num);
    if (err != VISCA_SUCCESS) {
        qWarning() << Q_FUNC_INFO << "Invalid address";
        VISCA_close_serial(&interface);
        return VISCA_FAILURE;
    }

    camera.address = 1;
    err = VISCA_clear(&interface, &camera);

    err = VISCA_get_camera_info(&interface, &camera);
    if (err != VISCA_SUCCESS) {
        qWarning() << Q_FUNC_INFO << "Cannot get camera info";
        VISCA_close_serial(&interface);
        return VISCA_FAILURE;
    }

    if (camera.vendor != VISCA_VENDOR_SONY) {
        qWarning() << "Invalid Vendor:" << Qt::hex << camera.vendor;
        VISCA_close_serial(&interface);
        return VISCA_FAILURE;
    }

    qDebug() << "Camera connected successfully. Model:" << Qt::hex << camera.model;
    bIsConnected = true;

    // Синхронизируем режим фокуса с камерой: после включения питания он Auto.
    if (!getFocusMode())
        qWarning() << Q_FUNC_INFO << "Cannot read initial focus mode";

    timer.start(TV_VISCA_BUSY_POLL_INTERVAL_MS);

    return VISCA_SUCCESS;
}



void TVCamera::disconnect()
{
    timer.stop();
   // m_buff.clear();  // FIX: очищаем буфер при отключении (если есть доступ)
    if (VISCA_close_serial(&interface) != VISCA_SUCCESS) {
        qWarning() << "camera_ui: unable to close serial device";
    }

    bIsConnected = false;
}


/* ========================================================================
 * FIX #5: bIsBusy для speed-команд zoom/focus
 * ======================================================================== */
bool TVCamera::isMotionCommand(Command cmd)
{
    switch (cmd) {
    case ZoomTele:
    case ZoomWide:
    case ZoomPos:
    case FocusFar:
    case FocusNear:
    case FocusPos:
    case FocusOnePush:
        return true;
    default:
        return false;
    }
}

void TVCamera::setBusy(bool busy)
{
    bIsBusy = busy;
    if (busy) m_busyTimer.restart();
}

quint32 TVCamera::executeCommand(Command cmd, quint32 arg)
{
    quint32 err = VISCA_SUCCESS;

    // Пока объектив движется, новые команды движения не шлём;
    // опрос состояния и телеметрия при этом продолжают работать.
    if (isMotionCommand(cmd) && bIsBusy) {
        qWarning() << Q_FUNC_INFO << "VISCA busy — motion command ignored:" << cmd;
        return VISCA_FAILURE;
    }

    switch (cmd) {
    case TVCamera::FocusFar:
        qDebug() << Q_FUNC_INFO << "focus far";
        if (!ensureManualFocusMode()) {
            err = VISCA_FAILURE;
            break;
        }
        setBusy(true);                    // FIX: ставим busy
        err = VISCA_set_focus_far_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::FocusNear:
        qDebug() << Q_FUNC_INFO << "focus near";
        if (!ensureManualFocusMode()) {
            err = VISCA_FAILURE;
            break;
        }
        setBusy(true);                    // FIX: ставим busy
        err = VISCA_set_focus_near_speed(&interface, &camera, (quint8)arg);
        break;
    case TVCamera::FocusMode:
        // STATeless toggle 0x38 10 мешает "включить/выключить": повтор команды
        // возвращает прежний режим. Читаем фактический режим и ставим нужный.
        qDebug() << Q_FUNC_INFO << "focus mode toggle";
        err = toggleFocusMode() ? VISCA_SUCCESS : VISCA_FAILURE;
        break;
    case TVCamera::FocusAutoOn:
        qDebug() << Q_FUNC_INFO << "focus auto ON";
        err = setFocusModeAuto() ? VISCA_SUCCESS : VISCA_FAILURE;
        break;
    case TVCamera::FocusAutoOff:
        qDebug() << Q_FUNC_INFO << "focus auto OFF (manual)";
        err = setFocusModeManual() ? VISCA_SUCCESS : VISCA_FAILURE;
        break;
    case TVCamera::FocusOnePush:
        // One Push AF работает только из Manual (по VISCA EV9500M).
        qDebug() << Q_FUNC_INFO << "focus one push";
        if (!ensureManualFocusMode()) {
            err = VISCA_FAILURE;
            break;
        }
        err = VISCA_set_focus_one_push(&interface, &camera);
        if (err == VISCA_SUCCESS && interface.last_error != 0) err = VISCA_FAILURE;
        break;
    case TVCamera::AFModeNormal:
    case TVCamera::AFModeInterval:
    case TVCamera::AFModeZoomTrig:
        err = setAfMode(cmd == TVCamera::AFModeNormal   ? VISCA_AF_MODE_NORMAL_AF
                       : cmd == TVCamera::AFModeInterval ? VISCA_AF_MODE_INTERVAL_AF
                                                        : VISCA_AF_MODE_ZOOM_TRIG_AF)
                  ? VISCA_SUCCESS : VISCA_FAILURE;
        break;
    case TVCamera::FocusPos:
        qDebug() << Q_FUNC_INFO << "focus pos";
        if (!ensureManualFocusMode()) {
            err = VISCA_FAILURE;
            break;
        }
        setBusy(true);                    // FIX: позиционирование тоже busy
        err = VISCA_set_focus_value(&interface, &camera, (quint32)arg);
        // Distance-команда: завершение пришло вместе с Completion.
        setBusy(false);
        break;
    case TVCamera::FocusStop:
        setBusy(false);
        err = VISCA_set_focus_stop(&interface, &camera);
        break;
    case TVCamera::ZoomStop:
        setBusy(false);
        err = VISCA_set_zoom_stop(&interface, &camera);
        break;
    case TVCamera::ZoomTele:
        setBusy(true);                    // FIX: ставим busy
        qDebug() << Q_FUNC_INFO << "zoom tele";
        err = VISCA_set_zoom_tele_speed(&interface, &camera, (quint8)arg);
        if (err != VISCA_SUCCESS) setBusy(false);
        break;
    case TVCamera::ZoomWide:
        setBusy(true);                    // FIX: ставим busy
        qDebug() << Q_FUNC_INFO << "zoom wide";
        err = VISCA_set_zoom_wide_speed(&interface, &camera, (quint8)arg);
        if (err != VISCA_SUCCESS) setBusy(false);
        break;
    case TVCamera::ZoomPos:
        setBusy(true);
        qDebug() << Q_FUNC_INFO << "zoom pos" << arg;
        err = VISCA_set_zoom_value(&interface, &camera, (quint16)arg);
        setBusy(false);   // Completion уже получен — объектив на позиции
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
    case ChangeBaudrate: {
        qDebug() << Q_FUNC_INFO << "change baudrate" << arg;

        if (!baudrateRegHash.contains(arg)) {
            qWarning() << Q_FUNC_INFO << "Unsupported baudrate:" << arg;
            err = VISCA_FAILURE;
            break;
        }

        quint32 newBaudrate = arg;  // реальное число: 9600, 19200, 38400, 115200
        quint8 regVal = (quint8)baudrateRegHash.value(arg);

        err = VISCA_set_register(&interface, &camera, 0x00, regVal);
        if (err != VISCA_SUCCESS) {
            qWarning() << Q_FUNC_INFO << "Failed to set baudrate register";
            break;
        }

        // Закрываем и переоткрываем с НОВЫМ реальным числом
        VISCA_close_serial(&interface);

        if (VISCA_open_serial(&interface, m_portName.toStdString().c_str(), newBaudrate) != VISCA_SUCCESS) {
            qWarning() << Q_FUNC_INFO << "Failed to reopen port at" << newBaudrate;
            bIsConnected = false;
            err = VISCA_FAILURE;
            break;
        }

        // Верификация
        quint8 checkVal = 0xFF;
        err = VISCA_get_register(&interface, &camera, 0x00, &checkVal);
        if (err != VISCA_SUCCESS || checkVal != regVal) {
            qWarning() << Q_FUNC_INFO << "Verification failed. Got:" << checkVal << "expected:" << regVal;
            bIsConnected = false;
            err = VISCA_FAILURE;
            break;
        }

        qDebug() << Q_FUNC_INFO << "Baudrate changed successfully to" << newBaudrate;
        err = VISCA_SUCCESS;
        break;
    }

    /* ═══════════════════════════════════════════════════════════════════
     *  Расширенные команды EV9500M (табл. 6 протокола ГОЭН-220 v1.1)
     * ═══════════════════════════════════════════════════════════════════ */
    case IrisManual:
        err = VISCA_set_iris_value(&interface, &camera, arg);
        break;
    case ShutterManual:
        err = VISCA_set_shutter_value(&interface, &camera, arg);
        break;
    case GainManual:
        err = VISCA_set_gain_value(&interface, &camera, arg);
        break;

    case DZoomOn:
        err = VISCA_set_dzoom(&interface, &camera, VISCA_DZOOM_ON);
        break;
    case DZoomOff:
        err = VISCA_set_dzoom(&interface, &camera, VISCA_DZOOM_OFF);
        break;
    case DZoomLimit:
        err = VISCA_set_dzoom_limit(&interface, &camera, arg);
        break;
    case DZoomMode:
        err = VISCA_set_dzoom_mode(&interface, &camera, arg);
        break;

    case SpotFocusOn:
        err = VISCA_set_on_spot_focus(&interface, &camera);
        break;
    case SpotFocusOff:
        err = VISCA_set_off_spot_focus(&interface, &camera);
        break;
    case SpotFocusParam:
        err = VISCA_set_spot_focus_param(&interface, &camera,
                                         (arg >> 24) & 0xFF, (arg >> 16) & 0xFF,
                                         (arg >> 8) & 0xFF, arg & 0xFF);
        break;

    case CorrBlockIR:
        err = VISCA_set_standard_ir_correction(&interface, &camera);
        break;
    case CorrPassIR:
        err = VISCA_set_light_ir_correction(&interface, &camera);
        break;

    case WBMode:
        err = VISCA_set_whitebal_mode(&interface, &camera, arg);
        break;
    case WBTrigger:
        err = VISCA_set_whitebal_one_push(&interface, &camera);
        break;

    case SpotAWBOn:
        err = VISCA_set_spot_awb_on(&interface, &camera);
        break;
    case SpotAWBOff:
        err = VISCA_set_spot_awb_off(&interface, &camera);
        break;
    case SpotAWBPar:
        err = VISCA_set_spot_awb_position(&interface, &camera,
                                          (arg >> 24) & 0xFF, (arg >> 16) & 0xFF,
                                          (arg >> 8) & 0xFF, arg & 0xFF);
        break;
    case SpotAWBDispOn:
        err = VISCA_set_spot_awb_display(&interface, &camera, VISCA_ON_OFF_ON);
        break;
    case SpotAWBDispOff:
        err = VISCA_set_spot_awb_display(&interface, &camera, VISCA_ON_OFF_OFF);
        break;

    case SlowShutterAuto:
        err = VISCA_set_slow_shutter_auto(&interface, &camera, VISCA_ON_OFF_ON);
        break;
    case SlowShutterManual:
        err = VISCA_set_slow_shutter_auto(&interface, &camera, VISCA_ON_OFF_OFF);
        break;

    case FlickerReduction:
        err = (arg == EVCAM_MODE_ON)
                ? VISCA_set_flicker_reduction_on(&interface, &camera)
                : VISCA_set_flicker_reduction_off(&interface, &camera);
        break;
    case BackLight:
        err = (arg == EVCAM_MODE_ON)
                ? VISCA_set_backlight_on(&interface, &camera)
                : VISCA_set_backlight_off(&interface, &camera);
        break;
    case GammaValue:
        // Протокол: 1=Standard, 2=Straight; VISCA: 0=Standard, 1=Straight.
        err = VISCA_set_gamma(&interface, &camera,
                              (arg == EVCAM_MODE_ON) ? VISCA_GAMMA_MODE_STRAIGHT
                                                     : VISCA_GAMMA_MODE_STANDARD);
        break;
    case GammaOffset:
        err = VISCA_set_gamma_offset_value(&interface, &camera, static_cast<int8_t>(arg));
        break;

    case ExpCompReset:
        err = VISCA_set_exp_comp_reset(&interface, &camera);
        break;
    case ExpCompUp:
        err = VISCA_set_ex_exp_comp_up(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case ExpCompDown:
        err = VISCA_set_ex_exp_comp_down(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case ExpCompDirect:
        err = VISCA_set_ex_exp_comp_value(&interface, &camera, arg);
        break;

    case StabilizerMode:
        err = VISCA_set_stabilizer(&interface, &camera,
                                   arg == EVCAM_MODE_OFF       ? VISCA_STABILIZER_MODE_OFF
                                   : arg == EVCAM_STABILIZER_HOLD ? VISCA_STABILIZER_MODE_HOLD
                                                                 : VISCA_STABILIZER_MODE_ON);
        break;
    case StabilizerLevel:
        err = VISCA_set_stabilizer_level(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case HighSensitivity:
        err = (arg == EVCAM_MODE_ON)
                ? VISCA_set_on_highsens(&interface, &camera)
                : VISCA_set_off_highsens(&interface, &camera);
        break;
    case WideDMode:
        err = (arg == EVCAM_MODE_ON)
                ? VISCA_set_wd_on(&interface, &camera)
                : VISCA_set_ve_off(&interface, &camera);
        break;
    case WideDParams:
        err = VISCA_set_ve_params(&interface, &camera,
                                  static_cast<uint8_t>(arg & 0xFF),
                                  static_cast<uint8_t>((arg >> 8) & 0xFF),
                                  static_cast<uint8_t>((arg >> 16) & 0xFF));
        break;

    case NrLevel:
        err = VISCA_set_nr(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case Nr2D3D:
        err = VISCA_set_23dnr(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case HlcParams:
        err = VISCA_set_hlc(&interface, &camera,
                            static_cast<uint8_t>(arg & 0xFF),
                            static_cast<uint8_t>((arg >> 8) & 0xFF));
        break;

    case IcrMode:
        // CAM_ICR, а не коррекция ИК-подсветки: 8x 01 04 01 02/03 FF.
        err = (arg == EVCAM_MODE_OFF)
                ? VISCA_set_zero_lux_shot(&interface, &camera, VISCA_ON_OFF_OFF)
                : VISCA_set_zero_lux_shot(&interface, &camera, VISCA_ON_OFF_ON);
        break;
    case AutoIcrMode:
        err = VISCA_set_auto_icr(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case AutoIcrThreshold:
        err = VISCA_set_auto_icr_threshold(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case MinShutterMode:
        err = (arg == EVCAM_MODE_ON)
                ? VISCA_set_on_min_shutter(&interface, &camera)
                : VISCA_set_off_min_shutter(&interface, &camera);
        break;
    case MinShutterLimit:
        err = VISCA_set_shutter_limit(&interface, &camera, static_cast<uint8_t>(arg));
        break;
    case PictureFlip:
        err = VISCA_set_picture_flip(&interface, &camera,
                                     (arg == EVCAM_MODE_ON) ? VISCA_ON_OFF_ON
                                                            : VISCA_ON_OFF_OFF);
        break;

    default:
        err = VISCA_SUCCESS;
        break;
    }

    // Ошибки камеры (Syntax Error / Command Not Executable / Buffer Full)
    // приходят в last_error и больше не маскируются под успех.
    if (err == VISCA_SUCCESS && interface.last_error != 0) {
        qWarning() << Q_FUNC_INFO << "VISCA command rejected, code"
                   << Qt::hex << interface.last_error;
        err = VISCA_FAILURE;
    }
    if (err != VISCA_SUCCESS) ++errViscaCount;

    return err;
}


/* ========================================================================
 *  Режим фокуса FCB-EV9500M по VISCA
 *  Auto    = 8x 01 04 38 02 FF (АФ включён)
 *  Manual  = 8x 01 04 38 03 FF (АФ выключен)
 *  Toggle  = 8x 01 04 38 10 FF (не задаёт состояние!)
 *  Ошибки камеры (например 90 6y 41 FF — Command Not Executable) приходят
 *  в last_error и больше не маскируются под успех.
 * ======================================================================== */
bool TVCamera::setFocusModeAuto()
{
    if (!bIsConnected) return false;

    const uint32_t err = VISCA_set_focus_mode_auto(&interface, &camera);
    if (err != VISCA_SUCCESS || interface.last_error != 0) {
        qWarning() << Q_FUNC_INFO << "VISCA error" << err
                   << "code" << Qt::hex << interface.last_error;
        return false;
    }
    m_desiredFocusMode = VISCA_FOCUS_AUTO_ON;
    m_status.focus_mode = VISCA_FOCUS_AUTO_ON;
    return true;
}

bool TVCamera::setFocusModeManual()
{
    if (!bIsConnected) return false;

    const uint32_t err = VISCA_set_focus_mode_manual(&interface, &camera);
    if (err != VISCA_SUCCESS || interface.last_error != 0) {
        qWarning() << Q_FUNC_INFO << "VISCA error" << err
                   << "code" << Qt::hex << interface.last_error;
        return false;
    }
    m_desiredFocusMode = VISCA_FOCUS_AUTO_OFF;
    m_status.focus_mode = VISCA_FOCUS_AUTO_OFF;
    return true;
}

bool TVCamera::getFocusMode()
{
    if (!bIsConnected) return false;

    quint8 mode = 0;
    const uint32_t err = VISCA_get_focus_mode(&interface, &camera, &mode);
    if (err != VISCA_SUCCESS || interface.last_error != 0) return false;
    if (mode != VISCA_FOCUS_AUTO_ON && mode != VISCA_FOCUS_AUTO_OFF) return false;

    m_status.focus_mode = mode;
    m_desiredFocusMode = mode;
    return true;
}

bool TVCamera::toggleFocusMode()
{
    if (!bIsConnected) return false;

    // Читаем фактический режим: аппаратный toggle 0x10 не годится для
    // команды "включить/выключить", т.к. повтор инвертирует состояние.
    if (getFocusMode()) {
        return (m_status.focus_mode == VISCA_FOCUS_AUTO_ON)
                   ? setFocusModeManual()
                   : setFocusModeAuto();
    }

    // Inquiry недоступна — используем аппаратный toggle как fallback.
    const uint32_t err = VISCA_set_focus_mode_toggle(&interface, &camera);
    if (err != VISCA_SUCCESS || interface.last_error != 0) {
        qWarning() << Q_FUNC_INFO << "VISCA error" << err
                   << "code" << Qt::hex << interface.last_error;
        return false;
    }
    m_desiredFocusMode = TV_FOCUS_MODE_UNKNOWN;   // после toggle режим неизвестен
    return true;
}

bool TVCamera::setAfMode(quint8 mode)
{
    if (!bIsConnected) return false;

    uint32_t err = VISCA_FAILURE;
    switch (mode) {
    case VISCA_AF_MODE_NORMAL_AF:     err = VISCA_set_normal_af_mode(&interface, &camera); break;
    case VISCA_AF_MODE_INTERVAL_AF:   err = VISCA_set_interval_af_mode(&interface, &camera); break;
    case VISCA_AF_MODE_ZOOM_TRIG_AF:  err = VISCA_set_zoom_trig_af_mode(&interface, &camera); break;
    default:
        qWarning() << Q_FUNC_INFO << "Unsupported AF mode" << mode;
        return false;
    }
    if (err != VISCA_SUCCESS || interface.last_error != 0) {
        qWarning() << Q_FUNC_INFO << "VISCA error" << err
                   << "code" << Qt::hex << interface.last_error;
        return false;
    }
    return true;
}

bool TVCamera::ensureManualFocusMode()
{
    if (!bIsConnected) return false;

    // Сверяемся с камерой каждый раз: после цикла питания она возвращается
    // в Auto (Initial Setting), а сохранённое состояние может быть устаревшим.
    if (!getFocusMode()) return false;
    if (m_status.focus_mode == VISCA_FOCUS_AUTO_OFF) return true;
    return setFocusModeManual();
}


void TVCamera::process()
{
    qDebug() << "thread started";
}


void TVCamera::setBrightness(quint32 value)
{
    VISCA_set_bright_value(&interface, &camera, value);
}

void TVCamera::getBrightness()
{
    if (bIsBusy) return;
    uint16_t value = 0;
    VISCA_get_bright_value(&interface, &camera, &value);
    qDebug() << Q_FUNC_INFO << value;
}


/* ========================================================================
 * FIX #7: Корректная семантика bool (VISCA_SUCCESS=0 → true)
 * ======================================================================== */

void TVCamera::setVideoFormat(quint8 format)
{
    if (bIsBusy) return;
    quint8 val = 0;
    VISCA_get_register(&interface, &camera, 0x60, &val);
    qDebug() << Q_FUNC_INFO << "reg_val=" << val;

    VISCA_get_register(&interface, &camera, 72, &val);
    qDebug() << Q_FUNC_INFO << "rate_val=" << val;
}

bool TVCamera::getIris()
{
    quint16 value = 0;
    uint32_t res = VISCA_get_iris_value(&interface, &camera, &value);
    m_status.iris = value;
    return (res == VISCA_SUCCESS);   // FIX: true = успех
}

bool TVCamera::getTemperature()
{
    uint32_t res = VISCA_get_temperature(&interface, &camera, &m_status.temperature);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::getShutter()
{
    if (bIsBusy) return false;
    uint32_t res = VISCA_get_shutter_value(&interface, &camera, &m_status.shutter);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::getGain()
{
    uint32_t res = VISCA_get_gain_value(&interface, &camera, &m_status.gain);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::getContrast()
{
    uint32_t res = VISCA_get_contrast(&interface, &camera, &m_status.contrast);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::setZoomValue(quint32 value)
{
    uint32_t res = VISCA_set_zoom_value(&interface, &camera, value);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::setFocusValue(quint32 value)
{
    uint32_t res = VISCA_set_focus_value(&interface, &camera, value);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::getFocusValue()
{
    uint32_t res = VISCA_get_focus_value(&interface, &camera, &m_status.focus_val);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::getZoomValue()
{
    uint32_t res = VISCA_get_zoom_value(&interface, &camera, &m_status.zoom_val);
    return (res == VISCA_SUCCESS);   // FIX
}

bool TVCamera::getAWBDisplayMode()
{
    uint32_t res = VISCA_get_spot_awb_display_mode(&interface, &camera, &m_status.awb_display_mode);
    return (res == VISCA_SUCCESS);   // FIX
}
