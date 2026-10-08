#include "cv_label.hpp"

namespace cvw
{

using namespace std;
using namespace cv;


CVLabel::CVLabel()
{

} // -- END CVLabel


CVLabel::CVLabel(std::string text_, CVMainWindow * main_window, cv::Rect2f rct_)
{
    text = text_;
    std::cout << "Create CVButton in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    frame = Mat(Size(120,40), CV_8UC3, clr_base);
    main_window->add_mouse_rect(rct);
    func_show = std::bind(&CVLabel::show, this,  std::placeholders::_1);
    main_window->add_child_show(func_show);
    func_mouse_handler = std::bind(&CVLabel::mouse_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);
    f_change = true;
    clr_base += cv::Scalar(15,15,15);
} // -- END CVButton


CVLabel::~CVLabel()
{
    std::cout << "Destructor CVLabel " << name << std::endl;
} // -- END ~CVLabel

void CVLabel::show(cv::Mat &img)
{
    if(f_change)
    {
        show_text(text, img);
        f_change = false;
    } // END if(f_change_text)
} // -- END show


void CVLabel::setText(std::string &text_, cv::Mat &img) {text = text_; f_change = true; show(img); }

} // -- END namespace cvw
