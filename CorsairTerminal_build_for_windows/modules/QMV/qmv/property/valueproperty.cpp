#include "valueproperty.h"
#include "qmv/qmvmodel.h"
#include <QTimer>
#include <QVariant>

ValueProperty::ValueProperty(QMVModel * model, uint32_t id, QString name) :
    Property(model, id, name)
{
    model->insert_value_property(id, name, this);
}
void ValueProperty::set(const QVariant & v)
{
    set_val(v);
    emit value_changed(get_val());
}
void ValueProperty::update_view()
{
    emit value_changed(get_val());
}
void ValueProperty::view_set(const QVariant & v)
{
    if(timeout_timer->isActive()) return;
    if(!set_val(v))
    {
        emit value_changed(get_val());
    }
    model->set(id);
}
void ValueProperty::view_get()
{
    if(timeout_timer->isActive()) return;
    model->get(id);
    update_view();
}
