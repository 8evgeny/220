#include "plot.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

Plot::Plot(const std::string & section, const std::string & config_path, bool & ok)
{
    cout << "Plot constructor" << endl;
    ok = get_ini_params(config_path, section);
    if(!ok) {cout << "Plot::Error ini params!" << endl; return;}
    frame_size = Size(plot_w, plot_h);
    vv_func.resize(num_f);

    img = cv::Mat(frame_size, CV_8UC3, clr_fone);
#ifdef USE_GUI
    namedWindow(winname, WINDOW_NORMAL);
#endif // USE_GUI
    cout << "OK setup image" << endl;
    return;
    // frame = Mat(frame_size, CV_8UC3, clr_fone);
} // -- END Plot

void Plot::set_vv(std::vector<std::vector<cv::Point2f> > &v_xy)
{
    cout << "call set vv" << endl;
    return;
} // -- END set_vv

char Plot::show()
{
    std::vector<std::vector<cv::Point>> vv_func_pix;
    vv_func_pix.resize(vv_func.size());
    float x_d_1 = 1.f / (x_max - x_min);
    float y_d_1 = 1.f / (y_max - y_min);
    img = clr_fone;

    Point p_c = Point(round((0 - x_min) * x_d_1  * (float)frame_size.width), frame_size.height - round((0 - y_min) * y_d_1  * (float)frame_size.height));
    // if(x_min > 0) {p_c.x = 0;}
    X_step = round((x_step) * x_d_1  * (float)frame_size.width);
    Y_step = round((y_step) * y_d_1  * (float)frame_size.height);

    int ix_max = (x_max - x_min) / x_step;
    int ix_min = x_min / x_step;
    int iy_max = (y_max - y_min) / y_step;
    int iy_min = y_min / y_step;

    for(int i = ix_min; i <= ix_min + ix_max; i++)
    {
        Point p_left =  Point(p_c.x + i * X_step, 0);
        Point p_right = Point(p_c.x + i * X_step, frame_size.height);
        line(img, p_left, p_right, clr_line, 1);
        std::string x_axis = "";
        if(i < 0) {x_axis += "-";}
        x_axis += to_string((int)abs((float)i * x_step )) + "." + to_string((int)abs(((float)i * x_step) * 100.f) % 100);
        putText(img, x_axis, p_c + Point(i * X_step - 15, 15), 1, 1, Scalar(200, 50,50));
    } // END for(int i = ix_min; i <= ix_min + ix_max; i++)

    for(int i = iy_min; i <= iy_min + iy_max; i++)
    {
        Point p_up =  Point(0, p_c.y - i * Y_step);
        Point p_down = Point(frame_size.width, p_c.y - i * Y_step);
        line(img, p_up, p_down, clr_line, 1);
        std::string y_axis = "";
        if(i < 0) {y_axis += "-";}
        y_axis += to_string((int)abs((float)i * y_step )) + "." + to_string((int)abs(((float)i * y_step) * 100.f) % 100);
        Point p_text = p_c + Point(15, -i * Y_step + 7);
        if (p_text.x < 0) {p_text.x = 15;}
        putText(img, y_axis, p_text, 1, 1, Scalar(200, 50, 50));
    } // END for(int i = ix_min; i <= ix_min + ix_max; i++)

    line(img, Point(0, p_c.y), Point(frame_size.width, p_c.y), clr_line_m, 2);
    line(img, Point(p_c.x, 0), Point(p_c.x, frame_size.height), clr_line_m, 2);

    for(int i = 0; i < vv_func.size(); i++)
    {
        // cout << "i = " << i << endl;
        vv_func_pix[i].resize(0);
        for(int j = 0; j < vv_func[i].size(); j++)
        {
            // cout << "x, y norm = " << (vv_func[i][j].x - x_min) * x_d_1 << "; " << (vv_func[i][j].y - y_min) * y_d_1  << endl;
            vv_func_pix[i].emplace_back(Point(
                round((vv_func[i][j].x - x_min) * x_d_1 * (float)frame_size.width),
                frame_size.height - round((vv_func[i][j].y - y_min) * y_d_1 * (float)frame_size.height)
                ));
            // cout << "Y[" << i << "," << j << "]=" << vv_func_pix[i][j] << endl;
            if(j > 0) {line(img, vv_func_pix[i][j], vv_func_pix[i][j-1], v_clr[i], v_thik[i]);}
        } // END for(int j = 0; j < vv_func[i].size(); j++)
    } // END for(int i = 0; i < vv_func.size(); i++)

    imshow(winname, img);
    char key = 0; // waitKey(100);
    return key;
} // -- END show


bool Plot::get_ini_params(const std::string &config, const std::string & section)
{
    cout << "BEGIN get_ini_params" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    INIReader reader(config);
    cout << "OK create reader. ParseError = " << reader.ParseError() << endl;
    if(reader.ParseError() < 0)
    {
        cout << "Can't load '" << config << "'\n";
        return 0;
    } // END if(reader.ParseError() < 0)

    cout << "\n[Plot]:" << endl;

    winname = reader.Get(section, "winname", "oops");
    if(winname == "oops"){cout << "mat2rtsp winname not declared!\n"; return false;} // END  if(winname == "oops")
    std::cout << "\twinname = " << winname << ";\n";


    plot_w = reader.GetInteger(section, "plot_w", -1111111111);
    if(plot_w == -1111111111){std::cout << "\tplot_w not declared\n"; return 0;}
    std::cout << "\tplot_w = " << plot_w << ";\n";

    plot_h = reader.GetInteger(section, "plot_h", -1111111111);
    if(plot_h == -1111111111){std::cout << "\tplot_h not declared\n"; return 0;}
    std::cout << "\tplot_h = " << plot_h << ";\n";

    num_f = reader.GetInteger(section, "num_f", -1111111111);
    if(num_f == -1111111111){std::cout << "\tnum_f not declared\n"; return 0;}
    std::cout << "\tnum_f = " << num_f << ";\n";

    x_min = reader.GetReal(section, "x_min", -1111111111);
    if(x_min == -1111111111){std::cout << "\tx_min not declared\n"; return 0;}
    std::cout << "\ttx_min = " << x_min << ";\n";

    x_max = reader.GetReal(section, "x_max", -1111111111);
    if(x_max == -1111111111){std::cout << "\tx_max not declared\n"; return 0;}
    std::cout << "\tx_max = " << x_max << ";\n";

    x_step = reader.GetReal(section, "x_step", -1111111111);
    if(x_step == -1111111111){std::cout << "\tx_step not declared\n"; return 0;}
    std::cout << "\tx_step = " << x_step << ";\n";

    y_min = reader.GetReal(section, "y_min", -1111111111);
    if(y_min == -1111111111){std::cout << "\ty_min not declared\n"; return 0;}
    std::cout << "\ty_min = " << y_min << ";\n";

    y_max = reader.GetReal(section, "y_max", -1111111111);
    if(y_max == -1111111111){std::cout << "\ty_max not declared\n"; return 0;}
    std::cout << "\ty_max = " << y_max << ";\n";

    y_step = reader.GetReal(section, "y_step", -1111111111);
    if(y_step == -1111111111){std::cout << "\ty_step not declared\n"; return 0;}
    std::cout << "\ty_step = " << y_step << ";\n";
    return 1;
} // -- END get_ini_params

