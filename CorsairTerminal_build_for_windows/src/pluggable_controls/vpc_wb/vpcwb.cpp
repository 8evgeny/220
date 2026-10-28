#include "vpcwb.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QDebug>

namespace VpcWB {

static void mapFromJson(const QJsonObject &jo, Joystick *jstck, void (VpcWB::Joystick::*mptr)(int, int), int maxCtrlNum)
{
    for (const auto &key : jo.keys()) {
        bool ok;
        int origId = key.toInt(&ok);
        if (!ok) {
            qWarning() << Q_FUNC_INFO << "not a number-key:" << key;
            continue;
        }

        if (origId < 0 || origId >= maxCtrlNum) {
            qWarning() << Q_FUNC_INFO << "number-key exceeds max." << origId << ">=" << maxCtrlNum;
            continue;
        }
        auto v = jo.value(key);
        if (!v.isDouble()) {
            qWarning() << Q_FUNC_INFO << "value is not a number. key:" << key;
            continue;
        }

        auto id = v.toInt(0x4FFFFFFF);
        if (id == 0x4FFFFFFF) {
            qWarning() << Q_FUNC_INFO << "Could not convert value to integer. key:" << key;
            continue;
        }

        (jstck->*mptr)(origId, id);
    };
}

static bool mapFromJson(Joystick *joystick, const QByteArray &jsonData)
{
    QJsonParseError err;

    auto jd = QJsonDocument::fromJson(jsonData, &err);

    if (err.error != QJsonParseError::ParseError::NoError) {
        qCritical() << Q_FUNC_INFO << "json parser error:" << err.errorString()
                    << "offset:" << err.offset;
        return false;
    }

    auto jo = jd.object();
    auto keys = jo.keys();

    if (jo.contains("axes")) {
        auto v = jo.value("axes");
        if (v.isObject())
            mapFromJson(v.toObject(), joystick, &Joystick::mapAxis, Joystick::maxAxisNum());
    }

    if (jo.contains("buttons")) {
        auto v = jo.value("buttons");
        if (v.isObject())
            mapFromJson(v.toObject(), joystick, &Joystick::mapButton, Joystick::maxButtonNum());
    }

    return true;
}

bool mapFromJsonString(Joystick *joystick, const QString &jsonString)
{
    return mapFromJson(joystick, jsonString.toUtf8());
}

bool mapFromJsonFileName(Joystick *joystick, const QString &fileNamePath)
{
    QFile f(fileNamePath);

    if (!f.open(QIODevice::ReadOnly)) {
        qCritical() << Q_FUNC_INFO << "Unable to open:" << fileNamePath
                    << "due to error:" << f.errorString();
        return false;
    }

    return mapFromJson(joystick, f.readAll());
}

} // namespace VpcWB
