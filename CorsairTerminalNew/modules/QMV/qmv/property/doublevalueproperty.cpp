#include "doublevalueproperty.h"
#include <QVariant>

DoubleValueProperty::DoubleValueProperty(QMVModel * model, uint32_t id, QString name, double def, double min, double max) :
    ValueProperty(model, id, name),
    val(def),
    def(def),
    min(min),
    max(max)
{
}
double DoubleValueProperty::get() const
{
    return val;
}
bool DoubleValueProperty::set_val(const QVariant & v)
{
    bool ok = true;
    double v_double = v.toDouble(&ok);
    if(!ok)
    {
        val = def;
    }
    else if(v_double < min)
    {
        val = min;
        ok = false;
    }
    else if(v_double > max)
    {
        val = max;
        ok = false;
    }
    else
    {
        val = v_double;
    }
    return ok;
}
QVariant DoubleValueProperty::get_val() const
{
    return val;
}
