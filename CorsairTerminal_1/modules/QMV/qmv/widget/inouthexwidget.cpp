#include "inouthexwidget.h"
#include "ui_inoutwidget.h"
#include "keyfilter.h"
#include "enterkeyfilter.h"
#include <QLineEdit>

InoutHexWidget::InoutHexWidget(QWidget * parent):
    SetGetWidget(parent)
{
    pr = new QLineEdit(this);
    pr->setDisabled(true);
    pr->setObjectName("pr");
    pr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pr->setFixedSize(18, 20);
    pr->setText("0x");
    ui->root_layout->insertWidget(1, pr);

    val = new QLineEdit(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    val->setFixedHeight(20);
    ui->root_layout->insertWidget(2, val);

    val->setToolTip("hex");
    val->installEventFilter(new KeyFilter(this, KeyFilter::HEX));

    EnterKeyFilter * ekf = new EnterKeyFilter(this);
    val->installEventFilter(ekf);
    connect(ekf, SIGNAL(enter_key_pressed()), SLOT(set_clicked()));
}
void InoutHexWidget::set_val(const QVariant & v)
{
    bool ok;
    val->setText(QString::number(v.toUInt(&ok), 16));
}
QVariant InoutHexWidget::get_val()
{
    bool ok;
    return val->text().toUInt(&ok, 16);
}
