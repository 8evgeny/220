#include "cv_main_window.hpp"

using namespace std;
using namespace cv;

namespace cvw
{
int CVMainWindow::mouse_event = CVWidgetMouseEvent::NOTHING;
bool CVMainWindow::f_move = 0;
bool CVMainWindow::f_click_up = 0;
bool CVMainWindow::f_click_down = 0;
Point CVMainWindow::p_mouse(0, 0);
std::atomic<unsigned char> CVMainWindow::key = 0xFF;

CVMainWindow::CVMainWindow()
{
    f_exec = false;
    std::cout << "Default constructor CVMainWindow" << std::endl;
} // -- END CVMainWindow

CVMainWindow::CVMainWindow(const std::string & config, bool & ok, std::string &winname_, cv::Mat & image_)
{
    std::cout << "Constructor CVMainWindow" << std::endl;
    ok = get_ini_params(config);
    if(!ok) {cout << "CVMainWindow::ERROR read ini_params!" << endl; return;}
    f_exec = false;
    winname = winname_;
    frame = image_/*.clone()*/;
    std::cout << "CVMainWindow::show()" << std::endl;
    show();
    frame_size_2 = Size(round(0.5 * frame.cols), round(0.5 * frame.rows));
    std::cout << "END Constructor CVMainWindow" << std::endl;
} // -- END CVMainWindow

CVMainWindow::~CVMainWindow()
{
    destroyAllWindows();
    std::cout << "CVMainWindow Destructor" << std::endl;
} // -- END ~CVMainWindow

void CVMainWindow::show()
{
    for(int i = 0; i < v_child_show.size(); i++)
    {
        v_child_show[i](frame);
    } // END for(int i = 0; i < v_child_show.size(); i++)
} // -- END show

bool CVMainWindow::DirContent(string& path, vector<string>& fileList)
{
    fileList.clear();
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(path.c_str())) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            string filename = string(ent->d_name);
            // -- typ = 4 (folder), typ = 8 (file).
            if(ent->d_type == 8 && filename != "." && filename != "..")
            {
                fileList.emplace_back(filename);
            } // END if(ent->d_type == 8 && filename != "." && filename != "..")
        } // END while((ent = readdir (dir)) != NULL)
        closedir(dir);
        if(fileList.size() > 1){sort(fileList.begin(), fileList.end());}
        cout << "Dir " << path << " open success." << endl;
        return 1;
    } // -- END if((dir = opendir(way)) != NULL)
    cout << "Dir " << path << " can't open!" << endl;
    return 0;
} // END -- DirContent

bool CVMainWindow::get_ini_params(const std::string &config)
{
    bool ok = true;
    INIReader reader(config);
    if(reader.ParseError() < 0)
    {
        cout << "CVMainWindow::Can't load config_path='" << config_path << "'\n";
        return 0;
    } // -- END if(reader.ParseError() < 0)

    video_on = reader.GetInteger("recorder", "video_on", -1);
    if(video_on == -1)
    {
        cout << "Not found video_on in [recorder]!\n";
        return false;
    } // END if(video_on == -1)
    else if(video_on == 1)
    {
        path4frames = reader.Get("recorder", "path4frames", "oops");
        if(path4frames == "oops")
        {
            cout << "Not found path4frames in [recorder]!\n";
            return false;
        } // END if(path4frames == "oops")

        save_frames_skip = reader.GetInteger("recorder", "save_frames_skip", -1);
        if(save_frames_skip == -1)
        {
            cout << "Not found save_frames_skip in [recorder]!\n";
            return false;
        } // END  if(save_frames_skip == -1)

        save_frames_prefix = reader.GetInteger("recorder", "save_frames_prefix", -1);
        if(save_frames_prefix == -1)
        {
            cout << "Not found save_frames_prefix in [recorder]!\n";
            return false;
        } // END if(save_frames_prefix == -1)

        max_frames = reader.GetInteger("recorder", "max_frames", -1);
        if(max_frames == -1)
        {
            cout << "Not found max_frames in [recorder]!\n";
            return false;
        } // END if(max_frames == -1)

        if(DirContent(path4frames, fileList))
        {
            if(fileList.size())
            {
                string last_file = fileList[fileList.size() - 1];
                string last_file1 = last_file.substr(0, last_file.length() - 4);
                int last_num = stoi(last_file1) - save_frames_prefix;
                video_num = last_num + 2;
            } // END if(fileList.size())
            else
            {video_num = 1;}
        } // END if(DirContent(path4frames, fileList))
        else
        {
            cout << "Record direcry: " << path4frames << " NOT EXIST!" << endl;
            video_on = 0;
            quit();
            return false;
        } // END else
    } // END else if(video_on == 1)

    return ok;
} // -- END get_ini_params

void CVMainWindow::save_frame()
{
    if (f_write_frames)
    {
        if(video_on) // запись фрейма
        {
            if(video_num <= max_frames)
            {
                string path = path4frames + "/" + to_string(save_frames_prefix + video_num) + ".jpg";
                cout << "Save " << path << endl;
                imwrite(path, frame4save);
                video_num++;
            } // END if(video_num <= max_frames)
            else
            {
                video_on = 0;
            } // END else
        } // END if(video_on && (rec_on || find_some))
    }// END if (f_write_frames)
} // END -- save_frame

