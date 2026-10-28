#ifndef CMDPROPERTY_H
#define CMDPROPERTY_H

#include "qmv/qmv_global.h"
#include "property.h"

class QMVSHARED_EXPORT CmdProperty : public Property
{
    Q_OBJECT
public:
    explicit CmdProperty(QMVModel * model, uint32_t id, QString name);
public slots:
    virtual void view_cmd();
    virtual void waiting_stopped();
};

#endif // CMDPROPERTY_H
