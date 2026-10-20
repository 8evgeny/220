#include "video.h"
#include "ui_video.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
//#include <QtOpenGL>
#include <cmath>

#define MIN_ZOOM    (1.0)
#define MAX_ZOOM    (8.0)

Video::Video(QWidget * parent) :
    QWidget(parent),
    ui(new Ui::Video)
{
    ui->setupUi(this);

    m_image = QImage(1920, 1080, QImage::Format_RGB888);

    m_zoom = 1;
    m_x_offs = 0;
    m_y_offs = 0;
    m_img_w  = 0;
    m_img_h  = 0;
    m_img_x0 = 0;
    m_img_y0 = 0;

    m_curr_mouse_x = 0;
    m_curr_mouse_y = 0;
    m_af_start_x   = 0;
    m_af_start_y   = 0;

    ui->x_sb->setRange(0, 0);
    ui->x_sb->setPageStep(1);

    ui->y_sb->setRange(0, 0);
    ui->y_sb->setPageStep(1);

    connect(ui->x_sb, SIGNAL(valueChanged(int)), SLOT(x_offs_changed(int)));
    connect(ui->y_sb, SIGNAL(valueChanged(int)), SLOT(y_offs_changed(int)));

    installEventFilter(this);
    setMouseTracking(true);

//    setToolTip("Увеличение: Ctrl + Колесо мыши\nСмещение по вертикали: Колесо мыши\nСмещение по горизонтали: Shift + Колесо мыши");

    QCursor cross = QCursor(QPixmap(":/docs/image/cross.png"));
    this->setCursor(cross);

    m_fullScreened = false;
}
void Video::full_screen(const bool & on_off)
{
    m_fullScreened = on_off;
    if(on_off)
    {
        ui->x_sb->hide();
        ui->y_sb->hide();
    }
    else
    {
        ui->x_sb->show();
        ui->y_sb->show();
    }
}
void Video::mousePressEvent(QMouseEvent * event)
{
    m_mouse_frame_pos_A = m_widget_to_frm_tr.map(event->pos());


    if(event->modifiers() == Qt::ControlModifier)
    {
        if(event->button() == Qt::LeftButton)
        {
            QPoint pos = event->pos();
            int x = (pos.x() - m_img_x0 - m_x_offs);
            if(x > m_img_w - 1) x = m_img_w - 1;
            else if(x < 0) x = 0;
            m_af_start_x = x;
            int y = (pos.y() - m_img_y0 - m_y_offs);
            if(y > m_img_h - 1) y = m_img_h - 1;
            else if(y < 0) y = 0;
            m_af_start_y = y;

            double ratio = double(m_image.width()) / double(m_img_w);
            x = int(round(x * ratio));
            y = int(round(y * ratio));

            m_start_af = QPoint(x, y);

            m_af_in_process = true;
        }
    }
    else if(event->button() == Qt::LeftButton)
    {
        QPoint pos = event->pos();
        int x = (pos.x() - m_img_x0 - m_x_offs);
        if(x > m_img_w - 1) x = m_img_w - 1;
        else if(x < 0) x = 0;
        m_strobe_start_x = x;
        int y = (pos.y() - m_img_y0 - m_y_offs);
        if(y > m_img_h - 1) y = m_img_h - 1;
        else if(y < 0) y = 0;
        m_strobe_start_y = y;

        double ratio = double(m_image.width()) / double(m_img_w);
        x = int(round(x * ratio));
        y = int(round(y * ratio));

        m_start_strobe = QPoint(x, y);

        m_designation_in_process = true;
    }
}
void Video::mouseReleaseEvent(QMouseEvent * event)
{
    m_mouse_frame_pos_B = m_widget_to_frm_tr.map(event->pos());

    if(event->button() == Qt::LeftButton)
    {
        if (m_designation_in_process)
        {
            m_designation_in_process = false;

            // QPoint pos = event->pos();
            // int x = (pos.x() - m_img_x0 - m_x_offs);
            // if(x > m_img_w - 1) x = m_img_w - 1;
            // else if(x < 0) x = 0;
            // int y = (pos.y() - m_img_y0 - m_y_offs);
            // if(y > m_img_h - 1) y = m_img_h - 1;
            // else if(y < 0) y = 0;

            // double ratio = double(m_image.width()) / double(m_img_w);
            // x = int(round(x * ratio));
            // y = int(round(y * ratio));

            // m_stop_strobe = QPoint(x, y);

            // int x1,y1,x2,y2;
            // if (m_start_strobe.x() <= m_stop_strobe.x()) {
            //     x1 = m_start_strobe.x();
            //     x2 = m_stop_strobe.x();
            // } else {
            //     x2 = m_start_strobe.x();
            //     x1 = m_stop_strobe.x();
            // }

            // if (m_start_strobe.y() <= m_stop_strobe.y()) {
            //     y1 = m_start_strobe.y();
            //     y2 = m_stop_strobe.y();
            // } else {
            //     y2 = m_start_strobe.y();
            //     y1 = m_stop_strobe.y();
            // }

            { // правильное получение рамки объекта в координатах реального кадра

                auto x0 = qBound(20, qMin(m_mouse_frame_pos_A.x(), m_mouse_frame_pos_B.x()), m_frame_size.width() - 20);
                auto x1 = qBound(20, qMax(m_mouse_frame_pos_A.x(), m_mouse_frame_pos_B.x()), m_frame_size.width() - 20);
                auto y0 = qBound(20, qMin(m_mouse_frame_pos_A.y(), m_mouse_frame_pos_B.y()), m_frame_size.height() - 20);
                auto y1 = qBound(20, qMax(m_mouse_frame_pos_A.y(), m_mouse_frame_pos_B.y()), m_frame_size.height() - 20);
                auto w = x1 - x0;
                auto h = y1 - y0;
                if (w < 40) {
                    x0 = qBound(0, x1 - w / 2 - 20, m_frame_size.width());
                    w = 40;
                }
                if (h < 40) {
                    y0 = qBound(0, y1 - h / 2 - 20, m_frame_size.height());
                    h = 40;
                }
                // qDebug() << "mouse rect:" << x0 << y0 << "[" <<w << h << "]";

                emit new_object(false, x0, y0, w, h);
            }


            // int w{x2 - x1};
            // int h{y2 - y1};
            // if(w >= 20 && h >= 20)  { // target area
            //     emit new_object(false, x1, y1, w, h);
            // } else { // point
            //     w = 40;
            //     h = 40;
            //     emit new_object(false, x1, y1, w, h);
            // }
        }
        else if (m_af_in_process)
        {
            m_af_in_process = false;

            QPoint pos = event->pos();
            int x = (pos.x() - m_img_x0 - m_x_offs);
            if(x > m_img_w - 1) x = m_img_w - 1;
            else if(x < 0) x = 0;
            int y = (pos.y() - m_img_y0 - m_y_offs);
            if(y > m_img_h - 1) y = m_img_h - 1;
            else if(y < 0) y = 0;

            double ratio = double(m_image.width()) / double(m_img_w);
            x = int(round(x * ratio));
            y = int(round(y * ratio));

            m_stop_af = QPoint(x, y);

            int x1,y1,x2,y2;
            if (m_start_af.x() <= m_stop_af.x()) {
                x1 = m_start_af.x();
                x2 = m_stop_af.x();
            } else {
                x2 = m_start_af.x();
                x1 = m_stop_af.x();
            }

            if (m_start_af.y() <= m_stop_af.y()) {
                y1 = m_start_af.y();
                y2 = m_stop_af.y();
            } else {
                y2 = m_start_af.y();
                y1 = m_stop_af.y();
            }

            emit new_af_zone(x1, y1, x2, y2);
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        emit stop_object(false);
    }
}

QPoint Video::displayToFrame(const QPoint &pos)
{

}

QPoint Video::frameToDisplay(const QPoint &pos)
{

}

bool Video::eventFilter(QObject *, QEvent * event)
{
    if(event->type() == QEvent::Wheel)
    {
        QWheelEvent * we = dynamic_cast<QWheelEvent *>(event);
        Qt::KeyboardModifiers kbm = we->modifiers();
        int d = we->delta();
        if(kbm & Qt::ControlModifier) // scale
        {
//            if(d < 0)
//            {
//                zoom /= 1.1;
//                if(zoom < MIN_ZOOM) zoom = MIN_ZOOM;
//            }
//            else
//            {
//                zoom *= 1.1;
//                if(zoom > MAX_ZOOM) zoom = MAX_ZOOM;
//            }
        }
        else if(kbm & Qt::ShiftModifier) // offset X
        {
            if(m_img_x0 < 0)
            {
                if(d < 0)
                {
                    m_x_offs -= 10;
                    if(m_x_offs < m_img_x0) m_x_offs = m_img_x0;
                }
                else
                {
                    m_x_offs += 10;
                    if(m_x_offs > -m_img_x0) m_x_offs = -m_img_x0;
                }
                ui->x_sb->setValue(m_x_offs);
            }
            else
            {
                m_x_offs = 0;
            }
        }
        else // offset Y
        {
            if(m_img_y0 < 0)
            {
                if(d > 0)
                {
                    m_y_offs -= 10;
                    if(m_y_offs < m_img_y0) m_y_offs = m_img_y0;
                }
                else
                {
                    m_y_offs += 10;
                    if(m_y_offs > -m_img_y0) m_y_offs = -m_img_y0;
                }
                ui->y_sb->setValue(m_y_offs);
            }
            else
            {
                m_y_offs = 0;
            }
        }
//        update();
        return true;
    }
    else if(event->type() == QEvent::MouseMove)
    {
        QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        int x = (pos.x() - m_img_x0 - m_x_offs);
        if(x > m_img_w - 1) x = m_img_w - 1;
        else if(x < 0) x = 0;
        m_curr_mouse_x = x;
        int y = (pos.y() - m_img_y0 - m_y_offs);
        if(y > m_img_h - 1) y = m_img_h - 1;
        else if(y < 0) y = 0;
        m_curr_mouse_y = y;
        double ratio = double(m_image.width()) / double(m_img_w);
        x = int(round(x * ratio));
        y = int(round(y * ratio));
        emit mouse_pos(x, y);
    }
    return false;
}
void Video::new_image(const QImage &img, const QSize &frm_size, const QTransform & img_to_frm_tr)
{
    m_image = img;
    m_frame_size = frm_size;
    m_img_to_frm_tr = img_to_frm_tr;
    update();
}
void Video::x_offs_changed(int val)
{
    m_x_offs = val;
//    update();
}
void Video::y_offs_changed(int val)
{
    m_y_offs = val;
    //    update();
}
#define ZOOM_EN
void Video::paintEvent(QPaintEvent *)
{
    int widget_w = m_fullScreened ? width()  : width()  - ui->y_sb->width();
    int widget_h = m_fullScreened ? height() : height() - ui->x_sb->height();
#ifdef ZOOM_EN
    int w = int(round(widget_w * m_zoom));
    int h = int(round(widget_h * m_zoom));
    QImage tmp;
    if((w != m_image.width()) || (h != m_image.height()))
    {
        tmp = m_image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        tmp = m_image;
    }
#else
    QImage tmp = *image;
#endif
    m_img_w  = tmp.width();
    m_img_h  = tmp.height();
    m_img_x0 = ( widget_w - m_img_w ) / 2;
    m_img_y0 = ( widget_h - m_img_h ) / 2;

    const QTransform T(1, 0, 0, 1,
                       -(m_img_x0 + m_x_offs),
                       -(m_img_y0 + m_y_offs));

    const double sx = double(m_image.width())  / double(m_img_w);
    const double sy = double(m_image.height()) / double(m_img_h);
    const QTransform S(sx, 0, 0, sy, 0, 0);

    m_widget_to_img_tr = (T * S);
    m_widget_to_frm_tr = m_widget_to_img_tr * m_img_to_frm_tr;

    QPainter p;
    p.begin(this);
    p.fillRect(0, 0, widget_w, widget_h, Qt::black);
    p.drawImage(m_img_x0 + m_x_offs, m_img_y0 + m_y_offs, tmp);
    if (m_af_in_process)
    {
        p.setPen(QPen(Qt::green, 2, Qt::DashLine));
        int x, y, w, h;
        x = (m_af_start_x <= m_curr_mouse_x) ? m_af_start_x : m_curr_mouse_x;
        y = (m_af_start_y <= m_curr_mouse_y) ? m_af_start_y : m_curr_mouse_y;
        w = (m_af_start_x <= m_curr_mouse_x) ? (m_curr_mouse_x - m_af_start_x) : (m_af_start_x - m_curr_mouse_x);
        h = (m_af_start_y <= m_curr_mouse_y) ? (m_curr_mouse_y - m_af_start_y) : (m_af_start_y - m_curr_mouse_y);
        p.drawRect(x + m_img_x0 + m_x_offs , y + m_img_y0 + m_y_offs , w, h);
    }
    else if (m_designation_in_process)
    {
        p.setPen(QPen(Qt::yellow, 2, Qt::DashLine));
        int x, y, w, h;
        x = (m_strobe_start_x <= m_curr_mouse_x) ? m_strobe_start_x : m_curr_mouse_x;
        y = (m_strobe_start_y <= m_curr_mouse_y) ? m_strobe_start_y : m_curr_mouse_y;
        w = (m_strobe_start_x <= m_curr_mouse_x) ? (m_curr_mouse_x - m_strobe_start_x) : (m_strobe_start_x - m_curr_mouse_x);
        h = (m_strobe_start_y <= m_curr_mouse_y) ? (m_curr_mouse_y - m_strobe_start_y) : (m_strobe_start_y - m_curr_mouse_y);
        p.drawRect(x + m_img_x0 + m_x_offs , y + m_img_y0 + m_y_offs , w, h);
    }
    p.end();
    if(m_img_x0 >= 0)
    {
        m_x_offs = 0;
        ui->x_sb->setRange(0, 0);
    }
    else
    {
        ui->x_sb->setRange(m_img_x0, -m_img_x0);
    }
    if(m_img_y0 >= 0)
    {
        m_y_offs = 0;
        ui->y_sb->setRange(0, 0);
    }
    else
    {
        ui->y_sb->setRange(m_img_y0, -m_img_y0);
    }
}
Video::~Video()
{
    delete ui;
}

QPoint Video::frameCenter() const
{
    QPoint c = rect().center();
    return m_widget_to_frm_tr.map(c);
}
