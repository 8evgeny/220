#ifndef TELEMETRYVIEW_H
#define TELEMETRYVIEW_H

#include <qmv/qmvview.h>
#include "dev_types.h"

namespace Ui {
class TelemetryView;
}

class TelemetryView : public QMVView
{
    Q_OBJECT

public:
    explicit TelemetryView(QWidget * parent = nullptr);
    ~TelemetryView();

private:
    void connect_model();

private:
    Ui::TelemetryView * ui;
};

#endif // TELEMETRYVIEW_H
