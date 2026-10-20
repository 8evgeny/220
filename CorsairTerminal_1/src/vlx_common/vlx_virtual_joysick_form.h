#ifndef VLX_VIRTUAL_JOYSICK_FORM_H
#define VLX_VIRTUAL_JOYSICK_FORM_H

#include <QWidget>

namespace Ui {
class VlxVirtualJoysickForm;
}

class VlxVirtualJoysickForm : public QWidget
{
    Q_OBJECT

public:
    explicit VlxVirtualJoysickForm(QWidget *parent = nullptr);
    ~VlxVirtualJoysickForm();
protected:

    void updatePosition();

private:
    Ui::VlxVirtualJoysickForm *ui;
    QPoint m_pos;
    QPoint m_c;
    QPointF m_k;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void showEvent(QShowEvent *event);
};

#endif // VLX_VIRTUAL_JOYSICK_FORM_H
