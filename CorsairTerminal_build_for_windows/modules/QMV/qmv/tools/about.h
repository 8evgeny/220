#ifndef ABOUT_H
#define ABOUT_H

#include "qmv/qmv_global.h"
#include <QWidget>

namespace Ui {
class About;
}

class QMVSHARED_EXPORT About : public QWidget
{
    Q_OBJECT
public:
    ~About();

    static void about(QWidget * parent = 0, const QString & source = QString("qrc:/docs/about.html"));

private:
    explicit About(QWidget * parent = 0, const QString & source = QString("qrc:/docs/about.html"));

    void keyReleaseEvent(QKeyEvent * event);

private:
    Ui::About *ui;
};

#endif // ABOUT_H
