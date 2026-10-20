#include "inoutfloatwidget.h"
#include "ui_inoutwidget.h"
#include "keyfilter.h"
#include "enterkeyfilter.h"
#include <QLineEdit>
#include <QDebug>

InoutFloatWidget::InoutFloatWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QLineEdit(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    val->setFixedHeight(20);
    ui->root_layout->insertWidget(1, val);

    val->setToolTip("float");
    val->installEventFilter(new KeyFilter(this, KeyFilter::FLOAT));

    EnterKeyFilter * ekf = new EnterKeyFilter(this);
    val->installEventFilter(ekf);
    connect(ekf, SIGNAL(enter_key_pressed()), SLOT(set_clicked()));
}

void InoutFloatWidget::set_val(const QVariant & v)
{
    bool ok;

    if (v == std::numeric_limits<double>::infinity()) {
        val->setText("∞");
    } else if (v == -std::numeric_limits<double>::infinity()) {
        val->setText("-∞");
    } else {
        val->setText(QString::number(v.toDouble(&ok)));
    }
}

QVariant InoutFloatWidget::get_val()
{
    bool ok;
    return val->text().toDouble(&ok);
}
