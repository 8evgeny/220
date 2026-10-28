#include "inoutwidget.h"
#include "ui_inoutwidget.h"
#include "waitinggifmoviesingleton.h"
#include "qmv/property/property.h"
#include <QVariant>
#include <QLabel>
#include <QMouseEvent>

InoutWidget::InoutWidget(QWidget * parent) :
    QWidget(parent),
    ui(new Ui::InoutWidget)
{
    ui->setupUi(this);

    waiting_label = new QLabel(this);
    waiting_label->hide();
    waiting_label->setAlignment(Qt::AlignCenter);
    waiting_label->setMovie(WaitingGifMovieSingleton::instance());
    waiting_label->setBackgroundRole(QPalette::Dark);
    //____________________________
//    waiting_label->setStyleSheet("background-color: rgba(100, 100, 100, 100);border:1px solid rgba(100,100,100,100);border-radius:5px;");
    // QFile styleF;
    // styleF.setFileName(":/QSS_master/ManjaroMix.qss"); // V
    // styleF.open(QFile::ReadOnly);
    // QString qssStr = styleF.readAll();
    // this->setStyleSheet(qssStr);
    //----------------------------
    waiting_label->setWindowOpacity(1.0);
    waiting_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    waiting_label->setFixedHeight(26);

    waiting_label->installEventFilter(this);

    set_neutral();
}
bool InoutWidget::eventFilter(QObject * watched, QEvent * event)
{
    if(watched == waiting_label)
    {
        if(event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent * me = static_cast<QMouseEvent*>(event);
            if(me->button() == Qt::LeftButton)
            {
                emit waiting_stopped();
                return true;
            }
        }
    }
    return false;
}
void InoutWidget::resizeEvent(QResizeEvent *)
{
    waiting_label->setFixedWidth(ui->root->width());
}
void InoutWidget::connect_property(Property * p)
{
    connect(p, SIGNAL(set_success()), this, SLOT(set_success()));
    connect(p, SIGNAL(set_error()), this, SLOT(set_error()));
    connect(p, SIGNAL(set_warning()), this, SLOT(set_warning()));
    connect(p, SIGNAL(set_info()), this, SLOT(set_info()));
    connect(p, SIGNAL(set_neutral()), this, SLOT(set_neutral()));
    connect(p, SIGNAL(set_disabled()), this, SLOT(set_disabled()));
    connect(p, SIGNAL(set_enabled()), this, SLOT(set_enabled()));

    connect(p, SIGNAL(start_waiting()), this, SLOT(start_waiting()));
    connect(p, SIGNAL(stop_waiting()), this, SLOT(stop_waiting()));

    connect(this, SIGNAL(waiting_stopped()), p, SLOT(waiting_stopped()));
}
void InoutWidget::start_waiting()
{
    waiting_label->show();
}
void InoutWidget::stop_waiting()
{
    waiting_label->hide();
}
void InoutWidget::set_error()
{
//    ui->root->setStyleSheet("#root {"
//                            "padding:2px;"
//                            "border:1px solid #CC0000;"
//                            "border-radius:5px;"
//                            "background-color:#ff4444;"
//                            "}");
}
void InoutWidget::set_success()
{
//    ui->root->setStyleSheet("#root {"
//                            "padding:2px;"
//                            "border:1px solid #007E33;"
//                            "border-radius:5px;"
//                            "background-color:#00C851;"
//                            "}");
}
void InoutWidget::set_warning()
{
//    ui->root->setStyleSheet("#root {"
//                            "padding:2px;"
//                            "border:1px solid #FF8800;"
//                            "border-radius:5px;"
//                            "background-color:#ffbb33;"
//                            "}");
}
void InoutWidget::set_info()
{
//    ui->root->setStyleSheet("#root {"
//                            "padding:2px;"
//                            "border:1px solid #0099CC;"
//                            "border-radius:5px;"
//                            "background-color:#33b5e5;"
//                            "}");
}
void InoutWidget::set_neutral()
{
//    ui->root->setStyleSheet("#root {"
//                            "padding:2px;"
//                            "border:1px solid #bdbdbd;"
//                            "border-radius:5px;"
//                            "background-color:#e0e0e0;"
//                            "}");
}
void InoutWidget::set_disabled()
{
    this->setDisabled(true);
}
void InoutWidget::set_enabled()
{
    this->setDisabled(false);
}
InoutWidget::~InoutWidget()
{
    delete ui;
}
