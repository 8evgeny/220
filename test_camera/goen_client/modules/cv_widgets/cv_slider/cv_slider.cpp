#include "cv_slider.hpp"

using namespace std;
using namespace cv;

namespace cvw
{

void default_slider_slot(MainWindowConstructor * mw) {cout << "This CVButton have no slot" << endl;}


CVSlider::CVSlider(std::string name_, CVMainWindow *main_window, cv::Rect2f rct_)
{
    name = name_;
    std::cout << "Create " << name << " in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    main_window->add_mouse_rect(rct);

    func_mouse_handler = std::bind(&CVSlider::mouse_handler, this,  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);

    func_show = std::bind(&CVSlider::show, this, std::placeholders::_1);
    main_window->add_child_show(func_show);

    frame = cv::Mat(rct.size(), CV_8UC3, clr_base);
    add_slot(default_slider_slot, this->mw_default);
} // -- END CVSlider


void CVSlider::setPos(int pos_)
{
    if(pos_ > pos_max) {pos = pos_max;}
    else if(pos_ < pos_min) {pos = pos_min;}
    else {pos = pos_;}
} // -- END setPos

void CVSlider::add_slot(std::function<void (MainWindowConstructor *)> func, MainWindowConstructor *mw_)
{
    slot = func;
    mw_default = mw_;
}// -- END add_slot

void CVSlider::setInterval(float p1, float p2, float p0)
{
    if(p1 == p2)
    {
        std::cout << "CVSlider::" << name << ": interval is single numer " << p1 << std::endl;
        pos_min = p1;
        pos_max = p1;
        pos_zero = p0;
    } // END if(p1 == p2)
    else
    {
        pos_min = MIN(p1, p2);
        pos_max = MAX(p1, p2);
        pos_zero = p0;
        std::cout << "CVSlider::" << name << ": new interval is " << cv::Point(pos_min, pos_max) << std::endl;
    } // END if(!p1 == p2)

} // -- END setInterval

void CVSlider::mouse_handler(int &event, int x, int y)
{
    // -1 - за пределами, 0 - мышь наведена, 1 - мышь нажата, 2 - мышь отпущена, 3 - двойной клик.
    if(f_mouse_move)
    {
        float x_rel = (x - rct.x - pt_lpanel.x) / float(pt_rpanel.x - pt_lpanel.x - 1);
        pos = round((x_rel - (pos_zero - pos_min)/(pos_max - pos_min)) * float( (pos_max - pos_min)));
        if(pos < pos_min) {pos = pos_min;}
        if(pos > pos_max) {pos = pos_max;}
        slot(mw_default);
    }
    if((event == CVWidgetMouseEvent::LBTN_UP || event == CVWidgetMouseEvent::NOTHING) && f_mouse_move)
    {
        f_mouse_move = false;
    } // -- END if((event == CVWidgetMouseEvent::LBTN_UP || CVWidgetMouseEvent::NOTHING) && f_mouse_move)
    if(event == CVWidgetMouseEvent::LBTN_DOWN && Point(x - rct.x,y - rct.y).inside(rct_pos))
    {
        f_mouse_move = true;
    } // -- END if(event == CVWidgetMouseEvent::LBTN_DOWN && Point(x - rct.x,y - rct.y).inside(rct_pos))
} // -- END mouse_handler


void CVSlider::show(cv::Mat &img)
{
//    cout << "CALL " << name << " show!" << endl;
    frame = clr_base;
    int slider_thik = 2;
    int slider_thik1 = slider_thik + 1;
    int slider_w = 24;
    int slider_w_2 = 0.5 * slider_w;
    pt_lpanel = Point(slider_w_2, rct.height * 0.5);
    pt_rpanel = Point(rct.width - slider_w_2, rct.height * 0.5);
    pt_zero = Point(slider_w_2 + (rct.width - slider_w) * (pos_zero - pos_min) / (pos_max - pos_min), rct.height * 0.5);
    float pos_rel = (pos - pos_min) / (pos_max - pos_min);
    int pos_dist_pix = pt_rpanel.x - pt_rpanel.y;

    rct_pos = {pt_lpanel.x + (int)round(pos_rel * pos_dist_pix) - slider_w / 2, pt_lpanel.y - slider_w  / 2, slider_w, slider_w};

    int step_pix = round((step / (pos_max - pos_min)) * (pt_rpanel.x - pt_lpanel.y));
    int num_steps = (pos_max - pos_zero) / step;
    for(int i = 0; i <= num_steps; i++)
    {
        rectangle(frame, pt_zero + Point(i * step_pix - 1, -slider_w_2 * 0.5), pt_zero + Point(i * step_pix + 1, slider_w_2 * 0.5), clr_mouse_down, -1);
    } // -- END for(int i = 0; i <= num_steps; i++)

    num_steps = (pos_zero - pos_min) / step;
    for(int i = 0; i <= num_steps; i++)
    {
        rectangle(frame, pt_zero - Point(i * step_pix - 1, -slider_w_2 * 0.5), pt_zero - Point(i * step_pix + 1, slider_w_2 * 0.5), clr_mouse_down, -1);
    } // -- END for(int i = 0; i <= num_steps; i++)

    rectangle(frame, pt_zero - Point(slider_thik, slider_w_2), pt_zero + Point(2, slider_w_2), Scalar(50,50,255), -1);
    rectangle(frame, pt_lpanel - Point(slider_thik1, slider_thik1) , pt_rpanel + Point(slider_thik1, slider_thik1), clr_mouse_down, -1);
    rectangle(frame, pt_lpanel - Point(slider_thik, slider_thik), pt_rpanel+ Point(slider_thik, slider_thik), clr_mouse_in, -1);
    rectangle(frame, pt_zero  - Point(0, slider_thik), Point(pt_lpanel.x + (int)round(pos_rel * pos_dist_pix), pt_lpanel.y)  + Point(slider_thik, slider_thik), Scalar(255,200,200), -1);
    rectangle(frame, rct_pos, clr_mouse_down, -1);
    rectangle(frame, rct_pos - Size(slider_thik,slider_thik) + Point(1,1), clr_mouse_in, -1);
    frame.copyTo( img(rct)); // = frame.clone();
    return;
} // -- END show




}; // -- END namespace cvw
