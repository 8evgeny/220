#ifndef BOOLEANVALUEPROPERTY_H
#define BOOLEANVALUEPROPERTY_H

#include "qmv/qmv_global.h"
#include "valueproperty.h"

class QMVSHARED_EXPORT BooleanValueProperty : public ValueProperty
{
    Q_OBJECT
public:
    explicit BooleanValueProperty(QMVModel * model, uint32_t id, QString name, bool def);
    virtual bool get() const;
    bool set_val(const QVariant &) override;
    QVariant get_val() const override;
protected:
    bool val;
    bool def;
};

#endif // BOOLEANVALUEPROPERTY_H
