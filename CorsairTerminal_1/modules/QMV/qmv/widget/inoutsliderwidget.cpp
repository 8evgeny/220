#include "inoutsliderwidget.h"
#include "ui_inoutwidget.h"
#include "enterkeyfilter.h"
#include "keyfilter.h"
#include <QVariant>
#include <QWheelEvent>
#include <QSlider>
#include <QLineEdit>

InoutSliderWidget::InoutSliderWidget(QWidget * parent):
    SetGetWidget(parent)
{
    val = new QLineEdit(this);
    slider_val = new QSlider(Qt::Horizontal, this);
    val->setObjectName("val");
    val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    val->setFixedHeight(20);
    slider_val->setObjectName("slider_val");
    slider_val->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slider_val->setFixedHeight(26);

    ui->root_layout->insertWidget(1, val);
    ui->widget_layout->insertWidget(1, slider_val);

    EnterKeyFilter * enterKeyFilter = new EnterKeyFilter(this);
    connect(enterKeyFilter, SIGNAL(enter_key_pressed()), this, SLOT(set_clicked()));
    connect(enterKeyFilter, SIGNAL(enter_key_pressed()), this, SLOT(moveSlider()));
    val->installEventFilter(enterKeyFilter);

    val->installEventFilter(new KeyFilter(this, KeyFilter::UINT));

    connect(slider_val, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));

    slider_val->installEventFilter(this);
}
void InoutSliderWidget::set_slider(const int & min, const int & max, const int & small_step, const int & big_step)
{
    slider_val->setRange(min, max);
    slider_val->setTickInterval(small_step);
    this->small_step = small_step;
    this->big_step = big_step;
}
bool InoutSliderWidget::eventFilter(QObject * watched, QEvent * event)
{
    if(watched == slider_val)
    {
        if(event->type() == QEvent::Wheel)
        {
            QWheelEvent * we = static_cast<QWheelEvent *>(event);
            if(we->delta() > 0) set_val(val->text().toInt() + small_step);
            else set_val(val->text().toInt() - small_step);
            set_clicked();
            return false;
        }
        else if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent * ke = static_cast<QKeyEvent *>(event);
            if(ke->key() == Qt::Key_Left) set_val(val->text().toInt() - big_step);
            else if(ke->key() == Qt::Key_Right) set_val(val->text().toInt() + big_step);
            else return false;
            set_clicked();
            return false;
        }
    }
    return false;
}
void InoutSliderWidget::sliderMoved(int v)
{
    val->setText(QString::number(v));
    set_clicked();
}
void InoutSliderWidget::moveSlider()
{
    bool ok;
    slider_val->setValue(val->text().toInt(&ok));
}
void InoutSliderWidget::set_val(const QVariant & v)
{
    moveSlider();
    bool ok;
    val->setText(QString::number(v.toInt(&ok), 10));
    moveSlider();
}
QVariant InoutSliderWidget::get_val()
{
    return val->text();
}
