#include "i_direct_frame_handler.hpp"

IDirectFrameHandler::IDirectFrameHandler()
{

} // -- END IDirectFrameHandler

void IDirectFrameHandler::register_frame_handler(
    const std::function<void(cv::Mat &frame)> &handler)
{
    this->handler = handler;
} // -- END register_frame_handler

void IDirectFrameHandler::handle(cv::Mat &frame)
{
    handler(frame);
} // -- END handler