#ifndef IFRAMEHANDLER_H
#define IFRAMEHANDLER_H

#include <functional>
#include <cstdint>

class IFrameHandler
{
public:
    IFrameHandler();
    virtual ~IFrameHandler() {}
    void register_frame_handler(
            const std::function<void(uint8_t *, int, int, int, int)> &handler);
    virtual void handle(uint8_t * f, int w, int h, int num, int id);

private:
    std::function<void(uint8_t *, int, int, int, int)> handler;
};

#endif // IFRAMEHANDLER_H
