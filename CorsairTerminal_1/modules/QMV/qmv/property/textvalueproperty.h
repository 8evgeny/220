#ifndef TEXTVALUEPROPERTY_H
#define TEXTVALUEPROPERTY_H

#include "qmv/qmv_global.h"
#include "valueproperty.h"

class QMVSHARED_EXPORT TextValueProperty : public ValueProperty
{
    Q_OBJECT
public:
    explicit TextValueProperty(QMVModel * model, uint32_t id, QString name, QString def = "empty");
    virtual void append(const QString &);
    virtual void clear();
    virtual QString get() const;
    bool set_val(const QVariant &) override;
    QVariant get_val() const override;
signals:
    void value_appended(const QVariant &);
    void value_cleared();
protected:
    QString val;
    QString def;
};

#endif // TEXTVALUEPROPERTY_H
