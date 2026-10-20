#include "outplaintextwidget.h"
#include "ui_inoutwidget.h"
#include "qmv/property/property.h"
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>

OutPlainTextWidget::OutPlainTextWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QPlainTextEdit(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    val->setMinimumHeight(20);
    val->setReadOnly(true);

    ui->root->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->root->setMinimumHeight(26);
    ui->root->setMaximumHeight(16667);

    label->hide();
    set->hide();
    get->hide();

    ui->root_layout->insertWidget(1, val);
}
void OutPlainTextWidget::connect_property(Property * p)
{
    connect(p, SIGNAL(value_appended(QVariant)), this, SLOT(append_val(QVariant)));
    connect(p, SIGNAL(value_cleared()), this, SLOT(clear_val()));

    SetGetWidget::connect_property(p);
}
void OutPlainTextWidget::set_val(const QVariant & v)
{
    val->clear();
    val->appendPlainText(v.toString());
}
void OutPlainTextWidget::append_val(const QVariant & v)
{
    val->appendPlainText(v.toString());
}
void OutPlainTextWidget::clear_val()
{
    val->clear();
}
QVariant OutPlainTextWidget::get_val()
{
    return val->toPlainText();
}
