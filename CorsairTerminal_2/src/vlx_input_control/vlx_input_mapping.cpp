#include "vlx_input_mapping.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStringList>

namespace Vlx::Input {

// === ActionSequence ========================================================

QString ActionSequence::toString() const {
    QStringList l;
    for (Cmd c : cmds) l << cmdKey(c);
    return l.join(QStringLiteral(", "));
}

QString ActionSequence::toDisplayString() const {
    QStringList l;
    l.reserve(cmds.size());
    for (Cmd c : cmds)
        l << cmdDisplayName(c);
    return l.join(QStringLiteral(", "));
}

ActionSequence ActionSequence::fromString(const QString& s) {
    ActionSequence out;
    const auto parts = s.split(QRegularExpression(QStringLiteral("[,\\s]+")));
    for (QString p : parts) {
        p = p.trimmed();
        if (p.isEmpty()) continue;
        const Cmd c = cmdFromKey(p, Cmd::Action_None);
        if (isRealActionCmd(c)) out.cmds.append(c);
    }
    return out;
}

QJsonArray ActionSequence::toJson() const {
    QJsonArray a;
    for (Cmd c : cmds) a.append(cmdKey(c));
    return a;
}

ActionSequence ActionSequence::fromJson(const QJsonValue& v) {
    ActionSequence out;
    if (v.isArray()) {
        for (const QJsonValue& x : v.toArray()) {
            const Cmd c = cmdFromKey(x.toString(), Cmd::Action_None);
            if (isRealActionCmd(c)) out.cmds.append(c);
        }
    } else if (v.isString()) {
        out = fromString(v.toString());
    }
    return out;
}

// === AxisBinding ===========================================================

QJsonObject AxisBinding::toJson() const {
    QJsonObject o;
    o["cmd"]         = cmdKey(cmd);
    o["axis"]        = axis;
    o["sensitivity"] = sensitivity;
    o["deadZone"]    = deadZone;
    o["smoothing"]   = smoothing;
    o["inverted"]    = inverted;
    return o;
}

AxisBinding AxisBinding::fromJson(const QJsonObject& o) {
    AxisBinding b;
    b.cmd         = cmdFromKey(o.value("cmd").toString(), Cmd::Axis_None);
    if (!isRealAxisCmd(b.cmd) && b.cmd != Cmd::Axis_None) b.cmd = Cmd::Axis_None;
    b.axis        = o.value("axis").toInt(-1);
    b.sensitivity = o.value("sensitivity").toDouble(1.0);
    b.deadZone    = o.value("deadZone").toInt(50);
    b.smoothing   = o.value("smoothing").toDouble(0.9);
    b.inverted    = o.value("inverted").toBool(false);
    return b;
}

// === ButtonBinding =========================================================

QJsonObject ButtonBinding::toJson() const {
    QJsonObject o;
    o["button"]      = button;
    o["hat"]         = hat;
    o["hatMask"]     = hatMask;
    o["press"]       = press.toJson();
    o["release"]     = release.toJson();
    o["toggle"]      = toggleEnabled;
    o["toggleA"]     = toggleA.toJson();
    o["toggleB"]     = toggleB.toJson();
    return o;
}

ButtonBinding ButtonBinding::fromJson(const QJsonObject& o) {
    ButtonBinding b;
    b.button        = o.value("button").toInt(-1);
    b.hat           = o.value("hat").toInt(-1);
    b.hatMask       = o.value("hatMask").toInt(0);
    b.press         = ActionSequence::fromJson(o.value("press"));
    b.release       = ActionSequence::fromJson(o.value("release"));
    b.toggleEnabled = o.value("toggle").toBool(false);
    b.toggleA       = ActionSequence::fromJson(o.value("toggleA"));
    b.toggleB       = ActionSequence::fromJson(o.value("toggleB"));
    return b;
}

// === JoystickMapping =======================================================

QJsonObject JoystickMapping::toJson() const {
    QJsonObject o;
    o["name"] = name;
    QJsonArray axes;
    for (const auto& a : this->axes) axes.append(a.toJson());
    QJsonArray buttons;
    for (const auto& b : this->buttons) buttons.append(b.toJson());
    o["axes"]    = axes;
    o["buttons"] = buttons;
    return o;
}

JoystickMapping JoystickMapping::fromJson(const QJsonObject& o) {
    JoystickMapping jm;
    jm.name = o.value("name").toString();
    for (const QJsonValue& v : o.value("axes").toArray())
        jm.axes.append(AxisBinding::fromJson(v.toObject()));
    for (const QJsonValue& v : o.value("buttons").toArray())
        jm.buttons.append(ButtonBinding::fromJson(v.toObject()));
    return jm;
}

// === MappingConfig =========================================================

const JoystickMapping* MappingConfig::find(const QString& name) const {
    for (const auto& jm : joysticks)
        if (jm.name == name) return &jm;
    return nullptr;
}

JoystickMapping* MappingConfig::findOrCreate(const QString& name) {
    for (auto& jm : joysticks)
        if (jm.name == name) return &jm;
    JoystickMapping jm; jm.name = name;
    joysticks.append(jm);
    return &joysticks.last();
}

void MappingConfig::mergeWith(const MappingConfig& other) {
    for (const auto& src : other.joysticks) {
        JoystickMapping* dst = findOrCreate(src.name);
        for (const auto& sa : src.axes) {
            bool replaced = false;
            for (auto& da : dst->axes) {
                if (sa.axis >= 0 && da.axis == sa.axis) { da = sa; replaced = true; break; }
            }
            if (!replaced) dst->axes.append(sa);
        }
        for (const auto& sb : src.buttons) {
            bool replaced = false;
            for (auto& db : dst->buttons) {
                if (sb.button >= 0 && db.button == sb.button)   { db = sb; replaced = true; break; }
                if (sb.hat >= 0 && db.hat == sb.hat && db.hatMask == sb.hatMask) { db = sb; replaced = true; break; }
            }
            if (!replaced) dst->buttons.append(sb);
        }
    }
}

QJsonObject MappingConfig::toJson() const {
    QJsonArray arr;
    for (const auto& jm : joysticks) {
        QJsonObject o;
        o["name"] = jm.name;
        QJsonArray axes;
        for (const auto& a : jm.axes)    axes.append(a.toJson());
        QJsonArray buttons;
        for (const auto& b : jm.buttons) buttons.append(b.toJson());
        o["axes"]    = axes;
        o["buttons"] = buttons;
        arr.append(o);
    }
    QJsonObject root;
    root["version"]   = 1;
    root["joysticks"] = arr;
    return root;
}

MappingConfig MappingConfig::fromJson(const QJsonObject& o, QString* err) {
    MappingConfig cfg;
    if (!o.value("joysticks").isArray()) {
        if (err) *err = QStringLiteral("Missing 'joysticks' array");
        return cfg;
    }
    for (const QJsonValue& v : o.value("joysticks").toArray()) {
        if (v.isObject()) cfg.joysticks.append(JoystickMapping::fromJson(v.toObject()));
    }
    return cfg;
}

bool MappingConfig::save(const QString& path, QString* err) const {
    const QFileInfo fi(path);
    if (!fi.absoluteDir().exists()) QDir().mkpath(fi.absolutePath());

    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (err) *err = f.errorString();
        return false;
    }
    f.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
    if (!f.commit()) {
        if (err) *err = f.errorString();
        return false;
    }
    return true;
}

bool MappingConfig::load(const QString& path, QString* err) {
    joysticks.clear();
    if (!QFile::exists(path)) return true;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        if (err) *err = f.errorString();
        return false;
    }
    QJsonParseError pe;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &pe);
    if (pe.error != QJsonParseError::NoError) {
        if (err) *err = pe.errorString();
        return false;
    }
    *this = fromJson(doc.object(), err);
    return true;
}

QString MappingConfig::defaultPath()
{
    static const QString path = []{
        const QString dir = QDir(QDir::currentPath())
        .filePath(QStringLiteral("vlx"));
        QDir().mkpath(dir);
        return QDir(dir).filePath(QStringLiteral("joystick_mapping.json"));
    }();
    return path;
}

} // namespace Vlx::Input
