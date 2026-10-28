#include "vlx_input_cmd.hpp"

#include <QHash>
#include <QDebug>

namespace Vlx::Input {

namespace {

struct CmdInfo {
    QString displayName;
    QString description;
};

// ---------------------------------------------------------------------------
// ДЕКЛАРАТИВНАЯ ТАБЛИЦА ПЕРЕВОДОВ
//
// Формат: { Cmd::XXX, { "Короткое имя для UI", "Описание (tooltip)" } }
//
// Добавили новую Cmd в enum? Просто добавьте строку сюда.
// Если забыли — в консоль уйдёт qWarning при первом обращении, а UI
// покажет английский ключ вместо русского имени.
// ---------------------------------------------------------------------------

const QHash<Cmd, CmdInfo>& cmdInfoTable() {
    static const QHash<Cmd, CmdInfo> t = []{
        QHash<Cmd, CmdInfo> m = {
            { Cmd::None,                    { QStringLiteral("—"), QStringLiteral("Не задано") } },
            { Cmd::Axis_None,               { QStringLiteral("—"), QStringLiteral("Ось не задана") } },
            { Cmd::Axis_Pitch,              { QStringLiteral("УГЛ.МСТ"), QStringLiteral("Поворот по углу места") } },
            { Cmd::Axis_Yaw,                { QStringLiteral("АЗИМУТ"), QStringLiteral("Поворот по азимуту") } },
            { Cmd::Axis_TrackingTargetSize, { QStringLiteral("РАМКА.ЦЕЛЬ"), QStringLiteral("Изменение рамки цели") } },

            { Cmd::Action_None, { QStringLiteral("-"), QStringLiteral("Действие не задано") } },
            { Cmd::Action_TrackingOn, { QStringLiteral("Сопр.ВКЛ"), QStringLiteral("Включение сопровождения") } },
            { Cmd::Action_TrackingCapture, { QStringLiteral("Сопр.ЗАХВАТ"), QStringLiteral("Захват цели на сопровождение") } },
            { Cmd::Action_TrackingOff, { QStringLiteral("Сопр.ВЫКЛ"), QStringLiteral("Выключение сопровождения") } },
            { Cmd::Action_ZoomIn, { QStringLiteral("Зум.ПРИБЛ."), QStringLiteral("Приближение") } },
            { Cmd::Action_ZoomOut, { QStringLiteral("Зум.ОТД."), QStringLiteral("Отдаление") } },
            { Cmd::Action_ZoomStop, { QStringLiteral("Зум.СТОП"), QStringLiteral("Остановка зума") } },
            { Cmd::Action_FocusNear, { QStringLiteral("Фокус.БЛИЖ"), QStringLiteral("Изменение фокуса на ближний объект") } },
            { Cmd::Action_FocusFar, { QStringLiteral("Фокус.ДАЛЬ"), QStringLiteral("Изменение фокуса на дальний объект") } },
            { Cmd::Action_FocusStop, { QStringLiteral("Фокус.СТОП"), QStringLiteral("Остановка фокуса") } },
            { Cmd::Action_FocusAuto, { QStringLiteral("Фокус.АВТО"), QStringLiteral("Автофокус") } },
            { Cmd::Action_LrfOn, { QStringLiteral("ЛДЦ.ВКЛ"), QStringLiteral("Включение дальномера") } },
            { Cmd::Action_LrfOff, { QStringLiteral("ЛДЦ.ВЫКЛ"), QStringLiteral("Выключение дальномера") } },
            { Cmd::Action_LrfSingle, { QStringLiteral("ЛДЦ.ЗАМЕР"), QStringLiteral("Единичный замер дальности") } },
            { Cmd::Action_LrfStart, { QStringLiteral("ЛДЦ.СТАРТ"), QStringLiteral("Запуска частотного режима измерения") } },
            { Cmd::Action_LrfStop, { QStringLiteral("ЛДЦ.СТОП"), QStringLiteral("Стоп частотного режима измерения") } },
            { Cmd::Action_ChannelToggle, { QStringLiteral("КАНАЛ.ПЕРЕКЛ"), QStringLiteral("Переключение между каналами") } },
            { Cmd::Action_ChannelTv, { QStringLiteral("КАНАЛ.ТВ"), QStringLiteral("Включение канала ТВ") } },
            { Cmd::Action_ChannelIr, { QStringLiteral("КАНАЛ.ТПВ"), QStringLiteral("Включение канала ТПВ") } },
            { Cmd::Action_GspToggleStabilization, { QStringLiteral("ГСП.СТАБ.ПЕРЕКЛ"), QStringLiteral("Переключение режима стабилиазции") } },
            { Cmd::Action_GspStabilizationOn, { QStringLiteral("ГСП.СТАБ.ВКЛ"), QStringLiteral("Включение стабилизации") } },
            { Cmd::Action_GspStabilizationOff, { QStringLiteral("ГСП.СТАБ.ВЫКЛ"), QStringLiteral("Выключение стабилизации") } },
            { Cmd::Action_GspPark, { QStringLiteral("ГСП.ПАРК"), QStringLiteral("Парковка ГСП") } },
            { Cmd::Action_GspGotoZero, { QStringLiteral("ГСП.НОЛЬ"), QStringLiteral("Вывод ГСП в нулевую позицию") } },
            };

               // Проверка покрытия: все ли значения enum'а описаны?
        const QMetaEnum me = QMetaEnum::fromType<Cmd>();
        for (int i = 0; i < me.keyCount(); ++i) {
            const Cmd c = static_cast<Cmd>(me.value(i));
            if (!m.contains(c))
                qWarning() << "Cmd missing UI info:" << me.key(i)
                           << "— will fall back to cmdKey()";
        }
        return m;
    }();
    return t;
}

} // namespace

QString cmdDisplayName(Cmd c) {
    const auto& t = cmdInfoTable();
    auto it = t.find(c);
    if (it != t.end() && !it->displayName.isEmpty())
        return it->displayName;
    return cmdKey(c);   // fallback — английский ключ
}

QString cmdDescription(Cmd c) {
    const auto& t = cmdInfoTable();
    auto it = t.find(c);
    if (it != t.end())
        return it->description;
    return {};
}

} // namespace Vlx::Input
