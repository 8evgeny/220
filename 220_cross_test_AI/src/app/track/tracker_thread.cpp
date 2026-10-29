#include "tracker_thread.h"
#include <iostream>

TrackerThread::TrackerThread(FrameSlot *slot, QObject *parent)
    : QThread(parent), m_slot(slot) {}

TrackerThread::~TrackerThread() {
    m_running = false;
    requestInterruption();
    wait(3000);
}

void TrackerThread::submitCmd(TrackerCmd cmd) {
    QMutexLocker lock(&m_cmdMutex);
    m_pendingCmd = std::move(cmd);
}

void TrackerThread::setActiveChannel(int ch) {
    m_activeChannel.store(ch, std::memory_order_release);
}

TrShWrapper::Rect TrackerThread::objectRect() const {
    QMutexLocker lock(&m_resultMutex);
    return m_obj;
}

TrShWrapper::Rect TrackerThread::roiRect() const {
    QMutexLocker lock(&m_resultMutex);
    return m_roi;
}

bool TrackerThread::isActive() const {
    QMutexLocker lock(&m_resultMutex);
    return m_active;
}

bool TrackerThread::isLockedOn() const {
    QMutexLocker lock(&m_resultMutex);
    return m_locked;
}
void TrackerThread::run() {
    m_running = true;
    TrShWrapper tracker;

    while (m_running && !isInterruptionRequested()) {

        // Команды
        {
            QMutexLocker lock(&m_cmdMutex);
            switch (m_pendingCmd.type) {
            case TrackerCmd::SWITCH:
                tracker.stopTracking();
                tracker.reset();
                tracker.setIniFileNamePath(m_pendingCmd.iniPath);
                {
                    QMutexLocker rlock(&m_resultMutex);
                    m_obj    = {};
                    m_roi    = {};
                    m_active = false;
                    m_locked = false;
                }
                std::cerr << "[TrackerThread] SWITCH done" << std::endl;
                break;



            case TrackerCmd::REINIT:
                tracker.reset();
                tracker.setIniFileNamePath(m_pendingCmd.iniPath);
                {
                    QMutexLocker rlock(&m_resultMutex);
                    m_obj    = {};
                    m_roi    = {};
                    m_active = false;
                    m_locked = false;
                }
                std::cerr << "[TrackerThread] REINIT ini="
                          << m_pendingCmd.iniPath << std::endl;
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
                }
                std::cerr << "[TrackerThread] STOP" << std::endl;
                break;

            default:
                break;
            }
            m_pendingCmd.type = TrackerCmd::NONE;
        }

        // Кадр из slot
        cv::Mat frame;
        int ch = -1;
        if (!m_slot->read(frame, ch)) {
            msleep(5);
            continue;
        }

        // Фильтр неактивного канала
        int activeCh = m_activeChannel.load(std::memory_order_acquire);
        if (activeCh >= 0 && ch != activeCh)
            continue;

        // Трекинг — НЕ выполняем если трекер не создан и нет захвата
        tracker.trackFrame(frame);

        // Результаты
        {
            QMutexLocker lock(&m_resultMutex);
            m_obj    = tracker.objectRect();
            m_roi    = tracker.roiRect();
            m_active = tracker.active();
            m_locked = tracker.lockedOn();
        }
    }

    std::cerr << "[TrackerThread] stopped" << std::endl;
}

void TrackerThread::forceResetResults()
{
    QMutexLocker lock(&m_resultMutex);
    m_obj    = {};
    m_roi    = {};
    m_active = false;
    m_locked = false;
}
