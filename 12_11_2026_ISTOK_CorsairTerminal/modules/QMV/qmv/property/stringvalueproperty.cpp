#include "stringvalueproperty.h"
#include <QVariant>

StringValueProperty::StringValueProperty(QMVModel * model, uint32_t id, QString name, QString def) :
    ValueProperty(model, id, name),
    val(def),
    def(def)
{
}
QString StringValueProperty::get() const
{
    return val;
}
bool StringValueProperty::set_val(const QVariant & v)
{
    val = v.toString();
    return true;
}
QVariant StringValueProperty::get_val() const
{
    return val;
}
