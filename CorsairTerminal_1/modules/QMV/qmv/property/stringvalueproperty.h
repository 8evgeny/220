#ifndef STRINGVALUEPROPERTY_H
#define STRINGVALUEPROPERTY_H

#include "qmv/qmv_global.h"
#include "valueproperty.h"

class QMVSHARED_EXPORT StringValueProperty : public ValueProperty
{
    Q_OBJECT
public:
    explicit StringValueProperty(QMVModel * model, uint32_t id, QString name, QString def = "empty");
    virtual QString get() const;
    bool set_val(const QVariant &) override;
    QVariant get_val() const override;
protected:
    QString val;
    QString def;
};

#endif // STRINGVALUEPROPERTY_H
