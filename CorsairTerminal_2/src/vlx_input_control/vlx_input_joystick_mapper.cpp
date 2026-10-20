#include "vlx_input_joystick_mapper.hpp"

#include <QCoreApplication>


#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

namespace Vlx::Input {

// --- helpers --------------------------------------------------------------

static inline double rawToNorm(int raw) {
    return raw < 0 ? double(raw) / 32768.0 : double(raw) / 32767.0;
}

// --- ctor/dtor ------------------------------------------------------------

JoystickMapper::JoystickMapper(QObject* parent)
    : QObject(parent)
{
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    _timer.setInterval(_pollInterval);
    connect(&_timer, &QTimer::timeout, this, &JoystickMapper::poll);
    loadMapping(); // auto-load from default path
}

JoystickMapper::~JoystickMapper() {
    stop();
    closeJoystick();
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

int JoystickMapper::axisCount()   const { return _joy ? SDL_JoystickNumAxes(_joy)    : 0; }
int JoystickMapper::buttonCount() const { return _joy ? SDL_JoystickNumButtons(_joy) : 0; }
int JoystickMapper::hatCount()    const { return _joy ? SDL_JoystickNumHats(_joy)    : 0; }

// --- enumeration ----------------------------------------------------------

int JoystickMapper::joystickCount() {
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) return 0;
    SDL_JoystickUpdate();
    return SDL_NumJoysticks();
}

QString JoystickMapper::joystickName(int index) {
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) return {};
    const char* nm = SDL_JoystickNameForIndex(index);
    return nm ? QString::fromUtf8(nm) : QString();
}

QString JoystickMapper::joystickName() const {
    if (!_joy) return {};
    const char* nm = SDL_JoystickName(_joy);
    return nm ? QString::fromUtf8(nm) : QString();
}

// --- config ---------------------------------------------------------------

bool JoystickMapper::loadMapping(const QString& path) {
    _path = path.isEmpty() ? MappingConfig::defaultPath() : path;
    QString err;
    if (!_config.load(_path, &err)) {
        emit errorOccurred(tr("Не удалось загрузить %1: %2").arg(_path, err));
        return false;
    }
    return true;
}

bool JoystickMapper::reloadMapping() { return loadMapping(_path); }

// --- bindings -------------------------------------------------------------

void JoystickMapper::bindAction(Cmd cmd, ActionFn fn) {
    _actionFns.insert(static_cast<int>(cmd), std::move(fn));
}

void JoystickMapper::bindAxis(Cmd cmd, AxisFn fn) {
    _axisFns.insert(static_cast<int>(cmd), std::move(fn));
}

void JoystickMapper::unbind(Cmd cmd) {
    _actionFns.remove(static_cast<int>(cmd));
    _axisFns.remove(static_cast<int>(cmd));
}

void JoystickMapper::clearBindings() {
    _actionFns.clear();
    _axisFns.clear();
}

// --- device ---------------------------------------------------------------

bool JoystickMapper::openJoystick(int index) {
    closeJoystick();
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
        emit errorOccurred(QString::fromUtf8(SDL_GetError()));
        return false;
    }
    SDL_JoystickUpdate();
    if (index < 0 || index >= SDL_NumJoysticks()) {
        emit errorOccurred(tr("Джойстик #%1 не найден").arg(index));
        return false;
    }
    _joy = SDL_JoystickOpen(index);
    if (!_joy) {
        emit errorOccurred(QString::fromUtf8(SDL_GetError()));
        return false;
    }
    _joyIndex = index;
    _axisStates.clear();
    _buttonStates.clear();
    emit opened(joystickName());
    return true;
}

bool JoystickMapper::openJoystick(const QString& name) {
    const int n = joystickCount();
    for (int i = 0; i < n; ++i) {
        if (joystickName(i) == name) return openJoystick(i);
    }
    emit errorOccurred(tr("Джойстик '%1' не найден").arg(name));
    return false;
}

