#include "vlx_display_parameters_dialog.h"
#include "ui_vlx_display_parameters_dialog.h"

#include "vlx_settings.h"

#include <QPushButton>

static VlxDisplayParametersDialog *s_inst = nullptr;

VlxDisplayParametersDialog::VlxDisplayParametersDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VlxDisplayParametersDialog)
{
    ui->setupUi(this);
    s_inst = this;
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &VlxDisplayParametersDialog::saveSettings);

    ui->cbJoystickPollFreq->addItem("20", 50);
    ui->cbJoystickPollFreq->addItem("10", 100);
    ui->cbJoystickPollFreq->addItem("5", 200);
    ui->cbJoystickPollFreq->addItem("2", 500);

}

VlxDisplayParametersDialog::~VlxDisplayParametersDialog()
{
    delete ui;
}

VlxDisplayParametersDialog *VlxDisplayParametersDialog::inst()
{
    return s_inst;
}

void VlxDisplayParametersDialog::saveSettings()
{
    VlxSettings::i().setDispAlignmentCross(ui->chbAlignmentCrossEn->isChecked());
    VlxSettings::i().setJoystickPollInterval(ui->cbJoystickPollFreq->currentData().toInt());

    VlxSettings::i().setVirtualJoystickUsage(ui->chbVirtJoystick->isChecked());

    emit settingsSaved();
}

void VlxDisplayParametersDialog::accept()
{
    saveSettings();
    QDialog::accept();
}

void VlxDisplayParametersDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    ui->chbAlignmentCrossEn->setChecked(VlxSettings::i().dispAlignmentCross());

    auto v = VlxSettings::i().joystickPollIntervalMs();
    auto min_d = std::numeric_limits<int>::max();
    int min_i = 0;
    for(int i = 0; i < ui->cbJoystickPollFreq->count(); i++) {
        auto d = std::abs(ui->cbJoystickPollFreq->itemData(i).toInt() - v);
        if (min_d > d) {
            min_d = d;
            min_i = i;
        }
        if (d == 0)
            break;
    }
    ui->cbJoystickPollFreq->setCurrentIndex(min_i);

    ui->chbVirtJoystick->setChecked(VlxSettings::i().useVirtualJoystick());
}
