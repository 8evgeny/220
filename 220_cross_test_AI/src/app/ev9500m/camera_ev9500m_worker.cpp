#include "camera_ev9500m_worker.h"
#include <QThread>

Camera_EV9500M_Worker::Camera_EV9500M_Worker()
{
} /* Camera_EV9500M_Worker::Camera_EV9500M_Worker */

Camera_EV9500M_Worker::~Camera_EV9500M_Worker()
{
    qDebug() << Q_FUNC_INFO;

    if (m_timer)
    {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    } /* if */

    thread.quit(); // FIX: quit() вместо exit() — корректный останов event loop
    thread.wait(3000); // FIX: с таймаутом, чтобы не зависнуть навсегда

    qDebug() << Q_FUNC_INFO << "exit";
} /* Camera_EV9500M_Worker::~Camera_EV9500M_Worker */

void Camera_EV9500M_Worker::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;
} /* Camera_EV9500M_Worker::proc_Thread_Finished */

/* ========================================================================
 * FIX #8: cameraInit — защита от повторного вызова + корректное создание таймера
 * ======================================================================== */
void Camera_EV9500M_Worker::cameraInit()
{
    if (m_timer)
    {
        qWarning() << Q_FUNC_INFO << "Already initialized";
        return; // FIX: защита от повторного вызова
    } /* if */

    QObject::connect(
        &thread, &QThread::started, this,
        [this]()
        {
            qDebug() << Q_FUNC_INFO << "Camera VISCA thread id:" << QThread::currentThreadId();
            m_timer = new QTimer(this); // Создаём в worker-потоке (parent = this)

            connect(m_timer, &QTimer::timeout, this, &Camera_EV9500M_Worker::timeout);

        } /* lambda */,
        Qt::QueuedConnection);

    QObject::connect(&thread, &QThread::finished, this,
                     &Camera_EV9500M_Worker::proc_Thread_Finished);

    thread.start();
} /* Camera_EV9500M_Worker::cameraInit */

/* ========================================================================
 * FIX #9: proc_VISCA_CONNECT — ограничение попыток + очистка буфера
 * ======================================================================== */
void Camera_EV9500M_Worker::proc_VISCA_CONNECT(QString portName, quint32 baudrate)
{
    qDebug() << Q_FUNC_INFO << portName << baudrate;

    if (m_isConnected)
    {
        m_cam.disconnect();
        m_isConnected = false;
    } /* if */

    const int MAX_RETRIES = 10;
    int retries = 0;
    bool connected = false;

    while (retries < MAX_RETRIES)
    {
        if (m_cam.connect(portName, baudrate) == VISCA_SUCCESS)
        {
            connected = true;
            break;
        } /* if */
        m_cam.disconnect();
        retries++;
        qWarning() << Q_FUNC_INFO << "Connection attempt" << retries << "failed";

        // FIX: sleep в worker-потоке допустим, т.к. это не GUI thread
        QThread::sleep(1);
    } /* while */

    if (connected)
    {
        m_isConnected = true;
        if (m_timer)
        {
            m_timer->start(500);
        } /* if */
        qDebug() << Q_FUNC_INFO << "Camera connected after" << retries + 1 << "attempts";
    } /* if */
    else
    {
        m_isConnected = false;
        qCritical() << Q_FUNC_INFO << "Connection failed after" << MAX_RETRIES << "attempts";
    } /* else */

    emit sig_connected(m_isConnected);
} /* Camera_EV9500M_Worker::proc_VISCA_CONNECT */

void Camera_EV9500M_Worker::proc_VISCA_DISCONNECT()
{
    if (!m_isConnected)
        return;

    m_isConnected = false;

    if (m_timer)
    {
        m_timer->stop();
    } /* if */

    emit sig_connected(false);

    // FIX: даём время текущей операции завершиться
    QThread::msleep(50);

    m_cam.disconnect();
    qDebug() << "EV9500M UART closed";
} /* Camera_EV9500M_Worker::proc_VISCA_DISCONNECT */

void Camera_EV9500M_Worker::proc_VISCA_CMD(TVCamera::Command cmd, quint32 value)
{
    if (!m_isConnected)
    {
        qWarning() << Q_FUNC_INFO << "Camera not connected, ignoring command";
        return;
    } /* if */

    if (m_cam.executeCommand(cmd, value) != VISCA_SUCCESS)
    {
        qDebug() << "Visca command error for cmd:" << cmd;
    } /* if */
} /* Camera_EV9500M_Worker::proc_VISCA_CMD */

/* ========================================================================
 * FIX #10: timeout — корректная проверка результатов (после исправления bool)
 * ======================================================================== */
void Camera_EV9500M_Worker::timeout()
{
    if (!m_isConnected)
        return;

    if (!m_cam.isBusy())
    {
        bool allOk = true;
        allOk &= m_cam.getTemperature();
        allOk &= m_cam.getIris();
        allOk &= m_cam.getShutter();
        allOk &= m_cam.getGain();
        allOk &= m_cam.getContrast();
        allOk &= m_cam.getFocusValue();
        allOk &= m_cam.getZoomValue();
        allOk &= m_cam.getAWBDisplayMode();

        if (!allOk)
        {
            qDebug() << "EV9500M error status request";
        } /* if */

        CameraStatus status = *m_cam.status();
        emit sig_sendStatus(status);
    } /* if */
} /* Camera_EV9500M_Worker::timeout */
