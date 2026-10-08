#ifndef CV_BUTTON_HPP
#define CV_BUTTON_HPP

#include "cv_widget.hpp"

class MainWindowConstructor;


namespace cvw
{

class CVButton : public CVWidget
{
public:
    CVButton(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_);
    CVButton(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_, std::function<void(MainWindowConstructor *)> func, MainWindowConstructor * mw_);
    CVButton();
    void add_slot(std::function<void(MainWindowConstructor *)> func, MainWindowConstructor * mw_); // отклик на нажатие кнопки
    void setName(std::string new_name);
    void setIcon(cv::Mat & icon_);
    void setColorIcon(cv::Scalar clr_);
    void setShowType(int type_); // 0 - text, 1 - icon
    void setBordThic(int bord_) {bord = bord_; f_change = true;}
    ~CVButton();
protected:
    std::string name = "CVButton";
    int mouse_status = CVWidgetMouseEvent::NOTHING;
    void mouse_handler(int & event, int x, int y);
    void show(cv::Mat & img); // END show
    void show_icon(cv::Mat & img);
    std::function<void(MainWindowConstructor *)> slot; // defaultSlot;
    MainWindowConstructor * mw_default = nullptr;
    cv::Scalar clr_active = clr_base;
private:
    int bord = 2;
    cv::Rect rct_bord = cv::Rect(rct.x + bord, rct.y + bord, rct.width - 2 * bord, rct.height - 2 * bord);
    cv::Mat icon = cv::Mat(cv::Size(20,20), CV_8UC1, cv::Scalar(255));
    cv::Mat img_buf;
    cv::Scalar clr_icon = cv::Scalar(0,0,0);

    void show_text(std::string &text, cv::Mat & img);
}; // END CVButton

}; // END namespace cvw

#endif // CV_BUTTON_HPP
