#include "vlx_input_joystick_mapping_form.hpp"
#include "ui_vlx_input_joystick_mapping_form.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolButton>
#include <QVBoxLayout>

#include <SDL2/SDL.h>

#include <algorithm>
#include <cmath>

namespace Vlx::Input {

// ============================================================================
// helpers
// ============================================================================

namespace {

QIcon makeDotIcon(const QColor& color) {
    QPixmap pm(12, 12);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(2, 2, 8, 8);
    p.end();
    return QIcon(pm);
}

QIcon iconConnected() { static QIcon i = makeDotIcon(QColor(0, 180, 0));   return i; }
QIcon iconSaved()     { static QIcon i = makeDotIcon(QColor(160, 160, 160)); return i; }

const QColor kActiveAxisBg  (210, 255, 210);
const QColor kPressedBtnBg  (210, 220, 255);
const QColor kIdleBg        (255, 255, 255);

} // namespace

// ============================================================================
// ctor / dtor
// ============================================================================

JoystickMappingForm::JoystickMappingForm(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::VlxJoystickMappingForm)
{
    ui->setupUi(this);

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    _path = MappingConfig::defaultPath();

    buildCommandCombos();
    buildHatMaskCombo();
    setupConnections();

           // Table styling
    ui->activeTable->setColumnCount(4);
    ui->activeTable->setHorizontalHeaderLabels(
        { tr("Source"), tr("Mapping"), tr("Raw"), tr("State") });
    ui->activeTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->activeTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->activeTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->activeTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->activeTable->verticalHeader()->setVisible(false);

           // Combo editing behavior
    ui->joystickCombo->setInsertPolicy(QComboBox::NoInsert);

           // Splitters
    ui->vSplitter->setChildrenCollapsible(false);
    ui->hSplitter->setChildrenCollapsible(false);

           // Timers
    _learnTimer.setInterval(16);
    connect(&_learnTimer, &QTimer::timeout, this, &JoystickMappingForm::onLearnTick);

    _previewTimer.setInterval(33);
    connect(&_previewTimer, &QTimer::timeout, this, &JoystickMappingForm::onPreviewTick);
    _previewTimer.start();

    onReload();
    onRefreshJoysticks();

    ui->editorStack->setCurrentWidget(ui->emptyPage);
}

JoystickMappingForm::~JoystickMappingForm() {
    _learnTimer.stop();
    _previewTimer.stop();
    if (_learnJoy)   { SDL_JoystickClose(_learnJoy);   _learnJoy = nullptr;   }
    if (_previewJoy) { SDL_JoystickClose(_previewJoy); _previewJoy = nullptr; }
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    delete ui;
}

// ============================================================================
// combos / add-menu
// ============================================================================

void JoystickMappingForm::buildCommandCombos() {
    ui->axisCmdCombo->clear();

    ui->axisCmdCombo->addItem(cmdDisplayName(Cmd::Axis_None),
                              static_cast<int>(Cmd::Axis_None));
    ui->axisCmdCombo->setItemData(ui->axisCmdCombo->count() - 1,
                                  cmdDescription(Cmd::Axis_None), Qt::ToolTipRole);

    for (Cmd c : axisCmds()) {
        ui->axisCmdCombo->addItem(cmdDisplayName(c), static_cast<int>(c));
        ui->axisCmdCombo->setItemData(ui->axisCmdCombo->count() - 1,
                                      cmdDescription(c), Qt::ToolTipRole);
    }
}

void JoystickMappingForm::buildHatMaskCombo() {
    ui->hatMaskCombo->clear();
    ui->hatMaskCombo->addItem(tr("None"),  0);
    ui->hatMaskCombo->addItem(tr("Up"),    SDL_HAT_UP);
    ui->hatMaskCombo->addItem(tr("Down"),  SDL_HAT_DOWN);
    ui->hatMaskCombo->addItem(tr("Left"),  SDL_HAT_LEFT);
    ui->hatMaskCombo->addItem(tr("Right"), SDL_HAT_RIGHT);
}

void JoystickMappingForm::attachAddMenu(QToolButton* btn, QLineEdit* edit) {
    auto* menu = new QMenu(btn);
    for (Cmd c : actionCmds()) {
        QAction* act = menu->addAction(cmdDisplayName(c), this, [this, edit, c]() {
            QString cur = edit->text().trimmed();
            if (!cur.isEmpty() && !cur.endsWith(QLatin1Char(',')))
                cur += QStringLiteral(", ");
            cur += cmdKey(c);          // <-- в QLineEdit пишем КЛЮЧ (round-trip с JSON)
            edit->setText(cur);
            saveButtonToModel();
        });
        act->setToolTip(cmdDescription(c));
        act->setData(cmdKey(c));
    }
    btn->setMenu(menu);
    btn->setPopupMode(QToolButton::InstantPopup);
}

// ============================================================================
// connections
// ============================================================================

void JoystickMappingForm::setupConnections() {
    // profile bar
    connect(ui->refreshJoysticksButton, &QToolButton::clicked,
            this, &JoystickMappingForm::onRefreshJoysticks);
    connect(ui->joystickCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &JoystickMappingForm::onJoystickComboIndexChanged);
    if (ui->joystickCombo->lineEdit()) {
        connect(ui->joystickCombo->lineEdit(), &QLineEdit::editingFinished,
                this, &JoystickMappingForm::onJoystickComboEditFinished);
    }
    connect(ui->addJoystickButton,    &QToolButton::clicked,
            this, &JoystickMappingForm::onAddJoystick);
    connect(ui->cloneJoystickButton,  &QToolButton::clicked,
            this, &JoystickMappingForm::onCloneJoystick);
    connect(ui->deleteJoystickButton, &QToolButton::clicked,
            this, &JoystickMappingForm::onDeleteJoystick);

           // mapping list
    connect(ui->mappingList, &QListWidget::currentRowChanged,
            this, &JoystickMappingForm::onMappingSelectionChanged);
    connect(ui->addAxisButton,       &QToolButton::clicked, this, &JoystickMappingForm::onAddAxis);
    connect(ui->addButtonButton,     &QToolButton::clicked, this, &JoystickMappingForm::onAddButton);
    connect(ui->removeMappingButton, &QToolButton::clicked, this, &JoystickMappingForm::onRemoveMapping);

           // json io
    connect(ui->importButton, &QToolButton::clicked, this, &JoystickMappingForm::onImport);
    connect(ui->exportButton, &QToolButton::clicked, this, &JoystickMappingForm::onExport);
    connect(ui->mergeButton,  &QToolButton::clicked, this, &JoystickMappingForm::onMerge);
    connect(ui->reloadButton, &QToolButton::clicked, this, &JoystickMappingForm::onReload);
    connect(ui->saveButton,   &QToolButton::clicked, this, &JoystickMappingForm::onSave);

           // axis editor
    connect(ui->axisCmdCombo,        QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &JoystickMappingForm::saveAxisToModel);
    connect(ui->axisIndexSpin,       QOverload<int>::of(&QSpinBox::valueChanged),
            this, &JoystickMappingForm::saveAxisToModel);
    connect(ui->axisSensitivitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &JoystickMappingForm::saveAxisToModel);
    connect(ui->axisDeadZoneSpin,    QOverload<int>::of(&QSpinBox::valueChanged),
            this, &JoystickMappingForm::saveAxisToModel);
    connect(ui->axisFilterSpin,      QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &JoystickMappingForm::saveAxisToModel);
    connect(ui->axisInvertedCheck,   &QCheckBox::toggled,
            this, &JoystickMappingForm::saveAxisToModel);
    connect(ui->axisLearnButton,     &QToolButton::clicked,
            this, &JoystickMappingForm::onAxisLearn);

           // button editor
    connect(ui->buttonIndexSpin,   QOverload<int>::of(&QSpinBox::valueChanged),
            this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->hatIndexSpin,      QOverload<int>::of(&QSpinBox::valueChanged),
            this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->hatMaskCombo,      QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->buttonPressEdit,   &QLineEdit::editingFinished, this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->buttonReleaseEdit, &QLineEdit::editingFinished, this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->buttonToggleCheck, &QCheckBox::toggled,         this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->buttonToggleAEdit, &QLineEdit::editingFinished, this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->buttonToggleBEdit, &QLineEdit::editingFinished, this, &JoystickMappingForm::saveButtonToModel);
    connect(ui->buttonLearnButton, &QToolButton::clicked, this, &JoystickMappingForm::onButtonLearn);
    connect(ui->hatLearnButton,    &QToolButton::clicked, this, &JoystickMappingForm::onHatLearn);

    attachAddMenu(ui->buttonPressAdd,   ui->buttonPressEdit);
    attachAddMenu(ui->buttonReleaseAdd, ui->buttonReleaseEdit);
    attachAddMenu(ui->buttonToggleAAdd, ui->buttonToggleAEdit);
    attachAddMenu(ui->buttonToggleBAdd, ui->buttonToggleBEdit);
}

// ============================================================================
// profile combo — refresh / select / add / clone / delete
// ============================================================================

void JoystickMappingForm::onRefreshJoysticks() {
    if (!_currentProfileName.isEmpty() && !_config.find(_currentProfileName))
        _currentProfileName.clear();

    const QString prev = _currentProfileName;

    {
        QSignalBlocker block(ui->joystickCombo);
        ui->joystickCombo->clear();

        SDL_JoystickUpdate();
        const int n = SDL_NumJoysticks();
        for (int i = 0; i < n; ++i) {
            const char* nm = SDL_JoystickNameForIndex(i);
            const QString name = nm ? QString::fromUtf8(nm) : tr("Joystick %1").arg(i);

            ui->joystickCombo->addItem(iconConnected(), name, name);
            ui->joystickCombo->setItemData(ui->joystickCombo->count() - 1,
                                           tr("Connected device"), Qt::ToolTipRole);
        }

        for (const auto& jm : _config.joysticks) {
            bool found = false;
            for (int i = 0; i < ui->joystickCombo->count(); ++i) {
                if (ui->joystickCombo->itemData(i).toString() == jm.name) { found = true; break; }
            }
            if (!found) {
                ui->joystickCombo->addItem(iconSaved(), jm.name, jm.name);
                ui->joystickCombo->setItemData(ui->joystickCombo->count() - 1,
                                               tr("Saved profile (device not connected)"), Qt::ToolTipRole);
            }
        }

        int idx = ui->joystickCombo->findData(prev);
        if (idx < 0 && ui->joystickCombo->count() > 0) idx = 0;
        ui->joystickCombo->setCurrentIndex(idx);
        if (idx < 0) ui->joystickCombo->setEditText(QString());
    }

    if (ui->joystickCombo->currentIndex() >= 0) {
        onJoystickComboIndexChanged(ui->joystickCombo->currentIndex());
    } else {
        _currentProfileName.clear();
        ui->mappingList->clear();
        ui->editorStack->setCurrentWidget(ui->emptyPage);
        closePreview();
        updateStatus(tr("No profiles"));
    }
}

void JoystickMappingForm::onJoystickComboIndexChanged(int index) {
    if (_updating || index < 0) return;
    const QString name = ui->joystickCombo->itemData(index).toString();
    if (name.isEmpty()) return;
    loadProfile(name);
}

void JoystickMappingForm::onJoystickComboEditFinished() {
    if (_updating) return;

    const QString newName = ui->joystickCombo->currentText().trimmed();
    if (newName == _currentProfileName) return;

           // empty → revert
    if (newName.isEmpty()) {
        QSignalBlocker b(ui->joystickCombo);
        ui->joystickCombo->setEditText(_currentProfileName);
        return;
    }

           // existing profile with that name?
    int idx = -1;
    for (int i = 0; i < ui->joystickCombo->count(); ++i) {
        if (ui->joystickCombo->itemData(i).toString() == newName) { idx = i; break; }
    }

    if (idx >= 0 && newName != _currentProfileName) {
        // switch to that profile
        {
            QSignalBlocker b(ui->joystickCombo);
            ui->joystickCombo->setCurrentIndex(idx);
        }
        loadProfile(newName);
        return;
    }

           // no current profile → create new
    if (_currentProfileName.isEmpty()) {
        if (_config.find(newName)) {
            // shouldn't happen (handled above)
            QSignalBlocker b(ui->joystickCombo);
            ui->joystickCombo->setEditText(QString());
            return;
        }
        _config.findOrCreate(newName);
        ui->joystickCombo->addItem(iconSaved(), newName, newName);
        {
            QSignalBlocker b(ui->joystickCombo);
            ui->joystickCombo->setCurrentIndex(ui->joystickCombo->count() - 1);
        }
        loadProfile(newName);
        onSave();
        return;
    }

           // rename current → newName
    const QString oldName = _currentProfileName;
    JoystickMapping* jm = _config.findOrCreate(oldName);
    if (!jm) return;
    jm->name = newName;

    for (int i = 0; i < ui->joystickCombo->count(); ++i) {
        if (ui->joystickCombo->itemData(i).toString() == oldName) {
            ui->joystickCombo->setItemText(i, newName);
            ui->joystickCombo->setItemData(i, newName);
            break;
        }
    }

    _currentProfileName = newName;
    onSave();
    loadProfile(newName);
    updateStatus(tr("Renamed '%1' → '%2'").arg(oldName, newName));
}

void JoystickMappingForm::selectProfile(const QString& name) {
    int idx = -1;
    for (int i = 0; i < ui->joystickCombo->count(); ++i) {
        if (ui->joystickCombo->itemData(i).toString() == name) { idx = i; break; }
    }
    if (idx < 0) {
        ui->joystickCombo->addItem(iconSaved(), name, name);
        idx = ui->joystickCombo->count() - 1;
    }
    {
        QSignalBlocker b(ui->joystickCombo);
        ui->joystickCombo->setCurrentIndex(idx);
    }
    loadProfile(name);
}

void JoystickMappingForm::onAddJoystick() {
    bool ok = false;
    const QString name = QInputDialog::getText(
                             this, tr("Add Joystick"),
                             tr("Name of new joystick profile:"),
                             QLineEdit::Normal, QString(), &ok).trimmed();

    if (!ok || name.isEmpty()) return;

    if (_config.find(name)) {
        const auto r = QMessageBox::question(this, tr("Profile exists"),
                                             tr("Profile '%1' already exists.\nSwitch to it?").arg(name),
                                             QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
        if (r == QMessageBox::Yes) selectProfile(name);
        return;
    }

    _config.findOrCreate(name);
    ui->joystickCombo->addItem(iconSaved(), name, name);
    selectProfile(name);
    onSave();
    updateStatus(tr("Created profile '%1'").arg(name));
}

void JoystickMappingForm::onCloneJoystick() {
    if (_currentProfileName.isEmpty()) {
        QMessageBox::information(this, tr("Clone"), tr("No profile selected."));
        return;
    }
    const JoystickMapping* src = _config.find(_currentProfileName);
    if (!src) return;

    bool ok = false;
    const QString name = QInputDialog::getText(
                             this, tr("Clone Joystick"),
                             tr("Name for new profile:"),
                             QLineEdit::Normal,
                             _currentProfileName + QStringLiteral(" (copy)"),
                             &ok).trimmed();

    if (!ok || name.isEmpty()) return;

    JoystickMapping clone = *src;
    clone.name = name;

    if (_config.find(name)) {
        const auto r = QMessageBox::question(this, tr("Profile exists"),
                                             tr("Profile '%1' already exists.\nOverwrite it?").arg(name),
                                             QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (r != QMessageBox::Yes) return;

        for (auto& jm : _config.joysticks) {
            if (jm.name == name) { jm = clone; break; }
        }
    } else {
        _config.joysticks.append(clone);
        ui->joystickCombo->addItem(iconSaved(), name, name);
    }

    selectProfile(name);
    onSave();
    updateStatus(tr("Cloned to '%1'").arg(name));
}

void JoystickMappingForm::onDeleteJoystick() {
    if (_currentProfileName.isEmpty()) return;

    const QString name = _currentProfileName;
    const auto r = QMessageBox::question(this, tr("Delete Profile"),
                                         tr("Delete joystick profile '%1'?").arg(name),
                                         QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (r != QMessageBox::Yes) return;

    for (int i = 0; i < _config.joysticks.size(); ++i) {
        if (_config.joysticks[i].name == name) {
            _config.joysticks.removeAt(i);
            break;
        }
    }
    for (int i = 0; i < ui->joystickCombo->count(); ++i) {
        if (ui->joystickCombo->itemData(i).toString() == name) {
            ui->joystickCombo->removeItem(i);
            break;
        }
    }

    _currentProfileName.clear();
    onSave();

    if (ui->joystickCombo->count() > 0) {
        {
            QSignalBlocker b(ui->joystickCombo);
            ui->joystickCombo->setCurrentIndex(0);
        }
        onJoystickComboIndexChanged(0);
        updateStatus(tr("Deleted '%1'").arg(name));
    } else {
        ui->joystickCombo->setCurrentIndex(-1);
        ui->joystickCombo->setEditText(QString());
        ui->mappingList->clear();
        ui->editorStack->setCurrentWidget(ui->emptyPage);
        closePreview();
        updateStatus(tr("Config is empty"));
    }
}

// ============================================================================
// load profile / current profile
// ============================================================================

void JoystickMappingForm::loadProfile(const QString& name) {
    _currentProfileName = name;
    _config.findOrCreate(name);

           // find SDL index
    _currentJoyIndex = -1;
    SDL_JoystickUpdate();
    const int n = SDL_NumJoysticks();
    for (int i = 0; i < n; ++i) {
        const char* nm = SDL_JoystickNameForIndex(i);
        if (nm && name == QString::fromUtf8(nm)) { _currentJoyIndex = i; break; }
    }

    rebuildMappingList();

    if (_currentJoyIndex >= 0) openPreview();
    else                       closePreview();

    updateStatus(tr("Joystick: %1%2")
                     .arg(name,
                          _currentJoyIndex >= 0 ? QString() : tr(" (not connected)")));
}

JoystickMapping* JoystickMappingForm::currentProfile() {
    if (_currentProfileName.isEmpty()) return nullptr;
    return _config.findOrCreate(_currentProfileName);
}

// ============================================================================
// mapping list
// ============================================================================

void JoystickMappingForm::rebuildMappingList() {
    QSignalBlocker block(ui->mappingList);
    ui->mappingList->clear();
    _curKind = -1; _curIndex = -1;

    JoystickMapping* jm = currentProfile();
    if (!jm) {
        ui->editorStack->setCurrentWidget(ui->emptyPage);
        _previewRebuildNeeded = true;
        return;
    }

    for (int i = 0; i < jm->axes.size(); ++i) {
        auto* item = new QListWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(QPair<int,int>(0, i)));
        ui->mappingList->addItem(item);
        updateItemText(0, i);
    }
    for (int i = 0; i < jm->buttons.size(); ++i) {
        auto* item = new QListWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(QPair<int,int>(1, i)));
        ui->mappingList->addItem(item);
        updateItemText(1, i);
    }

    if (ui->mappingList->count() > 0)
        ui->mappingList->setCurrentRow(0);
    else
        ui->editorStack->setCurrentWidget(ui->emptyPage);

    _previewRebuildNeeded = true;
}

void JoystickMappingForm::updateItemText(int kind, int idx) {
    JoystickMapping* jm = currentProfile();
    if (!jm) return;
    const int row = (kind == 0) ? idx : jm->axes.size() + idx;
    if (row < 0 || row >= ui->mappingList->count()) return;

    QListWidgetItem* item = ui->mappingList->item(row);
    if (kind == 0) {
        const AxisBinding& a = jm->axes[idx];
        item->setText(tr("Ось %1 → %2  [sens %3, мз %4‰%5]")
                          .arg(a.axis)
                          .arg(cmdDisplayName(a.cmd))            // <-- русский
                          .arg(a.sensitivity, 0, 'f', 2)
                          .arg(a.deadZone)
                          .arg(a.inverted ? tr(", инверт.") : QString()));
        item->setToolTip(cmdDescription(a.cmd));
    } else {
        const ButtonBinding& b = jm->buttons[idx];
        const QString src = b.hat >= 0
                                ? tr("Хэт %1 [%2]").arg(b.hat).arg(b.hatMask, 0, 16)
                                : tr("Кнопка %1").arg(b.button);
        const QString seq = b.toggleEnabled
                                ? tr("toggle A:[%1] B:[%2]")
                                      .arg(b.toggleA.toDisplayString(), b.toggleB.toDisplayString())
                                : tr("press:[%1] release:[%2]")
                                      .arg(b.press.toDisplayString(),  b.release.toDisplayString());
        item->setText(tr("%1 → %2").arg(src, seq));
        item->setToolTip(tr("Raw JSON: press=[%1] release=[%2] toggleA=[%3] toggleB=[%4]")
                             .arg(b.press.toString(), b.release.toString(),
                                  b.toggleA.toString(), b.toggleB.toString()));
    }
}

void JoystickMappingForm::onMappingSelectionChanged() {
    _curKind = -1; _curIndex = -1;
    QListWidgetItem* item = ui->mappingList->currentItem();
    if (!item) { ui->editorStack->setCurrentWidget(ui->emptyPage); return; }

    const QPair<int,int> p = item->data(Qt::UserRole).value<QPair<int,int>>();
    _curKind  = p.first;
    _curIndex = p.second;

    if      (_curKind == 0) showAxis(_curIndex);
    else if (_curKind == 1) showButton(_curIndex);
    else                    ui->editorStack->setCurrentWidget(ui->emptyPage);
}

void JoystickMappingForm::showAxis(int idx) {
    JoystickMapping* jm = currentProfile();
    if (!jm || idx < 0 || idx >= jm->axes.size()) return;
    const AxisBinding& a = jm->axes[idx];

    _updating = true;
    const int ci = ui->axisCmdCombo->findData(static_cast<int>(a.cmd));
    ui->axisCmdCombo->setCurrentIndex(ci >= 0 ? ci : 0);
    ui->axisIndexSpin->setValue(a.axis);
    ui->axisSensitivitySpin->setValue(a.sensitivity);
    ui->axisDeadZoneSpin->setValue(a.deadZone);
    ui->axisFilterSpin->setValue(a.smoothing);
    ui->axisInvertedCheck->setChecked(a.inverted);
    _updating = false;

    ui->editorStack->setCurrentWidget(ui->axisPage);
}

void JoystickMappingForm::showButton(int idx) {
    JoystickMapping* jm = currentProfile();
    if (!jm || idx < 0 || idx >= jm->buttons.size()) return;
    const ButtonBinding& b = jm->buttons[idx];

    _updating = true;
    ui->buttonIndexSpin->setValue(b.button);
    ui->hatIndexSpin->setValue(b.hat);
    const int ci = ui->hatMaskCombo->findData(b.hatMask);
    ui->hatMaskCombo->setCurrentIndex(ci >= 0 ? ci : 0);
    ui->buttonPressEdit->setText(b.press.toString());
    ui->buttonReleaseEdit->setText(b.release.toString());
    ui->buttonToggleCheck->setChecked(b.toggleEnabled);
    ui->buttonToggleAEdit->setText(b.toggleA.toString());
    ui->buttonToggleBEdit->setText(b.toggleB.toString());
    _updating = false;

    const bool tog = b.toggleEnabled;
    ui->buttonToggleAEdit->setEnabled(tog);
    ui->buttonToggleAAdd->setEnabled(tog);
    ui->buttonToggleBEdit->setEnabled(tog);
    ui->buttonToggleBAdd->setEnabled(tog);

    ui->editorStack->setCurrentWidget(ui->buttonPage);
}

// ============================================================================
// add / remove mapping
// ============================================================================

void JoystickMappingForm::onAddAxis() {
    JoystickMapping* jm = currentProfile();
    if (!jm) return;
    AxisBinding a; a.cmd = Cmd::Axis_None; a.axis = 0;
    jm->axes.append(a);
    rebuildMappingList();
    ui->mappingList->setCurrentRow(jm->axes.size() - 1);
}

void JoystickMappingForm::onAddButton() {
    JoystickMapping* jm = currentProfile();
    if (!jm) return;
    jm->buttons.append(ButtonBinding{});
    rebuildMappingList();
    ui->mappingList->setCurrentRow(jm->axes.size() + jm->buttons.size() - 1);
}

void JoystickMappingForm::onRemoveMapping() {
    JoystickMapping* jm = currentProfile();
    if (!jm || _curKind < 0) return;
    if (_curKind == 0 && _curIndex >= 0 && _curIndex < jm->axes.size())
        jm->axes.removeAt(_curIndex);
    else if (_curKind == 1 && _curIndex >= 0 && _curIndex < jm->buttons.size())
        jm->buttons.removeAt(_curIndex);
    rebuildMappingList();
}

// ============================================================================
// editor → model
// ============================================================================

void JoystickMappingForm::saveAxisToModel() {
    if (_updating) return;
    JoystickMapping* jm = currentProfile();
    if (!jm || _curKind != 0 || _curIndex < 0 || _curIndex >= jm->axes.size()) return;

    AxisBinding& a = jm->axes[_curIndex];
    a.cmd         = static_cast<Cmd>(ui->axisCmdCombo->currentData().toInt());
    a.axis        = ui->axisIndexSpin->value();
    a.sensitivity = ui->axisSensitivitySpin->value();
    a.deadZone    = ui->axisDeadZoneSpin->value();
    a.smoothing   = ui->axisFilterSpin->value();
    a.inverted    = ui->axisInvertedCheck->isChecked();
    updateItemText(0, _curIndex);
    _previewRebuildNeeded = true;
}

void JoystickMappingForm::saveButtonToModel() {
    if (_updating) return;
    JoystickMapping* jm = currentProfile();
    if (!jm || _curKind != 1 || _curIndex < 0 || _curIndex >= jm->buttons.size()) return;

    ButtonBinding& b = jm->buttons[_curIndex];
    b.button        = ui->buttonIndexSpin->value();
    b.hat           = ui->hatIndexSpin->value();
    b.hatMask       = ui->hatMaskCombo->currentData().toInt();
    b.press         = ActionSequence::fromString(ui->buttonPressEdit->text());
    b.release       = ActionSequence::fromString(ui->buttonReleaseEdit->text());
    b.toggleEnabled = ui->buttonToggleCheck->isChecked();
    b.toggleA       = ActionSequence::fromString(ui->buttonToggleAEdit->text());
    b.toggleB       = ActionSequence::fromString(ui->buttonToggleBEdit->text());

    const bool tog = b.toggleEnabled;
    ui->buttonToggleAEdit->setEnabled(tog);
    ui->buttonToggleAAdd->setEnabled(tog);
    ui->buttonToggleBEdit->setEnabled(tog);
    ui->buttonToggleBAdd->setEnabled(tog);

    updateItemText(1, _curIndex);
    _previewRebuildNeeded = true;
}

// ============================================================================
// learn
// ============================================================================

void JoystickMappingForm::onAxisLearn()   { startLearn(LearnTarget::Axis);   }
void JoystickMappingForm::onButtonLearn() { startLearn(LearnTarget::Button); }
void JoystickMappingForm::onHatLearn()    { startLearn(LearnTarget::Hat);    }

void JoystickMappingForm::startLearn(LearnTarget target) {
    if (_currentJoyIndex < 0) {
        QMessageBox::warning(this, tr("Learn"), tr("Selected joystick is not connected."));
        return;
    }
    if (_curKind < 0) return;

    if (_learnJoy) { SDL_JoystickClose(_learnJoy); _learnJoy = nullptr; }
    _learnJoy = SDL_JoystickOpen(_currentJoyIndex);
    if (!_learnJoy) {
        QMessageBox::warning(this, tr("Learn"), QString::fromUtf8(SDL_GetError()));
        return;
    }

    SDL_JoystickUpdate();
    _prevAxes.clear();
    const int nAxes = SDL_JoystickNumAxes(_learnJoy);
    for (int i = 0; i < nAxes; ++i) _prevAxes.append(SDL_JoystickGetAxis(_learnJoy, i));

    _prevButtons.clear();
    const int nButtons = SDL_JoystickNumButtons(_learnJoy);
    for (int i = 0; i < nButtons; ++i)
        _prevButtons.append(SDL_JoystickGetButton(_learnJoy, i) ? 1 : 0);

    _prevHats.clear();
    const int nHats = SDL_JoystickNumHats(_learnJoy);
    for (int i = 0; i < nHats; ++i) _prevHats.append(SDL_JoystickGetHat(_learnJoy, i));

    _learnTarget     = target;
    _axisLearnActive = false;
    _learnElapsed.invalidate();
    _learnTimer.start();
    updateStatus(tr("Learn: deflect the desired element..."));
}

void JoystickMappingForm::onLearnTick() {
    if (!_learnJoy) { _learnTimer.stop(); return; }
    SDL_JoystickUpdate();

    if (_learnTarget == LearnTarget::Axis) {
        const int n = qMin(_prevAxes.size(), SDL_JoystickNumAxes(_learnJoy));

        int curMaxDelta = 0;
        int curMaxAxis  = -1;
        for (int i = 0; i < n; ++i) {
            const int delta = qAbs(SDL_JoystickGetAxis(_learnJoy, i) - _prevAxes[i]);
            if (delta > curMaxDelta) { curMaxDelta = delta; curMaxAxis = i; }
        }

        if (!_axisLearnActive) {
            if (curMaxDelta > _learnThreshold && curMaxAxis >= 0) {
                _axisLearnActive = true;
                _learnElapsed.start();
                updateStatus(tr("Hold axis %1 (%2 ms)...")
                                 .arg(curMaxAxis).arg(_learnDurationMs));
            } else {
                updateStatus(tr("Deflect the desired axis..."));
            }
            return;
        }

        if (curMaxDelta < _learnThreshold) {
            _axisLearnActive = false;
            updateStatus(tr("Axis released too early — try again"));
            return;
        }

        const qint64 remaining = _learnDurationMs - _learnElapsed.elapsed();
        if (remaining > 0) {
            updateStatus(tr("Hold axis %1... %2 sec left")
                             .arg(curMaxAxis).arg(remaining / 1000.0, 0, 'f', 1));
            return;
        }

        ui->axisIndexSpin->setValue(curMaxAxis);
        finishLearn(tr("Captured axis %1 (delta %2)").arg(curMaxAxis).arg(curMaxDelta));
        return;
    }

    if (_learnTarget == LearnTarget::Button) {
        const int n = qMin(_prevButtons.size(), SDL_JoystickNumButtons(_learnJoy));
        for (int i = 0; i < n; ++i) {
            const int cur = SDL_JoystickGetButton(_learnJoy, i) ? 1 : 0;
            if (cur && !_prevButtons[i]) {
                ui->buttonIndexSpin->setValue(i);
                finishLearn(tr("Captured button %1").arg(i));
                return;
            }
            _prevButtons[i] = cur;
        }
        updateStatus(tr("Press the desired button..."));
        return;
    }

    if (_learnTarget == LearnTarget::Hat) {
        const int n = qMin(_prevHats.size(), SDL_JoystickNumHats(_learnJoy));
        for (int i = 0; i < n; ++i) {
            const int cur = SDL_JoystickGetHat(_learnJoy, i);
            if (cur != 0) {
                ui->hatIndexSpin->setValue(i);
                int mask = 0;
                if      (cur & SDL_HAT_UP)    mask = SDL_HAT_UP;
                else if (cur & SDL_HAT_DOWN)  mask = SDL_HAT_DOWN;
                else if (cur & SDL_HAT_LEFT)  mask = SDL_HAT_LEFT;
                else if (cur & SDL_HAT_RIGHT) mask = SDL_HAT_RIGHT;
                const int ci = ui->hatMaskCombo->findData(mask);
                if (ci >= 0) ui->hatMaskCombo->setCurrentIndex(ci);
                finishLearn(tr("Captured hat %1").arg(i));
                return;
            }
        }
        updateStatus(tr("Tilt the desired hat..."));
        return;
    }
}

void JoystickMappingForm::finishLearn(const QString& msg) {
    _learnTimer.stop();
    if (_learnJoy) { SDL_JoystickClose(_learnJoy); _learnJoy = nullptr; }
    _learnTarget = LearnTarget::None;
    _axisLearnActive = false;
    updateStatus(msg);

    if (_curKind == 0)      saveAxisToModel();
    else if (_curKind == 1) saveButtonToModel();
}

// ============================================================================
// json io
// ============================================================================

void JoystickMappingForm::onReload() {
    QString err;
    if (!_config.load(_path, &err)) {
        QMessageBox::warning(this, tr("Reload"),
                             tr("Failed to read %1:\n%2").arg(_path, err));
    }
    _currentProfileName.clear();
    onRefreshJoysticks();
    updateStatus(tr("Mapping file: %1").arg(_path));
}

void JoystickMappingForm::onSave() {
    // strip empty profiles
    QList<JoystickMapping> keep;
    for (const auto& jm : _config.joysticks)
        if (!jm.axes.isEmpty() || !jm.buttons.isEmpty()) keep.append(jm);
    _config.joysticks = keep;

    QString err;
    if (!_config.save(_path, &err)) {
        QMessageBox::critical(this, tr("Save"),
                              tr("Failed to save %1:\n%2").arg(_path, err));
        return;
    }
    emit mappingChanged(_path);
    updateStatus(tr("Saved: %1").arg(_path));
}

void JoystickMappingForm::onImport() {
    const QString p = QFileDialog::getOpenFileName(
        this, tr("Import Mapping"), _path, tr("JSON (*.json);;All files (*)"));
    if (p.isEmpty()) return;

    MappingConfig m; QString err;
    if (!m.load(p, &err)) {
        QMessageBox::warning(this, tr("Import"), err);
        return;
    }
    _config = m;
    _currentProfileName.clear();
    onRefreshJoysticks();
    updateStatus(tr("Imported from %1").arg(p));
}

void JoystickMappingForm::onMerge() {
    const QString p = QFileDialog::getOpenFileName(
        this, tr("Merge with File"), _path, tr("JSON (*.json);;All files (*)"));
    if (p.isEmpty()) return;

    MappingConfig m; QString err;
    if (!m.load(p, &err)) {
        QMessageBox::warning(this, tr("Merge"), err);
        return;
    }
    _config.mergeWith(m);
    onRefreshJoysticks();
    updateStatus(tr("Merged with %1").arg(p));
}

void JoystickMappingForm::onExport() {
    if (_config.joysticks.isEmpty()) {
        QMessageBox::information(this, tr("Export"), tr("No joysticks to export."));
        return;
    }

           // --- selection dialog ---
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Export Joysticks"));

    auto* outer = new QVBoxLayout(&dlg);
    outer->addWidget(new QLabel(tr("Select joysticks to export:"), &dlg));

    QVector<QCheckBox*> boxes;
    boxes.reserve(_config.joysticks.size());
    for (const auto& jm : _config.joysticks) {
        auto* cb = new QCheckBox(jm.name, &dlg);
        cb->setChecked(true);
        outer->addWidget(cb);
        boxes.append(cb);
    }

    auto* btnRow = new QHBoxLayout;
    auto* selAll  = new QPushButton(tr("Select All"), &dlg);
    auto* selNone = new QPushButton(tr("Select None"), &dlg);
    btnRow->addWidget(selAll);
    btnRow->addWidget(selNone);
    btnRow->addStretch();
    outer->addLayout(btnRow);

    QObject::connect(selAll,  &QPushButton::clicked, [&boxes]{
        for (auto* b : boxes) b->setChecked(true);
    });
    QObject::connect(selNone, &QPushButton::clicked, [&boxes]{
        for (auto* b : boxes) b->setChecked(false);
    });

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    outer->addWidget(bb);
    QObject::connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    MappingConfig subset;
    for (int i = 0; i < boxes.size(); ++i)
        if (boxes[i]->isChecked()) subset.joysticks.append(_config.joysticks[i]);

    if (subset.joysticks.isEmpty()) {
        QMessageBox::information(this, tr("Export"), tr("No joysticks selected."));
        return;
    }

    const QString p = QFileDialog::getSaveFileName(
        this, tr("Export Mapping"), _path, tr("JSON (*.json);;All files (*)"));
    if (p.isEmpty()) return;

    QString err;
    if (!subset.save(p, &err)) {
        QMessageBox::warning(this, tr("Export"), err);
        return;
    }
    updateStatus(tr("Exported %1 profile(s) to %2")
                     .arg(subset.joysticks.size()).arg(p));
}

// ============================================================================
// active preview
// ============================================================================

void JoystickMappingForm::openPreview() {
    if (_previewJoy) return;
    if (_currentJoyIndex < 0) return;

    SDL_JoystickUpdate();
    if (_currentJoyIndex >= SDL_NumJoysticks()) return;

    _previewJoy = SDL_JoystickOpen(_currentJoyIndex);
    if (_previewJoy) {
        _previewAxes    = SDL_JoystickNumAxes(_previewJoy);
        _previewButtons = SDL_JoystickNumButtons(_previewJoy);
        _previewHats    = SDL_JoystickNumHats(_previewJoy);
        _previewRebuildNeeded = true;
    }
}

void JoystickMappingForm::closePreview() {
    if (_previewJoy) {
        SDL_JoystickClose(_previewJoy);
        _previewJoy = nullptr;
    }
    _previewAxes = _previewButtons = _previewHats = 0;
    ui->activeTable->setRowCount(0);
}

void JoystickMappingForm::rebuildActiveTable() {
    _previewRebuildNeeded = true;
    updateActiveTable();
}

void JoystickMappingForm::updateActiveTable() {
    if (!_previewJoy) {
        ui->activeTable->setRowCount(0);
        return;
    }

           // --- Собираем карту "физический элемент → назначенное действие" ---
    QHash<int, QString> axisMap;   // axis index  → "Axis_TrackballX"
    QHash<int, QString> buttonMap; // button index → "Action_Fire / Action_Brake"
    QHash<int, QString> hatMap;    // hat*16+mask  → "Action_ToggleLight"

    if (JoystickMapping* jm = currentProfile()) {
        for (const auto& a : jm->axes) {
            if (a.axis >= 0 && a.cmd != Cmd::Axis_None)
                axisMap.insert(a.axis, cmdDisplayName(a.cmd));   // ← RU
        }
        for (const auto& b : jm->buttons) {
            QString desc;
            if (b.toggleEnabled) {
                desc = tr("A:[%1] B:[%2]")
                .arg(b.toggleA.toDisplayString(), b.toggleB.toDisplayString());
            } else {
                desc = b.press.toDisplayString();
                if (!b.release.isEmpty())
                    desc += QStringLiteral(" / ") + b.release.toDisplayString();
            }
            if (desc.isEmpty()) desc = QStringLiteral("—");

            if (b.hat >= 0 && b.hatMask != 0) {
                hatMap.insert(b.hat * 16 + (b.hatMask & 0xF), desc);
            } else if (b.button >= 0) {
                buttonMap.insert(b.button, desc);
            }
        }
    }

           // Хэты разбиваются на 4 строки (Up/Down/Left/Right) — каждая маска
           // в конфиге отдельный binding, так что и в тесте удобно видеть отдельно.
    const int hatRows  = _previewHats * 4;
    const int wantRows = _previewAxes + _previewButtons + hatRows;

    if (_previewRebuildNeeded || ui->activeTable->rowCount() != wantRows) {
        ui->activeTable->setRowCount(wantRows);
        int row = 0;

        for (int i = 0; i < _previewAxes; ++i, ++row) {
            ui->activeTable->setItem(row, 0, new QTableWidgetItem(tr("Axis %1").arg(i)));
            ui->activeTable->setItem(row, 1, new QTableWidgetItem());
            ui->activeTable->setItem(row, 2, new QTableWidgetItem());
            ui->activeTable->setItem(row, 3, new QTableWidgetItem());
        }
        for (int i = 0; i < _previewButtons; ++i, ++row) {
            ui->activeTable->setItem(row, 0, new QTableWidgetItem(tr("Button %1").arg(i)));
            ui->activeTable->setItem(row, 1, new QTableWidgetItem());
            ui->activeTable->setItem(row, 2, new QTableWidgetItem());
            ui->activeTable->setItem(row, 3, new QTableWidgetItem());
        }
        for (int i = 0; i < _previewHats; ++i) {
            for (int mask : {SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT}) {
                const QString maskName =
                    (mask == SDL_HAT_UP)    ? tr("Up")    :
                        (mask == SDL_HAT_DOWN)  ? tr("Down")  :
                        (mask == SDL_HAT_LEFT)  ? tr("Left")  :
                        tr("Right");
                ui->activeTable->setItem(row, 0,
                                         new QTableWidgetItem(tr("Hat %1 [%2]").arg(i).arg(maskName)));
                ui->activeTable->setItem(row, 1, new QTableWidgetItem());
                ui->activeTable->setItem(row, 2, new QTableWidgetItem());
                ui->activeTable->setItem(row, 3, new QTableWidgetItem());
                ++row;
            }
        }
        _previewRebuildNeeded = false;
    }

    int row = 0;

           // --- Axes ---
    for (int i = 0; i < _previewAxes; ++i, ++row) {
        const int    raw = SDL_JoystickGetAxis(_previewJoy, i);
        const double n   = (raw < 0) ? double(raw) / 32768.0 : double(raw) / 32767.0;
        const bool   active = std::fabs(n) > 0.03;

        ui->activeTable->item(row, 1)->setText(axisMap.value(i, QStringLiteral("—")));
        ui->activeTable->item(row, 2)->setText(QString::number(raw));
        ui->activeTable->item(row, 3)->setText(
            QStringLiteral("%1%2").arg(n, 0, 'f', 2).arg(active ? "  ACTIVE" : ""));

        const QColor bg = active ? kActiveAxisBg : kIdleBg;
        for (int c = 0; c < 4; ++c)
            ui->activeTable->item(row, c)->setBackground(bg);
    }

           // --- Buttons ---
    for (int i = 0; i < _previewButtons; ++i, ++row) {
        const bool pressed = SDL_JoystickGetButton(_previewJoy, i) != 0;

        ui->activeTable->item(row, 1)->setText(buttonMap.value(i, QStringLiteral("—")));
        ui->activeTable->item(row, 2)->setText(pressed ? QStringLiteral("1") : QStringLiteral("0"));
        ui->activeTable->item(row, 3)->setText(pressed ? tr("PRESSED") : tr("idle"));

        const QColor bg = pressed ? kPressedBtnBg : kIdleBg;
        for (int c = 0; c < 4; ++c)
            ui->activeTable->item(row, c)->setBackground(bg);
    }

           // --- Hats ---
    for (int i = 0; i < _previewHats; ++i) {
        const int h = SDL_JoystickGetHat(_previewJoy, i);
        for (int mask : {SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT}) {
            const bool pressed = (h & mask) == mask;
            const int  key     = i * 16 + (mask & 0xF);

            ui->activeTable->item(row, 1)->setText(hatMap.value(key, QStringLiteral("—")));
            ui->activeTable->item(row, 2)->setText(pressed ? QStringLiteral("1") : QStringLiteral("0"));
            ui->activeTable->item(row, 3)->setText(pressed ? tr("PRESSED") : tr("idle"));

            const QColor bg = pressed ? kPressedBtnBg : kIdleBg;
            for (int c = 0; c < 4; ++c)
                ui->activeTable->item(row, c)->setBackground(bg);
            ++row;
        }
    }
}

void JoystickMappingForm::onPreviewTick() {
    SDL_JoystickUpdate();

           // Discovery / hot-plug: try to (re)open a preview for current profile
    if (!_previewJoy && !_currentProfileName.isEmpty()) {
        const int n = SDL_NumJoysticks();
        for (int i = 0; i < n; ++i) {
            const char* nm = SDL_JoystickNameForIndex(i);
            if (nm && _currentProfileName == QString::fromUtf8(nm)) {
                _currentJoyIndex = i;
                _previewJoy = SDL_JoystickOpen(i);
                if (_previewJoy) _previewRebuildNeeded = true;
                break;
            }
        }
    }

    if (!_previewJoy) return;

    if (!SDL_JoystickGetAttached(_previewJoy)) {
        closePreview();
        return;
    }

    updateActiveTable();
}

// ============================================================================
// misc
// ============================================================================

void JoystickMappingForm::updateStatus(const QString& text) {
    ui->statusLabel->setText(text);
}

void JoystickMappingForm::closeEvent(QCloseEvent* e) {
    _learnTimer.stop();
    if (_learnJoy) { SDL_JoystickClose(_learnJoy); _learnJoy = nullptr; }

    onSave();

    QDialog::closeEvent(e);
}

} // namespace Vlx::Input
