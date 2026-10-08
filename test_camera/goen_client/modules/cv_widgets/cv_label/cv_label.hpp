#ifndef CV_LABEL_HPP
#define CV_LABEL_HPP

#include "cv_widget.hpp"



namespace cvw
{

class CVLabel : public CVWidget
{
public:
    CVLabel();
    CVLabel(std::string text_, CVMainWindow * main_window, cv::Rect2f rct_); // END CVWidget(CVMainWindow * main_window)
    void setText(std::string & text_, cv::Mat & img); // отклик на нажатие кнопки
    ~CVLabel();
protected:
    std::string name = "CVLabel";
    std::string text = " ";
    void show(cv::Mat & img);; // END show
}; // END CVLabel

}; // END namespace cvw

#endif // CV_LABEL_HPP
