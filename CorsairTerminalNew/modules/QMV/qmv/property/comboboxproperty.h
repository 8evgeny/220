#ifndef COMBOBOXPROPERTY_H
#define COMBOBOXPROPERTY_H

#include "qmv/qmv_global.h"
#include "valueproperty.h"
#include <QVariant>

class QMVSHARED_EXPORT ComboBoxProperty : public ValueProperty
{
    Q_OBJECT
public:
    explicit ComboBoxProperty(QMVModel * model, uint32_t id, QString name, QVariant def = 0);
    void set_items(const QList<QString> & texts, const QList<QVariant> & values);
    void set(const QVariant &) override;
    void update_view() override;
    bool set_val(const QVariant &) override;
    QVariant get_val() const override;
public slots:
    void view_set(const QVariant &) override;
signals:
    void items_changed(const QList<QString> &, const QList<QVariant> &, const QVariant & = "");
private:
    QList<QString> texts;
    QList<QVariant> values;
    QVariant val;
};

#endif // COMBOBOXPROPERTY_H
