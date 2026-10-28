#ifndef FUSIONCTRLVIEW_H
#define FUSIONCTRLVIEW_H

#include <qmv/qmvview.h>

namespace Ui {
class FusionCtrlView;
}

class FusionCtrlView : public QMVView
{
    Q_OBJECT

public:
    explicit FusionCtrlView(QWidget *parent = nullptr);
    ~FusionCtrlView();

private:
    void connect_model();

private:
    Ui::FusionCtrlView *ui;
};

#endif // FUSIONCTRLVIEW_H
