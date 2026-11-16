#include "waitinggifmoviesingleton.h"

WaitingGifMovieSingleton::WaitingGifMovieSingleton()
{
    this->setFileName(":/img/waiting_gif.gif");
    this->start();
}
QMovie * WaitingGifMovieSingleton::instance()
{
    static QMovie * m = new WaitingGifMovieSingleton();
    return m;
}
WaitingGifMovieSingleton::~WaitingGifMovieSingleton()
{
    this->stop();
}
