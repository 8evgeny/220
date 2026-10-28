#include "vlx_media_parameters_dialog.h"
#include "ui_vlx_media_parameters_dialog.h"

#include <QShowEvent>
#include <QDebug>
#include <QPushButton>

#include "vlx_settings.h"
#include "vlx_media/vlx_av.h"
#include "vlx_stream_channel.h"
#include "libavutil/log.h"

static VlxMediaParametersDialog *s_inst = nullptr;

static int s_av_log_levels[] = {
    AV_LOG_QUIET,
    AV_LOG_PANIC,
    AV_LOG_FATAL,
    AV_LOG_ERROR,
    AV_LOG_WARNING,
    AV_LOG_INFO,
    AV_LOG_VERBOSE,
    AV_LOG_DEBUG,
    AV_LOG_TRACE,
};

static int _avLogLevelToIndex(int level)
{
    auto n = sizeof(s_av_log_levels) / sizeof(int);
    for(decltype(n) i = 0; i < n; i++)
        if (s_av_log_levels[i] == level)
            return (int)i;
    return -1;
}

static const char *s_av_log_level_names[] = {
    "AV_LOG_QUIET",
    "AV_LOG_PANIC",
    "AV_LOG_FATAL",
    "AV_LOG_ERROR",
    "AV_LOG_WARNING",
    "AV_LOG_INFO",
    "AV_LOG_VERBOSE",
    "AV_LOG_DEBUG",
    "AV_LOG_TRACE",
};

VlxMediaParametersDialog::VlxMediaParametersDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VlxMediaParametersDialog)
{
    ui->setupUi(this);
    s_inst = this;

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &VlxMediaParametersDialog::saveSettings);

    auto n = sizeof(s_av_log_levels) / sizeof(int);

    for(decltype(n) i = 0; i < n; i++)
        ui->cbAvLogLevel->addItem(s_av_log_level_names[i], s_av_log_levels[i]);
}

VlxMediaParametersDialog::~VlxMediaParametersDialog()
{
    delete ui;
}

VlxMediaParametersDialog *VlxMediaParametersDialog::inst()
{
    return s_inst;
}

void VlxMediaParametersDialog::saveSettings()
{
    auto &s = VlxSettings::i();
    s.setHwIndex(ui->gbHwAccel->isChecked()
                     ? ui->cbHwAccelSelector->currentIndex()
                     : -1);
    s.setDevId(ui->cbDevId->currentIndex() - 1);

    if (ui->cbAvLogLevel->currentIndex() >= 0) {
        auto l = ui->cbAvLogLevel->currentData().toInt();
        s.setAvLogLevel(l);
    }

    emit settingsSaved();
}

void VlxMediaParametersDialog::accept()
{
    saveSettings();

    QDialog::accept();
}

void VlxMediaParametersDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (ui->cbHwAccelSelector->count() == 0) {
        auto &list = Vlx::Av::InputStream::supportedHwAccelList();
        for(int i = 0; i < list.count(); i++) {
            auto name = list.at(i);
            ui->cbHwAccelSelector->addItem(name);
        }
    }

    auto hw_idx = VlxSettings::i().hwIndex();
    auto dev_id = VlxSettings::i().devId();

    ui->gbHwAccel->setChecked(hw_idx >= 0);
    ui->cbHwAccelSelector->setCurrentIndex(hw_idx);
    ui->cbDevId->setCurrentIndex(dev_id + 1);

    auto level = VlxSettings::i().avLogLevel();
    ui->cbAvLogLevel->setCurrentIndex( _avLogLevelToIndex(level) );
}
