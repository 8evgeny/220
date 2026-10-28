#ifndef VDBMSGSENDERVIEW_H
#define VDBMSGSENDERVIEW_H
//-------------------------------------------------------------------------------------------------
#include <QWidget>
#include "device_parts/cmd/packet.h"
//-------------------------------------------------------------------------------------------------
namespace Ui {
class vdb_msg_sender;
}

struct t_msg;
class QButtonGroup;

class vdbMsgSenderView : public QWidget
{
    Q_OBJECT
public:

    explicit vdbMsgSenderView(QWidget *parent = nullptr);
    virtual ~vdbMsgSenderView();

private:

    bool update_data_from_ui(t_msg& msg);

    /** Interface */
    Ui::vdb_msg_sender* _ui;

    QButtonGroup* _current_scale;
    QButtonGroup* _current_lenght;


public slots:

    void reply_to_msg(Packet req);


private slots:

    void on_pbtn_send_msg_clicked();


signals:

    void signal_send_msg(Packet& req);



};
//-------------------------------------------------------------------------------------------------
#endif // VDBMSGSENDERVIEW_H
