#include "cv_line_edit.hpp"

using namespace std;
using namespace cv;

//void default_slot(MainWindowConstructor * mw) {cout << "This CVLineEdit have no slot" << endl;}
void default_slot(MainWindowConstructor * mw, std::string & str) {cout << "This CVLineEdit have no slot" << endl;}

namespace cvw
{

CVLineEdit::CVLineEdit()
{
} // -- END CVLineEdit

CVLineEdit::CVLineEdit(std::string text_, CVMainWindow * main_window, cv::Rect2f rct_/*, char & key_ */)
{
    name = "CVLineEdit";
    text = text_;

    std::cout << "Create " << name << " in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));

    frame = Mat(rct.size(), CV_8UC3, clr_base);
    main_window->add_mouse_rect(rct);

    func_mouse_handler = std::bind(&CVLineEdit::mouse_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);

    func_show = std::bind(&CVLineEdit::show, this,  std::placeholders::_1);
    add_slot(default_slot, this->mw_default);
    main_window->add_child_show(func_show);
    mw_parent = main_window;
    f_change = true;

} // -- END CVLineEdit

void CVLineEdit::editText()
{
    std::string old_text = text;
    cout << "Start edit text in " << name << endl;
    char key_;

    while(choose_line)
    {
        f_change = true;

        key_ = key.load();
        key.store(255);
        //        cout << "CVLineEdit::" << name << ": key = " << (int)key_ << endl;
        if(key_ >= 33 && key_ <= 125)
        {
            text += key_;
            cout << "New text = " << text << endl;
        } // END if(key_ >= 33 && key <= 125)
        if(key_ == 27) // esc
        {
            text = old_text;
            choose_line = 0;
            cout << "Exit edit text in " << name << endl;
            return;
        } // END if(key_ == 27)
        if(key_ == 8) // backspace
        {
            if(text.size() > 0)
            {
                text.pop_back();
            } // END if(text.size() > 0)
            if(text.size() == 0)
            {
                text = " ";
            } // END if(text.size() == 0)
        }
        if(key_ == 13) // enter
        {
            cout << "OK edit text in " << name << endl;
            choose_line = 0;
            slot(mw_default, text);
            return;
        } // END if(key == 32)
        this_thread::sleep_for(chrono::milliseconds(10));
    } // END while(true)

    return;
} // -- END editText

void CVLineEdit::editFloatNum()
{
    unsigned char key_ = 255;
    std::string old_text = text;
    while(true)
    {
        f_change = true;

        key_ = key.load();
        key.store(0xFF);
        int key_digit = -1;
        bool plot = 0;
        bool not_point = 1;
        if(key_ == '/'){break;}
        if(key_ != 255)
        {
            //cout << "key_ = " << (int)key_ << endl;
            if(key_ == 13 && text.size() > 0) // ENTER
            {
                if (!text.size() || text == " ")
                {
                    text = "0";
                } // END if (!text.size() || text == " ")
                num_f = stof(text);
                if(num_f < num_min) {num_f = num_min;}
                if(num_f > num_max) {num_f = num_max;}
                std::stringstream stream;
                stream << std::fixed << std::setprecision(precision) << num_f;

                text = stream.str();
                cout << "CVLineEdit::" << name << ":num_f = " << num_f << endl;
                choose_line = 0;
                slot(mw_default, text);
                return;
            } // END if(key_ == 13 && text.size() > 0)
            if(key_ == 8 && text.size() > 0) // backspace
            {
                plot = 1;
                if(text[text.size() - 1] == '.'){not_point = 1;}
                text.resize(text.size() - 1);
                if(!text.size())
                {
                    text = " ";
                } // END if(!text.size())

            } // END if(key_ == 8 && text.size() > 0)
            key_digit = (int)key_ - 48;
            if(key_digit > -1 && key_digit < 10)
            {
                plot = 1;
                text += to_string(key_digit);
            } // END if(key_digit > -1 && key_digit < 10)
            if(key_ == 46 && not_point) // point
            {
                plot = 1;
                text += '.';
                not_point = 0;
            } // END if(key_ == 46 && not_point)
            if(key_ == '-' && (text == " " || text == "0")) // -
            {
                text = "-";
            }
            if(key_ == 27) // esc
            {
                text = old_text;
                choose_line = 0;
                cout << "Exit edit text in " << name << endl;
                return;
            } // END if(key_ == 27)

        } // END if(key_ != 255)
        this_thread::sleep_for(chrono::milliseconds(10));

    } // END while(true)
    return;
} // -- END editFloatNum

void CVLineEdit::editIntNum()
{
    unsigned char key_ = 255;
    std::string old_text = text;
    while(true)
    {
        f_change = true;

        key_ = key.load();
        key.store(0xFF);
        int key_digit = -1;
        bool plot = 0;
        if(key_ == '/'){break;}
        if(key_ != 255)
        {
            //cout << "key_ = " << (int)key_ << endl;
            if(key_ == 13 && text.size() > 0) // ENTER
            {
                if (!text.size() || text == " ")
                {
                    text = "0";
                } // END if (!text.size() || text == " ")
                num_i = stof(text);
                if(num_i < num_min) {num_i = (int)num_min;}
                if(num_i > num_max) {num_i = (int)num_max;}
                std::stringstream stream;
                stream << std::fixed << std::setprecision(precision) << num_i;

                text = stream.str();
                cout << "CVLineEdit::" << name << ":num_i = " << num_i << endl;
                choose_line = 0;
                slot(mw_default, text);

                return;
            } // END if(key_ == 13 && text.size() > 0)
            if(key_ == 8 && text.size() > 0) // backspace
            {
                plot = 1;
                text.resize(text.size() - 1);
                if(!text.size())
                {
                    text = " ";
                } // END if(!text.size())

            } // END if(key_ == 8 && text.size() > 0)
            key_digit = (int)key_ - 48;
            if(key_digit > -1 && key_digit < 10)
            {
                plot = 1;
                text += to_string(key_digit);
            } // END if(key_digit > -1 && key_digit < 10)
            if(key_ == 45 && text == " ") // -
            {
                text += "-";
            }
            if(key_ == 27) // esc
            {
                text = old_text;
                choose_line = 0;
                cout << "Exit edit text in " << name << endl;
                return;
            } // END if(key_ == 27)

        } // END if(key_ != 255)
        this_thread::sleep_for(chrono::milliseconds(10));

    } // END while(true)
    return;
} // -- END editIntNum

void CVLineEdit::mouse_handler(int &event, int x, int y)
{
    // -1 - за пределами, 0 - мышь наведена, 1 - мышь нажата, 2 - мышь отпущена, 3 - двойной клик.
    if(event == CVWidgetMouseEvent::LBTN_UP)
    {
        f_change = true;
        if(choose_line) {choose_line = 0;}
        else
        {
            choose_line = 1;
            switch (type)
            {
            case EDIT_TEXT:
            {
                std::thread thrd(&CVLineEdit::editText, this);
                thrd.detach();
                break;
            } // END case TEXT:
            case EDIT_FLOAT:
            {
                std::thread thrd(&CVLineEdit::editFloatNum, this);
                thrd.detach();
                break;
            } // END case TEXT:
            case EDIT_INTEGER:
            {
                std::thread thrd(&CVLineEdit::editIntNum, this);
                thrd.detach();
                break;
            } // END case TEXT:

            } // -- END switch (type)

        } // END if(!coose_line)
    } // END if(event == CVWidgetMouseEvent::LBTN_UP)
} // -- END mouse_handler

void CVLineEdit::show(cv::Mat &img)
{
    if(f_change)
    {
        show_text(text, img);

        if(choose_line)
        {
            int bord = 2;
            Rect rct_bord = Rect(rct.x + bord, rct.y + bord, rct.width - 2 * bord, rct.height - 2 * bord);
            rectangle(img, rct_bord, Scalar(0,0,255), bord);
        } // END if(choose_line)
        f_change = false;
    }
    return;
} // -- END show

void CVLineEdit::show_text(std::string & text, cv::Mat & img)
{
    int bord = 2;
    Rect rct_inside = Rect(rct.x + bord, rct.y + bord, rct.width - 2 * bord, rct.height - 2 * bord);
    font_scale = getFontScaleFromHeight(font_face,  font_face_kf * rct.height);
    if(font_scale > 2.0) {font_scale = 2.0;}
    int text_thic = round(1.0 + font_scale);
    int baseline = 0;
    text_size = getTextSize(text, font_face, font_scale, text_thic, &baseline);
    frame = clr_base;
    if(text.size() == 0)
    {
        img(rct) = clr_base;
    } // END if(text.size() == 0)
    else if(text_size.width > rct.width)
    {
        resize(frame, frame, text_size + Size(0, 2 * baseline));
        cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), text_size.height + baseline);
        cv::putText(frame, text, pt_text, font_face, font_scale, text_clr, text_thic);
        resize(frame, img(rct), rct.size());
    } // if(text_size.width > rct.width)
    else
    {
        resize(frame, frame, rct.size());
        cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), text_size.height + baseline);
        cv::putText(frame, text, pt_text, font_face, font_scale, text_clr, text_thic);
        resize(frame, img(rct), rct.size());
    } // END if(text_size.width <= rct.width)
} // -- END show_text


