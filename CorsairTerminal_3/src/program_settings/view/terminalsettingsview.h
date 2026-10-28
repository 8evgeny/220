#ifndef TERMINAL_SETTINGS_H
#define TERMINAL_SETTINGS_H

#include <qmv/qmvview.h>

namespace Ui {
class terminalsettingsview;
}

class terminalsettingsview : public QMVView
{
    Q_OBJECT

public:
    explicit terminalsettingsview(QWidget *parent = nullptr);
    ~terminalsettingsview();

signals:
    void sg_infographic_setShipPointer();
    void sg_infographic_setHelicopterPointer();

private slots:
    void on_rb_pointerShip_clicked(bool checked);
    void on_rb_pointerHelicopter_clicked(bool checked);

private:
    void connect_model();

private:
    Ui::terminalsettingsview *_ui;
};

#endif // TERMINAL_SETTINGS_H
