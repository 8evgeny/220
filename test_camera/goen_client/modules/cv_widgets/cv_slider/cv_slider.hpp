#ifndef CV_SLIDER_HPP
#define CV_SLIDER_HPP

#include "cv_widget.hpp"
#include <functional>

class MainWindowConstructor;

namespace cvw
{

class CVSlider : public CVWidget
{
public:
    CVSlider() {std::cout << "Default constructor CVSlider" << std::endl;}
    CVSlider(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_);
    ~CVSlider() {std::cout << "Destructor CVSlider" << std::endl;}
    int getPos() {return pos;}
    int getMax() {return pos_max;}
    int getMin() {return pos_min;}
    void setPos(int pos_);
    void add_slot(std::function<void(MainWindowConstructor *)> func, MainWindowConstructor * mw_); // отклик на нажатие кнопки
    void setInterval(float p1, float p2, float zero = 0); // END setInterval
    void setStep(float step_) {step = step_;};
protected:
    std::string name = "CVSlider";
    std::function<void(MainWindowConstructor *)> slot ; // defaultSlot;
    MainWindowConstructor * mw_default = nullptr;
    // -1 - за пределами, 0 - мышь наведена, 1 - мышь нажата, 2 - мышь отпущена, 3 - двойной клик.
    void mouse_handler(int &event, int x, int y);
    void show(cv::Mat & img);

    float step = 10;
    float pos_min = 0;
    float pos_max = 100;
    float pos_zero = 0;
    int pos = 150;
    cv::Rect rct_pos;
    bool f_mouse_move = false;
    cv::Point pt_lpanel ;
    cv::Point pt_rpanel ;
    cv::Point pt_zero ;
}; // -- END class CVSlider


} // -- END cvw

#endif // CV_SLIDER_HPP
