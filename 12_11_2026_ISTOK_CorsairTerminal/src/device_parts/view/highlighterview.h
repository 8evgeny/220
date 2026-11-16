#ifndef HighlighterView_H
#define HighlighterView_H

#include <qmv/qmvview.h>

namespace Ui {
class HighlighterView;
}

class Packet;

class HighlighterView : public QMVView
{
    Q_OBJECT

public:
    explicit HighlighterView(QWidget *parent = nullptr);
    ~HighlighterView();

private slots:
    void on_pbtn_laser_off_clicked();
    void on_pbtn_single_measure_clicked();
    void on_pbtn_podsvet_clicked();
    void on_pbtn_single_shot_clicked();
    void on_pbtn_test_shot_clicked();
    void on_pbtn_power_on_clicked();
    void on_pbtn_test_mode_on_clicked();
    void on_pbtn_test_mode_off_clicked();

//    void on_pbtn_send_freq_clicked();

//    void on_cboc_arc_stateChanged(int arg1);

private:

    void connect_model();

    void telemetry_enable_on_module(bool enable);
    void set_telemetry_address_on_module(uint16_t address);
    void telemetry_period_on_module(uint16_t period);



    Ui::HighlighterView *ui;

    const uint8_t _cmd = 0;
    const uint8_t _status = 0;
    const uint16_t _src = 0;

signals:

    void signal_power_on_switch();
    void signal_start_measurement();
    void signal_start_illumination();
    void signal_stop_radiation();

    void signal_send_msg(Packet& req);
    void vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length);
    void vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word);
    void vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data);

};

#endif // HighlighterView_H
