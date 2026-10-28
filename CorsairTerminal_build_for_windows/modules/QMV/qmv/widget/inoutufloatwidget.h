#ifndef INOUTUFLOATWIDGET_H
#define INOUTUFLOATWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QLineEdit;

class QMVSHARED_EXPORT InoutUfloatWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutUfloatWidget(QWidget * parent = 0);
public slots:
    virtual void set_val(const QVariant &) override;
    virtual QVariant get_val() override;
protected:
    QLineEdit * val;
};

#endif // INOUTUFLOATWIDGET_H
