#ifndef INOUTFLOATWIDGET_H
#define INOUTFLOATWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QLineEdit;

class QMVSHARED_EXPORT InoutFloatWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutFloatWidget(QWidget * parent = 0);
public slots:
    virtual void set_val(const QVariant &) override;
    virtual QVariant get_val() override;
protected:
    QLineEdit * val;
};

#endif // INOUTFLOATWIDGET_H
