// ═══════════════════════════════════════════════════════════════════════════
//  Тесты модуля vdb_vrt_tracker_adapter (математика контура наведения).
//
//  Запуск standalone (без Qt):
//    g++ -std=c++17 -I../../src/app/gpf/tracker_adapter
//        test_tracker_adapter.cpp
//        ../../src/app/gpf/tracker_adapter/vdb_vrt_tracker_adapter_impl.cpp
//        ../../src/app/gpf/tracker_adapter/pid_regulator.cpp
//        -o test_tracker_adapter && ./test_tracker_adapter
//  или через CMake (см. CMakeLists.txt в этой директории).
//
//  Покрываемое:
//   1. SlewRateLimiter            — ограничение приращения за шаг
//   2. SimpleExponentialFilter    — инициализация/blend/reset
//   3. PIDRegulator               — P/I/D, clamp интеграла и выхода, reset
//   4. Геометрия: рамка→угол (верхний-левая конвенция dist, знак ошибки)
//   5. Аспект: betta = distY × FOV_y (вертикальный FOV, не горизонтальный)
//   6. Gain scheduling по FOV     — wide/tele/середина/кламп за диапазон
//   7. Смена канала               — сброс интеграла PID
//   8. stopTracking               — (0,0) в sink + сброс фильтров/slew
//   9. PrepareScore               — затухание squarekoef
//  10. Детект изменений dist      — нет перевыпуска при неизменном входе
//  11. Замкнутый контур yaw       — сходимость, ограничение выхода
//  12. Замкнутый контур pitch     — сходимость с аспектом 2:1
//
//  Конвенция знаков (совпадает со старым контуром GyroPlatformHandler,
//  kW=kH=-50): цель ПРАВЕЕ/НИЖЕ центра → скорость отрицательная.
//  Модель платформы: theta += -cmd * dt  (репродуцирует старый рабочий контур).
// ═══════════════════════════════════════════════════════════════════════════

#include "vdb_vrt_tracker_adapter_impl.hpp"
#include "pid_regulator.hpp"
#include "slewRatelimiter.hpp"
#include "simple_exponential_filter.hpp"

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <limits>

// ─── микро-фреймворк ────────────────────────────────────────────────────────
static int g_pass = 0;
static int g_fail = 0;

static void checkNear(double a, double b, double eps, const char *what, int line)
{
    if (std::fabs(a - b) <= eps) {
        ++g_pass;
    } else {
        ++g_fail;
        std::printf("  FAIL (line %d): %s\n    got %.6f, want %.6f (eps %.6f)\n",
                    line, what, a, b, eps);
    }
}

static void checkTrue(bool x, const char *what, int line)
{
    if (x) { ++g_pass; }
    else   { ++g_fail; std::printf("  FAIL (line %d): %s\n", line, what); }
}

#define CHECK_NEAR(a, b, eps)  checkNear((a), (b), (eps), #a " ~= " #b, __LINE__)
#define CHECK_TRUE(x)          checkTrue((x), #x, __LINE__)

static constexpr double kNan = std::numeric_limits<double>::quiet_NaN();

// ─── sink: приёмник скоростей от адаптера ───────────────────────────────────
class TestSink : public ITrackerSpeedSink
{
public:
    void onTrackerSpeed(float pitch, float yaw) override
    {
        m_pitch = pitch;
        m_yaw   = yaw;
        ++m_calls;
    }
    double m_pitch = kNan;
    double m_yaw   = kNan;
    int    m_calls = 0;
};

// ─── построители конфигурации (индексы: Channel TV=0/TPV=1, Axis X=0/Y=1) ───
struct KoefParams
{
    float kpX = 1.f, kiX = 0.f, kdX = 0.f, ilimX = 100.f, outlimX = 100.f;
    float kpY = 1.f, kiY = 0.f, kdY = 0.f, ilimY = 100.f, outlimY = 100.f;
    float fovWide = 50.f, fovTele = 5.f;
    float kKpWideX = 1.f, kKpTeleX = 1.f;   // остальные k* = 1
};

