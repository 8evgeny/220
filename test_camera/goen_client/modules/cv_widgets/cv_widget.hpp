#ifndef CVWIDGET_HPP
#define CVWIDGET_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "iostream"
#include "cv_main_window.hpp"


namespace cvw // CV Widgets
{

class CVWidget : public CVMainWindow
{
public:
    CVWidget(){std::cout << "Default constructor " << name << std::endl;};
    CVWidget(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_); // END CVWidget(CVMainWindow * main_window)
    ~CVWidget();
    std::string getName() {return name;}
    void setName(std::string new_name) {name = new_name; f_change = true;}
    CVMainWindow * parent;
    cv::Rect rct = {0,0,100,20};
    void set_clr_base(cv::Scalar clr_, cv::Scalar clr_mouse_in_);
    void setImage(cv::Mat & image)
    {
        img2show = image.clone();
        f_change = true;
    }
    void setShowType(int type_); // 0 - text, 1 - icon
protected:
    std::string name = "CVWidget";
    cv::Mat frame;
    std::atomic_bool choose_line = 0; // -1 - за пределами, 0 - мышь наведена, 1 - мышь нажата, 2 - мышь отпущена, 3 - двойной клик.
    bool f_change = true;

    std::function<void(int &, int, int)> func_mouse_handler;
    std::function<void(cv::Mat&)> func_show;
    void mouse_handler(int & event, int x, int y);
    void show(cv::Mat & img); // END show
    void show_text(std::string & text, cv::Mat & img); // END show_text
    void show_image(cv::Mat & img);;
    cv::Mat img2show;
    int font_face = 3 ; // cv::FONT_HERSHEY_SIMPLEX; // 3 - русский язык
    double font_face_kf = 0.530864 ;
    float font_scale = 1.0 ;
    cv::Size text_size = {10,10};
    cv::Scalar text_clr = cv::Scalar(0,0,0);
    cv::Scalar clr_base = cv::Scalar(220,220,220);
    cv::Scalar clr_mouse_in = cv::Scalar(250,250,250);
    cv::Scalar clr_mouse_down = cv::Scalar(140,140,140);
    int show_type = 0; // 0 - text, 1 - icon, 2 - image
}; // END class CVWidget

} // END namespace cvw
#endif // CVWIDGET_HPP
