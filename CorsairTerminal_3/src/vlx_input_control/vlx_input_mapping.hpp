#pragma once

#include "vlx_input_cmd.hpp"

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace Vlx::Input {

/// Упорядоченная последовательность Action_*
struct ActionSequence {
    QList<Cmd> cmds;

    bool isEmpty() const { return cmds.isEmpty(); }
    void clear()          { cmds.clear(); }

    QString toString() const;                // "Action_Fire, Action_Boost"  (для JSON и QLineEdit)
    QString toDisplayString() const;         // "Огонь, Ускорение"            (для UI-списков)
    static ActionSequence fromString(const QString& s);

    QJsonArray toJson() const;
    static ActionSequence fromJson(const QJsonValue& v);
};

/// Один канал оси джойстика.
struct AxisBinding {
    Cmd    cmd         = Cmd::Axis_None;   // Axis_*
    int    axis        = -1;               // SDL axis index; -1 = не назначено
    double sensitivity = 1.0;              // коэффициент чувствительности
    int    deadZone    = 50;               // ‰ от полного диапазона (0..1000)
    double smoothing   = 0.9;              // коэф. exp-фильтра (0..1)
    bool   inverted    = false;

    bool isBound() const { return axis >= 0 && cmd != Cmd::Axis_None; }

    QJsonObject toJson() const;
    static AxisBinding fromJson(const QJsonObject& o);
};

/// Кнопка/хэт. Может иметь press/release и toggle-режим с двумя группами.
struct ButtonBinding {
    int button  = -1;       // SDL button index; -1 = не назначено
    int hat     = -1;       // SDL hat index; -1 = не хэт
    int hatMask = 0;        // SDL_HAT_* mask

    ActionSequence press;
    ActionSequence release;

    bool           toggleEnabled = false;
    ActionSequence toggleA;
    ActionSequence toggleB;

    bool isBound() const { return button >= 0 || hat >= 0; }

    QJsonObject toJson() const;
    static ButtonBinding fromJson(const QJsonObject& o);
};

/// Настройки одного джойстика (идентификация по SDL_JoystickName).
struct JoystickMapping {
    QString            name;
    QList<AxisBinding>   axes;
    QList<ButtonBinding> buttons;

    QJsonObject toJson() const;
    static JoystickMapping fromJson(const QJsonObject& o);
};

/// Активный конфиг (несколько джойстиков).
struct MappingConfig {
    QList<JoystickMapping> joysticks;

    const JoystickMapping* find(const QString& name) const;
    JoystickMapping*       findOrCreate(const QString& name);

    /// Слияние: элементы из other перекрывают совпадающие по ключу, остальные добавляются.
    void mergeWith(const MappingConfig& other);

    QJsonObject toJson() const;
    static MappingConfig fromJson(const QJsonObject& o, QString* err = nullptr);

    bool save(const QString& path, QString* err = nullptr) const;
    bool load(const QString& path, QString* err = nullptr);

    /// <app dir>/joystick_mapping.json
    static QString defaultPath();
};

} // namespace Vlx::Input
