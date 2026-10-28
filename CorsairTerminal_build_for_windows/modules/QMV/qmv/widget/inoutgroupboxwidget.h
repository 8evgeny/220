#ifndef INOUTGROUPBOXWIDGET_H
#define INOUTGROUPBOXWIDGET_H

#include <QGroupBox>
#include "mv/widget/setgetwidget.h"

class InoutGroupBoxWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutGroupBoxWidget(QWidget * parent = 0);

    void set_smallsize() override;
    void set_bigsize() override;
public slots:
    void set_val(const QVariant &) override;
    QVariant get_val() override;
private:
    QGroupBox * val;
};

#endif // INOUTGROUPBOXWIDGET_H
