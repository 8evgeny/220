#pragma once

#include <QDialog>


namespace Ui {
class VlxMediaParametersDialog;
}

class VlxMediaParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VlxMediaParametersDialog(QWidget *parent = nullptr);
    ~VlxMediaParametersDialog();

    static VlxMediaParametersDialog *inst();

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
    Ui::VlxMediaParametersDialog *ui;

};
