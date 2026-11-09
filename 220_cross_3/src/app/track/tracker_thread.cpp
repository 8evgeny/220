#include "tracker_thread.h"
#include <algorithm>
#include <iostream>

TrackerThread::TrackerThread(FrameSlot *slot, QObject *parent)
    : QThread(parent), m_slot(slot)
{
} // TrackerThread::TrackerThread

TrackerThread::~TrackerThread()
{
    stop(3000); // единая точка остановки (см. stop())
} // TrackerThread::~TrackerThread

bool TrackerThread::stop(int timeoutMs)
{
    m_running = false;
    requestInterruption();
    return wait(timeoutMs);
} // TrackerThread::stop

void TrackerThread::submitCmd(TrackerCmd cmd)
{
    QMutexLocker lock(&m_cmdMutex);
    m_pendingCmd = std::move(cmd);
} // TrackerThread::submitCmd

void TrackerThread::setActiveChannel(int ch)
{
    m_activeChannel.store(ch, std::memory_order_release);
} // TrackerThread::setActiveChannel

TrShWrapper::Rect TrackerThread::objectRect() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_obj;
} // TrackerThread::objectRect

TrShWrapper::Rect TrackerThread::roiRect() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_roi;
} // TrackerThread::roiRect

bool TrackerThread::isActive() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_active;
} // TrackerThread::isActive

bool TrackerThread::isLockedOn() const
{
    QMutexLocker lock(&m_resultMutex);
    return m_locked;
} // TrackerThread::isLockedOn

bool TrackerThread::clampTarget(TrShWrapper::Rect &rect, float roiExt) const
{
    // Если размеры кадра ещё неизвестны (первый кадр не обработан) — пропускаем
    if (m_frameWidth <= 0 || m_frameHeight <= 0)
        return true;

    // Минимальный размер для валидности
    if (rect.w < 2 || rect.h < 2)
    {
        std::cerr << "[TrackerThread] SET_TARGET rejected: w=" << rect.w
                  << " h=" << rect.h << std::endl;
        return false;
    } // if (rect.w < 2 || rect.h < 2)

    // rect.cx/cy — левый/верхний край (после конвертации из центра).
    // Трекер расширяет ROI в roiExt раз вокруг цели.
    // Рассчитываем эффективные границы ROI:
    //   roi_left   = cx - (w/2)*roiExt
    //   roi_right  = cx + w + (w/2)*roiExt = cx + w*(1 + roiExt/2)
    // Нужно чтобы roi_left >= 0 и roi_right <= frameWidth.
    const int halfW_ext = static_cast<int>(rect.w * roiExt / 2.0f);
    const int halfH_ext = static_cast<int>(rect.h * roiExt / 2.0f);
    const int roiTotalW = static_cast<int>(rect.w) + 2 * halfW_ext;
    const int roiTotalH = static_cast<int>(rect.h) + 2 * halfH_ext;

    // Если ROI целиком больше кадра — трекер не сможет корректно работать
    if (roiTotalW > m_frameWidth || roiTotalH > m_frameHeight)
    {
        std::cerr << "[TrackerThread] SET_TARGET rejected: ROI (" << roiTotalW
                  << "x" << roiTotalH << ") larger than frame ("
                  << m_frameWidth << "x" << m_frameHeight << ")" << std::endl;
        return false;
    } // if (roiTotalW > m_frameWidth || roiTotalH > m_frameHeight)

    // Допустимый диапазон для левого края цели, чтобы ROI помещался:
    //   cx >= halfW_ext  (чтобы roi_left >= 0)
    //   cx <= frameWidth - w - halfW_ext  (чтобы roi_right <= frameWidth)
    const int cxMin = halfW_ext;
    const int cxMax = m_frameWidth - static_cast<int>(rect.w) - halfW_ext;
    const int cyMin = halfH_ext;
    const int cyMax = m_frameHeight - static_cast<int>(rect.h) - halfH_ext;

    int left = static_cast<int>(rect.cx);
    int top = static_cast<int>(rect.cy);

    // Сдвигаем в допустимые границы
    if (left < cxMin)
        left = cxMin;
    if (left > cxMax)
        left = cxMax;
    if (top < cyMin)
        top = cyMin;
    if (top > cyMax)
        top = cyMax;

    // Проверяем, что после сдвига значение не стало отрицательным
    // (возможно если cxMin > cxMax при очень малом кадре)
    if (left < 0)
        left = 0;
    if (top < 0)
        top = 0;

    rect.cx = static_cast<unsigned short>(left);
    rect.cy = static_cast<unsigned short>(top);

    return true;
} // TrackerThread::clampTarget

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
                } // case TrackerCmd::SWITCH
                m_frameWidth = 0;
                m_frameHeight = 0;
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
                } // case TrackerCmd::REINIT
                std::cerr << "[TrackerThread] REINIT ini="
                          << m_pendingCmd.iniPath << std::endl;
                break;

            case TrackerCmd::SET_TARGET:
                if (clampTarget(m_pendingCmd.target, tracker.roiExt()))
                {
                    tracker.setTargetObject(m_pendingCmd.target);
                    std::cerr << "[TrackerThread] SET_TARGET cx=" << m_pendingCmd.target.cx
                              << " cy=" << m_pendingCmd.target.cy
                              << " w=" << m_pendingCmd.target.w
                              << " h=" << m_pendingCmd.target.h << std::endl;
                } // if (clampTarget(m_pendingCmd.target, tracker.roiExt()))
                else
                {
                    std::cerr << "[TrackerThread] SET_TARGET rejected (out of bounds)" << std::endl;
                } // else
                break;

            case TrackerCmd::STOP:
                tracker.stopTracking();
                {
                    QMutexLocker rlock(&m_resultMutex);
                    m_active = false;
                    m_locked = false;
                } // case TrackerCmd::STOP
                std::cerr << "[TrackerThread] STOP" << std::endl;
                break;

            default:
                break;
            } // switch (m_pendingCmd.type)
            m_pendingCmd.type = TrackerCmd::NONE;
        } // block

        // Кадр из slot
        cv::Mat frame;
        int ch = -1;
        if (!m_slot->read(frame, ch))
        {
            msleep(5);
            continue;
        } // if (!m_slot->read(frame, ch))

        // Фильтр неактивного канала
        int activeCh = m_activeChannel.load(std::memory_order_acquire);
        if (activeCh >= 0 && ch != activeCh)
            continue;

        // Запоминаем размеры кадра для валидации целевого ROI
        m_frameWidth = frame.cols;
        m_frameHeight = frame.rows;

        // Трекинг — НЕ выполняем если трекер не создан и нет захвата
        tracker.trackFrame(frame);

        // Результаты
        {
            QMutexLocker lock(&m_resultMutex);
            m_obj = tracker.objectRect();
            m_roi = tracker.roiRect();
            m_active = tracker.active();
            m_locked = tracker.lockedOn();
        } // block
    } // while (m_running && !isInterruptionRequested())

    std::cerr << "[TrackerThread] stopped" << std::endl;
} // TrackerThread::run

void TrackerThread::forceResetResults()
{
    QMutexLocker lock(&m_resultMutex);
    m_obj = {};
    m_roi = {};
    m_active = false;
    m_locked = false;
} // TrackerThread::forceResetResults
