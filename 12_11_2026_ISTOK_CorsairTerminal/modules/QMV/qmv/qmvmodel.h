#ifndef MVMODEL_H
#define MVMODEL_H

#include "qmv/qmv_global.h"
#include <QObject>
class StringValueProperty;
class CmdProperty;
class ValueProperty;

class QMVSHARED_EXPORT QMVModel : public QObject
{
    Q_OBJECT
public:
    explicit QMVModel(QObject * parent = 0, uint32_t id = 0, QString name = "");
    virtual ~QMVModel();

    virtual void update_view();

    uint32_t get_id(){return id;}
    QString get_name(){return name;}

    ValueProperty * value_property(const uint32_t & id);
    ValueProperty * value_property(const QString & name);
    CmdProperty * cmd_property(const uint32_t & id);
    CmdProperty * cmd_property(const QString & name);

    void insert_value_property(const uint32_t & id, const QString & name, ValueProperty * val);
    void insert_cmd_property(const uint32_t & id, const QString & name, CmdProperty * cmd);

    void save(const QString &);
    void load(const QString &);

    virtual void set(const uint32_t & id){Q_UNUSED(id);}
    virtual void get(const uint32_t & id){Q_UNUSED(id);}
    virtual void cmd(const uint32_t & id){Q_UNUSED(id);}

signals:
    void warning(const QString &, const QString &);
    void information(const QString &, const QString &);
    void critical(const QString &, const QString &);
    void log(const QString &);

public slots:

protected:
    void save_def();
    void load_def();
    virtual void logout(const QString &);

private:
    bool _save(const QString &);
    bool _load(const QString &);

protected:
    const uint32_t id;
    const QString name;

    QMap<uint32_t, ValueProperty *> * values;
    QMap<uint32_t, CmdProperty *> * cmds;

    QMap<QString, ValueProperty *> * values_s;
    QMap<QString, CmdProperty *> * cmds_s;

    StringValueProperty * version;
    StringValueProperty * last_path;
};

#endif // MVMODEL_H