void JoystickMapper::closeJoystick() {
    if (_joy) {
        SDL_JoystickClose(_joy);
        _joy = nullptr;
        _joyIndex = -1;
        _axisStates.clear();
        _buttonStates.clear();
        emit closed();
    }
}

void JoystickMapper::setPollInterval(int ms) {
    _pollInterval = std::max(1, ms);
    _timer.setInterval(_pollInterval);
}

void JoystickMapper::start() { _timer.start(); }
void JoystickMapper::stop()  { _timer.stop();  }
bool JoystickMapper::isRunning() const { return _timer.isActive(); }

int JoystickMapper::rawAxis(int axis) const {
    return _joy ? SDL_JoystickGetAxis(_joy, axis) : 0;
}
bool JoystickMapper::rawButton(int button) const {
    return _joy && SDL_JoystickGetButton(_joy, button) != 0;
}
int JoystickMapper::rawHat(int hat) const {
    return _joy ? SDL_JoystickGetHat(_joy, hat) : 0;
}

// --- processing -----------------------------------------------------------

void JoystickMapper::fire(const ActionSequence& seq) {
    for (Cmd c : seq.cmds) {
        auto it = _actionFns.find(static_cast<int>(c));
        if (it != _actionFns.end() && it.value()) it.value()();
    }
}

void JoystickMapper::applyAxis(const AxisBinding& b, double /*dt*/) {
    if (!_joy || b.axis < 0 || b.cmd == Cmd::Axis_None) return;

    // 1) нормализация
    double n = rawToNorm(SDL_JoystickGetAxis(_joy, b.axis));

    // 2) мёртвая зона
    const double dz = std::clamp(b.deadZone / 1000.0, 0.0, 0.99);
    if (std::fabs(n) <= dz) n = 0.0;

    // 3) кривая (квадрат, с сохранением знака) — как в рабочем примере
    n = std::copysign(n * n, n);

    // 4) экспоненциальный фильтр (cur = prev*(1-a) + next*a)
    auto& st = _axisStates[b.axis];
    if (st.first) {
        st.value = n;
        st.first = false;
    } else {
        const double a = std::clamp(b.smoothing, 0.0, 1.0);
        st.value = st.value * (1.0 - a) + n * a;
    }
    if (std::fabs(st.value) <= dz) st.value = 0.0;

    // 5) инверсия и чувствительность
    double out = st.value;
    if (b.inverted) out = -out;
    out *= b.sensitivity;

    auto it = _axisFns.find(static_cast<int>(b.cmd));
    if (it != _axisFns.end() && it.value()) it.value()(out);
}

void JoystickMapper::applyButton(const ButtonBinding& b) {
    if (!_joy) return;

    bool pressed = false;
    int  key     = -1;

    if (b.hat >= 0) {
        const int h = SDL_JoystickGetHat(_joy, b.hat);
        pressed = b.hatMask != 0 && (h & b.hatMask) == b.hatMask;
        key     = 1000 + b.hat * 100 + (b.hatMask & 0xFF);
    } else if (b.button >= 0) {
        pressed = SDL_JoystickGetButton(_joy, b.button) != 0;
        key     = b.button;
    } else {
        return;
    }

    auto& st = _buttonStates[key];
    const bool was = st.pressed;
    st.pressed = pressed;

    if (pressed && !was) {
        if (b.toggleEnabled) {
            fire(st.togglePhase ? b.toggleB : b.toggleA);
            st.togglePhase = !st.togglePhase;
        } else {
            fire(b.press);
        }
    } else if (!pressed && was) {
        fire(b.release);
    }
}

void JoystickMapper::poll() {
    if (!_joy) return;

    SDL_JoystickUpdate();

    if (!SDL_JoystickGetAttached(_joy)) {
        emit errorOccurred(tr("Джойстик отключён"));
        closeJoystick();
        return;
    }

    const JoystickMapping* map = _config.find(joystickName());
    if (!map) return;

    const double dt = _pollInterval / 1000.0;
    for (const auto& a : map->axes)    applyAxis  (a, dt);
    for (const auto& b : map->buttons) applyButton(b);
}

} // namespace Vlx::Input
