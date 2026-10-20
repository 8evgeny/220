#ifndef VALUEPROPERTY_H
#define VALUEPROPERTY_H

#include "qmv/qmv_global.h"
#include "property.h"

class QMVSHARED_EXPORT ValueProperty : public Property
{
    Q_OBJECT
public:
    explicit ValueProperty(QMVModel * model, uint32_t id, QString name);
    virtual void update_view();
    virtual void set(const QVariant &);
    virtual bool set_val(const QVariant &) = 0;
    virtual QVariant get_val() const = 0;
public slots:
    virtual void view_set(const QVariant &);
    virtual void view_get();
signals:
    void value_changed(const QVariant &);
};

#endif // VALUEPROPERTY_H
