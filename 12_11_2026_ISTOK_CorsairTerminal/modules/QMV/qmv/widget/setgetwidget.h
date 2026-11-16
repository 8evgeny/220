#ifndef SETGETWIDGET_H
#define SETGETWIDGET_H

#include "qmv/qmv_global.h"
#include "inoutwidget.h"
#include <QVariant>
class QLabel;
class QPushButton;

class QMVSHARED_EXPORT SetGetWidget : public InoutWidget
{
    Q_OBJECT
public:
    explicit SetGetWidget(QWidget * parent = 0);

    void hide_set_btn();
    void hide_get_btn();

    void show_set_btn();
    void show_get_btn();

    virtual void connect_property(Property *);
public slots:
    virtual void set_val(const QVariant &) = 0;
    virtual QVariant get_val() = 0;
    void set_label_text(const QString &);
    void set_label_width(const int &);
protected slots:
    void set_clicked();
    void get_clicked();
signals:
    void val_set(const QVariant &);
    QVariant val_get();
protected:
    QLabel * label;
    QPushButton * set;
    QPushButton * get;
};

#endif // SETGETWIDGET_H
