#ifndef MWIRCTRLVIEW_H
#define MWIRCTRLVIEW_H

#include <qmv/qmvview.h>

class InoutCmdWidget;

namespace Ui {
class MWIRCtrlView;
}

class MWIRCtrlView : public QMVView
{
    Q_OBJECT

public:
    explicit MWIRCtrlView(QWidget *parent = nullptr);
    ~MWIRCtrlView();

private slots:
    void on_zoom_in_pressed();
    void on_zoom_in_released();
    void on_zoom_out_pressed();
    void on_zoom_out_released();
    void on_iris_in_pressed();
    void on_iris_in_released();
    void on_iris_out_pressed();
    void on_iris_out_released();
    void on_focus_near_pressed();
    void on_focus_near_released();
    void on_focus_far_pressed();
    void on_focus_far_released();

    void on_sensor_ssettings_save_to_flash_clicked();

    void on_sensor_ssettings_load_from_flash_clicked();

    void on_sensor_ssettings_initialization_value_clicked();

private:
    void connect_model();

private:
    Ui::MWIRCtrlView * ui;
    InoutCmdWidget * zoom_in_start;
    InoutCmdWidget * zoom_in_stop;
    InoutCmdWidget * zoom_out_start;
    InoutCmdWidget * zoom_out_stop;
    InoutCmdWidget * iris_in_start;
    InoutCmdWidget * iris_in_stop;
    InoutCmdWidget * iris_out_start;
    InoutCmdWidget * iris_out_stop;
    InoutCmdWidget * focus_near_start;
    InoutCmdWidget * focus_near_stop;
    InoutCmdWidget * focus_far_start;
    InoutCmdWidget * focus_far_stop;
};

#endif // MWIRCTRLVIEW_H
