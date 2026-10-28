#ifndef VIDEO_H
#define VIDEO_H

#include <QWidget>
#include <QImage>
#include <QMouseEvent>

namespace Ui {
class Video;
}

class Video : public QWidget
{
    Q_OBJECT

public:
    explicit Video(QWidget * parent = nullptr);
    virtual ~Video() override;

    QPoint frameCenter() const;

protected:
    void paintEvent(QPaintEvent *) override;
    bool eventFilter(QObject * watched, QEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

    QPoint displayToFrame(const QPoint &pos);
    QPoint frameToDisplay(const QPoint &pos);

public slots:
    void full_screen(const bool & on_off);

    void new_image(const QImage &img, const QSize &frm_size, const QTransform & img_to_frm_tr);

signals:
    void new_object(bool is_single, const int & x0, const int & y0, const int & w, const int & h);
    void video_click(const int & x0, const int & y0);
    void stop_object(bool is_single);
    void mouse_pos(const int & x, const int & y);

    void new_af_zone(const int & x0, const int & y0, const int & w, const int & h);

private slots:
    void x_offs_changed(int);
    void y_offs_changed(int);

private:

private:
    Ui::Video * ui;
    QImage m_image;
    QSize m_frame_size;
    QTransform m_img_to_frm_tr;
    QTransform m_widget_to_img_tr;
    QTransform m_widget_to_frm_tr;
    QPoint m_mouse_frame_pos_A;
    QPoint m_mouse_frame_pos_B;

    double m_zoom;
    int m_x_offs;
    int m_y_offs;
    int m_img_w;
    int m_img_h;
    int m_img_x0;
    int m_img_y0;

    int m_strobe_start_x;
    int m_strobe_start_y;
    int m_af_start_x;
    int m_af_start_y;
    int m_curr_mouse_x;
    int m_curr_mouse_y;

    bool m_designation_in_process = false;
    QPoint m_start_strobe;
    QPoint m_stop_strobe;
    bool m_af_in_process = false;
    QPoint m_start_af;
    QPoint m_stop_af;

    bool m_fullScreened;
};

#endif // VIDEO_H
