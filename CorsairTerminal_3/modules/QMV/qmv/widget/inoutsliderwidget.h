#ifndef INOUTSLIDERWIDGET_H
#define INOUTSLIDERWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QSlider;
class QLineEdit;

class QMVSHARED_EXPORT InoutSliderWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit InoutSliderWidget(QWidget * parent = 0);

    void set_slider(const int & min, const int & max, const int & small_step, const int & big_step);

    bool eventFilter(QObject *watched, QEvent *event) override;
public slots:
    void set_val(const QVariant &) override;
    QVariant get_val() override;
private slots:
    void sliderMoved(int);
    void moveSlider();
private:
    QLineEdit * val;
    QSlider * slider_val;

    int small_step;
    int big_step;
};

#endif // INOUTSLIDERWIDGET_H
