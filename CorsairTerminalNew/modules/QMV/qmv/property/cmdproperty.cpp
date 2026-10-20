#include "cmdproperty.h"
#include "qmv/qmvmodel.h"
#include <QVariant>

CmdProperty::CmdProperty(QMVModel * model, uint32_t id, QString name) :
    Property(model, id, name)
{
    model->insert_cmd_property(id, name, this);
}
void CmdProperty::view_cmd()
{
    model->cmd(id);
}
void CmdProperty::waiting_stopped()
{
    model->cmd(id);
}
