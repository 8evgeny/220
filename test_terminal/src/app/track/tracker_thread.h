#ifndef TRACKER_THREAD_H
#define TRACKER_THREAD_H

#include <QThread>
#include <QMutex>
#include <opencv2/core.hpp>
#include <atomic>
#include <string>
#include "trsh_wrapper.h"

// ─── Lock-free single-slot (seqlock) ─────────────────────────────
struct FrameSlot {
    std::atomic<uint64_t> seq{0};
    cv::Mat               data;
    int                   channel = -1;

    void write(const cv::Mat &mat, int ch) {
        uint64_t s = seq.load(std::memory_order_relaxed);
        seq.store(s + 1, std::memory_order_release);
        data    = mat.clone();
        channel = ch;
        seq.store(s + 2, std::memory_order_release);
    }

    bool read(cv::Mat &out, int &ch) {
        uint64_t s1 = seq.load(std::memory_order_acquire);
        if ((s1 & 1) || s1 == 0) return false;

        cv::Mat tmp = data.clone();
        ch = channel;

        uint64_t s2 = seq.load(std::memory_order_acquire);
        if (s1 != s2) return false;

        out = std::move(tmp);
        return true;
    }
};

// ─── Команды трекеру ──────────────────────────────────────────────
struct TrackerCmd {
    enum Type { NONE, SET_TARGET, STOP, REINIT, SWITCH };
    Type                type = NONE;
    TrShWrapper::Rect   target{};
    std::string         iniPath;
};
class TrackerThread : public QThread {
public:
    explicit TrackerThread(FrameSlot *slot, QObject *parent = nullptr);
    ~TrackerThread() override;

    void submitCmd(TrackerCmd cmd);
    void setActiveChannel(int ch);

    /// Немедленно обнуляет результаты (вызывается из внешнего потока).
    /// После этого isActive()/isLockedOn() вернут false,
    /// а objectRect()/roiRect() — пустой прямоугольник.
    void forceResetResults();

    TrShWrapper::Rect objectRect() const;
    TrShWrapper::Rect roiRect() const;
    bool isActive() const;
    bool isLockedOn() const;

protected:
    void run() override;

private:
    FrameSlot *m_slot = nullptr;

    mutable QMutex m_cmdMutex;
    TrackerCmd     m_pendingCmd;

    mutable QMutex m_resultMutex;
    TrShWrapper::Rect m_obj{};
    TrShWrapper::Rect m_roi{};
    bool m_active = false;
    bool m_locked = false;

    std::atomic<bool> m_running{false};
    std::atomic<int>  m_activeChannel{-1};
};
#endif // TRACKER_THREAD_H
