#ifndef VLX_SDL_DUMP_DIALOG_H
#define VLX_SDL_DUMP_DIALOG_H

#include <QDialog>
#include <qtimer.h>

#include <SDL2/SDL.h>


namespace Ui {
class VlxSdlDumpDialog;
}

class Joystick;

class VlxSdlDumpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VlxSdlDumpDialog(QWidget *parent = nullptr);
    ~VlxSdlDumpDialog();

public slots:

    void openDump();

private slots:

    void updateDump();

private:


private:
    Ui::VlxSdlDumpDialog *ui;
    QTimer m_timUpdate;
    QString m_dump;
    SDL_Joystick *m_joystick;
};

#endif // VLX_SDL_DUMP_DIALOG_H
