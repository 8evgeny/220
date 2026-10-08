#include "cv_combo_box.hpp"


using namespace std;
using namespace cv;

namespace cvw
{

int CVComboBox::mouse_event = CVWidgetMouseEvent::NOTHING;
bool CVComboBox::f_move = 0;
bool CVComboBox::f_click_up = 0;
bool CVComboBox::f_click_down = 0;
//bool CVComboBox::f_dclick = 0;
Point CVComboBox::p_mouse(0, 0);



CVComboBox::CVComboBox()
{
} // -- END CVComboBox

CVComboBox::CVComboBox(std::string name_, CVMainWindow * main_window, cv::Rect2f rct_)
{
    name = name_;
    std::cout << "Create " << name << " in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    frame = Mat(Size(120,40), CV_8UC3, clr_base);
    frame_combobox = frame.clone();
    main_window->add_mouse_rect(rct_);

    func_mouse_handler = std::bind(&CVComboBox::mouse_handler, this,  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);

    func_show = std::bind(&CVComboBox::show, this,  std::placeholders::_1);
    main_window->add_child_show(func_show);

    cout << "name = " << name << endl;
} // -- END CVComboBox

CVComboBox::CVComboBox(std::string name_, CVMainWindow *main_window, cv::Rect2f rct_, std::function<void (MainWindowConstructor *, int&, int&)> func, MainWindowConstructor *mw_)
{
    name = name_;
    std::cout << "Create " << name << " in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    frame = Mat(Size(120,40), CV_8UC3, clr_base);
    frame_combobox = frame.clone();
    main_window->add_mouse_rect(rct);

    func_mouse_handler = std::bind(&CVComboBox::mouse_handler, this,  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);

    func_show = std::bind(&CVComboBox::show, this,  std::placeholders::_1);
    main_window->add_child_show(func_show);
    mw_default = mw_;
    add_slot(func, mw_);
} // -- END CVComboBox

void CVComboBox::mouse_handler(int &event, int x, int y)
{
    // -1 - за пределами, 0 - мышь наведена, 1 - мышь нажата, 2 - мышь отпущена, 3 - двойной клик.
    //    cout << "Call " << name << " mouse handler" << endl;
    mouse_status = event;
} // -- END mouse_handler

void CVComboBox::show(cv::Mat &img)
{

    if(f_win_create)
    {
        if(v_combobox_str.size() > 0)
        {
            int text_w = 0;
            int text_h = 0;
            for(int i = 0; i < v_combobox_str.size(); i++)
            {
                int baseline = 0;
                Size sz_text_i = cv::getTextSize(v_combobox_str[i], font_face, font_scale, round(font_scale), &baseline);
                if(sz_text_i.width > text_w) {text_w = sz_text_i.width;}
                if(sz_text_i.height > text_h) {text_h = sz_text_i.height;}
            } // END for(int i = 0; i < v_combobox_str.size(); i++)
            resize(frame_combobox, frame_combobox, Size(text_w, text_h * v_combobox_str.size()));
            setMouseCallback(name, aimFrameCallback, &p_mouse);

            for(int i = 0; i < v_combobox_str.size(); i++)
            {
                Rect rct_str = Rect(0, i * text_h, frame_combobox.cols, text_h);
                if(p_mouse.inside(rct_str))
                {
                    frame_combobox(rct_str) = clr_base;
                    if(f_click_up)
                    {
                        f_win_create = false;
                        destroyWindow(name);
                        combobox_res_str = v_combobox_str[i];
                        pos = i;
                        cout << "pos = " << pos << endl;
                        f_change = true;
                        f_click_down = 0;
                        f_click_up = 0;
                        //                        show_text(name, img);
                        return;
                    } // END else if(f_click_up)
                    else if(f_click_down)
                    {
                        frame_combobox(rct_str) = clr_mouse_down;
                    } // END if(f_click_down)
                    else
                    {
                        frame_combobox(rct_str) = clr_mouse_in;
                    } // END else
                } // END if(p_mouse.inside(rct_str))
                else
                {
                    frame_combobox(rct_str) = clr_base;
                } // END if(!p_mouse.inside(rct_str))
                string text = to_string(i + 1)  + ". " + v_combobox_str[i];
                show_rct(text, rct_str);
            } // END for(int i = 0; i < v_combobox_str.size(); i++)

            imshow(name, frame_combobox);
        } // END if(v_combobox_str.size() > 0)
    } // END f_win_create
    int bord = 2;
    //    imshow("frame", frame);
    Rect rct_bord = Rect(rct.x + bord, rct.y + bord, rct.width - 2 * bord, rct.height - 2 * bord);
    switch(mouse_status)
    {
    case CVWidgetMouseEvent::NOTHING:
    {
        clr_now = clr_base;
        break;
    } // END case -1:
    case CVWidgetMouseEvent::MOVE:
    {
        clr_now = clr_mouse_in;
        break;
    } // END case 0:
    case CVWidgetMouseEvent::LBTN_DOWN:
    {
        clr_now = clr_mouse_down;
        break;
    } // END case 1:
    case CVWidgetMouseEvent::LBTN_UP:
    {
        clr_now = clr_mouse_in;
        int x=0,y=0;

        slot(mw_default, x, y);
        if(!f_win_create)
        {
            namedWindow(name, WINDOW_GUI_NORMAL);
            setWindowProperty(name, WND_PROP_AUTOSIZE, WINDOW_AUTOSIZE);
            cout << "Move window to " << Point(rct.x / (float)img.cols * 1920.f, rct.y / (float)img.rows * 1080.f) << endl;
            Rect rct_window = getWindowImageRect("win");
            int x_win = rct_window.x + (rct.x) / (float)img.cols * rct_window.width;
            int y_win = rct_window.y + (rct.y + rct.height) / (float)img.rows * rct_window.height;
            moveWindow(name, x_win, y_win);
            f_win_create = true;
            mouse_status = CVWidgetMouseEvent::MOVE;
        } // END if(!f_win_create)
        else
        {
            destroyWindow(name);
            f_win_create = false;
            mouse_status = CVWidgetMouseEvent::MOVE;
        } // END if(f_win_create)
        break;
    } // END case 2:
    default:
    {
        rectangle(img, rct, clr_base, 5);
        break;
    } // END case default
    } // END switch(mouse_status)
    show_text(combobox_res_str, img);
    rectangle(img, rct, cv::Scalar(30,30,30), bord);
    return;
} // -- END show

void CVComboBox::show_rct(std::string & text, Rect & rct_)
{
    int text_thic = round(font_scale );
    cv::Point pt_text = cv::Point(rct_.x /*+ rct_.width * 0.5 - text.size() * (10 * font_scale)*/, rct_.y + rct_.height * 0.5 + 8 * font_scale);
    cv::putText(frame_combobox, text, pt_text, font_face, font_scale - 0.5, text_clr, text_thic);
} // -- END show_rct

void CVComboBox::show_text(std::string & text, cv::Mat & img)
{
    // Вычичсляем кол-во строк и их длины, а также максимальную длину
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
        frame = clr_now;
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
        frame = clr_now;
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


CVComboBox::~CVComboBox()
{
    cout << "CVComboBox::" << name << " Destructor!" << endl;
} // -- END CVWidget


void CVComboBox::aimFrameCallback(int event, int x, int y, int flags, void *param)
{
    Point *p = reinterpret_cast<Point*>(param);
    p->x = x;
    p->y = y;
    p_mouse = Point(x, y);
    mouse_event = event;
    switch(event)
    {
    case EVENT_LBUTTONDOWN:
        f_click_down = 1;
        break;
    case EVENT_LBUTTONUP:
        f_click_up = 1;
        break;
    case EVENT_MOUSEMOVE:
        f_move = 1;
        break;
    } // -- END switch(event)
} // -- END aimFrameCallback

}; // END namespace cvw
