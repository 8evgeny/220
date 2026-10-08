#ifndef CV_COMBO_BOX_HPP
#define CV_COMBO_BOX_HPP

#include "cv_widget.hpp"

class MainWindowConstructor;



namespace cvw
{

class CVComboBox : public CVWidget
{
public:
    CVComboBox(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_); // END CVWidget(CVMainWindow * main_window)
    CVComboBox(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_, std::function<void(MainWindowConstructor *, int &, int &)> func, MainWindowConstructor * mw_); // END CVWidget(CVMainWindow * main_window)
    CVComboBox();
    std::string get_str(){return combobox_res_str;};
    int get_pos(){return pos;};
    void set_vec(std::vector<std::string> & v_str){v_combobox_str = v_str;}
    void set_res(std::string & str_) {combobox_res_str = str_;}
    void add_slot(std::function<void((MainWindowConstructor *, int&, int&))> func, MainWindowConstructor * mw_) {slot = func; mw_default = mw_;} // отклик на нажатие кнопки
    ~CVComboBox();
protected:
    std::string name = "CVComboBox";
    int mouse_status = CVWidgetMouseEvent::NOTHING;
    void mouse_handler(int & event, int x, int y);
    void show(cv::Mat & img); // END show
    std::function<void(MainWindowConstructor *, int &, int &)> slot; // defaultSlot;
    MainWindowConstructor * mw_default = nullptr;
private:

    static cv::Point p_mouse;
    static bool f_click_up; // флаг клика
    static bool f_click_down; // флаг клика
    static bool f_move; // флаг движения
    static int mouse_event;
    bool f_win_create = 0;
    cv::Mat frame_combobox;
    cv::Scalar clr_now = clr_base;
    std::vector<std::string> v_combobox_str = {"hello", "world", "how are you"};
    std::string combobox_res_str;
    int pos = 0;

    static void aimFrameCallback(int event, int x, int y, int flags, void* param);
    bool mouseHandler();
    void show_text(std::string & text, cv::Mat & img);
    void show_rct(std::string & text, cv::Rect & rct_);
}; // END CVComboBox

} // END namespace cvw
#endif // END CV_COMBO_BOX_HPP
