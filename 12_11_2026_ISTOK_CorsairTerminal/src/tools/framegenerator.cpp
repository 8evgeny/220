#include "framegenerator.h"

FrameGenerator::FrameGenerator() :
    QObject(nullptr),
    f(nullptr),
    w(0),
    h(0),
    frame_ready(false),
    rdy_f(nullptr),
    rdy_w(0),
    rdy_h(0),
    next_w(1920),
    next_h(1080),
    move(2)
{
}
FrameGenerator::~FrameGenerator()
{
    delete [] f;
    delete [] rdy_f;
}
FrameGenerator & FrameGenerator::inst()
{
    static FrameGenerator instance;
    return instance;
}
uint8_t * FrameGenerator::get_frame(int & w, int & h)
{
    m.lock();
    if(!frame_ready)
    {
        m.unlock();
        return nullptr;
    }
    frame_ready = false;
    w = rdy_w;
    h = rdy_h;
    m.unlock();
    return rdy_f;
}
void FrameGenerator::tmr_timedout()
{
    if(next_w != w || next_h != h || !f)
    {
        w = next_w;
        h = next_h;
        delete [] f;
        f = new uint8_t[w * h];
        for(int y = 0; y < h; ++y)
        {
            for(int x = 0; x < w; ++x)
            {
                f[x + y * w] = uint8_t(x + y);
            }
        }
    }
    if(f)
    {
        int n = w * h;
        for(int i = 0; i < n; ++i)
        {
            f[i] += uint8_t(move);
        }
        if(m.tryLock(1))
        {
            if(rdy_w != w || rdy_h != h || !rdy_f)
            {
                rdy_w = w;
                rdy_h = h;
                delete [] rdy_f;
                rdy_f = new uint8_t[rdy_w * rdy_h];
            }
            if(rdy_f)
            {
                memcpy(rdy_f, f, sizeof(uint8_t) * size_t(rdy_w * rdy_h));
            }
            frame_ready = true;
            m.unlock();
        }
    }
}
