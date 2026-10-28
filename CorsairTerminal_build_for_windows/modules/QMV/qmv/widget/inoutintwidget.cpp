#include "inoutintwidget.h"
#include "ui_inoutwidget.h"
#include "keyfilter.h"
#include "enterkeyfilter.h"
#include <QLineEdit>

InoutIntWidget::InoutIntWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QLineEdit(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    val->setFixedHeight(20);
    ui->root_layout->insertWidget(1, val);

    val->setToolTip("int");
    val->installEventFilter(new KeyFilter(this, KeyFilter::INT));

    EnterKeyFilter * ekf = new EnterKeyFilter(this);
    val->installEventFilter(ekf);
    connect(ekf, SIGNAL(enter_key_pressed()), SLOT(set_clicked()));
}
void InoutIntWidget::set_val(const QVariant & v)
{
    bool ok;
    val->setText(QString::number(v.toInt(&ok), 10));
}
QVariant InoutIntWidget::get_val()
{
    bool ok;
    return val->text().toInt(&ok, 10);
}
