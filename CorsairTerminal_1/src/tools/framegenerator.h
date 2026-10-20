#ifndef FRAMEGENERATOR_H
#define FRAMEGENERATOR_H

#include <QObject>
#include <QTimer>
#include <QMutex>

class FrameGenerator : public QObject
{
    Q_OBJECT
public:
    static FrameGenerator & inst();

    void get_frame();

private:
    explicit FrameGenerator();
    ~FrameGenerator();

private slots:

signals:

public slots:
    void tmr_timedout();
    void set_frame_size(const int & w, const int & h){next_w = w;next_h = h;}
    void set_w(const int & w){next_w = w;}
    void set_h(const int & h){next_h = h;}
    void set_move(const int & move){this->move = move;}
    int get_w(){return w;}
    int get_h(){return h;}
    int get_move(){return move;}
    uint8_t * get_frame(int & w, int & h);

private:
    uint8_t * f;
    int w;
    int h;
    bool frame_ready;
    uint8_t * rdy_f;
    int rdy_w;
    int rdy_h;
    int next_w;
    int next_h;
    int move;
    QMutex m;
};

#endif // FRAMEGENERATOR_H
