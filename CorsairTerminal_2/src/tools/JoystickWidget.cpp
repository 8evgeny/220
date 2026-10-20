#include "JoystickWidget.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

JoystickWidget::JoystickWidget(QWidget *parent)
    : QWidget(parent)
    , m_timer(new QTimer(this))
    , m_penLine(Qt::blue, 5)
    , m_colorText(QColor(255, 255, 255, 255))
    , m_colorPress(QColor(255, 255, 255, 200))
    , m_colorRelease(QColor(255, 255, 255, 100))
    , m_penRelease(m_colorRelease, 5)
    , m_penPress(m_colorPress, 5)
    , m_brushRelease(m_colorRelease, Qt::SolidPattern)
    , m_brushPress(m_colorPress, Qt::SolidPattern)
    , m_joystickTimer(new QTimer(this))
//    , m_penRelease(Qt::darkCyan)
//    , m_penPress(Qt::darkRed)
{
    this->setFixedSize(QSize(SIZE_X, SIZE_Y));
    this->setMouseTracking(true);

//    m_penLine.setWidth(1);
//    m_penRelease.setWidth(15);
//    m_penPress.setWidth(15);

    connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&JoystickWidget::update));
    m_timer->start(20);

    connect(m_joystickTimer, &QTimer::timeout, this, &JoystickWidget::sl_sendNewExcursion);
    m_joystickTimer->start(100);

}

void JoystickWidget::sl_sendNewExcursion()
{
    static bool en = false;
    if(m_isLeftButtonPress) {
        emit sg_newExcursion(m_yRelative, m_xRelative);
        en = true;
    } else if(en) {
        m_xRelative = 0.;
        m_yRelative = 0.;
        emit sg_newExcursion(m_yRelative, m_xRelative);
        en = false;
    }
}

void JoystickWidget::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::LeftButton: {
            m_isLeftButtonPress = true;
            QPointF point = event->localPos();
//            QPointF point = event->pos();
//            QPointF point = event->position();
            m_x = point.x();
            m_y = point.y();
            if(m_x > SIZE_X) {m_x = SIZE_X;}
            else if(m_x < 0) {m_x = 0;}
            if(m_y > SIZE_Y) {m_y = SIZE_Y;}
            else if(m_y < 0) {m_y = 0;}
            m_xRelative = -static_cast<double>(SIZE_X/2 - m_x) / (SIZE_X/2);
            m_yRelative =  static_cast<double>(SIZE_Y/2 - m_y) / (SIZE_Y/2);
            m_strOut = QString("X = %1, Y = %2").arg(m_xRelative, 5, 'g', 2).arg(m_yRelative, 5, 'g', 2);
            // qDebug() << "Left button pressed" << Qt::endl;
//            emit sg_newExcursion(m_xRelative, m_yRelative);
            JoystickWidget::update();
        } break;
        default:
        break;
    }
}

void JoystickWidget::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::LeftButton: {
            m_isLeftButtonPress = false;
            m_x = SIZE_X/2;
            m_y = SIZE_Y/2;
            // qDebug() << "Left button released" << Qt::endl;
            m_xRelative = 0.;
            m_yRelative = 0.;
//            emit sg_newExcursion(m_xRelative, m_yRelative);
            JoystickWidget::update();
        } break;
        default:
        break;
    }
}

void JoystickWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF point = event->localPos();
//    QPointF point = event->scenePosition();
    if(m_isLeftButtonPress) {
        m_x = point.x();
        m_y = point.y();
        if(m_x > SIZE_X) {m_x = SIZE_X;}
        else if(m_x < 0) {m_x = 0;}
        if(m_y > SIZE_Y) {m_y = SIZE_Y;}
        else if(m_y < 0) {m_y = 0;}
        m_xRelative = -static_cast<double>(SIZE_X/2 - m_x) / (SIZE_X/2);
        m_yRelative =  static_cast<double>(SIZE_Y/2 - m_y) / (SIZE_Y/2);
//        emit sg_newExcursion(m_xRelative, m_yRelative);
    } else {
        m_x = SIZE_X/2;
        m_y = SIZE_Y/2;
        m_xRelative = 0.;
        m_yRelative = 0.;
    }
    m_strOut = QString("X = %1, Y = %2").arg(m_xRelative, 5, 'g', 2).arg(m_yRelative, 5, 'g', 2);
    // QString str("Current position: X = %1, Y = %2");
    // qDebug() << str.arg(m_x).arg(m_y) << " | " << (m_isLeftButtonPress ? "Press" : "Null") << Qt::endl;
//    JoystickWidget::update();
}

void JoystickWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    /** Background */
    painter.setBrush(QBrush(QColor(0, 0, 0, 100), Qt::SolidPattern));
    painter.setPen(QPen(QColor(0, 0, 0, 100), 4, Qt::SolidLine));
    int32_t radius = 10;
    painter.drawRoundedRect(0, 0, SIZE_X, SIZE_Y, radius, radius);

    painter.setFont(QFont("Arial", FONT_SIZE));
    // Line
    painter.setPen(m_penLine);
    painter.drawLine(m_lineVert);
    painter.drawLine(m_lineHor);
    // Point
    if(m_isLeftButtonPress) {
        painter.setPen(m_penPress);
        painter.setBrush(m_brushPress);
    } else {
        painter.setPen(m_penRelease);
        painter.setBrush(m_brushRelease);
    }
//    painter.drawPoint(m_x, m_y);
    painter.drawEllipse(m_x - m_xSize/2, m_y - m_ySize/2, m_xSize, m_ySize);
    painter.setPen(QPen(m_colorText, 4));
//    painter.drawText(m_posText, m_strOut);
}
