#ifndef ACCESSOR_H
#define ACCESSOR_H

#include "qmv/qmv_global.h"
#include <QObject>

class QMVSHARED_EXPORT Accessor
{
private:
    Accessor();
public:
    static Accessor & instance();
    static bool check(QWidget * parent = 0);
    static void set_password(const QString &);
    static QString get_passowrd();
    static void reset();
    void _set_password(const QString &);
    QString _get_password();
    bool is_accessed() {return accessed;}
    void set_accessed() {accessed = true;}
    void reset_accessed() {accessed = false;}
private:
    QString password;
    bool accessed;
};

#endif // ACCESSOR_H