static TrackerAdapter::ChannelsPidMultiplexors makeKoefs(const KoefParams &p)
{
    TrackerAdapter::ChannelsPidMultiplexors k{};
    for (int ch = 0; ch < int(Channel::channelsCounter); ++ch) {
        k.oneChannelK[ch].basePidK[int(Axis::x)] = { p.kpX, p.kiX, p.kdX, p.ilimX, p.outlimX };
        k.oneChannelK[ch].basePidK[int(Axis::y)] = { p.kpY, p.kiY, p.kdY, p.ilimY, p.outlimY };
        k.oneChannelK[ch].multiAxisK[int(FOVType::wide)].fovVal = p.fovWide;
        k.oneChannelK[ch].multiAxisK[int(FOVType::tele)].fovVal = p.fovTele;
        for (int ax = 0; ax < int(Axis::counterAxis); ++ax) {
            k.oneChannelK[ch].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[ax] = { 1.f, 1.f, 1.f, 1.f };
            k.oneChannelK[ch].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[ax] = { 1.f, 1.f, 1.f, 1.f };
        }
        k.oneChannelK[ch].multiAxisK[int(FOVType::wide)].oneAxisPIDFov[int(Axis::x)].kKp = p.kKpWideX;
        k.oneChannelK[ch].multiAxisK[int(FOVType::tele)].oneAxisPIDFov[int(Axis::x)].kKp = p.kKpTeleX;
    }
    return k;
}

// Статус трекера по ЦЕНТРУ рамки (хендлер передаёт левый верхний: center - size/2)
static TrackerAdapter::PldTrackerStatus makeStatus(float centerX, float centerY,
                                                   float fov, float fovY,
                                                   float sizeX = 0.1f, float sizeY = 0.1f,
                                                   Channel ch = Channel::TV)
{
    TrackerAdapter::PldTrackerStatus st{};
    st.sizeX = sizeX;
    st.sizeY = sizeY;
    st.distX = centerX - 0.5f * sizeX;   // конвенция адаптера: левый верхний [0;1]
    st.distY = centerY - 0.5f * sizeY;
    st.fov   = fov;
    st.fovY  = fovY;
    st.curChannel = ch;
    return st;
}

// ═══ 1. SlewRateLimiter ═════════════════════════════════════════════════════
static void test_slew()
{
    std::puts("[1] SlewRateLimiter");
    SlewRateLimiter s(10.0f);
    CHECK_NEAR(s.limit(5.0f),    5.0, 1e-6);   // в пределах — проходит
    CHECK_NEAR(s.limit(30.0f),  15.0, 1e-6);   // +10 за шаг
    CHECK_NEAR(s.limit(-30.0f),  5.0, 1e-6);   // -10 за шаг
    s.reset();
    CHECK_NEAR(s.limit(-100.0f), -10.0, 1e-6); // reset → старт с 0
}

// ═══ 2. SimpleExponentialFilter ═════════════════════════════════════════════
static void test_expfilter()
{
    std::puts("[2] SimpleExponentialFilter");
    SimpleExponentialFilter f(0.5f);
    CHECK_NEAR(f.update(10.0f), 10.0, 1e-6);   // первый отсчёт — без лага (init)
    CHECK_NEAR(f.update(0.0f),   5.0, 1e-6);   // 0.5*0 + 0.5*10
    CHECK_NEAR(f.update(0.0f),   2.5, 1e-6);
    f.reset();
    CHECK_NEAR(f.update(7.0f),   7.0, 1e-6);   // после reset — снова init
}

// ═══ 3. PIDRegulator ════════════════════════════════════════════════════════
static void test_pid()
{
    std::puts("[3] PIDRegulator");
    { // P + ограничение выхода
        PIDRegulator p;
        p.set_coefs(2.f, 0.f, 0.f, 10.f, 1.f);
        CHECK_NEAR(p.calc(1.0f),    2.0, 1e-6);
        CHECK_NEAR(p.calc(100.0f), 10.0, 1e-6);   // outLim
        CHECK_NEAR(p.calc(-100.0f), -10.0, 1e-6);
    }
    { // I + anti-windup по iLim
        PIDRegulator p;
        p.set_coefs(0.f, 1.f, 0.f, 100.f, 3.f);
        CHECK_NEAR(p.calc(1.0f), 1.0, 1e-6);
        CHECK_NEAR(p.calc(1.0f), 2.0, 1e-6);
        CHECK_NEAR(p.calc(1.0f), 3.0, 1e-6);
        CHECK_NEAR(p.calc(1.0f), 3.0, 1e-6);   // интеграл зажат iLim
        CHECK_NEAR(p.calc(-10.0f), -3.0, 1e-6); // clamp с сохранением знака
    }
    { // D: реакция на разность ошибок
        PIDRegulator p;
        p.set_coefs(0.f, 0.f, 1.f, 10.f, 1.f);
        CHECK_NEAR(p.calc(5.0f),  5.0, 1e-6);   // err_prev=0
        CHECK_NEAR(p.calc(2.0f), -3.0, 1e-6);
        p.reset();
        CHECK_NEAR(p.calc(2.0f),  2.0, 1e-6);   // reset обнулил err_prev
    }
}

