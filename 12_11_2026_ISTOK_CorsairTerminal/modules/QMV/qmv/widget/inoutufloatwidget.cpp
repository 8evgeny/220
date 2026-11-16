#include "inoutufloatwidget.h"
#include "ui_inoutwidget.h"
#include "keyfilter.h"
#include "enterkeyfilter.h"
#include <QLineEdit>

InoutUfloatWidget::InoutUfloatWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QLineEdit(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    val->setFixedHeight(20);
    ui->root_layout->insertWidget(1, val);

    val->setToolTip("ufloat");
    val->installEventFilter(new KeyFilter(this, KeyFilter::UFLOAT));

    EnterKeyFilter * ekf = new EnterKeyFilter(this);
    val->installEventFilter(ekf);
    connect(ekf, SIGNAL(enter_key_pressed()), SLOT(set_clicked()));
}
void InoutUfloatWidget::set_val(const QVariant & v)
{
    bool ok;
    val->setText(QString::number(v.toDouble(&ok)));
}
QVariant InoutUfloatWidget::get_val()
{
    bool ok;
    return val->text().toDouble(&ok);
}