void CVMainWindow::key_handler()
{
    if(key != 0xFF)
    {
        for(int i = 0; i < v_key_handlers.size(); i++)
        {
            v_key_handlers[i](mwc, key);
        } // END for(int i = 0; i < v_key_handlers.size(); i++)
    } // END if(key != 0xFF)

    if(key.load() == '/') {f_exec = false;}
    if((key.load() == 'r' || key.load() == 'R') && f_write_frames && (chrono::system_clock::now() > time_press_button + chrono::duration(500ms)))
    {
        time_press_button = chrono::system_clock::now();
        f_write_frames = false;
        ++video_num;
        cout << "====== Stop write frames ======\n";
    } // END if((key.load() == 'r' || key.load() == 'R') && f_write_frames && (chrono::system_clock::now() > time_press_button + chrono::duration(1s)))
    if((key.load() == 'r' || key.load() == 'R') && !f_write_frames && (chrono::system_clock::now() > time_press_button + chrono::duration(1s)))
    {
        time_press_button = chrono::system_clock::now();
        f_write_frames = true;
        cout << "====== Start write frames ======\n";
    } // END if((key.load() == 'r' || key.load() == 'R') && !f_write_frames && (chrono::system_clock::now() > time_press_button + chrono::duration(1s)))

} // -- END key_handler

void CVMainWindow::exec(bool topmost)
{
    if(topmost){namedWindow(winname, WINDOW_GUI_NORMAL); setWindowProperty(winname, WINDOW_GUI_NORMAL, WND_PROP_FULLSCREEN);}
    else {namedWindow(winname, WINDOW_GUI_NORMAL);}
    show();
    f_exec = true;
    while(f_exec)
    {
        mouseHandler();
        show();
        resize(frame, frame4save, frame_size_2);
        imshow(winname, frame);
//        imshow(winname, frame4save);
        if(topmost)
        {
            setWindowProperty(winname, WINDOW_GUI_NORMAL, WND_PROP_TOPMOST);
            string name = "win";
            Rect rct_window = getWindowImageRect("win");
            int x_win = rct_window.x + rct_window.width * 0.3;
            int y_win = rct_window.y + rct_window.height * 0.3;
            moveWindow(winname, x_win, y_win);
        } // END if(topmost)
        save_frame();
        key.store(waitKey(30));
        key_handler();
    } // END while(true)
    destroyWindow(winname);
} // END exec

void CVMainWindow::add_mouse_handler(std::function<void (int &, int, int)> &func)
{
    v_child_mouse_handlers.push_back(func);
} // -- END add_mouse_handler

void CVMainWindow::add_child_show(std::function<void (cv::Mat &)> &func)
{
    v_child_show.push_back(func);
} // -- END add_child_show

void CVMainWindow::add_key_handler(std::function<void (MainWindowConstructor*, unsigned char )> func)
{
    cout << "Add KEY HANDLER TO CVMainWindow" << endl;
    v_key_handlers.emplace_back(func);
} // -- END add_key_handler

void CVMainWindow::add_mouse_rect(cv::Rect rct_)
{
    v_child_rct.push_back(rct_);
} // -- END add_mouse_rect

bool CVMainWindow::mouseHandler()
{
    setMouseCallback(winname, aimFrameCallback, &p_mouse);
    bool in_main_window = true;
    for(int i = 0; i < v_child_rct.size(); i++) // выясняем в каком окне находится курсор
    {
        if(p_mouse.inside(v_child_rct[i]))
        {
            in_main_window = false;
            win_id_prev = win_id;
            win_id = i;
        } // END if(p_mouse.inside(v_child_rct[i]))
    } // END for(int i = 0; i < v_child_rct.size(); i++)
    if (in_main_window)
    {
        win_id = -1;
        f_move = 0;
        f_click_up = 0;
        f_click_down = 0;
        if(win_id_prev != -1)
        {
            int event = CVWidgetMouseEvent::NOTHING;
            for(int i = 0; i < v_child_mouse_handlers.size(); i++)
            {
                v_child_mouse_handlers[i](event, p_mouse.x, p_mouse.y);
            } // END for(int i = 0; i < v_child_mouse_handlers.size(); i++)
            win_id_prev = -1;
        } // END if(win_id_prev != -1)
        return true;
    } // -- END if (in_main_window)
    else if(win_id != win_id_prev)
    {
        for(int i = 0; i < v_child_mouse_handlers.size(); i++)
        {
            int event = CVWidgetMouseEvent::NOTHING;
            if(i != win_id)
            {
                v_child_mouse_handlers[i](event, p_mouse.x, p_mouse.y);
            } // END if(i != win_id)
        } // END for(int i = 0; i < v_child_mouse_handlers.size(); i++)
    } // END else if(win_id != win_id_prev)
    else
    {
        if(!f_click_down && !f_click_up)
        {
            int event = CVWidgetMouseEvent::MOVE;
            v_child_mouse_handlers[win_id](event, p_mouse.x, p_mouse.y);
        } // END if(!f_click_down && !f_click_up)
        else if(f_click_down)
        {
            int event = CVWidgetMouseEvent::LBTN_DOWN;
            v_child_mouse_handlers[win_id](event, p_mouse.x, p_mouse.y);
        } // END else if(f_click_down)
        if(f_click_up)
        {
            int event = CVWidgetMouseEvent::LBTN_UP;
            v_child_mouse_handlers[win_id](event, p_mouse.x, p_mouse.y);
            f_click_up = 0;
            f_click_down = 0;
        } // END if(f_click_up)
    } // END else
    return true;
} // -- END mouseHandler

void CVMainWindow::aimFrameCallback(int event, int x, int y, int flags, void *param)
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
