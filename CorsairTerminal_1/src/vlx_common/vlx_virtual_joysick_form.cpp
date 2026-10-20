#include "vlx_virtual_joysick_form.h"
#include "ui_vlx_virtual_joysick_form.h"

#include <QPainter>
#include <QMouseEvent>
#include <QShowEvent>
#include <QDebug>

#include "pluggable_controls/joystick.h"
#include "vlx_common/vlx_settings.h"

int    MIN_TRACKBALL_COORD          = -30704.0;
int    MAX_TRACKBALL_COORD          = 30703.0;

VlxVirtualJoysickForm::VlxVirtualJoysickForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VlxVirtualJoysickForm)
{
    ui->setupUi(this);

    setVisible(VlxSettings::i().useVirtualJoystick());
}

VlxVirtualJoysickForm::~VlxVirtualJoysickForm()
{
    delete ui;
}

void VlxVirtualJoysickForm::updatePosition()
{
    if (!VlxSettings::i().useVirtualJoystick())
        return;

    auto j = Joystick::currentJoystick();
    if (j == nullptr)
        return;

    QPointF pos = m_pos - m_c;

    Sint16 vx = pos.x() * m_k.x();
    Sint16 vy = -(pos.y() * m_k.y());

    SDL_JoystickSetVirtualAxis(j, 0, vx);
    SDL_JoystickSetVirtualAxis(j, 1, vy);
}

void VlxVirtualJoysickForm::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);

    p.setPen(Qt::white);
    p.setBrush(Qt::darkGray);
    p.drawRect(rect());
    p.setBrush(Qt::darkGray);
    p.drawEllipse(m_c, 24, 24);
    p.setBrush(Qt::red);
    p.drawEllipse(m_pos, 22, 22);

}

void VlxVirtualJoysickForm::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    m_pos = event->pos();
    update();
    updatePosition();
}

void VlxVirtualJoysickForm::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    m_pos = m_c;
    update();
    updatePosition();
}

static QPoint clampPoint(const QPoint& pos, const QRect& rect) {
    int x = qBound(rect.left(), pos.x(), rect.right());
    int y = qBound(rect.top(),  pos.y(), rect.bottom());
    return QPoint(x, y);
}

void VlxVirtualJoysickForm::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    m_pos = clampPoint(event->pos(), rect());
    update();
    updatePosition();
}

void VlxVirtualJoysickForm::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    m_pos = rect().center();
    m_c = m_pos;

    m_k.setX( (MAX_TRACKBALL_COORD - MIN_TRACKBALL_COORD) / width() );
    m_k.setY( (MAX_TRACKBALL_COORD - MIN_TRACKBALL_COORD) / height() );

    updatePosition();
}
