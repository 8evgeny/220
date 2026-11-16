#include "vlx_cmd_info_dialog.h"
#include "ui_vlx_cmd_info_dialog.h"

#include <QTextStream>
#include <QDateTime>
#include <QScrollBar>
#include <QFileDialog>

VlxCmdInfoDialog::VlxCmdInfoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VlxCmdInfoDialog)
{
    ui->setupUi(this);
    connect(ui->pbOk, &QPushButton::clicked, this, &VlxCmdInfoDialog::close);
    connect(ui->pbClear, &QPushButton::clicked, ui->pte, &QPlainTextEdit::clear);

    connect(ui->pbSave, &QPushButton::clicked, this, [this]{
        auto fn = QFileDialog::getSaveFileName(nullptr, "Сохранить файл", {}, "Txt (*.txt);;Все(*.*)");
        if (fn.isEmpty())
            return;

        QFile file(fn);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << ui->pte->toPlainText();
            file.close();
        }
    });
}

VlxCmdInfoDialog::~VlxCmdInfoDialog()
{
    delete ui;
}

void VlxCmdInfoDialog::connectCmdManger(CmdManager *mgr)
{
    connect(mgr, &CmdManager::notifyCmd, this, [this](COMNEW cmd, QVariantMap m){
        if (!isVisible())
            return;

        QString str = ui->pte->toPlainText();
        QTextStream ts(&str, QIODevice::OpenModeFlag::Append);

        ts << "*** " << QDateTime::currentDateTime().toString("hh:mm:ss::zzz\n");

        switch (cmd) {
        case SET_SPEED:
            ts << "SET_SPEED:\n";
            ts << "  X: " << m["x"].toString() << "\n";
            ts << "  Z: " << m["z"].toString() << "\n";
            break;
        case MAN_ZOOM_IN:
            ts << "MAN_ZOOM_IN:\n";
            ts << " speed: " << m["speed"].toString();
            break;
        case MAN_ZOOM_OUT:
            ts << "MAN_ZOOM_OUT:\n";
            ts << " speed: " << m["speed"].toString();
            break;
        case MAN_ZOOM_STOP:
            ts << "MAN_ZOOM_STOP:\n";
            break;
        case MAN_FOCUS_NEAR:
            ts << "MAN_FOCUS_NEAR:\n";
            ts << " speed: " << m["speed"].toString();
            break;
        case MAN_FOCUS_FAR:
            ts << "MAN_FOCUS_FAR:\n";
            ts << " speed: " << m["speed"].toString();
            break;
        case MAN_FOCUS_STOP:
            ts << "MAN_FOCUS_STOP:\n";
            break;
        default:
            ts << "--:" << cmd << "  \n";
            break;
        }
        ui->pte->setPlainText(str);
        QScrollBar *vScrollBar = ui->pte->verticalScrollBar();
        vScrollBar->setValue(vScrollBar->maximum());
    });
}

void VlxCmdInfoDialog::openMonitor()
{
    if (!isVisible()) {
        QString str = ui->pte->toPlainText();
        QTextStream ts(&str, QIODevice::OpenModeFlag::Append);
        ts << "==== " << QDateTime::currentDateTime().toString("hh:mm:ss::zzz\n");
        ui->pte->setPlainText(str);
        QScrollBar *vScrollBar = ui->pte->verticalScrollBar();
        vScrollBar->setValue(vScrollBar->maximum());
    }

    QDialog::show();
    QDialog::raise();
    QDialog::activateWindow();
}
