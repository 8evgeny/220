#include "tracker_thread.h"
#include <iostream>

TrackerThread::TrackerThread(FrameSlot *slot, QObject *parent)
    : QThread(parent), m_slot(slot)
{
} /* TrackerThread::TrackerThread */

TrackerThread::~TrackerThread()
{
    m_running = false;
    requestInterruption();
    wait(3000);
} /* TrackerThread::~TrackerThread */

void TrackerThread::submitCmd(TrackerCmd cmd)
{
    QMutexLocker lock(&m_cmdMutex);
    m_pendingCmd = std::move(cmd);
} /* TrackerThread::submitCmd */

void TrackerThread::setActiveChannel(int ch)
{
    m_activeChannel.store(ch, std::memory_order_release);
} /* TrackerThread::setActiveChannel */

TrShWrapper::Rect TrackerThread::objectRect() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_obj;
} /* TrackerThread::objectRect */

TrShWrapper::Rect TrackerThread::roiRect() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_roi;
} /* TrackerThread::roiRect */

bool TrackerThread::isActive() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_active;
} /* TrackerThread::isActive */

bool TrackerThread::isLockedOn() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_locked;
} /* TrackerThread::isLockedOn */
void TrackerThread::run()
{
    m_running = true;
    TrShWrapper tracker;

    while (m_running && !isInterruptionRequested())
    {

        // Команды
        {
            QMutexLocker lock(&m_cmdMutex);
            switch (m_pendingCmd.type)
            {
            case TrackerCmd::SWITCH:
                tracker.stopTracking();
                tracker.reset();
                tracker.setIniFileNamePath(m_pendingCmd.iniPath);
                {
                    QMutexLocker rlock(&m_resultMutex);
                    m_obj = {};
                    m_roi = {};
                    m_active = false;
                    m_locked = false;
                } /* case */
                std::cerr << "[TrackerThread] SWITCH done" << std::endl;
                break;

            case TrackerCmd::REINIT:
                tracker.reset();
                tracker.setIniFileNamePath(m_pendingCmd.iniPath);
                {
                    QMutexLocker rlock(&m_resultMutex);
                    m_obj = {};
                    m_roi = {};
                    m_active = false;
                    m_locked = false;
                } /* case */
                std::cerr << "[TrackerThread] REINIT ini=" << m_pendingCmd.iniPath << std::endl;
                break;

            case TrackerCmd::SET_TARGET:
                tracker.setTargetObject(m_pendingCmd.target);
                std::cerr << "[TrackerThread] SET_TARGET" << std::endl;
                break;

            case TrackerCmd::STOP:
                tracker.stopTracking();
                {
                    QMutexLocker rlock(&m_resultMutex);
                    m_active = false;
                    m_locked = false;
                } /* case */
                std::cerr << "[TrackerThread] STOP" << std::endl;
                break;

            default:
                break;
            } /* switch */
            m_pendingCmd.type = TrackerCmd::NONE;
        } /* block */

        // Кадр из slot
        cv::Mat frame;
        int ch = -1;
        if (!m_slot->read(frame, ch))
        {
            msleep(5);
            continue;
        } /* if */

        // Фильтр неактивного канала
        int activeCh = m_activeChannel.load(std::memory_order_acquire);
        if (activeCh >= 0 && ch != activeCh)
            continue;

        // Трекинг — НЕ выполняем если трекер не создан и нет захвата
        tracker.trackFrame(frame);

        // Результаты
        {
            QMutexLocker lock(&m_resultMutex);
            m_obj = tracker.objectRect();
            m_roi = tracker.roiRect();
            m_active = tracker.active();
            m_locked = tracker.lockedOn();
        } /* block */
    } /* while */

    std::cerr << "[TrackerThread] stopped" << std::endl;
} /* TrackerThread::run */

void TrackerThread::forceResetResults()
{
    QMutexLocker lock(&m_resultMutex);
    m_obj = {};
    m_roi = {};
    m_active = false;
    m_locked = false;
} /* TrackerThread::forceResetResults */
