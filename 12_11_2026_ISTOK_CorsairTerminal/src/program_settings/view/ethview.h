#ifndef ETHVIEW_H
#define ETHVIEW_H

#include <qmv/qmvview.h>

namespace Ui {
class EthView;
}

class EthView : public QMVView
{
    Q_OBJECT

public:
    explicit EthView(QWidget * parent = nullptr);
    ~EthView();

private:
    void connect_model();

private:
    Ui::EthView * ui;
};

#endif // ETHVIEW_H
