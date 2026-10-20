#ifndef MVVIEW_H
#define MVVIEW_H

#include <QWidget>
#include "qmv/qmv_global.h"
class QMVModel;
class InoutStringWidget;

class QMVSHARED_EXPORT QMVView : public QWidget
{
    Q_OBJECT
public:
    explicit QMVView(QWidget * parent = 0);

    void set_model(QMVModel *);

protected:
    virtual void connect_model(){}

    void keyReleaseEvent(QKeyEvent * event);

signals:

public slots:
    void warning(const QString &, const QString &);
    void information(const QString &, const QString &);
    void critical(const QString &, const QString &);

private slots:
    void on_load_clicked();
    void on_save_clicked();

protected:
    QMVModel * model;
    InoutStringWidget * last_path;
    InoutStringWidget * version;
};

#endif // MVVIEW_H
