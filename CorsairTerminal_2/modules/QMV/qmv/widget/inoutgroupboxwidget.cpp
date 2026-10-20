#include "inoutgroupboxwidget.h"
#include <QVariant>
#include <QRadioButton>
#include <QVBoxLayout>

InoutGroupBoxWidget::InoutGroupBoxWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QGroupBox(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->root_layout->insertWidget(1, val);
    QRadioButton * rb0 = new QRadioButton("rb0", this);
    QRadioButton * rb1 = new QRadioButton("rb1", this);
    QVBoxLayout * vbox = new QVBoxLayout(this);
    vbox->addWidget(rb0);
    vbox->addWidget(rb1);
    val->setLayout(vbox);
}
void InoutGroupBoxWidget::set_smallsize()
{
    val->setFixedHeight(20);
    SetGetWidget::set_smallsize();
}
void InoutGroupBoxWidget::set_bigsize()
{
    val->setFixedHeight(40);
    SetGetWidget::set_bigsize();
}
void InoutGroupBoxWidget::set_val(const QVariant &)
{
}
QVariant InoutGroupBoxWidget::get_val()
{
    return 0;
}
