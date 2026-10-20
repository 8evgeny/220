#include "highlighterview.h"
#include "ui_highlighterview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

#include "device_parts/cmd/vdb_registers.h"
#include "device_parts/cmd/packet.h"

HighlighterView::HighlighterView(QWidget *parent) :
    QMVView(parent),
    ui(new Ui::HighlighterView)
{
    ui->setupUi(this);

    ui->status->set_label_text("Статус дальномера");
    ui->status->set_label_width(150);
    ui->status->hide_set_btn();
    ui->status->hide_set_btn();
    ui->status->hide_get_btn();
    ui->status->setDisabled(true);

    ui->meas_status->set_label_text("Статус измерений");
    ui->meas_status->set_label_width(150);
    ui->meas_status->hide_set_btn();
    ui->meas_status->hide_get_btn();
    ui->meas_status->setDisabled(true);

    ui->last_distance->set_label_text("Дальность[м]");
    ui->last_distance->set_label_width(150);
    ui->last_distance->hide_set_btn();
    ui->last_distance->hide_get_btn();
    ui->last_distance->setDisabled(true);

    ui->lrf_correction->set_label_text("Коррекция[м]");
    ui->lrf_correction->set_label_width(150);
    ui->lrf_correction->hide_set_btn();
    ui->lrf_correction->hide_get_btn();
    ui->lrf_correction->setDisabled(true);

    ui->avg_distance->set_label_text("Средняя дальность[м]");
    ui->avg_distance->set_label_width(150);
    ui->avg_distance->hide_set_btn();
    ui->avg_distance->hide_get_btn();
    ui->avg_distance->setDisabled(true);

    ui->rms_distance->set_label_text("СКО дальности[м]");
    ui->rms_distance->set_label_width(150);
    ui->rms_distance->hide_set_btn();
    ui->rms_distance->hide_get_btn();
    ui->rms_distance->setDisabled(true);

    ui->body_temp->set_label_text("Температура тела[град.Ц.]");
    ui->body_temp->set_label_width(150);
    ui->body_temp->hide_set_btn();
    ui->body_temp->hide_get_btn();
    ui->body_temp->setDisabled(true);

    ui->lrf_RMS->set_label_text("СКО сигнала");
    ui->lrf_RMS->set_label_width(150);
    ui->lrf_RMS->hide_set_btn();
    ui->lrf_RMS->hide_get_btn();
    ui->lrf_RMS->setDisabled(true);

    ui->lrf_Amp->set_label_text("Амплитуда сигнала");
    ui->lrf_Amp->set_label_width(150);
    ui->lrf_Amp->hide_set_btn();
    ui->lrf_Amp->hide_get_btn();
    ui->lrf_Amp->setDisabled(true);

    ui->ready->set_label_text("ready");
    ui->ready->set_label_width(70);
    ui->ready->hide_set_btn();
    ui->ready->hide_get_btn();
    ui->ready->setDisabled(true);

    ui->power_on->set_label_text("power on");
    ui->power_on->set_label_width(70);
    ui->power_on->hide_set_btn();
    ui->power_on->hide_get_btn();
    ui->power_on->setDisabled(true);


    ui->radiation->set_label_text("radiation");
    ui->radiation->set_label_width(70);
    ui->radiation->hide_set_btn();
    ui->radiation->hide_get_btn();
    ui->radiation->setDisabled(true);

    ui->cooling->set_label_text("cooling");
    ui->cooling->set_label_width(70);
    ui->cooling->hide_set_btn();
    ui->cooling->hide_get_btn();
    ui->cooling->setDisabled(true);

    ui->body_ovrht->set_label_text("overheat");
    ui->body_ovrht->set_label_width(70);
    ui->body_ovrht->hide_set_btn();
    ui->body_ovrht->hide_get_btn();
    ui->body_ovrht->setDisabled(true);

    ui->test_mode->set_label_text("test mode");
    ui->test_mode->set_label_width(70);
    ui->test_mode->hide_set_btn();
    ui->test_mode->hide_get_btn();
    ui->test_mode->setDisabled(true);

    ui->vavd_set->set_label_text("Установить");
    ui->vavd_set->set_label_width(70);
    ui->vavd_set->hide_get_btn();
    ui->vavd_get->set_label_text("Узнать");
    ui->vavd_get->set_label_width(70);
    ui->vavd_get->hide_set_btn();

    ui->frequency_set->set_label_text("Установить");
    ui->frequency_set->set_label_width(70);
    ui->frequency_set->hide_get_btn();
    ui->frequency_get->set_label_text("Узнать");
    ui->frequency_get->set_label_width(70);
    ui->frequency_get->hide_set_btn();

    // Work Boundings
    // Horizontal Sector 1
    ui->dangerZone_L1_begin->set_label_text("Нога 1: начало");
    ui->dangerZone_L1_begin->set_label_width(100);
    ui->dangerZone_L1_end->set_label_text("Нога 1: конец");
    ui->dangerZone_L1_end->set_label_width(100);
    // Horizontal Sector 2
    ui->dangerZone_L2_begin->set_label_text("Нога 2: начало");
    ui->dangerZone_L2_begin->set_label_width(100);
    ui->dangerZone_L2_end->set_label_text("Нога 2: конец");
    ui->dangerZone_L2_end->set_label_width(100);
    // Horizontal Sector 3
    ui->dangerZone_L3_begin->set_label_text("Нога 3: начало");
    ui->dangerZone_L3_begin->set_label_width(100);
    ui->dangerZone_L3_end->set_label_text("Нога 3: конец");
    ui->dangerZone_L3_end->set_label_width(100);
    // Horizontal Sector 4
    ui->dangerZone_L4_begin->set_label_text("Нога 4: начало");
    ui->dangerZone_L4_begin->set_label_width(100);
    ui->dangerZone_L4_begin->setDisabled(true);
    ui->dangerZone_L4_end->set_label_text("Нога 4: конец");
    ui->dangerZone_L4_end->set_label_width(100);
    ui->dangerZone_L4_end->setDisabled(true);

    ui->dangerZone_deltaTvLrf->set_label_text("Дельта ТВ и Дальномера");
    ui->dangerZone_deltaTvLrf->set_label_width(130);
    ui->dangerZone_lrfD->set_label_text("Диаметр дальномера");
    ui->dangerZone_lrfD->set_label_width(130);
    ui->dangerZone_reserve->set_label_text("Запас");
    ui->dangerZone_reserve->set_label_width(130);

    // Vertical Sector
    ui->dangerZone_x_base->set_label_text("Граница основания");
    ui->dangerZone_x_base->set_label_width(120);
    ui->dangerZone_x_legs->set_label_text("Граница ног коптера");
    ui->dangerZone_x_legs->set_label_width(120);
    // Cmds
    ui->dangerZone_onoff->set_cmd_text("Выключить ограничения");
    // test
    ui->dangerZone_test_x->set_label_text("Тест X");
    ui->dangerZone_test_x->set_label_width(100);
    ui->dangerZone_test_x->hide();
    ui->dangerZone_test_z->set_label_text("Тест Z");
    ui->dangerZone_test_z->set_label_width(100);
    ui->dangerZone_test_z->hide();

}
void HighlighterView::connect_model()
{
    ui->status->connect_property(model->value_property(ui->status->objectName()));
    ui->meas_status->connect_property(model->value_property(ui->meas_status->objectName()));
    ui->last_distance->connect_property(model->value_property(ui->last_distance->objectName()));
    ui->lrf_correction->connect_property(model->value_property(ui->lrf_correction->objectName()));
    ui->avg_distance->connect_property(model->value_property(ui->avg_distance->objectName()));
    ui->rms_distance->connect_property(model->value_property(ui->rms_distance->objectName()));
    ui->body_temp->connect_property(model->value_property(ui->body_temp->objectName()));
    ui->lrf_RMS->connect_property(model->value_property(ui->lrf_RMS->objectName()));
    ui->lrf_Amp->connect_property(model->value_property(ui->lrf_Amp->objectName()));

    ui->ready->connect_property(model->value_property(ui->ready->objectName()));
    ui->power_on->connect_property(model->value_property(ui->power_on->objectName()));
    ui->radiation->connect_property(model->value_property(ui->radiation->objectName()));
    ui->cooling->connect_property(model->value_property(ui->cooling->objectName()));
    ui->body_ovrht->connect_property(model->value_property(ui->body_ovrht->objectName()));
    ui->test_mode->connect_property(model->value_property(ui->test_mode->objectName()));
//    ui->lrf_on->connect_property(model->value_property(ui->lrf_on->objectName()));
//    ui->pause_on->connect_property(model->value_property(ui->pause_on->objectName()));
//    ui->bd_ovrht->connect_property(model->value_property(ui->bd_ovrht->objectName()));
//    ui->_tec_on->connect_property(model->value_property(ui->_tec_on->objectName()));
//    ui->mode0->connect_property(model->value_property(ui->mode0->objectName()));
//    ui->mode1->connect_property(model->value_property(ui->mode1->objectName()));
//    ui->no_izm->connect_property(model->value_property(ui->no_izm->objectName()));
//    ui->t_check->connect_property(model->value_property(ui->t_check->objectName()));
    ui->vavd_set->connect_property(model->value_property(ui->vavd_set->objectName()));
    ui->vavd_get->connect_property(model->value_property(ui->vavd_get->objectName()));
    ui->frequency_set->connect_property(model->value_property(ui->frequency_set->objectName()));
    ui->frequency_get->connect_property(model->value_property(ui->frequency_get->objectName()));

    // Work Boundings
    // Horizontal Sector 1
    ui->dangerZone_L1_begin ->connect_property(model->value_property(ui->dangerZone_L1_begin->objectName()));
    ui->dangerZone_L1_end   ->connect_property(model->value_property(ui->dangerZone_L1_end->objectName()));
    // Horizontal Sector 2
    ui->dangerZone_L2_begin ->connect_property(model->value_property(ui->dangerZone_L2_begin->objectName()));
    ui->dangerZone_L2_end   ->connect_property(model->value_property(ui->dangerZone_L2_end->objectName()));
    // Horizontal Sector 3
    ui->dangerZone_L3_begin ->connect_property(model->value_property(ui->dangerZone_L3_begin->objectName()));
    ui->dangerZone_L3_end   ->connect_property(model->value_property(ui->dangerZone_L3_end->objectName()));
    // Horizontal Sector 4
    ui->dangerZone_L4_begin ->connect_property(model->value_property(ui->dangerZone_L4_begin->objectName()));
    ui->dangerZone_L4_end   ->connect_property(model->value_property(ui->dangerZone_L4_end->objectName()));

    ui->dangerZone_deltaTvLrf   ->connect_property(model->value_property(ui->dangerZone_deltaTvLrf->objectName()));
    ui->dangerZone_lrfD         ->connect_property(model->value_property(ui->dangerZone_lrfD->objectName()));
    ui->dangerZone_reserve      ->connect_property(model->value_property(ui->dangerZone_reserve->objectName()));

    // Vertical Sector
    ui->dangerZone_x_base   ->connect_property(model->value_property(ui->dangerZone_x_base->objectName()));
    ui->dangerZone_x_legs   ->connect_property(model->value_property(ui->dangerZone_x_legs->objectName()));

    ui->dangerZone_onoff    ->connect_property(model->cmd_property(ui->dangerZone_onoff->objectName()));

    ui->dangerZone_test_x   ->connect_property(model->value_property(ui->dangerZone_test_x->objectName()));
    ui->dangerZone_test_z   ->connect_property(model->value_property(ui->dangerZone_test_z->objectName()));
}

