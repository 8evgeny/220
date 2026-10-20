#ifndef INOUTBINWIDGET_H
#define INOUTBINWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QLineEdit;

class QMVSHARED_EXPORT InoutBinWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutBinWidget(QWidget * parent = 0);
public slots:
    virtual void set_val(const QVariant &) override;
    virtual QVariant get_val() override;
protected:
    QLineEdit * val;
    QLineEdit * pr;
};

#endif // INOUTBINWIDGET_H
