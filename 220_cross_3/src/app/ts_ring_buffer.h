#ifndef TS_RING_BUFFER_H
#define TS_RING_BUFFER_H

// ═══════════════════════════════════════════════════════════════════
//  TsRingBuffer — SPSC-кольцо для MPEG-TS с дропом ЦЕЛЫХ 188-байтных пакетов
//
//  Назначение: полностью развязать producer (encode, вызывается из
//  streaming-потока GStreamer через pad-probe) и consumer (pump-поток,
//  пушащий в appsrc транспорта). Producer НИКОГДА не блокируется: если
//  кольцо переполнено (транспорт стоит/не дренирует), удаляются САМЫЕ
//  СТАРЫЕ байты на границе пакета 188 → поток остаётся выровненным и
//  валидным для mpegts, только с «дыркой» (gaps) вместо corrupt.
//
//  Инвариант выравнивания: capacity кратно 188; запись всегда приходит
//  целыми буферами mpegtsmux (каждый кратен 188); eviction и чтение —
//  только порциями, кратными 188 → read/write индексы всегда на границе
//  пакета. Это исключает рассинхрон границ TS-пакетов на приёмнике.
// ═══════════════════════════════════════════════════════════════════

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <vector>

class TsRingBuffer
{
  public:
    static constexpr std::size_t TS_PKT = 188;

    void init(std::size_t capacityBytes)
    {
        // Округляем вверх до кратного размеру TS-пакета.
        const std::size_t cap = ((capacityBytes + TS_PKT - 1) / TS_PKT) * TS_PKT;
        std::lock_guard<std::mutex> lk(m_mtx);
        m_buf.assign(cap, 0);
        m_cap = cap;
        m_head = 0;  // индекс чтения (старые данные)
        m_tail = 0;  // индекс записи (новые данные)
        m_count = 0; // байт в буфере
        m_stop = false;
        m_totalDroppedPackets.store(0);
    } // init

    // Запись producer'а. Возвращает true, если записано полностью;
    // false — если пакет больше всего кольца (отброшен целиком).
    // При нехватке места удаляются старые байты (round-up до 188), чтобы
    // освободить место под новый пакет — блокировки нет никогда.
    bool write(const uint8_t *data, std::size_t len)
    {
        if (!data || len == 0)
            return false;

        std::unique_lock<std::mutex> lk(m_mtx);
        if (len > m_cap)
            return false; // пакет физически не помещается

        if (m_count + len > m_cap)
        {
            // Освобождаем ровно столько, сколько нужно, округляя ВВЕРХ до
            // 188, чтобы read-индекс остался на границе пакета.
            std::size_t needDrop = m_count + len - m_cap;
            needDrop = ((needDrop + TS_PKT - 1) / TS_PKT) * TS_PKT;
            if (needDrop > m_count)
                needDrop = m_count;
            const std::size_t droppedPkts = needDrop / TS_PKT;
            m_totalDroppedPackets.fetch_add(static_cast<long long>(droppedPkts),
                                            std::memory_order_relaxed);
            m_head = (m_head + needDrop) % m_cap;
            m_count -= needDrop;
        } // if (m_count + len > m_cap)

        const std::size_t first = std::min(len, m_cap - m_tail);
        std::memcpy(&m_buf[m_tail], data, first);
        if (len > first)
            std::memcpy(&m_buf[0], data + first, len - first);
        m_tail = (m_tail + len) % m_cap;
        m_count += len;

        lk.unlock();
        m_cv.notify_all();
        return true;
    } // write

    // Чтение consumer'ом: копирует не более maxWant байт, округляя ВНИЗ до
    // 188, чтобы appsrc получал только целые TS-пакеты. Возвращает число
    // реально скопированных байт (0 — если данных меньше одного пакета).
    std::size_t read(uint8_t *out, std::size_t maxWant)
    {
        if (!out || maxWant < TS_PKT)
            return 0;

        std::lock_guard<std::mutex> lk(m_mtx);
        std::size_t want = (std::min(maxWant, m_count) / TS_PKT) * TS_PKT;
        if (want == 0)
            return 0;

        const std::size_t first = std::min(want, m_cap - m_head);
        std::memcpy(out, &m_buf[m_head], first);
        if (want > first)
            std::memcpy(out + first, &m_buf[0], want - first);
        m_head = (m_head + want) % m_cap;
        m_count -= want;
        return want;
    } // read

    // Сколько целых пакетов готово к чтению прямо сейчас.
    std::size_t availableBytes() const
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        return m_count;
    } // availableBytes

    bool hasData() const
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        return m_count >= TS_PKT;
    } // hasData

    // Ожидание появления данных (или сигнала stop). timeoutMs<0 — бессрочно.
    void waitData(int timeoutMs)
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        if (timeoutMs < 0)
            m_cv.wait(lk, [this]
                      {
                          return m_stop || m_count >= TS_PKT;
                      }); // lambda [this]
        else
            m_cv.wait_for(lk, std::chrono::milliseconds(timeoutMs),
                          [this]
                          {
                              return m_stop || m_count >= TS_PKT;
                          }); // lambda [this]
    } // waitData

    // Пробудить ожидающего (для корректной остановки pump-потока).
    void requestStop()
    {
        {
            std::lock_guard<std::mutex> lk(m_mtx);
            m_stop = true;
        } // block
        m_cv.notify_all();
    } // requestStop

    // Сброс флага stop перед повторным запуском pump-потока.
    // Без этого waitData() возвращается мгновенно и pump крутится вхолостую.
    void resetStop()
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        m_stop = false;
    } // resetStop

    void clear()
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        m_head = m_tail = m_count = 0;
    } // clear

    long long droppedPackets() const
    {
        return m_totalDroppedPackets.load(std::memory_order_relaxed);
    } // droppedPackets

  private:
    mutable std::mutex m_mtx;
    std::condition_variable m_cv;
    std::vector<uint8_t> m_buf;
    std::size_t m_cap = 0;
    std::size_t m_head = 0;
    std::size_t m_tail = 0;
    std::size_t m_count = 0;
    bool m_stop = false;
    std::atomic<long long> m_totalDroppedPackets{0};
}; // class TsRingBuffer

#endif // TS_RING_BUFFER_H
