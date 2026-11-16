#include "keyfilter.h"
#include <QKeyEvent>
#include <QEvent>

KeyFilter::KeyFilter(QObject * parent, Type type) : QObject(parent), type(type)
{
}
bool KeyFilter::eventFilter(QObject *, QEvent * event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
        if(     keyEvent->matches(QKeySequence::Copy) ||
                keyEvent->matches(QKeySequence::SelectAll) ||
                keyEvent->matches(QKeySequence::Redo) ||
                keyEvent->matches(QKeySequence::Undo) ||
                keyEvent->matches(QKeySequence::Paste) ||
                keyEvent->matches(QKeySequence::Cut)    )
        {
            return false;
        }
        switch(keyEvent->key())
        {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Backspace:
        case Qt::Key_Delete:
        case Qt::Key_End:
        case Qt::Key_Home:
        case Qt::Key_Copy:
        case Qt::Key_Paste:
        case Qt::Key_Cut:
        case Qt::Key_Select:
        case Qt::Key_Undo:
        case Qt::Key_Redo:
        case Qt::Key_Control:
        case Qt::Key_Shift:
            return false;
        }
        switch(type)
        {
        case INT:
            switch(keyEvent->key())
            {
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
            case Qt::Key_Minus:
                return false;
            default:
                return true;
            }
        case UINT:
            switch(keyEvent->key())
            {
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
                return false;
            default:
                return true;
            }
        case FLOAT:
            switch(keyEvent->key())
            {
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
            case Qt::Key_Minus:
            case Qt::Key_Period:
                return false;
            default:
                return true;
            }
        case UFLOAT:
            switch(keyEvent->key())
            {
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
            case Qt::Key_Period:
                return false;
            default:
                return true;
            }
        case BIN:
            return true;
        case HEX:
            switch(keyEvent->key())
            {
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
            case Qt::Key_A:
            case Qt::Key_B:
            case Qt::Key_C:
            case Qt::Key_D:
            case Qt::Key_E:
            case Qt::Key_F:
                return false;
            default:
                return true;
            }
        default:
            return false;
        }
    }
    return false;
}
