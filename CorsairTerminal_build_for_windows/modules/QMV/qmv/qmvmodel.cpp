#include "qmvmodel.h"
#ifndef SUBJECT_PROJECT
#include "qmv/tools/resource.h"
#else
#include "../../src/tools/resource.h"
#endif
#include "qmv/property/valueproperty.h"
#include "qmv/property/cmdproperty.h"
#include "qmv/property/stringvalueproperty.h"
#include <QMap>
#include <QVariant>

#define VERSION         ("1.0.0.0")

#define VERSION_ID      (0xFFFFFFFFu)
#define VERSION_NAME    ("version")

#define LAST_PATH_ID    (0xFFFFFFFEu)
#define LAST_PATH_NAME  ("last_path")

#define DEF_RSC_PATH    ("rsc/")

QMVModel::QMVModel(QObject * parent, uint32_t id, QString name) :
    QObject(parent),
    id(id),
    name(name),
    values(new QMap<uint32_t, ValueProperty *>()),
    cmds(new QMap<uint32_t, CmdProperty *>()),
    values_s(new QMap<QString, ValueProperty *>()),
    cmds_s(new QMap<QString, CmdProperty *>()),
    version(new StringValueProperty(this, VERSION_ID, VERSION_NAME, VERSION)),
    last_path(new StringValueProperty(this, LAST_PATH_ID, LAST_PATH_NAME, DEF_RSC_PATH))
{
}
ValueProperty * QMVModel::value_property(const uint32_t & id){return values->value(id, nullptr);}
ValueProperty * QMVModel::value_property(const QString & name){return values_s->value(name, nullptr);}
CmdProperty * QMVModel::cmd_property(const uint32_t & id){return cmds->value(id, nullptr);}
CmdProperty * QMVModel::cmd_property(const QString & name){return cmds_s->value(name, nullptr);}
void QMVModel::insert_value_property(const uint32_t & id, const QString & name, ValueProperty * val){values->insert(id, val);values_s->insert(name, val);}
void QMVModel::insert_cmd_property(const uint32_t & id, const QString & name, CmdProperty * cmd){cmds->insert(id, cmd);cmds_s->insert(name, cmd);}
void QMVModel::update_view()
{
    load_def();
    foreach(ValueProperty * vp, values->values())
    {
        vp->update_view();
    }
}
bool QMVModel::_save(const QString & fileName)
{
    Resource rsc(name);
    for(auto p = values->begin(); p != values->end(); ++p)
    {
        rsc.set((*p)->get_name(), (*p)->get_val());
    }
    return rsc.save(fileName);
}
bool QMVModel::_load(const QString & fileName)
{
    Resource rsc(name);
    if(rsc.load(fileName))
    {
        for(auto p = values->begin(); p != values->end(); ++p)
        {
            bool ok;
            QVariant v = rsc.get((*p)->get_name(), ok);
            if(ok) (*p)->set(v);
        }
        return true;
    }
    return false;
}
void QMVModel::save(const QString & fileName)
{
    bool res = _save(fileName);
    if(res)
    {
        QString path = fileName.section('/', 0, -2).append('/');
        last_path->set(path);
    }
    QMVModel::logout("save_settings to \"" + fileName + "\" " + (res ? "success" : "error"));
}
void QMVModel::load(const QString & fileName)
{
    bool res = _load(fileName);
    if(res)
    {
        QString path = fileName.section('/', 0, -2).append('/');
        last_path->set(path);
    }
    QMVModel::logout("load_settings from \"" + fileName + "\" " + (res ? "success" : "error"));
}
void QMVModel::save_def()
{
    QString fileName = DEF_RSC_PATH + name + ".xml";
    bool res = _save(fileName);
    QMVModel::logout("save_def_settings to \"" + fileName + "\" " + (res ? "success" : "error"));
}
void QMVModel::load_def()
{
    QString fileName = DEF_RSC_PATH + name + ".xml";
    bool res = _load(fileName);
    QMVModel::logout("load_def_settings from \"" + fileName + "\" " + (res ? "success" : "error"));
}
void QMVModel::logout(const QString & s)
{
    QString _s = name + "::" + s;
    emit log(_s);
}
QMVModel::~QMVModel()
{
    save_def();
    delete values;
    delete cmds;
    delete values_s;
    delete cmds_s;
}
