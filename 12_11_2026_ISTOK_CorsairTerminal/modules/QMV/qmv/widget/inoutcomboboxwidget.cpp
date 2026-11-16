#include "inoutcomboboxwidget.h"
#include "ui_inoutwidget.h"
#include "qmv/property/property.h"
#include <QComboBox>

InoutComboBoxWidget::InoutComboBoxWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QComboBox(this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    val->setFixedHeight(20);

    ui->root_layout->insertWidget(1, val);

    connect(val, SIGNAL(activated(int)), this, SLOT(set_clicked()));
}
void InoutComboBoxWidget::connect_property(Property * p)
{
    connect(p, SIGNAL(items_changed(QList<QString>, QList<QVariant>, QVariant)), this, SLOT(set_items(QList<QString>, QList<QVariant>, QVariant)));

    SetGetWidget::connect_property(p);
}
void InoutComboBoxWidget::set_val(const QVariant & v)
{
    int index = val->findData(v);
    if(index != -1)
    {
        val->setCurrentIndex(index);
    }
    else
    {
        val->setCurrentIndex(-1);
    }
}
QVariant InoutComboBoxWidget::get_val()
{
    return val->currentData();
}
void InoutComboBoxWidget::set_items(const QList<QString> & texts, const QList<QVariant> & values, const QVariant & item)
{
    val->clear();
    for(int i = 0; i < texts.length(); ++i)
    {
        val->addItem(texts.at(i), values.at(i));
        val->setItemData(i, values.at(i), Qt::ToolTipRole);
    }
    set_val(item);
}
