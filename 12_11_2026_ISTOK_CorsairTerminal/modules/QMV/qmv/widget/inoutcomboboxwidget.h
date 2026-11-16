#ifndef INOUTCOMBOBOXWIDGET_H
#define INOUTCOMBOBOXWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QComboBox;

class QMVSHARED_EXPORT InoutComboBoxWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutComboBoxWidget(QWidget * parent = 0);

    virtual void connect_property(Property *);
public slots:
    void set_val(const QVariant &) override;
    QVariant get_val() override;
    void set_items(const QList<QString> &, const QList<QVariant> &, const QVariant &);
signals:
    void get_items();
private:
    QComboBox * val;
};

#endif // INOUTCOMBOBOXWIDGET_H
