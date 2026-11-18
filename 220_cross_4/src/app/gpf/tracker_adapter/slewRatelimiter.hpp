#ifndef SLEW_RATE_LIMITER_HPP
#define SLEW_RATE_LIMITER_HPP

#include <algorithm>

class SlewRateLimiter
{
public:
    // max_delta_per_frame — макс. изменение выхода за ОДИН кадр
    explicit SlewRateLimiter(float max_delta_per_frame)
        : m_max_delta(max_delta_per_frame) {}

    void reset(float value = 0.0f) { m_prev = value; }

    float limit(float target)
    {
        float delta = std::clamp(target - m_prev, -m_max_delta, m_max_delta);
        m_prev += delta;
        return m_prev;
    }

private:
    float m_max_delta;  // ед. выхода на кадр
    float m_prev = 0.0f;
};

#endif // SLEW_RATE_LIMITER_HPP