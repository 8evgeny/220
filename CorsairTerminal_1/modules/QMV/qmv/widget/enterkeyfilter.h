#ifndef ENTERKEYFILTER_H
#define ENTERKEYFILTER_H

#include "qmv/qmv_global.h"
#include <QObject>

class QMVSHARED_EXPORT EnterKeyFilter : public QObject
{
    Q_OBJECT
public:
    explicit EnterKeyFilter(QObject *parent = 0);

    bool eventFilter(QObject * watched, QEvent * event) override;

signals:
    void enter_key_pressed();

public slots:
};

#endif // ENTERKEYFILTER_H
