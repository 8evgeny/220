#include "property.h"
#include "qmv/qmvmodel.h"
#include <QTimer>

Property::Property(QMVModel * model, uint32_t id, QString name) :
    QObject(model),
    model(model),
    id(id),
    name(name),
    timeout_timer(new QTimer(this))
{
    connect(timeout_timer, SIGNAL(timeout()), this, SLOT(timeout_slot()));
}
void Property::start_wait(const int & timeout)
{
    if(timeout > 0) timeout_timer->start(timeout);
    emit start_waiting();
}
void Property::stop_wait()
{
    timeout_timer->stop();
    emit stop_waiting();
}
void Property::timeout_slot()
{
    timeout_timer->stop();
    emit set_error();
    emit stop_waiting();
}
void Property::waiting_stopped()
{
    timeout_timer->stop();
    emit stop_waiting();
}