/*
    font_scale = getFontScaleFromHeight(font_face,  font_face_kf * rct.height);
    if(font_scale > 2.0) {font_scale = 2.0;}
    int text_thic = round(1.0 + font_scale);
    int baseline = 0;
    text_size = getTextSize(text, font_face, font_scale, text_thic, &baseline);
    frame = clr_base;
    if(text.size() == 0)
    {
        img(rct) = clr_base;
    } // END if(text.size() == 0)
    else if(text_size.width > rct.width)
    {
        resize(frame, frame, text_size + Size(0, 2 * baseline));
        cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), text_size.height + baseline);
        cv::putText(frame, text, pt_text, font_face, font_scale, text_clr, text_thic);
        resize(frame, img(rct), rct.size());
    } // if(text_size.width > rct.width)
    else
    {
        resize(frame, frame, rct.size());
        cv::Point pt_text = cv::Point(0.5 * (frame.cols - text_size.width), text_size.height + baseline);
        cv::putText(frame, text, pt_text, font_face, font_scale, text_clr, text_thic);
        resize(frame, img(rct), rct.size());
    } // END if(text_size.width <= rct.width)

  */

void CVLineEdit::setText(std::string &text_, cv::Mat &img) {text = text_; show(img); f_change = true;}

void CVLineEdit::setType(int type_)
{
    f_change = true;
    switch (type)
    {
    case EDIT_TEXT:
    {
        cout << "Set TEXT type for " << name << endl;
        type = type_;
        break;
    } // END case TEXT:
    case EDIT_FLOAT:
    {
        cout << "Set FLOAT type for " << name << endl;
        type = type_;
        break;
    } // END case TEXT:
    case EDIT_INTEGER:
    {
        cout << "Set INTEGER type for " << name << endl;
        type = type_;
        break;
    } // END case TEXT:
    default:
    {
        cout << "UNDIFINED type for " << name << ". Set TEXT type" << endl;
        type = EDIT_TEXT;
        break;
    } /// END default
    } // -- END switch (type)

} // -- END setType

void CVLineEdit::setInterval(int p1, int p2)
{
    f_change = true;
    if(p1 == p2)
    {
        std::cout << "CVLineEdit::" << name << ": interval is single numer " << p1 << std::endl;
        num_min = p1;
        num_max = p1;
    } // END if(p1 == p2)
    else
    {
        num_min = MIN(p1, p2);
        num_max = MAX(p1, p2);
        std::cout << "CVLineEdit::" << name << ": new interval is " << cv::Point(num_min, num_max) << std::endl;
    } // END if(!p1 == p2)
} // -- END setInterval

CVLineEdit::~CVLineEdit()
{
    cout << "CVLineEdit::" << name << " Destructor!" << endl;
} // -- END CVWidget


}; // END namespace cvw
