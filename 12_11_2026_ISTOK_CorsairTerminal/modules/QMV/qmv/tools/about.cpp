#include "about.h"
#include "ui_about.h"

#include <QKeyEvent>

About::About(QWidget * parent, const QString & source) :
    QWidget(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Ui::About)
{
    ui->setupUi(this);
    // Set Window Title
    setWindowTitle("About");
    // Set about.html source
    ui->text->setSource(source);
    // Resize
    resize(480, 300);
}
void About::about(QWidget * parent, const QString & source)
{
    static About * about = new About(parent, source);
    about->show();
}
void About::keyReleaseEvent(QKeyEvent * key_event)
{
    if(key_event->isAutoRepeat()) return;
    switch(key_event->key())
    {
    case Qt::Key_Escape:
        hide();
    }
}
About::~About()
{
    delete ui;
}
