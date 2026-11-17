#ifndef ADAPTIVETRACKERADAPTERIMPL_HPP
#define ADAPTIVETRACKERADAPTERIMPL_HPP

#include "stdint.h"
#include "simple_exponential_filter.hpp"
#include "pid_regulator.hpp"
#include <memory>
#include <chrono>
#include "slewRatelimiter.hpp"

inline constexpr int FOV_COUNT = 2;
enum class Axis : uint8_t
{
    x,
    y,
    counterAxis
};
enum class Channel : uint8_t
{
    TV,
    TPV,
    channelsCounter
};

enum class FOVType : uint8_t
{
    wide,
    tele,
    fovCounter
};

class TrackerAdapter
{
public:
#pragma pack(push, 1)
    // текущие парметры углового поля и рамки трекера
    struct PldTrackerStatus
    {
        float sizeX{0.1f}; // размер рамки по X [относительные координаты][0;1]
        float sizeY{0.1f}; // размер рамки по Y [относительные координаты][0;1]
        float distX{0.1f}; // расстояние центра рамки от центра кадра по X [относительные координаты][-0.5;0.5]
        float distY{0.1f}; // расстояние центра рамки от центра кадра по Y [относительные координаты][-0.5;0.5]
        float fov{7};//текщее углвое поле камеры в градусах
        float trackerPrepareScoreNorm{0.0f};//степень готовности полноценного двжения при захвате (дать трекеру схватиться) [0;1]
        uint8_t enableSlowdownByPrepareScore{0x00};//отклюение[0]/включени[1] использования верхнего параметра в алгоритме
        Channel curChannel{Channel::TV};//текущий канал
    };

struct BasePid
{
    float Kp{1};
    float Ki{1};
    float Kd{1};
    float iLim{1};
    float outLim{10};
};
struct PidFovMultiplexorOneAxis 
{
    float kKp{1};
    float kKi{1};
    float kKd{1};
    float kKiLim{1};
};
enum class axis  : size_t
{
    x,
    y,
    counter
};

struct PidFovMultiplexorMultiAxis
{
    PidFovMultiplexorOneAxis oneAxisPIDFov[int(Axis::counterAxis)];
    float fovVal{20};
};


struct ChannelPidMultiplexor 
{
    PidFovMultiplexorMultiAxis  multiAxisK[int(FOVType::fovCounter)];
    BasePid                     basePidK[int(Axis::counterAxis)];

};

struct ChannelsPidMultiplexors
{
    ChannelPidMultiplexor oneChannelK[int(Channel::channelsCounter)];
};
#pragma pack(pop)

public:
    TrackerAdapter(ChannelsPidMultiplexors multiplexor);

    void startTracking();
    void stopTracking();
    void setTrackerStatusUpdate(PldTrackerStatus v_params);
    void calcAngles();
    void runtime();
    void setNewKoefs(ChannelsPidMultiplexors multiplexor);

private:
    // структура с информацией о рамке трекера
    struct trackerFrame
    {
        float sizeX{0.0f};
        float sizeY{0.0f};
        float area{0.00f};
        float distX{0.0f};
        float distY{0.0f};
        float distTotal{0.0f};
        bool newValSize{false};
        bool newValDist{false};
    };
    // структура с информацией о FOV
    struct FOV
    {
        float x{10};
        float y{10};
        bool newVal{true};
    };
    // ошибки по углу тангажа и курса (рассогласование между центро рамки трекера и центром кадра)
    struct anglesErrors
    {
        float betta{0.f}; // угол тангажа
        float alfa{0.f};  // угол курса
    };

private:
    void setCurFOV(float v_xFov, float v_yFOV);
    void setCurTrackerFrameSize(float sizeX, float sizeY);
    void setCurTrackerFrameDist(float distX, float distY);
    void TrackerAdapter::interpolateAndpreparePIDKoefs();
    void send_speed_to_gsp(float pitch, float yaw);

    PIDRegulator m_pidAlfa;
    PIDRegulator m_pidBetta;
    SlewRateLimiter *m_slewRateLimitterAlfa;
    SlewRateLimiter *m_slewRateLimitterBetta;
    FOV m_FOVCur;
    SimpleExponentialFilter m_expFiltBetta;
    SimpleExponentialFilter m_expFiltAlfa;
    trackerFrame m_trackerFrameCur;
    anglesErrors m_angErrors;
    ChannelsPidMultiplexors m_pidKoefs;
    bool tracking = {false};
    float m_k_pd_fov_x{1};
    float m_k_pd_fov_y{1};
    float m_k_lim_fov_x{1};
    float m_k_lim_fov_y{1};
    float m_trackerPrepareScoreNorm = {0.0f};
    bool m_enableSlowdownByPrepareScore = {false};
    Channel m_curChannel{Channel::TV};
};

#endif // ADAPTIVETRACKERCONTROLLER_H//
