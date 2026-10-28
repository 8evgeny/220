#ifndef GSPCTRLVIEW_H
#define GSPCTRLVIEW_H

#include <qmv/qmvview.h>

class InoutCmdWidget;

namespace Ui {
class GSPCtrlView;
}

class Controller;

class GSPCtrlView : public QMVView
{
    Q_OBJECT

public:
    explicit GSPCtrlView(QWidget *parent = nullptr);
    ~GSPCtrlView();

    void setController(Controller *ctrl);

private slots:
    void on_up_pressed();
    void on_up_released();
    void on_down_pressed();
    void on_down_released();
    void on_left_pressed();
    void on_left_released();
    void on_right_pressed();
    void on_right_released();
    void on_stop_clicked();

    void gyCal_calibrate();
    void gyCal_auto();

private:
    void connect_model();

private:
    Ui::GSPCtrlView * ui;
    InoutCmdWidget * up_start;
    InoutCmdWidget * down_start;
    InoutCmdWidget * left_start;
    InoutCmdWidget * right_start;
    InoutCmdWidget * stop;
    Controller *m_controller = nullptr;
};

#endif // GSPCTRLVIEW_H
