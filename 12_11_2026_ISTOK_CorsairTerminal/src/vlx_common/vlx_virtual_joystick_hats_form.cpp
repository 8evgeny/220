#include "vlx_virtual_joystick_hats_form.h"
#include "ui_vlx_virtual_joystick_hats_form.h"

#include "pluggable_controls/joystick.h"
#include "vlx_common/vlx_settings.h"

#include <QMouseEvent>
#include <QDebug>
#include <QPainter>

VlxVirtualJoystickHatsForm::VlxVirtualJoystickHatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VlxVirtualJoystickHatsForm)
{
    ui->setupUi(this);
    setVisible(VlxSettings::i().useVirtualJoystick());
}

VlxVirtualJoystickHatsForm::~VlxVirtualJoystickHatsForm()
{
    delete ui;
}

void VlxVirtualJoystickHatsForm::updateHats()
{
    if (!VlxSettings::i().useVirtualJoystick())
        return;

    auto j = Joystick::currentJoystick();
    if (j == nullptr)
        return;


    Uint8 x_dir = (m_x == 0) ? SDL_HAT_LEFT : (m_x == 2) ? SDL_HAT_RIGHT : SDL_HAT_CENTERED;
    Uint8 y_dir = (m_y == 0) ? SDL_HAT_UP : (m_y == 2) ? SDL_HAT_DOWN : SDL_HAT_CENTERED;

    SDL_JoystickSetVirtualHat(j, 0, x_dir | y_dir);
}

void VlxVirtualJoystickHatsForm::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        auto p = event->pos();
        m_x = qBound(0, p.x() / m_dx, 2);
        m_y = qBound(0, p.y() / m_dy, 2);
    } else {
        m_x = 1;
        m_y = 1;
    }

    update();
    updateHats();
}

void VlxVirtualJoystickHatsForm::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::darkGray);

    auto c = rect().center();
    QPoint o (m_dx * (m_x - 1), m_dy * (m_y - 1));

    p.setBrush(Qt::darkRed);
    p.drawEllipse(c + o, 18, 18);

}

void VlxVirtualJoystickHatsForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_dx = rect().width() / 3;
    m_dy = rect().height() / 3;
}
