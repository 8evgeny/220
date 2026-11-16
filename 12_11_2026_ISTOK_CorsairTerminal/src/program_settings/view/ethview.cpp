#include "ethview.h"
#include "ui_ethview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

EthView::EthView(QWidget * parent) :
    QMVView(parent),
    ui(new Ui::EthView)
{
    ui->setupUi(this);

    ui->auto_connect->set_label_text("Авто подключение");
    ui->auto_connect->set_label_width(120);
    ui->auto_connect->hide_set_btn();
    ui->auto_connect->hide_get_btn();

    ui->connect->set_cmd_text("Установить подключение");

    ui->dev->set_label_text("Интерфейс");
    ui->dev->set_label_width(60);
    ui->dev->setToolTip("lo");

    ui->src_mac->set_label_text("MAC-адрес");
    ui->src_mac->set_label_width(60);
    ui->src_mac->setToolTip("ff-ff-ff-ff-ff-ff (hex)");
    ui->src_mac->hide_get_btn();

    ui->src_ip->set_label_text("IP-адрес");
    ui->src_ip->set_label_width(60);
    ui->src_ip->setToolTip("255.255.255.255 (dec)");
    ui->src_ip->hide_get_btn();

    ui->src_port->set_label_text("Port-адрес");
    ui->src_port->set_label_width(60);
    ui->src_port->setToolTip("12345 (dec)");
    ui->src_port->hide_get_btn();

    ui->dst_mac->set_label_text("MAC-адрес");
    ui->dst_mac->set_label_width(60);
    ui->dst_mac->setToolTip("ff-ff-ff-ff-ff-ff (hex)");
    ui->dst_mac->hide_get_btn();

    ui->dst_ip->set_label_text("IP-адрес");
    ui->dst_ip->set_label_width(60);
    ui->dst_ip->setToolTip("255.255.255.255 (dec)");
    ui->dst_ip->hide_get_btn();

    ui->dst_port->set_label_text("Port-адрес");
    ui->dst_port->set_label_width(60);
    ui->dst_port->setToolTip("12345 (dec)");
    ui->dst_port->hide_get_btn();

#ifndef PCAP
    ui->dev->hide();
    ui->src_mac->hide();
    ui->dst_mac->hide();
#endif
}
void EthView::connect_model()
{
    ui->auto_connect->connect_property(model->value_property(ui->auto_connect->objectName()));
    ui->connect->connect_property(model->cmd_property(ui->connect->objectName()));

    ui->dev->connect_property(model->value_property(ui->dev->objectName()));

    ui->src_mac->connect_property(model->value_property(ui->src_mac->objectName()));
    ui->src_ip->connect_property(model->value_property(ui->src_ip->objectName()));
    ui->src_port->connect_property(model->value_property(ui->src_port->objectName()));

    ui->dst_mac->connect_property(model->value_property(ui->dst_mac->objectName()));
    ui->dst_ip->connect_property(model->value_property(ui->dst_ip->objectName()));
    ui->dst_port->connect_property(model->value_property(ui->dst_port->objectName()));
}
EthView::~EthView()
{
    delete ui;
}
