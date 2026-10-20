#ifndef INOUTUINTWIDGET_H
#define INOUTUINTWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QLineEdit;

class QMVSHARED_EXPORT InoutUintWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutUintWidget(QWidget * parent = 0);
public slots:
    virtual void set_val(const QVariant &) override;
    virtual QVariant get_val() override;
protected:
    QLineEdit * val;
};

#endif // INOUTUINTWIDGET_H
