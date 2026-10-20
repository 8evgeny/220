#ifndef KEYFILTER_H
#define KEYFILTER_H

#include "qmv/qmv_global.h"
#include <QObject>

class QMVSHARED_EXPORT KeyFilter : public QObject
{
    Q_OBJECT
public:    
    enum Type {
        INT,
        UINT,
        FLOAT,
        UFLOAT,
        BIN,
        HEX,
        STRING
    };

    explicit KeyFilter(QObject * parent = 0, Type type = INT);

    bool eventFilter(QObject * watched, QEvent * event) override;

signals:

public slots:

private:
    const Type type;
};

#endif // KEYFILTER_H
