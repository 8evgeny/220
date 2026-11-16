#ifndef INOUTSTRINGWIDGET_H
#define INOUTSTRINGWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QLineEdit;

class QMVSHARED_EXPORT InoutStringWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutStringWidget(QWidget * parent = 0);
public slots:
    virtual void set_val(const QVariant &) override;
    virtual QVariant get_val() override;
protected:
    QLineEdit * val;
};

#endif // INOUTSTRINGWIDGET_H
