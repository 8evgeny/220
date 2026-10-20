#include "ethmodel.h"

#include <QTimer>
#include <qmv/property/comboboxproperty.h>
#include <qmv/property/booleanvalueproperty.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/stringvalueproperty.h>
#include <qmv/property/intvalueproperty.h>
#include "eth/ethcontroller.h"

#ifdef PCAP
#include "socket/pcapsocket.h"
#endif

#include <QList>

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logEth, "Eth")

EthModel::EthModel(QObject * parent, uint32_t id, QString name) :
    QMVModel(parent, id, name),
    auto_connect(new BooleanValueProperty(this, AUTO_CONNECT, "auto_connect", false)),
    connect(new CmdProperty(this, CONNECT, "connect")),
    dev(new ComboBoxProperty(this, DEV, "dev", "lo")),
    src_mac(new StringValueProperty(this, SRC_MAC, "src_mac", "ff-ff-ff-ff-ff-ff")),
    src_ip(new StringValueProperty(this, SRC_IP, "src_ip", "169.254.0.2")),
    src_port(new IntValueProperty(this, SRC_PORT, "src_port", 52515, 0, 65535)),
    dst_mac(new StringValueProperty(this, DST_MAC, "dst_mac", "00-40-d0-21-33-c7")),
    dst_ip(new StringValueProperty(this, DST_IP, "dst_ip", "169.254.0.1")),
    dst_port(new IntValueProperty(this, DST_PORT, "dst_port", 52516, 0, 65535)),

    auto_connect_tmr(new QTimer(this)),
    ethController(&EthController::inst())
{
    QObject::connect(auto_connect_tmr, SIGNAL(timeout()), SLOT(auto_connect_tmr_task()));
    auto_connect_tmr->start(500);
}
void EthModel::update_view()
{
#ifdef PCAP
    auto devs = PcapSocket::devices();
    QList<QString> texts;
    QList<QVariant> values;
    for(auto dev = devs.begin();dev != devs.end(); ++dev)
    {
        QString qdev = QString::fromStdString(*dev);
        texts.append(qdev);
        values.append(qdev);
    }
    dev->set_items(texts, values);
#endif
    QMVModel::update_view();
}
void EthModel::open()
{
    bool retVal = ethController->start(dev->get_val().toString().toStdString(),
                                       src_mac->get().toStdString(), src_ip->get().toStdString(), uint16_t(src_port->get()),
                                       dst_mac->get().toStdString(), dst_ip->get().toStdString(), uint16_t(dst_port->get()));
    if(retVal)
    {
        emit connect->set_success();
        emit connect->set_text("Разорвать соединение");
    }
    else
    {
        if(!auto_connect->get()) emit critical("Ошибка открытия соединения", "Устройство не найдено");
        emit connect->set_error();
        emit connect->set_text("Установить соединение");
    }
}
void EthModel::close()
{
    bool retVal = ethController->stop();
    if(retVal)
    {
        emit connect->set_error();
        emit connect->set_text("Установить соединение");
    }
    else
    {
        emit connect->set_success();
        emit connect->set_text("Разорвать соединение");
    }
}
void EthModel::auto_connect_tmr_task()
{
    if(ethController->is_started())
    {
//        if(!eth_tools::is_device_with_name(device->get_val().toString())) close();
    }
    else
    {
        if(auto_connect->get()) open();
    }
}
void EthModel::set(const uint32_t &)
{
}
void EthModel::get(const uint32_t & id)
{
    if(id == DEV)
    {
#ifdef PCAP
        auto devs = PcapSocket::devices();
        QList<QString> texts;
        QList<QVariant> values;
        for(auto dev = devs.begin();dev != devs.end(); ++dev)
        {
            QString qdev = QString::fromStdString(*dev);
            texts.append(qdev);
            values.append(qdev);
        }
        dev->set_items(texts, values);
#endif
    }
}
void EthModel::cmd(const uint32_t & id)
{
    if(id == CONNECT)
    {
        if(ethController->is_started()) close();
        else open();
    }
    switch(id)
    {
    case CONNECT:
        break;
    default:
        break;
    }
}
