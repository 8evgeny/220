#include "comboboxproperty.h"
#include "qmv/qmvmodel.h"
#include <QTimer>
#include <QVariant>

ComboBoxProperty::ComboBoxProperty(QMVModel * model, uint32_t id, QString name, QVariant def) :
    ValueProperty(model, id, name),
    val(def)
{
}
bool ComboBoxProperty::set_val(const QVariant & val)
{
    this->val = val;
    return true;
}
QVariant ComboBoxProperty::get_val() const
{
    return val;
}
void ComboBoxProperty::update_view()
{
    emit items_changed(texts, values, val);
}
void ComboBoxProperty::set_items(const QList<QString> & texts, const QList<QVariant> & values)
{
    this->texts = texts;
    this->values = values;
    emit items_changed(texts, values, val);
}
void ComboBoxProperty::set(const QVariant & v)
{
    set_val(v);
    emit value_changed(val);
}
void ComboBoxProperty::view_set(const QVariant & v)
{
    if(timeout_timer->isActive()) return;
    if(!set_val(v))
    {
        emit value_changed(val);
    }
    model->set(id);
}
