#include "inoutstringwidget.h"
#include "ui_inoutwidget.h"
#include "keyfilter.h"
#include "enterkeyfilter.h"
#include <QLineEdit>

InoutStringWidget::InoutStringWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QLineEdit(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    val->setFixedHeight(20);
    ui->root_layout->insertWidget(1, val);

    val->setToolTip("string");
    val->installEventFilter(new KeyFilter(this, KeyFilter::STRING));

    EnterKeyFilter * ekf = new EnterKeyFilter(this);
    val->installEventFilter(ekf);
    connect(ekf, SIGNAL(enter_key_pressed()), SLOT(set_clicked()));
}
void InoutStringWidget::set_val(const QVariant & v)
{
    val->setText(v.toString());
}
QVariant InoutStringWidget::get_val()
{
    return val->text();
}
