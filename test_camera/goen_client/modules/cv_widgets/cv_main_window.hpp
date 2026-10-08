#ifndef CVMAIN_WINDOW_HPP
#define CVMAIN_WINDOW_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "iostream"
#include "cv_panel.hpp"
#include <atomic>
#include <atomic>
#include <dirent.h>
#include "INIReader.h"

class MainWindowConstructor;

static int stop_other = 0; // Если !0, запрещает использовать виджеты, id которых != stop_other до завершения обработки
namespace cvw // CV Widgets
{

enum CVLineEditType
{
    EDIT_TEXT,
    EDIT_INTEGER,
    EDIT_FLOAT
};

enum CVWidgetMouseEvent
{
    NOTHING = -1,
    MOVE = 0,
    LBTN_DOWN = 1,
    LBTN_UP = 2,
    LBTN_DBL = 3,
};

class CVMainWindow
{
public:
    CVMainWindow();

    CVMainWindow(const std::string & config, bool & ok, std::string &winname_, cv::Mat & image_); // END CVMainWindow

    void show(); // END show
    void quit() {f_exec = false;}
    void exec(bool topmost = false); // END exec
    void add_mouse_handler(std::function<void(int &, int, int)> & func);
    void add_child_show(std::function<void(cv::Mat&)> & func);
    void add_key_handler(std::function<void(MainWindowConstructor*, unsigned char)>  func);;
    void setWindowConstructor(MainWindowConstructor * mwc_) {mwc = mwc_;}
    void add_mouse_rect(cv::Rect rct_);
    cv::Size getSize() {return frame.size();}
    bool get_record_status(){return video_on && f_write_frames;}
    ~CVMainWindow();
protected:
    cv::Mat frame ;
    cv::Mat frame4save;
    cv::Size frame_size_2;
    static std::atomic<unsigned char> key;
private:
    std::string config_path = "../config.ini";
    MainWindowConstructor * mwc = nullptr;
    std::string winname = "MainWindow";
    std::vector<cv::Rect> v_child_rct;
    std::vector<std::function<void(int&, int, int)>> v_child_mouse_handlers;
    std::vector<std::function<void(cv::Mat&)>> v_child_show;
    std::vector<std::function<void(MainWindowConstructor*, unsigned char)>> v_key_handlers;
    bool f_block = false; // флаг может подниматься из классов наследников, чтобы остановить отрисовку и возможность взаимодействия с клиентом (например, для окна подтверждения)
    bool f_exec = false;
    bool f_write_frames = false;
    std::chrono::system_clock::time_point time_press_button;

    int video_on = 0;
    int video_num = 1;
    std::string path4frames = "";
    int save_frames_skip = 0;
    int save_frames_prefix = 10000000;
    int max_frames = 0;
    std::vector<std::string> fileList;

    // Реализация отслеживания событий мыши
    static cv::Point p_mouse;
    static bool f_click_up; // флаг клика
    static bool f_click_down; // флаг клика
    static bool f_move; // флаг движения
    static int mouse_event;
    int win_id = -1; // id активного виджета - того, которому передаём событие мыши
    int win_id_prev = -1; // id предыдущего активного виджета
    bool f_win_id_changed = false;
    int default_event = -1;
    static void aimFrameCallback(int event, int x, int y, int flags, void* param);
    bool mouseHandler();
    bool DirContent(std::string& path, std::vector<std::string>& fileList);
    bool get_ini_params(const std::string & config);
    void key_handler();
    void save_frame();
}; // END class CVMainWindow

} // END namespace cvw
#endif // CVWIDGET_HPP
