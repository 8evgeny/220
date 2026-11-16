#ifndef INOUTCHECKBOXWIDGET_H
#define INOUTCHECKBOXWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QCheckBox;

class QMVSHARED_EXPORT InoutCheckBoxWidget : public SetGetWidget
{
    Q_OBJECT
public:
    InoutCheckBoxWidget(QWidget * parent = 0);
public slots:
    void set_val(const QVariant &) override;
    QVariant get_val() override;
private:
    QCheckBox * val;
};

#endif // INOUTCHECKBOXWIDGET_H
