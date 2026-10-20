#ifndef OUTPLAINTEXTWIDGET_H
#define OUTPLAINTEXTWIDGET_H

#include "qmv/qmv_global.h"
#include "setgetwidget.h"
class QPlainTextEdit;

class QMVSHARED_EXPORT OutPlainTextWidget : public SetGetWidget
{
    Q_OBJECT
public:
    explicit OutPlainTextWidget(QWidget * parent = 0);

    virtual void connect_property(Property *);
public slots:
    void set_val(const QVariant &) override;
    void append_val(const QVariant &);
    void clear_val();
    QVariant get_val() override;
private:
    QPlainTextEdit * val;
};

#endif // OUTPLAINTEXTWIDGET_H
