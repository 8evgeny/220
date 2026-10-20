#include "accessor.h"

#include <QMessageBox>
#include <QInputDialog>

Accessor::Accessor() :
    password(QString("")),
    accessed(false)
{
}
Accessor & Accessor::instance()
{
    static Accessor accessor;
    return accessor;
}
void Accessor::set_password(const QString & p)
{
    instance()._set_password(p);
}
QString Accessor::get_passowrd()
{
    return instance()._get_password();
}
void Accessor::_set_password(const QString & p)
{
    password = p;
}
QString Accessor::_get_password()
{
    return password;
}
void Accessor::reset()
{
    instance().reset_accessed();
}
bool Accessor::check(QWidget * parent)
{
    if(instance().is_accessed())
    {
        return true;
    }
    QString password = instance()._get_password();
    if(password == QString(""))
    {
        return true;
    }
    bool ok;
    QString p = QInputDialog::getText(parent, QString("Проверка прав доступа"), QString("Введите пароль:"), QLineEdit::Password, QString(""), &ok, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    if(ok)
    {
        if(p == password)
        {
            instance().set_accessed();
            return true;
        }
    }
    QMessageBox::critical(parent, QString("Проверка прав доступа"), QString("Доступ закрыт - неправильный пароль."));
    return false;
}
