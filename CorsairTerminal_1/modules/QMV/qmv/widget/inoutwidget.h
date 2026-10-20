#ifndef INOUTWIDGET_H
#define INOUTWIDGET_H

#include "qmv/qmv_global.h"
#include <QWidget>

namespace Ui {
class InoutWidget;
}

class QLabel;
class Property;

class QMVSHARED_EXPORT InoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InoutWidget(QWidget * parent = 0);
    ~InoutWidget();

    virtual void connect_property(Property *);

    void resizeEvent(QResizeEvent * event) override;
protected:
    bool eventFilter(QObject * watched, QEvent * event) override;

signals:
    void waiting_stopped();

public slots:
    void start_waiting();
    void stop_waiting();

    void set_error();
    void set_success();
    void set_warning();
    void set_info();
    void set_neutral();

    void set_disabled();
    void set_enabled();

protected:
    Ui::InoutWidget * ui;
    QLabel * waiting_label;
};

#endif // INOUTWIDGET_H
