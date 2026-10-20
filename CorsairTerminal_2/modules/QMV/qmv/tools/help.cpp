#include "help.h"
#include "ui_help.h"

#include <QKeyEvent>

Help::Help(QWidget * parent, const QString & source) :
    QWidget(parent, Qt::Window),
    ui(new Ui::Help)
{
    ui->setupUi(this);
    // Set Window Title
    setWindowTitle("Help");
    // Set about.html source
    ui->text->setSource(source);
    // Resize
    resize(640, 480);
}
void Help::help(QWidget * parent, const QString & source)
{
    static Help * help = new Help(parent, source);
    help->show();
    help->raise();
}
void Help::keyReleaseEvent(QKeyEvent * key_event)
{
    if(key_event->isAutoRepeat()) return;
    switch(key_event->key())
    {
    case Qt::Key_Escape:
        hide();
    }
}
Help::~Help()
{
    delete ui;
}
