#include "inoutcheckboxwidget.h"
#include "ui_inoutwidget.h"
#include <QCheckBox>

InoutCheckBoxWidget::InoutCheckBoxWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QCheckBox(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    val->setFixedSize(20, 20);

    ui->root_layout->insertWidget(1, val);

    connect(val, SIGNAL(clicked()), this, SLOT(set_clicked()));
}
void InoutCheckBoxWidget::set_val(const QVariant & v)
{
    val->setChecked(v.toBool());
}
QVariant InoutCheckBoxWidget::get_val()
{
    return val->isChecked();
}
