#ifndef INOUTCMDWIDGET_H
#define INOUTCMDWIDGET_H

#include "qmv/qmv_global.h"
#include "inoutwidget.h"
class QPushButton;

class QMVSHARED_EXPORT InoutCmdWidget : public InoutWidget
{
    Q_OBJECT
public:
    explicit InoutCmdWidget(QWidget * parent = 0);
    virtual void connect_property(Property *);

public slots:
    void set_cmd_icon(const QString&, const int& width = 16, const int& height = 16);
    void set_cmd_text(const QString&);
    void set_cmd_width(const int&);

signals:
    void cmd();

private:
    QPushButton * cmdbtn;
};

#endif // INOUTCMDWIDGET_H
