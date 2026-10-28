#pragma once

#include "vlx_input_mapping.hpp"

#include <QObject>
#include <QTimer>
#include <QString>
#include <QHash>

#include <functional>


struct _SDL_Joystick;
using SDL_Joystick = _SDL_Joystick;

namespace Vlx::Input {

/// Рантайм-обработчик мапинга.
///
/// Идентификация: SDL_JoystickName.
/// Внешнее API: сопоставление Cmd → std::function.
class JoystickMapper : public QObject {
    Q_OBJECT
public:
    using ActionFn = std::function<void()>;
    using AxisFn   = std::function<void(double)>;

    explicit JoystickMapper(QObject* parent = nullptr);
    ~JoystickMapper() override;


    int axisCount()   const;
    int buttonCount() const;
    int hatCount()    const;

           // --- Enumeration (без экземпляра) ---
    static int     joystickCount();
    static QString joystickName(int index);

           // --- Config ---
    bool loadMapping(const QString& path = QString()); // пустой = defaultPath()
    bool reloadMapping();
    QString mappingPath() const              { return _path; }
    const MappingConfig& mapping() const     { return _config; }

           // --- Bindings ---
    void bindAction(Cmd cmd, ActionFn fn);
    void bindAxis  (Cmd cmd, AxisFn  fn);
    void unbind    (Cmd cmd);
    void clearBindings();

           // --- Device ---
    bool openJoystick(int index);
    bool openJoystick(const QString& name);
    void closeJoystick();
    bool isOpen() const                      { return _joy != nullptr; }
    QString joystickName() const;

    void setPollInterval(int ms);
    void start();
    void stop();
    bool isRunning() const;

           // --- raw access ---
    int  rawAxis(int axis) const;
    bool rawButton(int button) const;
    int  rawHat(int hat) const;

signals:
    void opened(const QString& name);
    void closed();
    void errorOccurred(const QString& text);

private slots:
    void poll();

private:
    struct AxisState   { double value = 0.0; bool first = true; };
    struct ButtonState { bool pressed = false; bool togglePhase = false; };

    void applyAxis  (const AxisBinding&  b, double dt);
    void applyButton(const ButtonBinding& b);
    void fire(const ActionSequence& seq);

    QString        _path;
    MappingConfig  _config;
    QTimer         _timer;
    int            _pollInterval = 16;
    int            _joyIndex     = -1;
    SDL_Joystick*  _joy          = nullptr;

    QHash<int, ActionFn> _actionFns;   // key = int(Cmd)
    QHash<int, AxisFn>   _axisFns;     // key = int(Cmd)

    QHash<int, AxisState>   _axisStates;   // key = axis index
    QHash<int, ButtonState> _buttonStates; // key = button index / 1000+hat*100+mask
};

} // namespace Vlx::Input
