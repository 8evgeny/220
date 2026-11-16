#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QMetaEnum>

namespace Vlx::Input {

Q_NAMESPACE

enum class Cmd {
    None = 0,

    // --- Оси (обрабатываются как значения) ---
    Axis_None = 1000,
    Axis_Pitch,
    Axis_Yaw,
    Axis_TrackingTargetSize,

    // --- Разовые действия ---
    Action_None = 2000,
    Action_TrackingOn,
    Action_TrackingCapture,
    Action_TrackingOff,
    Action_ZoomIn,
    Action_ZoomOut,
    Action_ZoomStop,
    Action_FocusNear,
    Action_FocusFar,
    Action_FocusStop,
    Action_FocusAuto,
    Action_LrfOn,
    Action_LrfOff,
    Action_LrfSingle,
    Action_LrfStart,
    Action_LrfStop,
    Action_ChannelToggle,
    Action_ChannelTv,
    Action_ChannelIr,
    Action_GspToggleStabilization,
    Action_GspStabilizationOn,
    Action_GspStabilizationOff,
    Action_GspPark,
    Action_GspGotoZero
};

Q_ENUM_NS(Cmd)

// ---------------------------------------------------------------------------
// qHash для QHash<Cmd, ...> / QSet<Cmd>
// Qt5 не генерирует его автоматически для enum class.
// ADL найдёт эту перегрузку, т.к. Cmd и qHash в одном namespace.
// ---------------------------------------------------------------------------
inline uint qHash(Cmd c, uint seed = 0) noexcept {
    return ::qHash(static_cast<int>(c), seed);
}

// ---------------------------------------------------------------------------
// Метаданные для UI (русские display name + описание).
//
// Источник — декларативная таблица QHash<Cmd, CmdInfo> в vlx_input_cmd.cpp.
// В json и во внутренних структурах маппинга ВСЕГДА используется cmdKey().
// cmdDisplayName/cmdDescription — только для отображения в виджетах.
// ---------------------------------------------------------------------------

QString cmdDisplayName(Cmd c);   // короткое имя для combo/list/table/menu
QString cmdDescription(Cmd c);   // расширенное описание (tooltip)

// ---------------------------------------------------------------------------
// Хелперы на базе QMetaEnum (без изменений)
// ---------------------------------------------------------------------------

inline QString cmdKey(Cmd c) {
    const char* k = QMetaEnum::fromType<Cmd>().valueToKey(static_cast<int>(c));
    return k ? QString::fromLatin1(k) : QString();
}

inline bool isAxisCmd(Cmd c)   { return cmdKey(c).startsWith(QLatin1String("Axis_"));   }
inline bool isActionCmd(Cmd c) { return cmdKey(c).startsWith(QLatin1String("Action_")); }

inline bool isRealAxisCmd(Cmd c)   { return isAxisCmd(c)   && c != Cmd::Axis_None;   }
inline bool isRealActionCmd(Cmd c) { return isActionCmd(c) && c != Cmd::Action_None; }

inline QList<Cmd> allCmds() {
    const QMetaEnum me = QMetaEnum::fromType<Cmd>();
    QList<Cmd> out;
    out.reserve(me.keyCount());
    for (int i = 0; i < me.keyCount(); ++i)
        out.append(static_cast<Cmd>(me.value(i)));
    return out;
}

inline QList<Cmd> axisCmds() {
    QList<Cmd> out;
    for (Cmd c : allCmds()) if (isRealAxisCmd(c)) out.append(c);
    return out;
}

inline QList<Cmd> actionCmds() {
    QList<Cmd> out;
    for (Cmd c : allCmds()) if (isRealActionCmd(c)) out.append(c);
    return out;
}

inline Cmd cmdFromKey(const QString& key, Cmd fallback = Cmd::None) {
    bool ok = false;
    const int v = QMetaEnum::fromType<Cmd>().keyToValue(key.toLatin1().constData(), &ok);
    return ok ? static_cast<Cmd>(v) : fallback;
}

} // namespace Vlx::Input
