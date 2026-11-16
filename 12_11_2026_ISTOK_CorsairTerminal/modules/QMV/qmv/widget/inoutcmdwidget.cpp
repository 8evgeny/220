#include "inoutcmdwidget.h"
#include "ui_inoutwidget.h"
#include "qmv/property/cmdproperty.h"
#include <QIcon>
#include <QPushButton>
#include <QString>

InoutCmdWidget::InoutCmdWidget(QWidget * parent) :
    InoutWidget(parent)
{
    cmdbtn = new QPushButton(this);
    cmdbtn->setObjectName("cmd");
    cmdbtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cmdbtn->setFixedHeight(20);

    ui->root_layout->insertWidget(0, cmdbtn);

    connect(cmdbtn, SIGNAL(clicked()), SIGNAL(cmd()));
}
void InoutCmdWidget::connect_property(Property * p)
{
    connect(this, SIGNAL(cmd()), p, SLOT(view_cmd()));
    connect(p, SIGNAL(set_text(QString)), SLOT(set_cmd_text(QString)));

    InoutWidget::connect_property(p);
}
void InoutCmdWidget::set_cmd_icon(const QString& s, const int& width, const int& height)
{
    QIcon icon(s);
//    icon.addFile(QString::fromUtf8(s), QSize(), QIcon::Normal, QIcon::On);
    cmdbtn->setIcon(icon);
    cmdbtn->setIconSize(QSize(width, height));
}
void InoutCmdWidget::set_cmd_text(const QString& s)
{
    cmdbtn->setText(s);
}
void InoutCmdWidget::set_cmd_width(const int& w)
{
    cmdbtn->setMinimumWidth(w);
//    cmdbtn->setMaximumWidth(w);
}
