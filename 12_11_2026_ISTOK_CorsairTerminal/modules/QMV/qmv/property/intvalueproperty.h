#ifndef INTVALUEPROPERTY_H
#define INTVALUEPROPERTY_H

#include "qmv/qmv_global.h"
#include "valueproperty.h"

class QMVSHARED_EXPORT IntValueProperty : public ValueProperty
{
    Q_OBJECT
public:
    explicit IntValueProperty(QMVModel * model, uint32_t id, QString name, int def, int min, int max);
    virtual int get() const;
    bool set_val(const QVariant &) override;
    QVariant get_val() const override;
protected:
    int val;
    int def;
    int min;
    int max;
};

#endif // INTVALUEPROPERTY_H
