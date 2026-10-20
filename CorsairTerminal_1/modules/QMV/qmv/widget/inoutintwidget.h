#ifndef INOUTINTWIDGET_H
#define INOUTINTWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QLineEdit;

class QMVSHARED_EXPORT InoutIntWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutIntWidget(QWidget * parent = 0);
public slots:
    virtual void set_val(const QVariant &) override;
    virtual QVariant get_val() override;
protected:
    QLineEdit * val;
};

#endif // INOUTINTWIDGET_H
