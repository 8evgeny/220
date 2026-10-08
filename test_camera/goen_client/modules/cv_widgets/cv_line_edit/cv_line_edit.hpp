#ifndef CV_EDIT_HPP
#define CV_EDIT_HPP

#include "cv_widget.hpp"
#include <thread>
#include <iosfwd>
#include <iomanip>

class MainWindowConstructor;


namespace cvw
{



class CVLineEdit : protected CVWidget
{

public:

    CVLineEdit(std::string text_, CVMainWindow * main_window, cv::Rect2f rct_/*, char & key_ */); // END CVWidget(CVMainWindow * main_window)
    CVLineEdit();
    ~CVLineEdit();
    int getNumInt() {return num_i;}
    float getNumFloat() {return num_f;}
    std::string getText() {return text;}
    void setText(std::string & text_, cv::Mat & img);
    void setNum(float num_) {num_i = round(num_); num_f = round(num_);}
    void setType(int type_);
    void setInterval(int p1, int p2); // END setInterval
    void setPrecision(int p) {precision = abs(p);}
    void add_slot(std::function<void(MainWindowConstructor *, std::string &)> func, MainWindowConstructor * mw_) {slot = func; mw_default = mw_;} // отклик на нажатие кнопки
protected:

    std::string name = "CVLineEdit";
    std::string text = " ";
    float num_f = 0;
    int num_i = 0;
    int precision = 2;
    float num_min = 0;
    float num_max = 100;

    std::string num_str = "";
    float num_result = 0;

    int type = EDIT_TEXT;
    cv::Scalar clr_base = cv::Scalar(255,250,255);
    void mouse_handler(int & event, int x, int y);
    void show(cv::Mat & img); // END show
    void show_text(std::string &text, cv::Mat & img);
    std::function<void(MainWindowConstructor *, std::string &)> slot; // defaultSlot;

private:
    MainWindowConstructor * mw_default = nullptr;
    CVMainWindow * mw_parent = nullptr;

    void editText();
    void editFloatNum();
    void editIntNum();
}; // END CVLineEdit

}; // END namespace cvw

#endif // CV_EDIT_HPP
