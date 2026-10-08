#include "iframehandler.h"

IFrameHandler::IFrameHandler()
{

}

void IFrameHandler::register_frame_handler(
        const std::function<void(uint8_t *, int, int, int, int)> &handler)
{
    this->handler = handler;
}

void IFrameHandler::handle(uint8_t * f, int w, int h, int num, int id)
{
    handler(f, w, h, num, id);
}
