#include "tracker_thread.h"
#include <iostream>
#include <algorithm>

TrackerThread::TrackerThread(FrameSlot *slot, QObject *parent)
    : QThread(parent), m_slot(slot) {}

TrackerThread::~TrackerThread() {
    stop(3000);   // единая точка остановки (см. stop())
}

bool TrackerThread::stop(int timeoutMs) {
    m_running = false;
    requestInterruption();
    return wait(timeoutMs);
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

bool TrackerThread::clampTarget(TrShWrapper::Rect &rect, float roiExt) const
{
    // Если размеры кадра ещё неизвестны (первый кадр не обработан) — пропускаем
    if (m_frameWidth <= 0 || m_frameHeight <= 0)
        return true;

    // Минимальный размер для валидности
    if (rect.w < 2 || rect.h < 2) {
        std::cerr << "[TrackerThread] SET_TARGET rejected: w=" << rect.w
                  << " h=" << rect.h << std::endl;
        return false;
    }

    // rect.cx/cy — центр рамки. Трекер расширяет ROI вокруг этой точки.
    const int halfW_ext = static_cast<int>(rect.w * roiExt / 2.0f);
    const int halfH_ext = static_cast<int>(rect.h * roiExt / 2.0f);
    const int cxMin = halfW_ext;
    const int cxMax = m_frameWidth - halfW_ext;
    const int cyMin = halfH_ext;
    const int cyMax = m_frameHeight - halfH_ext;

    if (cxMax < cxMin || cyMax < cyMin) {
        std::cerr << "[TrackerThread] SET_TARGET rejected: ROI too large for frame"
                  << std::endl;
        return false;
    }

    rect.cx = static_cast<unsigned short>(std::clamp(static_cast<int>(rect.cx), cxMin, cxMax));
    rect.cy = static_cast<unsigned short>(std::clamp(static_cast<int>(rect.cy), cyMin, cyMax));

    return true;
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
                m_frameWidth  = 0;
                m_frameHeight = 0;
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
                if (clampTarget(m_pendingCmd.target, tracker.roiExt())) {
                    tracker.setTargetObject(m_pendingCmd.target);
                    std::cerr << "[TrackerThread] SET_TARGET cx=" << m_pendingCmd.target.cx
                              << " cy=" << m_pendingCmd.target.cy
                              << " w=" << m_pendingCmd.target.w
                              << " h=" << m_pendingCmd.target.h << std::endl;
                } else {
                    std::cerr << "[TrackerThread] SET_TARGET rejected (out of bounds)" << std::endl;
                }
                break;

            case TrackerCmd::SET_ROI_EXT:
                tracker.setRoiExt(m_pendingCmd.roiExt);
                std::cerr << "[TrackerThread] ROI_EXT=" << tracker.roiExt() << std::endl;
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

        // Запоминаем размеры кадра для валидации целевого ROI
        m_frameWidth  = frame.cols;
        m_frameHeight = frame.rows;

        // Трекинг — НЕ выполняем если трекер не создан и нет захвата
        tracker.trackFrame(frame);

        // Результаты
        {
            QMutexLocker lock(&m_resultMutex);
            m_active = tracker.active();
            m_locked = tracker.lockedOn();
            if (m_locked) {
                m_obj = tracker.objectRect();
                m_roi = tracker.roiRect();
            } else {
                m_obj = {};
                m_roi = {};
            }
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
