#include "cv_widget.hpp"

using namespace std;
using namespace cv;

namespace cvw
{

CVWidget::CVWidget(std::string name_, CVMainWindow *main_window, cv::Rect2f rct_)
{
    name = name_;
    std::cout << "Create CVWidget in " << rct_ << std::endl;
    cout << "rct_ = " << rct_ << endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    cout << "rct = " << rct << endl;
    frame = Mat(rct.size(), CV_8UC3, clr_base);
    main_window->add_mouse_rect(rct);
    func_mouse_handler = std::bind(&CVWidget::mouse_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);
    func_show = std::bind(&CVWidget::show, this,  std::placeholders::_1);
    main_window->add_child_show(func_show);
    if(font_face == cv::FONT_HERSHEY_SIMPLEX) {}
} // -- END CVWidget

CVWidget::~CVWidget()
{
    cout << "CVWidget::" << name << " Destructor!" << endl;
} // -- END CVWidget

void CVWidget::set_clr_base(cv::Scalar clr_, cv::Scalar clr_mouse_in_)
{
    clr_base = clr_;
    clr_mouse_in = clr_mouse_in_;
    f_change = true;
} // -- END set_clr_base

void CVWidget::setShowType(int type_) {show_type = type_;}


void CVWidget::mouse_handler(int & event, int x, int y)
{
    //    cout << "Mouse handler " << name << " was call" << endl;
    return;
} // -- END mouse_handler

void CVWidget::show(cv::Mat &img)
{
    if(f_change)
    {
        if(show_type == 0) {show_text(name, img);}
        if(show_type == 2) {show_image(img);}
        f_change = false;
    } // END if(f_change)
} // -- END show



void CVWidget::show_text(std::string & text, cv::Mat & img)
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
            if(word_length_max < v_text_params[i].y) {word_length_max = v_text_params[i].y; i_max = i;}
            pos_prev = pos_now + line_break_str.size();
            i++;
        } // END else
    } // END while(true)

    int bord = 2;
    font_scale = getFontScaleFromHeight(font_face,  font_face_kf * rct.height / (float)v_text_params.size());
    if(font_scale > 2) {font_scale = 2;}
    int text_thic = round(1 + font_scale);
    int baseline = 0;
    if(text.size() != 0)
    {
        text_size = getTextSize(text.substr(v_text_params[i_max].x, v_text_params[i_max].y), font_face, font_scale, text_thic, &baseline);
    } // END if(text.size() != 0)

    if(word_length_max == 0)
    {
        img(rct) = clr_base;
    } // END if(word_length_max == 0)
    else if(text_size.width > rct.width)
    {
        Size widget_rct_sz(text_size.width, (text_size.height + 2 * baseline) * v_text_params.size());
        resize(frame, frame, widget_rct_sz);
        frame = clr_base;
        for(int i = 0; i < v_text_params.size(); i++)
        {
            if(v_text_params[i].y == 0)
            {
                continue;
            } // END if(word_length_max == 0)
            else /*if(text_size.width > rct.width)*/
            {
                text_size = getTextSize(text.substr(v_text_params[i].x, v_text_params[i].y), font_face, font_scale, text_thic, &baseline);
                cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), i * (text_size.height + 2 * baseline) + text_size.height + baseline);
                cv::putText(frame, text.substr(v_text_params[i].x, v_text_params[i].y), pt_text, font_face, font_scale, text_clr, text_thic);
            } // END else if(text_size.width > rct.width)
        } // END for(int i = 0; i < v_text_params.size(); i++)
        resize(frame, img(rct), rct.size());
    } // END if(word_length_max != 0)
    else
    {
        int baseline = 0;
        text_size = getTextSize(text.substr(v_text_params[i_max].x, v_text_params[i_max].y), font_face, font_scale, text_thic, &baseline);
        resize(frame, frame, rct.size());
        frame = clr_base;
        for(int i = 0; i < v_text_params.size(); i++)
        {
            if(v_text_params[i].y == 0)
            {
                continue;
            } // END if(word_length_max == 0)
            else /*if(text_size.width > rct.width)*/
            {
                text_size = getTextSize(text.substr(v_text_params[i].x, v_text_params[i].y), font_face, font_scale, text_thic, &baseline);
                cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), i * (text_size.height + 2 * baseline) + text_size.height + baseline);
                cv::putText(frame, text.substr(v_text_params[i].x, v_text_params[i].y), pt_text, font_face, font_scale, text_clr, text_thic);
            } // END else if(text_size.width > rct.width)
        } // END for(int i = 0; i < v_text_params.size(); i++)
        resize(frame, img(rct), rct.size());
    } // END else
} // -- END show_text

void CVWidget::show_image(cv::Mat &img)
{
    resize(img2show, img(rct), rct.size());
} // -- END show_image

}; // END namespace cvw
