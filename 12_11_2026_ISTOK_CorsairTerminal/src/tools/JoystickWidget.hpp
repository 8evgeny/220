#ifndef JOYSTICKWIDGET_HPP
#define JOYSTICKWIDGET_HPP

#include <QPen>
#include <QWidget>
#include <QTimer>

class JoystickWidget : public QWidget
{
    Q_OBJECT
public:
    JoystickWidget(QWidget *parent = nullptr);
    ~JoystickWidget() = default;

signals:
    void sg_newExcursion(const double & x, const double & y);

private slots:
    void sl_sendNewExcursion();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int32_t SIZE_X{320};
    static constexpr int32_t SIZE_Y{320};
    static constexpr int32_t FONT_SIZE{12};
    const QPoint m_posText{FONT_SIZE, SIZE_Y - FONT_SIZE};
    const QLine m_lineVert{SIZE_X/2, 0,        SIZE_X/2, SIZE_Y};
    const QLine m_lineHor{0,         SIZE_Y/2, SIZE_X,   SIZE_Y/2};

    QTimer *m_timer{nullptr};
    QPen m_penLine;
    QColor m_colorText;
    QColor m_colorPress;
    QColor m_colorRelease;
    QPen m_penRelease;
    QPen m_penPress;
    QBrush m_brushRelease;
    QBrush m_brushPress;

    bool m_isLeftButtonPress{false};
    int32_t m_xSize{50};
    int32_t m_ySize{50};
    int32_t m_x{SIZE_X/2};
    int32_t m_y{SIZE_Y/2};
    double m_xRelative{0};
    double m_yRelative{0};
    QString m_strOut;

    QTimer * m_joystickTimer;
};
#endif // JOYSTICKWIDGET_HPP
