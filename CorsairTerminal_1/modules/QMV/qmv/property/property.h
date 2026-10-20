#ifndef PROPERTY_H
#define PROPERTY_H

#include "qmv/qmv_global.h"
#include <QObject>
class QTimer;
class QMVModel;

class QMVSHARED_EXPORT Property : public QObject
{
    Q_OBJECT
public:
    explicit Property(QMVModel * model, uint32_t id, QString name);
    virtual ~Property(){}

    uint32_t get_id(){return id;}
    QString get_name(){return name;}

    void start_wait(const int & = 0);
    void stop_wait();

public slots:
    virtual void waiting_stopped();

private slots:
    void timeout_slot();

signals:
    void set_success();
    void set_error();
    void set_info();
    void set_warning();
    void set_neutral();
    void set_disabled();
    void set_enabled();

    void start_waiting();
    void stop_waiting();

    void set_text(const QString &);

protected:
    QMVModel * model;
    const uint32_t id;
    const QString name;
    QTimer * timeout_timer;
};

#endif // PROPERTY_H
