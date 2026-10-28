#ifndef HELP_H
#define HELP_H

#include "qmv/qmv_global.h"
#include <QWidget>

namespace Ui {
class Help;
}

class QMVSHARED_EXPORT Help : public QWidget
{
    Q_OBJECT

public:
    ~Help();

    static void help(QWidget * parent = 0, const QString & source = QString("qrc:/docs/help.html"));

private:
    explicit Help(QWidget * parent = 0, const QString & source = QString("qrc:/docs/help.html"));

    void keyReleaseEvent(QKeyEvent * event);

private:
    Ui::Help *ui;
};

#endif // HELP_H
