#ifndef VLX_VIRTUAL_JOYSTICK_HATS_FORM_H
#define VLX_VIRTUAL_JOYSTICK_HATS_FORM_H

#include <QWidget>

namespace Ui {
class VlxVirtualJoystickHatsForm;
}

class VlxVirtualJoystickHatsForm : public QWidget
{
    Q_OBJECT

public:
    explicit VlxVirtualJoystickHatsForm(QWidget *parent = nullptr);
    ~VlxVirtualJoystickHatsForm();

protected:

    void updateHats();

private:
    Ui::VlxVirtualJoystickHatsForm *ui;
    int m_dx = 0;
    int m_dy = 0;

    int m_x = 1;
    int m_y = 1;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

};

#endif // VLX_VIRTUAL_JOYSTICK_HATS_FORM_H