// ═══ 4. Геометрия и знаки ═══════════════════════════════════════════════════
static void test_geometry_signs()
{
    std::puts("[4] geometry: dist=center-0.5, alfa=distX*FOVx, знак скорости");
    KoefParams kp;  kp.kpX = 2.f; kp.kpY = 2.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    // Цель на 0.01 правее центра, FOVx=50° → alfa = 0.5°
    a.setTrackerStatusUpdate(makeStatus(0.51f, 0.50f, 50.f, 25.f));
    a.step();
    CHECK_NEAR(sink.m_yaw, -1.0, 1e-4);   // Kp 2 × 0.5°, знак − (цель правее → −)
    CHECK_NEAR(sink.m_pitch, 0.0, 1e-6);  // по вертикали цель в центре

    // центр рамки вычисляется из левого верхнего: dist = size/2 + tl − 0.5
    TestSink s2;
    TrackerAdapter b(makeKoefs(kp));
    b.setSpeedSink(&s2);
    b.startTracking();
    TrackerAdapter::PldTrackerStatus st = makeStatus(0.5f, 0.5f, 50.f, 25.f);
    st.distX = 0.0f;   // левый верхний у левого края: size=0.1 → центр 0.05 → dist −0.45
    b.setTrackerStatusUpdate(st);
    b.step();
    // alfa = −0.45×50 = −22.5° → speed = −45°/с → slew 10/шаг → yaw = +10
    CHECK_NEAR(s2.m_yaw, 10.0, 1e-3);         // цель ЛЕВЕЕ центра → скорость +
}

// ═══ 5. Аспект: betta использует вертикальный FOV ══════════════════════════
static void test_aspect()
{
    std::puts("[5] aspect: betta = distY * FOVy (не FOVx)");
    KoefParams kp; kp.kpX = 1.f; kp.kpY = 1.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    // центр на 0.01 ниже, FOVy=25° → betta = 0.25° (было бы 0.5° при FOVx=50)
    a.setTrackerStatusUpdate(makeStatus(0.5f, 0.51f, 50.f, 25.f));
    a.step();
    CHECK_NEAR(sink.m_pitch, -0.25, 1e-4);
    CHECK_NEAR(sink.m_yaw, 0.0, 1e-6);

    // fovY=0 → fallback на fov (aspect 1:1, совместимость)
    TestSink s2;
    TrackerAdapter b(makeKoefs(kp));
    b.setSpeedSink(&s2);
    b.startTracking();
    b.setTrackerStatusUpdate(makeStatus(0.5f, 0.51f, 50.f, 0.f));
    b.step();
    CHECK_NEAR(s2.m_pitch, -0.5, 1e-4);
}

