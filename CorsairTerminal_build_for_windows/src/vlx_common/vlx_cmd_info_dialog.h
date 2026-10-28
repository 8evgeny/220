#ifndef VLX_CMD_INFO_DIALOG_H
#define VLX_CMD_INFO_DIALOG_H

#include <QDialog>
#include "device_parts/cmd/cmdmanager.h"

namespace Ui {
class VlxCmdInfoDialog;
}

class VlxCmdInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VlxCmdInfoDialog(QWidget *parent = nullptr);
    ~VlxCmdInfoDialog();

    void connectCmdManger(CmdManager *mgr);

public slots:

    void openMonitor();

private:
    Ui::VlxCmdInfoDialog *ui;
};

#endif // VLX_CMD_INFO_DIALOG_H
