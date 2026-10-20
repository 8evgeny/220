#pragma once

#include "vlx_input_cmd.hpp"
#include "vlx_input_mapping.hpp"

#include <QDialog>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QString>

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;

class QToolButton;
class QLineEdit;

namespace Ui { class VlxJoystickMappingForm; }

namespace Vlx::Input {

class JoystickMappingForm : public QDialog {
    Q_OBJECT
public:
    explicit JoystickMappingForm(QWidget* parent = nullptr);
    ~JoystickMappingForm() override;

    QString mappingPath() const                 { return _path; }
    void    setMappingPath(const QString& path) { _path = path; }

    void setAxisLearnDurationMs(int ms) { _learnDurationMs = qMax(200, ms); }
    int  axisLearnDurationMs() const    { return _learnDurationMs; }

signals:
    void mappingChanged(const QString& path);

protected:
    void closeEvent(QCloseEvent* e) override;

private slots:
    // profile management
    void onRefreshJoysticks();
    void onJoystickComboIndexChanged(int index);
    void onJoystickComboEditFinished();
    void onAddJoystick();
    void onCloneJoystick();
    void onDeleteJoystick();

           // mapping list
    void onMappingSelectionChanged();
    void onAddAxis();
    void onAddButton();
    void onRemoveMapping();

           // editor → model
    void saveAxisToModel();
    void saveButtonToModel();

           // learn
    void onAxisLearn();
    void onButtonLearn();
    void onHatLearn();
    void onLearnTick();

           // json io
    void onImport();
    void onExport();
    void onMerge();
    void onReload();
    void onSave();

           // preview
    void onPreviewTick();

private:
    enum class LearnTarget { None, Axis, Button, Hat };

    void setupConnections();
    void buildCommandCombos();
    void buildHatMaskCombo();
    void attachAddMenu(QToolButton* btn, QLineEdit* edit);

    void loadProfile(const QString& name);
    void selectProfile(const QString& name);
    JoystickMapping* currentProfile();

    void rebuildMappingList();
    void updateItemText(int kind, int idx);
    void showAxis(int idx);
    void showButton(int idx);

    void startLearn(LearnTarget target);
    void finishLearn(const QString& msg);

    void rebuildActiveTable();
    void updateActiveTable();
    void openPreview();
    void closePreview();

    void updateStatus(const QString& text);

    Ui::VlxJoystickMappingForm* ui;

    MappingConfig _config;
    QString       _path;
    QString       _currentProfileName;

    int  _curKind  = -1;
    int  _curIndex = -1;
    bool _updating = false;

    int _currentJoyIndex = -1;

           // --- learn state ---
    SDL_Joystick* _learnJoy = nullptr;
    QTimer        _learnTimer;
    QElapsedTimer _learnElapsed;
    int           _learnDurationMs = 2000;
    int           _learnThreshold  = 6000;
    bool          _axisLearnActive = false;
    LearnTarget   _learnTarget = LearnTarget::None;
    QVector<int>  _prevAxes;
    QVector<int>  _prevButtons;
    QVector<int>  _prevHats;

    // --- preview state ---
    SDL_Joystick* _previewJoy = nullptr;
    QTimer        _previewTimer;
    bool          _previewRebuildNeeded = true;
    int           _previewAxes    = 0;
    int           _previewButtons = 0;
    int           _previewHats    = 0;
};

} // namespace Vlx::Input
