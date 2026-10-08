#include "cv_button.hpp"


using namespace std;
using namespace cv;

namespace cvw
{

void default_btn_slot(MainWindowConstructor * mw) {cout << "This CVButton have no slot" << endl;}


CVButton::CVButton()
{
} // -- END CVButton


CVButton::CVButton(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_)
{
    name = name_;
    std::cout << "Create " << name << " in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    bord = 2;
    rct_bord = cv::Rect(rct.x + bord, rct.y + bord, rct.width - 2 * bord, rct.height - 2 * bord);
    frame = Mat(Size(120,40), CV_8UC3, clr_base);
    main_window->add_mouse_rect(rct_);

    func_mouse_handler = std::bind(&CVButton::mouse_handler, this,  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);

    func_show = std::bind(&CVButton::show, this,  std::placeholders::_1);
    main_window->add_child_show(func_show);
    add_slot(default_btn_slot, this->mw_default);
} // -- END CVButton

CVButton::CVButton(std::string name_, CVMainWindow *main_window, cv::Rect2f rct_, std::function<void (MainWindowConstructor *)> func, MainWindowConstructor *mw_)
{
    name = name_;
    std::cout << "Create " << name << " in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    rct_bord = cv::Rect(rct.x + bord, rct.y + bord, rct.width - 2 * bord, rct.height - 2 * bord);
    frame = Mat(Size(120,40), CV_8UC3, clr_base);
    main_window->add_mouse_rect(rct);

    func_mouse_handler = std::bind(&CVButton::mouse_handler, this,  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);

    func_show = std::bind(&CVButton::show, this,  std::placeholders::_1);
    main_window->add_child_show(func_show);
    mw_default = mw_;
    add_slot(func, mw_);
} // -- END CVButton

void CVButton::mouse_handler(int &event, int x, int y)
{
    // -1 - за пределами, 0 - мышь наведена, 1 - мышь нажата, 2 - мышь отпущена, 3 - двойной клик.
    mouse_status = event;
    f_change = true;
    //    std::cout << "Call CVButton mouse_Handler " << std::endl;
} // -- END mouse_handler

void CVButton::show(cv::Mat &img)
{
    if(f_change)
    {
        switch(mouse_status)
        {

        case CVWidgetMouseEvent::NOTHING:
        {
            clr_active = clr_base;
            break;
        } // END case -1:
        case CVWidgetMouseEvent::MOVE:
        {
            clr_active = clr_mouse_in;
            break;
        } // END case 0:
        case CVWidgetMouseEvent::LBTN_DOWN:
        {
            clr_active = clr_mouse_down;
            break;
        } // END case 1:
        case CVWidgetMouseEvent::LBTN_UP:
        {
            clr_active = clr_mouse_in;
            slot(mw_default);
            break;
        } // END case 2:
        default:
        {
            rectangle(img, rct, clr_base, 5);
            break;
        } // END case default
        } // END switch(mouse_status)
        if(show_type == 0) {show_text(name, img);}
        if(show_type == 1) {show_icon(img);}
        f_change = false;
    } // END if(f_change)
    return;
} // -- END show

void CVButton::show_icon(cv::Mat & img)
{
    img_buf = clr_icon;
    img(rct_bord) = clr_active;
    img_buf.copyTo(img(rct_bord), icon);
    rectangle(img, rct, cv::Scalar(30,30,30), bord);
} // -- END show_icon

void CVButton::show_text(std::string & text, cv::Mat & img)
{

    // Вычичсляем кол-во строк и их длдины, а также максимальную длину
    vector<Point> v_text_params;
    int pos_now = 0;
    int pos_prev = 0;
    int word_length_max = 0;
    const string line_break_str = "\n";
    int i = 0;
    int i_max = 0;
    while(true)
    {
        pos_now = text.find(line_break_str, pos_prev);

        if(pos_now == std::string::npos)
        {
            v_text_params.emplace_back(Point(pos_prev, text.size() - pos_prev));
            if(word_length_max < v_text_params[i].y) {word_length_max = v_text_params[i].y; i_max = i;}
            pos_prev = pos_now + line_break_str.size();
            i++;
            break;
        } // END if(pos_now == std::string::npos)
        else
        {
            v_text_params.emplace_back(Point(pos_prev, pos_now - pos_prev));
            if(word_length_max < v_text_params[i].y) {word_length_max = v_text_params[i].y;}
            pos_prev = pos_now + line_break_str.size();
            i++;
        } // END else
    } // END while(true)

    font_scale = getFontScaleFromHeight(font_face,  font_face_kf * rct.height / (float)v_text_params.size());
    if(font_scale > 2) {font_scale = 2;}
    int text_thic = round(1 + font_scale);
    int baseline = 0;
    text_size = getTextSize(text.substr(v_text_params[i_max].x, v_text_params[i_max].y), font_face, font_scale, text_thic, &baseline);

    if(word_length_max == 0)
    {
        img(rct_bord) = clr_active;
    } // END if(word_length_max == 0)
    else if(text_size.width > rct_bord.width)
    {
        Size btn_rct_sz(text_size.width, (text_size.height + 2 * baseline) * v_text_params.size());
        resize(frame, frame, btn_rct_sz);
        frame = clr_active;
        for(int i = 0; i < v_text_params.size(); i++)
        {
            if(v_text_params[i].y == 0)
            {
                continue;
            } // END if(word_length_max == 0)
            else /*if(text_size.width > rct_bord.width)*/
            {
                text_size = getTextSize(text.substr(v_text_params[i].x, v_text_params[i].y), font_face, font_scale, text_thic, &baseline);
                cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), i * (text_size.height + 2 * baseline) + text_size.height + baseline);
                cv::putText(frame, text.substr(v_text_params[i].x, v_text_params[i].y), pt_text, font_face, font_scale, text_clr, text_thic);
            } // END else if(text_size.width > rct_bord.width)
        } // END for(int i = 0; i < v_text_params.size(); i++)
        resize(frame, img(rct_bord), rct_bord.size());
    } // END if(word_length_max != 0)
    else
    {
        int baseline = 0;
        text_size = getTextSize(text.substr(v_text_params[i_max].x, v_text_params[i_max].y), font_face, font_scale, text_thic, &baseline);
        Size btn_rct_sz(text_size.width, (text_size.height + 2 * baseline) * v_text_params.size());
        resize(frame, frame, rct_bord.size());
        frame = clr_active;
        for(int i = 0; i < v_text_params.size(); i++)
        {
            if(v_text_params[i].y == 0)
            {
                continue;
            } // END if(word_length_max == 0)
            else /*if(text_size.width > rct_bord.width)*/
            {
                text_size = getTextSize(text.substr(v_text_params[i].x, v_text_params[i].y), font_face, font_scale, text_thic, &baseline);
                cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), i * (text_size.height + 2 * baseline) + text_size.height + baseline);
                cv::putText(frame, text.substr(v_text_params[i].x, v_text_params[i].y), pt_text, font_face, font_scale, text_clr, text_thic);
            } // END else if(text_size.width > rct_bord.width)
        } // END for(int i = 0; i < v_text_params.size(); i++)
        resize(frame, img(rct_bord), rct_bord.size());
    } // END else

    rectangle(img, rct, cv::Scalar(30,30,30), bord);
} // -- END show_text

void CVButton::add_slot(std::function<void (MainWindowConstructor *)> func, MainWindowConstructor *mw_) {slot = func; mw_default = mw_;}

void CVButton::setName(std::string new_name) {name = new_name; f_change = true;}

void CVButton::setIcon(cv::Mat &icon_)
{
    icon = icon_.clone();
    resize(icon, icon, rct_bord.size());
    img_buf = cv::Mat(rct_bord.size(), CV_8UC3, cv::Scalar(0,0,0));
    f_change = true;
} // -- END setIcon

void CVButton::setColorIcon(cv::Scalar clr_) {clr_icon = clr_; img_buf = clr_icon;}

void CVButton::setShowType(int type_) {show_type = type_;}

CVButton::~CVButton()
{
    cout << "CVButton::" << name << " Destructor!" << endl;
} // -- END CVWidget


}; // END namespace cvw
