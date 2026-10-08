#ifndef I_DIRECT_FRAME_HANDLER_HPP
#define I_DIRECT_FRAME_HANDLER_HPP

#include <functional>
#include <opencv2/core/mat.hpp>

class IDirectFrameHandler
{
    public:
        IDirectFrameHandler();
        virtual ~IDirectFrameHandler() {};
        void register_frame_handler(
            const std::function<void(cv::Mat &frame)> &handler);
        virtual void handle(cv::Mat &frame);
    private:
        std::function<void(cv::Mat &frame)> handler;
}; // class IDirectFrameHandler

#endif //I_DIRECT_FRAME_HANDLER_HPP
