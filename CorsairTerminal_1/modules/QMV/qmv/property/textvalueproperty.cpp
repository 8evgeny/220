#include "textvalueproperty.h"
#include <QVariant>

TextValueProperty::TextValueProperty(QMVModel * model, uint32_t id, QString name, QString def) :
    ValueProperty(model, id, name),
    val(def),
    def(def)
{
}
void TextValueProperty::append(const QString & v)
{
    QString tmp = val + "\n" + v;
    set(tmp);
}
void TextValueProperty::clear()
{
    QString tmp = "";
    set(tmp);
}
QString TextValueProperty::get() const
{
    return val;
}
bool TextValueProperty::set_val(const QVariant & v)
{
    val = v.toString();
    return true;
}
QVariant TextValueProperty::get_val() const
{
    return val;
}
