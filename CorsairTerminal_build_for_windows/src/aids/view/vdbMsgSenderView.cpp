#include "vdbMsgSenderView.h"

#include <QButtonGroup>


#include "ui_vdbMsgSenderView.h"



/** Хранилище данных с формы */
struct t_msg {
    uint8_t cmd;
    uint8_t status;
    uint16_t src;
    uint16_t dst;
    uint32_t data_length;
    uint32_t word_1;
    uint32_t word_2;
};

const char name_byte_number[] = "byte_num";
const QVariant byte_4_byte_number_in_word = QVariant(4);
const QVariant byte_2_byte_number_in_word = QVariant(2);
const QVariant byte_1_byte_number_in_word = QVariant(1);

//-------------------------------------------------------------------------------------------------
vdbMsgSenderView::vdbMsgSenderView(QWidget *parent)
    : QWidget(parent)
    , _ui(new Ui::vdb_msg_sender)
    , _current_scale(new QButtonGroup(this))
    , _current_lenght(new QButtonGroup(this))
{
  _ui->setupUi(this);

  _current_scale->addButton(_ui->radbtn_scale_dec, 10);
  _current_scale->addButton(_ui->radbtn_scale_hex, 16);

  _current_lenght->addButton(_ui->radbtn_data_lengths_0, 0);
  _current_lenght->addButton(_ui->radbtn_data_lengths_1, 1);
  _current_lenght->addButton(_ui->radbtn_data_lengths_2, 2);

  _ui->tab_4_byte->setProperty(name_byte_number, byte_4_byte_number_in_word);
  _ui->tab_2_byte->setProperty(name_byte_number, byte_2_byte_number_in_word);
  _ui->tab_1_byte->setProperty(name_byte_number, byte_1_byte_number_in_word);
}
//-------------------------------------------------------------------------------------------------
vdbMsgSenderView::~vdbMsgSenderView()
{
    delete _ui;
    _current_scale->deleteLater();
    _current_lenght->deleteLater();
}
//-------------------------------------------------------------------------------------------------
bool vdbMsgSenderView::update_data_from_ui(t_msg& msg)
{
    bool is_success = false;
    bool ok = false;

    int32_t scale = _current_scale->checkedId();
    int32_t lenght = _current_lenght->checkedId();

    if(scale != -1 && lenght != -1) {

        msg.cmd = _ui->ledit_cmd->text().toUShort(&ok, 10);
        is_success = ok;
        msg.status = _ui->ledit_status->text().toUShort(&ok, 10);
        is_success = is_success && ok;
        msg.src = _ui->ledit_src->text().toUShort(&ok, 16);
        is_success = is_success && ok;
        msg.dst = _ui->ledit_dst->text().toUShort(&ok, 16);
        is_success = is_success && ok;

        int32_t byte_number_in_word = _ui->tabwidget_data->currentWidget()->property(name_byte_number).toInt(&ok);
        is_success = is_success && ok;
        bool word_1_success = true;
        bool word_2_success = true;
        if(is_success) {
            switch(byte_number_in_word) {
            case 4:
            {
                msg.word_1 = _ui->ledit_4_byte_word_1->text().toUInt(&ok, scale);
                word_1_success = is_success && ok;
                msg.word_2 = _ui->ledit_4_byte_word_2->text().toUInt(&ok, scale);
                word_2_success = is_success && ok;
            } break;
            case 2:
            {
                uint16_t word_1_b0 = _ui->ledit_2_byte_word_1_b0->text().toUShort(&ok, scale);
                word_1_success = is_success && ok;
                uint32_t word_1_b1 = _ui->ledit_2_byte_word_1_b1->text().toUShort(&ok, scale);
                word_1_success = is_success && ok;
                msg.word_1 = word_1_b0 | ((word_1_b1 << 16) & 0xFFFF0000);
                uint16_t word_2_b0 = _ui->ledit_2_byte_word_2_b0->text().toUShort(&ok, scale);
                word_2_success = is_success && ok;
                uint32_t word_2_b1 = _ui->ledit_2_byte_word_2_b1->text().toUShort(&ok, scale);
                word_2_success = is_success && ok;
                msg.word_2 = word_2_b0 | ((word_2_b1 << 16) & 0xFFFF0000);
            } break;
            case 1:
            {
                uint32_t word_1_b0 = _ui->ledit_1_byte_word_1_b0->text().toUShort(&ok, scale);
                word_1_success = is_success && ok;
                uint32_t word_1_b1 = _ui->ledit_1_byte_word_1_b1->text().toUShort(&ok, scale);
                word_1_success = is_success && ok;
                uint32_t word_1_b2 = _ui->ledit_1_byte_word_1_b2->text().toUShort(&ok, scale);
                word_1_success = is_success && ok;
                uint32_t word_1_b3 = _ui->ledit_1_byte_word_1_b3->text().toUShort(&ok, scale);
                word_1_success = is_success && ok;
                msg.word_1 = (word_1_b0 & 0xFF) | ((word_1_b1 << 8) & 0xFF00) | ((word_1_b2 << 16) & 0xFF0000) | ((word_1_b3 << 24) & 0xFF000000);
                uint32_t word_2_b0 = _ui->ledit_1_byte_word_2_b0->text().toUShort(&ok, scale);
                word_2_success = is_success && ok;
                uint32_t word_2_b1 = _ui->ledit_1_byte_word_2_b1->text().toUShort(&ok, scale);
                word_2_success = is_success && ok;
                uint32_t word_2_b2 = _ui->ledit_1_byte_word_2_b2->text().toUShort(&ok, scale);
                word_2_success = is_success && ok;
                uint32_t word_2_b3 = _ui->ledit_1_byte_word_2_b3->text().toUShort(&ok, scale);
                word_2_success = is_success && ok;
                msg.word_2 = (word_2_b0 & 0xFF) | ((word_2_b1 << 8) & 0xFF00) | ((word_2_b2 << 16) | 0xFF0000) | ((word_2_b3 << 24) & 0xFF000000);
            } break;
            }

            msg.data_length = static_cast<uint32_t>(lenght);
            if(msg.data_length > 0) {
                is_success = is_success && word_1_success;
                if(msg.data_length > 1) {
                    is_success = is_success && word_2_success;
                }
            }
        }
    }
    return is_success;
}
//-------------------------------------------------------------------------------------------------
void vdbMsgSenderView::reply_to_msg(Packet req)
{
    _ui->pltexted_response->clear();
    QString str;
    QString num;

    str.append("cmd: ");
    switch (req.cmd()) {
    case vdb_cmd_request_write: {
        str.append("Запрос-Запись");
    } break;
    case vdb_cmd_reply_write: {
        str.append("Ответ-Запись");
    } break;
    case vdb_cmd_request_read: {
        str.append("Запрос-Чтение");
    } break;
    case vdb_cmd_reply_read: {
        str.append("Ответ-Чтение");
    } break;
    default: {
        str.append("Неверно");
    } break;
    }
    str.append(";\tstatus: ");
    switch (req.status()) {
    case vdb_st_request: {
        str.append("Запрос без ответа");
    } break;
    case vdb_st_request_reply: {
        str.append("Запрос с ответом / Успешный ответ / Ошибка");
    } break;
    default: {
        str.append("Неверно");
    } break;
    }
    str.append(";\n\n");

    str.append("Src: ").append(num.setNum(req.src_addr(), 16)).append("\n");
    str.append("Dst: ").append(num.setNum(req.dst_addr(), 16)).append("\n\n");

    std::vector<uint8_t> data_vect = req.data();
    for(uint32_t i = 0; i < data_vect.size(); i++) {
        str.append("Data: ").append(num.setNum(req.dst_addr(), 16)).append(" ");
    }
    str.append(";");

    _ui->pltexted_response->appendPlainText(str);

}
//-------------------------------------------------------------------------------------------------
void vdbMsgSenderView::on_pbtn_send_msg_clicked()
{
    t_msg msg;
    if(update_data_from_ui(msg)){

        Packet packet(msg.cmd, msg.status, msg.dst);
        packet.set_src_addr(msg.src);

        if(msg.data_length > 0) {
            packet.append_data(msg.word_1);
            if(msg.data_length > 1) {
                packet.append_data(msg.word_2);
            }
        }
        emit signal_send_msg(packet);
    }
}
//-------------------------------------------------------------------------------------------------