// ═══ 6. Gain scheduling по FOV ══════════════════════════════════════════════
static void test_gain_scheduling()
{
    std::puts("[6] gain scheduling: Kp(FOV) = base*(kTele + (kWide−kTele)*t), t∈[0;1]");
    KoefParams kp;
    kp.kpX = 1.f;  kp.kKpWideX = 2.f;  kp.kKpTeleX = 1.f;
    kp.fovWide = 50.f; kp.fovTele = 5.f;

    // err_norm = 0.01 → alfa = 0.01*fov; speed = Kp(fov)*alfa
    struct { float fov; float wantKp; } cases[] = {
        { 50.f, 2.0f },   // wide  → kWide
        {  5.f, 1.0f },   // tele  → kTele
        { 27.5f, 1.5f },  // середина → линейная интерполяция
        { 60.f, 2.0f },   // за wide → кламп (без экстраполяции)
        {  1.f, 1.0f },   // за tele → кламп
    };

    for (const auto &c : cases) {
        TestSink s;
        TrackerAdapter a(makeKoefs(kp));
        a.setSpeedSink(&s);
        a.startTracking();
        a.setTrackerStatusUpdate(makeStatus(0.51f, 0.5f, c.fov, c.fov));
        a.step();
        const float alfa = 0.01f * c.fov;
        const float want = -(c.wantKp * alfa);
        const float got  = (std::fabs(want) > 10.f) ? -10.f : want;   // slew 10/шаг
        CHECK_NEAR(s.m_yaw, got, 1e-3);
    }

    //wide==tele → нет деления на ноль (защита), Kp = base*k
    TestSink s2;
    KoefParams kpeq; kpeq.kpX = 1.f; kpeq.fovWide = 20.f; kpeq.fovTele = 20.f;
    kpeq.kKpWideX = 2.f; kpeq.kKpTeleX = 2.f;
    TrackerAdapter a2(makeKoefs(kpeq));
    a2.setSpeedSink(&s2);
    a2.startTracking();
    a2.setTrackerStatusUpdate(makeStatus(0.51f, 0.5f, 20.f, 20.f));
    a2.step();
    CHECK_NEAR(s2.m_yaw, -1.0f * (0.01f * 20.f) * 2.f, 1e-3);   // Kp=2
}

// ═══ 7. Смена канала — сброс интеграла ═════════════════════════════════════
static void test_channel_switch()
{
    std::puts("[7] switch TV→TPV: интеграл PID сброшен");
    KoefParams kp; kp.kpX = 0.f; kp.kiX = 1.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    a.setTrackerStatusUpdate(makeStatus(0.51f, 0.5f, 50.f, 25.f, 0.1f, 0.1f, Channel::TV));
    a.step();                                        // alfa 0.5 → integ 0.5
    CHECK_NEAR(sink.m_yaw, -0.5, 1e-4);

    a.setTrackerStatusUpdate(makeStatus(0.511f, 0.5f, 50.f, 25.f, 0.1f, 0.1f, Channel::TV));
    a.step();                                        // raw 0.55, filt=0.85*0.55+0.15*0.5=0.5425
    CHECK_NEAR(sink.m_yaw, -1.0425, 1e-3);           // integ = 0.5 + 0.5425

    // Смена канала при ИЗМЕНИВШЕЙся рамке (иначе step() молчит — нет newVal):
    // PID сброшен, фильтр остаётся: filt = 0.85*0.6 + 0.15*0.5425 = 0.591375
    a.setTrackerStatusUpdate(makeStatus(0.512f, 0.5f, 50.f, 25.f, 0.1f, 0.1f, Channel::TPV));
    a.step();                                        // integ = 0.591375 (не 1.0425+0.5914)
    CHECK_NEAR(sink.m_yaw, -0.591375, 1e-3);
}

// ═══ 8. stopTracking: (0,0) + полный сброс состояния ════════════════════════
static void test_stop_reset()
{
    std::puts("[8] stopTracking: (0,0) в sink, сброс фильтра/slew");
    KoefParams kp; kp.kpX = 2.f; kp.kpY = 2.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    a.setTrackerStatusUpdate(makeStatus(0.52f, 0.5f, 50.f, 25.f));  // alfa 1.0
    a.step();
    CHECK_NEAR(sink.m_yaw, -2.0, 1e-4);

    a.stopTracking();
    CHECK_NEAR(sink.m_yaw, 0.0, 1e-9);
    CHECK_NEAR(sink.m_pitch, 0.0, 1e-9);

    // Повторный захват: фильтр инициализирован заново (нет примеси −2° из прошлого
    // прогона: без reset было бы 0.85*0.5+0.15*1.0=0.575 → −1.15)
    a.startTracking();
    a.setTrackerStatusUpdate(makeStatus(0.51f, 0.5f, 50.f, 25.f));  // alfa 0.5
    a.step();
    CHECK_NEAR(sink.m_yaw, -1.0, 1e-4);
}

