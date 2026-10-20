#ifndef ETHMODEL_H
#define ETHMODEL_H

#include <qmv/qmvmodel.h>
class QTimer;
class ComboBoxProperty;
class BooleanValueProperty;
class CmdProperty;
class StringValueProperty;
class IntValueProperty;
class EthController;

class EthModel : public QMVModel
{
    Q_OBJECT
public:
    explicit EthModel(QObject * parent = nullptr, uint32_t id = 0, QString name = "");

    enum VAL_ID {
        AUTO_CONNECT,
        DEV,
        SRC_MAC,
        SRC_IP,
        SRC_PORT,
        DST_MAC,
        DST_IP,
        DST_PORT
    };

    enum CMD_ID {
        CONNECT
    };

    void update_view();

    void set(const uint32_t &);
    void get(const uint32_t &);
    void cmd(const uint32_t &);

private slots:
    void auto_connect_tmr_task();

private:
    void open();
    void close();

private:
    BooleanValueProperty * auto_connect;
    CmdProperty * connect;
    ComboBoxProperty * dev;
    StringValueProperty * src_mac;
    StringValueProperty * src_ip;
    IntValueProperty * src_port;
    StringValueProperty * dst_mac;
    StringValueProperty * dst_ip;
    IntValueProperty * dst_port;

    QTimer * auto_connect_tmr;

    EthController * ethController;
};

#endif // ETHMODEL_H
