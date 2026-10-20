#ifndef DOUBLEVALUEPROPERTY_H
#define DOUBLEVALUEPROPERTY_H

#include "qmv/qmv_global.h"
#include "valueproperty.h"

class QMVSHARED_EXPORT DoubleValueProperty : public ValueProperty
{
    Q_OBJECT

public:
    explicit DoubleValueProperty(QMVModel *model, uint32_t id, QString name,
                                 double def,
                                 double min = -std::numeric_limits<double>::infinity(),
                                 double max = std::numeric_limits<double>::infinity());
    virtual double get() const;
    bool set_val(const QVariant &) override;
    QVariant get_val() const override;
protected:
    double val;
    double def;
    double min;
    double max;
};

#endif // DOUBLEVALUEPROPERTY_H
