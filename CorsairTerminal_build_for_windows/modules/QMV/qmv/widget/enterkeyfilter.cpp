#include "enterkeyfilter.h"
#include <QKeyEvent>
#include <QEvent>

EnterKeyFilter::EnterKeyFilter(QObject * parent) : QObject(parent)
{
}
bool EnterKeyFilter::eventFilter(QObject *, QEvent * event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            emit enter_key_pressed();
        }
    }
    return false;
}