HighlighterView::~HighlighterView()
{
    delete ui;
}
void HighlighterView::telemetry_enable_on_module(bool enable)
{
//    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_telemetry_enable, enable);
    Q_UNUSED(enable)
}
void HighlighterView::set_telemetry_address_on_module(uint16_t address)
{
//    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_telemetry_address, adress);
    Q_UNUSED(address)
}
void HighlighterView::telemetry_period_on_module(uint16_t period)
{
//    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_telemetry_period, period);
    Q_UNUSED(period)
}
void HighlighterView::on_pbtn_laser_off_clicked()
{
    emit signal_stop_radiation();
}

void HighlighterView::on_pbtn_single_measure_clicked()
{
emit  signal_start_measurement();
}

void HighlighterView::on_pbtn_podsvet_clicked()
{
    emit signal_start_illumination();
}

void HighlighterView::on_pbtn_single_shot_clicked()
{
    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_radiation_cntr, 0x9EED);
}

void HighlighterView::on_pbtn_test_shot_clicked()
{
    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_radiation_cntr, 0xB99D);
}

void HighlighterView::on_pbtn_power_on_clicked()
{
    emit signal_power_on_switch();
}

void HighlighterView::on_pbtn_test_mode_on_clicked()
{
    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_test_mode, true);
}

void HighlighterView::on_pbtn_test_mode_off_clicked()
{
    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_test_mode, false);
}

//void HighlighterView::on_pbtn_send_freq_clicked()
//{
//    bool ok = false;
//    int freq = ui->ledit_freq->text().toInt(&ok);
//    if(ok) {
//        if(freq >= 47 && freq <= 50){
//            emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_freq, freq);
//        }
//    }
//}

//void HighlighterView::on_cboc_arc_stateChanged(int arg1)
//{
////    Packet packet(_cmd, _status, 0x0B0A);
//    bool state = ui->cboc_arc->checkState();
////    packet.append_data(uint8_t(state));
////    emit signal_send_msg(packet);
//    emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_arc, state);
//}
