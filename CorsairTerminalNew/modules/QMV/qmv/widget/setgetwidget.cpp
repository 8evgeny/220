#include "setgetwidget.h"
#include "ui_inoutwidget.h"
#include "qmv/property/property.h"
#include <QLabel>
#include <QPushButton>

SetGetWidget::SetGetWidget(QWidget * parent):
    InoutWidget(parent)
{
    label = new QLabel(this);
    label->setObjectName("label");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    label->setFixedHeight(20);

    set = new QPushButton(this);
    set->setObjectName("set");
    set->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    set->setFixedSize(20, 20);

    get = new QPushButton(this);
    get->setObjectName("get");
    get->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    get->setFixedSize(20, 20);

    set->setIcon(QIcon(":/img/set_icon.png"));
    get->setIcon(QIcon(":/img/get_icon.png"));

    ui->root_layout->insertWidget(0, label);
    ui->root_layout->insertWidget(1, set);
    ui->root_layout->insertWidget(2, get);

    connect(set, SIGNAL(clicked()), SLOT(set_clicked()));
    connect(get, SIGNAL(clicked()), SLOT(get_clicked()));
}
void SetGetWidget::hide_set_btn()
{
    set->hide();
}
void SetGetWidget::hide_get_btn()
{
    get->hide();
}
void SetGetWidget::show_set_btn()
{
    set->show();
}
void SetGetWidget::show_get_btn()
{
    get->show();
}
void SetGetWidget::set_label_text(const QString & s)
{
    label->setText(s);
}
void SetGetWidget::set_label_width(const int & w)
{
    label->setMinimumWidth(w);
//    label->setMaximumWidth(w);
}

void SetGetWidget::connect_property(Property * p)
{
    connect(p, SIGNAL(value_changed(QVariant)), this, SLOT(set_val(QVariant)));
    connect(this, SIGNAL(val_get()), p, SLOT(view_get()));
    connect(this, SIGNAL(val_set(QVariant)), p, SLOT(view_set(QVariant)));
    connect(p, SIGNAL(set_text(QString)), SLOT(set_label_text(QString)));

    InoutWidget::connect_property(p);
}
void SetGetWidget::set_clicked()
{
    emit val_set(get_val());
}
void SetGetWidget::get_clicked()
{
    emit val_get();
}