// ═══ 9. PrepareScore: квадратичное затухание ════════════════════════════════
static void test_prepare_score()
{
    std::puts("[9] prepare score: out = Kp*err*sq, sq=score^2");
    KoefParams kp; kp.kpX = 2.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    TrackerAdapter::PldTrackerStatus st = makeStatus(0.51f, 0.5f, 50.f, 25.f); // alfa 0.5
    st.trackerPrepareScoreNorm      = 0.5f;    // sq = 0.25
    st.enableSlowdownByPrepareScore = 1;
    a.setTrackerStatusUpdate(st);
    a.step();
    // Kp_eff = 2*0.25 = 0.5 → p = 0.25 → *0.25 = 0.0625
    CHECK_NEAR(sink.m_yaw, -0.0625, 1e-4);
}

// ═══ 10. Детект изменений: нет перевыпуска при неизменном входе ═════════════
static void test_change_detection()
{
    std::puts("[10] неизменный статус → step() молчит (детект по сырому dist)");
    KoefParams kp; kp.kpX = 1.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    a.setTrackerStatusUpdate(makeStatus(0.51f, 0.5f, 50.f, 25.f));
    a.step();
    CHECK_TRUE(sink.m_calls == 1);

    a.setTrackerStatusUpdate(makeStatus(0.51f, 0.5f, 50.f, 25.f));   // тот же вход
    a.step();
    CHECK_TRUE(sink.m_calls == 1);                                   // без перевыпуска

    a.setTrackerStatusUpdate(makeStatus(0.52f, 0.5f, 50.f, 25.f));   // цель сместилась
    a.step();
    CHECK_TRUE(sink.m_calls == 2);
}

// ═══ 11. Замкнутый контур yaw ═══════════════════════════════════════════════
// Модель: theta += -yaw*dt (репродуцирует знаки старого контура kW=−50).
static void test_closed_loop_yaw()
{
    std::puts("[11] closed loop yaw: сходимость к цели");
    KoefParams kp; kp.kpX = 2.5f; kp.outlimX = 20.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    const float fovX = 50.f;
    const float target = 10.f;          // цель на +10° от текущего прицела
    float theta = 0.f;
    const float dt = 0.02f;             // шаг = Step_MS
    float maxAbsCmd = 0.f;

    for (int i = 0; i < 400; ++i) {
        const float centerX = 0.5f + (target - theta) / fovX;
        a.setTrackerStatusUpdate(makeStatus(centerX, 0.5f, fovX, fovX));
        a.step();
        theta += -float(sink.m_yaw) * dt;
        maxAbsCmd = std::fmax(maxAbsCmd, std::fabs(sink.m_yaw));
    }
    CHECK_NEAR(theta, target, 0.05);
    CHECK_TRUE(maxAbsCmd <= 20.0 + 1e-3);   // outLim соблюдён
}

// ═══ 12. Замкнутый контур pitch (аспект 2:1) ════════════════════════════════
static void test_closed_loop_pitch()
{
    std::puts("[12] closed loop pitch: аспект FOVx=50/FOVy=25, сходимость");
    KoefParams kp; kp.kpY = 1.25f; kp.outlimY = 20.f;
    TrackerAdapter a(makeKoefs(kp));
    TestSink sink;
    a.setSpeedSink(&sink);
    a.startTracking();

    const float fovX = 50.f, fovY = 25.f;
    const float target = 6.f;
    float thetaP = 0.f;
    const float dt = 0.02f;

    for (int i = 0; i < 400; ++i) {
        const float centerY = 0.5f + (target - thetaP) / fovY;   // нормировка по высоте
        a.setTrackerStatusUpdate(makeStatus(0.5f, centerY, fovX, fovY));
        a.step();
        thetaP += -float(sink.m_pitch) * dt;
    }
    CHECK_NEAR(thetaP, target, 0.05);
}

// ═════════════════════════════════════════════════════════════════════════════
int main()
{
    std::puts("═══ tracker_adapter: unit + integration tests ═══");
    test_slew();
    test_expfilter();
    test_pid();
    test_geometry_signs();
    test_aspect();
    test_gain_scheduling();
    test_channel_switch();
    test_stop_reset();
    test_prepare_score();
    test_change_detection();
    test_closed_loop_yaw();
    test_closed_loop_pitch();

    std::printf("═══ %d passed, %d failed ═══\n", g_pass, g_fail);
    return (g_fail == 0) ? 0 : 1;
}
