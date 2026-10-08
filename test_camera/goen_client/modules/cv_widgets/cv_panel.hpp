#ifndef CV_PANEL_HPP
#define CV_PANEL_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "iostream"

namespace cvw // CV Widgets
{

cv::Rect2f rct_pix2f(cv::Rect rct_pix, cv::Size frame_size);
cv::Rect rct_f2pix(cv::Rect2f rct_pix, cv::Size frame_size);

struct Panel
{
public:
    Panel(cv::Rect2f main_rct_, uint cols_, uint rows_, uint bord_,  cv::Size sz_); // -- END Panel

    void show(cv::Mat & image); // END show(cv::Mat & image)

    cv::Rect get_pos_pix(uint x, uint y, cv::Size sz_); // END Rect getPos(uint x, uint y)
    cv::Rect2f get_pos_f(uint x, uint y);

    cv::Scalar show_clr = cv::Scalar(100,100,100);
    cv::Rect2f main_rct = {0,0,1.f,1.f};
    uint cols = 1;
    uint rows = 1;
    uint bord = 4;
    float bord_x_w ;
    float bord_y_h ;
    std::vector<cv::Rect2f> vrct;
}; // -- END Panel

static int stop_other = 0; // Если !0, запрещает использовать виджеты, id которых != stop_other до завершения обработки



} // END namespace cvw
#endif // CVWIDGET_HPP
