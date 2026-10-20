#ifndef VLX_DISPLAY_PARAMETERS_DIALOG_H
#define VLX_DISPLAY_PARAMETERS_DIALOG_H

#include <QDialog>

namespace Ui {
class VlxDisplayParametersDialog;
}

class VlxDisplayParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VlxDisplayParametersDialog(QWidget *parent = nullptr);
    ~VlxDisplayParametersDialog();

    static VlxDisplayParametersDialog *inst();

protected slots:

     void saveSettings();

signals:

    void settingsSaved();

public slots:

    virtual void accept() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::VlxDisplayParametersDialog *ui;
};

#endif // VLX_DISPLAY_PARAMETERS_DIALOG_H
