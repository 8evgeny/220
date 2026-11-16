#ifndef WAITINGGIFMOVIESINGLETON_H
#define WAITINGGIFMOVIESINGLETON_H

#include "qmv/qmv_global.h"
#include <QMovie>

class QMVSHARED_EXPORT WaitingGifMovieSingleton : public QMovie
{
public:
    static QMovie * instance();
    ~WaitingGifMovieSingleton();
private:
    WaitingGifMovieSingleton();
};

#endif // WAITINGGIFMOVIESINGLETON_H
