#include "intvalueproperty.h"
#include <QVariant>

IntValueProperty::IntValueProperty(QMVModel * model, uint32_t id, QString name, int def, int min, int max) :
    ValueProperty(model, id, name),
    val(def),
    def(def),
    min(min),
    max(max)
{
}
int IntValueProperty::get() const
{
    return val;
}
bool IntValueProperty::set_val(const QVariant & v)
{
    bool ok = true;
    int v_int = v.toInt(&ok);
    if(!ok)
    {
        val = def;
    }
    else if(v_int < min)
    {
        val = min;
        ok = false;
    }
    else if(v_int > max)
    {
        val = max;
        ok = false;
    }
    else
    {
        val = v_int;
    }
    return ok;
}
QVariant IntValueProperty::get_val() const
{
    return val;
}
