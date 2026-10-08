#include "application.hpp"

using namespace std;
using namespace cv;


void printMotorStatus(MotorStatus & ms, int i) // DBG::
{
    cout << "[MotorStatus " << i << "]:\n\tmode=0x" << uppercase << hex <<  (int)ms.mode << dec << endl;
    cout << "\tmotorErrors: ";
    if(ms.errors & 0b0000'0001) {cout << " |HIGH CURRENT| ";}
    if(ms.errors & 0b0000'0010) {cout << " |HEATING| "; }
    if(ms.errors & 0b0000'0100) {cout << " |HIGHT SPEED| ";}
    if(ms.errors & 0b0000'1000) {cout << " |LOW MAGNETIC FIELD| ";}
    if(ms.errors & 0b0001'0000) {cout << " |LOW VOLTAGE| ";}
    cout << endl;
    cout << "\tmotorFlags: ";
    if(ms.flags & 0b0000'0001) {cout << " |ERROR| ";}
    if(ms.flags & 0b0000'0010) {cout << " |LIMITATION SECTOR| ";}
    if(ms.flags & 0b0000'0100) {cout << " |CALIBRATION IN PROCESS| ";}
    if(ms.flags & 0b0000'1000) {cout << " |UNSAVED DATA| ";}
    cout << endl;
    cout << "\tmotorAngle=" << ms.angle << endl;
    cout << "\tmotorSpeed=" << ms.speed << endl;
} // -- END printMotorStatus

void printFromGoenTelemetry(FromGoenTelemetry & str) // DBG::
{
    cout << "\n[FromGoenTelemetry]:/n" <<
        "\tmode=0x" << uppercase << hex << (int)str.mode << endl <<
        "\tmems_mode=0x" << (int)str.mode_mems;
    if(str.mode_mems == 0xC0) {cout << " | DUMMY | ";}
    if(str.mode_mems == 0xC1) {cout << " | STABILIZATION | ";}
    if(str.mode_mems == 0xC2) {cout << " | ROTATY PLATFORM | ";}
    if(str.mode_mems == 0xC3) {cout << " | PARKIN | ";}
    if(str.mode_mems == 0xC4) {cout << " | OFF | ";}
    cout << dec << endl;
    "\tmems_errors=";
    if(str.mems_errors) {cout << "ERROR" << endl;} else {cout << "OK" << endl;}
    if(str.mems_flag) {cout << "\tmems_flag=SOME FLAG" << endl;} else {cout << "\tmems_flag=NO FLAG" << endl;}
    cout << "\tAxis switch mode=0x" << uppercase << hex <<(int)str.axis_switch_mode << dec << endl;
    cout << "\tMemsProcessMode=0x" << uppercase << hex << (int)str.mems_process_mode << dec << endl;
    cout << dec;
    cout << "\tSpeed(A/P)=[ " << str.speed_yaw / 3600.f << ", " << str.speed_pitch / 3600.f << "]\n";
    cout << "\tAngle(A/P)=[ " << str.angle_yaw / 3600.f << ", " << str.angle_pitch / 3600.f << "]\n";
    printMotorStatus(str.motor_yaw, 1);
    printMotorStatus(str.motor_pitch, 2);
    cout << dec << endl;
} // -- END printFromGoenTelemetry

#ifdef USE_GUI
bool Application::click_down = 0;
bool Application::click_up = 0;
Point Application::p_mouse(0, 0);
Point Application::p_down(0, 0);

std::string format_num(uint8_t num) // преобразование числа в двухзначное для отображение даты и времени в режим USE_GUI
{
    std::string num_str = "";
    if(num < 10) {return "0" + to_string((int)num);}
    else {return to_string((int)num);}
} // END format_num

/// Отклика флагов на события мыши
void Application::aimFrameCallback(int event, int x, int y, int flags, void *param)
{
    Point *p = reinterpret_cast<Point*>(param);
    p->x = x;
    p->y = y;
    p_mouse = Point(x, y);
    switch(event)
    {
    case EVENT_LBUTTONDOWN:
        click_down = 1;
        break;
    case EVENT_LBUTTONUP:
        if(click_down){click_up = 1;}
        break;
    } // -- END switch(event)
}

void Application::frame_show_setup()
{
    show2originImgRatio = (float)frame_show_width / frame_w;
    orig2show = 1.f / show2originImgRatio;
    zahvat_size_show = round(zahvat_size * show2originImgRatio);
    wh_2_zahvat_show = Point2i(zahvat_size_show, zahvat_size_show);

    frame_h_w = (float)frame_h/frame_w;
    frame_show_height = round(frame_h_w * frame_show_width);
    frame_show_width_2 = 0.5 * frame_show_width;
    frame_show_height_2 = 0.5 * frame_show_height;
    frame_show_w_1 = 1.f / frame_show_width;
    frame_show_h_1 = 1.f / frame_show_height;
    rct_center.height = round(frame_h_w * rct_center.width);
} // -- END aimFrameCallback

void Application::show_command_RS232(Mat &img)
{
    if(receivedCMD_show)
    {
        f_receivedCMD = true;
        f_command_str = to_string(receivedCMD_show) + ":";
        switch(receivedCMD_show)
        {
        case (int)COMMAND_RS232::notCMD:
        {
            break;
        } // END case (int)COMMAND_RS232::notCMD:

        case (int)COMMAND_RS232::PTZ:
        {
            f_command_str += "PTZ: ";
            if(from_bort_cmd_rs232_str.parameter_x > 0) {f_command_str += "RIGHT ";}
            if(from_bort_cmd_rs232_str.parameter_x < 0) {f_command_str += "LEFT ";}
            if(from_bort_cmd_rs232_str.parameter_y > 0) {f_command_str += "UP ";}
            if(from_bort_cmd_rs232_str.parameter_y < 0) {f_command_str += "DOWN ";}
            if(!from_bort_cmd_rs232_str.parameter_x && !from_bort_cmd_rs232_str.parameter_y) {f_command_str += "STOP ";}
            break;
        } // END case (int)COMMAND_RS232::PTZ:

        case (int)COMMAND_RS232::INFRA:
        {
            f_command_str += "IR CHANNEL";
            break;
        } // END case (int)COMMAND_RS232::INFRA:

        case (int)COMMAND_RS232::TV:
        {
            f_command_str += "TV CHANNEL";
            break;
        } // END case (int)COMMAND_RS232::TV:

        case (int)COMMAND_RS232::ZOOM:
        {
            cout << "Application::switch(receiveRS232_cmd):: ZOOM CMD! Change zoom for " << (int)from_bort_cmd_rs232_str.zoom_rate << endl;
            break;
        } // END case (int)COMMAND_RS232::ZOOM:

        case (int)COMMAND_RS232::TO_ZERO_POSITION:
        {
            f_command_str += "TO_ZERO_POSITION";
            break;
        } // END case (int)COMMAND_RS232::TO_ZERO_POSITION:

        case (int)COMMAND_RS232::TRACKING_START:
        {
            f_command_str += "TRACKING_START";
            break;
        } // END case (int)COMMAND_RS232::TRACKING_START:

        case (int)COMMAND_RS232::TRACKING_STOP:
        {
            f_command_str += "TRACKING_STOP";
            break;
        } // END case (int)COMMAND_RS232::TRACKING_STOP:

        case (int)COMMAND_RS232::MOTOR_ON:
        {
            f_command_str += "MOTOR_ON";
            break;
        } // END case (int)COMMAND_RS232::MOTOR_ON:

        case (int)COMMAND_RS232::MOTOR_OFF:
        {
            f_command_str += "MOTOR_OFF";
            break;
        } // END case (int)COMMAND_RS232::MOTOR_OFF:

        case (int)COMMAND_RS232::AZIMUTH_FOLLOW:
        {
            f_command_str += "AZIMUTH_FOLLOW";
            break;
        } // END case (int)COMMAND_RS232::AZIMUTH_FOLLOW:

        case (int)COMMAND_RS232::CLOSE_FOLLOW:
        {
            f_command_str += "CLOSE_FOLLOW";
            break;
        } // END case (int)COMMAND_RS232::CLOSE_FOLLOW:

        case (int)COMMAND_RS232::ELECTRIC_LOCK_ON:
        {
            f_command_str += "ELECTRIC_LOCK_ON";
            break;
        } // END case (int)COMMAND_RS232::ELECTRIC_LOCK_ON:

        case (int)COMMAND_RS232::ELECTRIC_LOCK_OFF:
        {
            f_command_str += "ELECTRIC_LOCK_OFF";
            break;
        } // END case (int)COMMAND_RS232::ELECTRIC_LOCK_OFF:

        case (int)COMMAND_RS232::SET_ZERO_POSITION:
        {
            f_command_str += "SET_ZERO_POSITION";
            break;
        } // END case (int)COMMAND_RS232::SET_ZERO_POSITION:

        case (int)COMMAND_RS232::SET_AZIMUT_PITCH:
        {
            f_command_str += "SET_AZIMUT_PITCH";
            break;
        } // END case (int)COMMAND_RS232::SET_AZIMUT_PITCH:

        case (int)COMMAND_RS232::TRAC_SIZE_CHANGE:
        {
            f_command_str += "TRAC_SIZE_CHANGE";
            break;
        } // END case (int)COMMAND_RS232::TRAC_SIZE_CHANGE:

        default:
        {
            f_command_str += "UNKNOWN CMD";
            break;
        } // END default

        } // END switch(rs232_cmd_type)
        receivedCMD_show = 0;
    } // END if(receivedCMD_show)


    if(f_receivedCMD)
    {
        frame_counter++;
        if(frame_counter < show_command_interval)
        {
            putText(frame_show, f_command_str, Point(100, 700), 3, 3, color::green, thik_panel);
        } // END if(frame_counter < show_command_interval)
        else
        {
            f_receivedCMD = 0;
            frame_counter = 0;
            f_command_str = "0";
        } // END if(!frame_counter < show_command_interval)
        } // END if(command232_flag)
    } // END void show_command_RS232(Mat &img)

bool Application::mouseHandler(Mat& img, Rect2f& object_rect)
{
    if(handle_flag)
    {
#if !defined(TKDNN)
        if(!isTracShatsInitedFlag)
        {
            if(click_down)
            {
                if(zahvat_size)
                {
                    if(first_click_down)
                    {
                        first_click_down = 0;
                        p_down = p_mouse;
                    } // -- END if(first_click_down)
                    Rect rct(p_mouse - wh_2_zahvat_show, p_mouse + wh_2_zahvat_show);
                    rectangle(img, rct, color::white, 1);
                } // END if(zahvat_size)
                else
                {
                    if(first_click_down)
                    {
                        first_click_down = 0;
                        p_down = p_mouse;
                    } // -- END if(first_click_down)
                    int left  = MIN(p_mouse.x, p_down.x);
                    int right = MAX(p_mouse.x, p_down.x);
                    int top   = MIN(p_mouse.y, p_down.y);
                    int down  = MAX(p_mouse.y, p_down.y);
                    Rect rct(Point(left, top), Point(right, down));
                    rectangle(img, rct, color::white, 1);
                } // END if(!zahvat_size)
                } //--END if(click_down)
            } // -- END if(!isTracShatsInitedFlag)
#endif // END #if !defined(TKDNN)
    } // END if(handle_flag)
    draw_trac();
    if(rs485_worker_ptr != nullptr && rs232_worker_ptr != nullptr)
    {
        draw_frame_telemetry(); // отрисовка положения шара (приходит от 60го)
        show_command_RS232(frame_show);
        //    draw_plane_telemetry(); // отрисовка телеметрии борта (приходит от АП)
    }//END if(rs485_worker_ptr != nullptr && rs232_worker_ptr != nullptr)
    imshow(app_win_name, img);
#ifdef USE_DBG_PLOT
    graph_ptr->show();
    plot_ptr->show();
#endif // USE_DBG_PLOT
    //moveWindow(win_name, 10, 10);
    key = waitKey(1);

    if(handle_flag)
    {
        keyHandler();
        device->keyHandler(key);
        if((key == key_quit_handler) || signal_flag)
        {
#if defined USE_I2C
            sensors_ptr->need_quit = true;
#endif //END #if defined USE_I2C
            common_data_ptr->set_need_quit(true);
            this_thread::sleep_for(10ms);
            return 0;
        }//END if((key == key_quit_handler) || signal_flag)

#if !defined(TKDNN)
        wh_2_zahvat += wh_2_ext[key];
        zahvat_size = wh_2_zahvat.x;
        frame_show_setup();
        if(!isTracShatsInitedFlag)
        {
            setMouseCallback(app_win_name, aimFrameCallback, &p_mouse);
            if(click_up)
            {
                if(zahvat_size)
                {
                    click_up = 0;
                    click_down = 0;
                    first_click_down = 1;
                    object_rect = Rect(p_mouse - wh_2_zahvat_show, p_mouse + wh_2_zahvat_show);
                    return true;
                } // END if(zahvat_size)
                else
                {
                    click_up = 0;
                    int left  = MIN(p_mouse.x, p_down.x);
                    int right = MAX(p_mouse.x, p_down.x);
                    int top   = MIN(p_mouse.y, p_down.y);
                    int down  = MAX(p_mouse.y, p_down.y);
                    Rect rct(Point(left, top), Point(right, down));
                    if(rct.area() > 0)
                    {
                        click_down = 0;
                        first_click_down = 1;
                        object_rect = rct;
                        return true;
                    } // -- END if(rct.area()>0)
                    } // END if(!zahvat_size)
                } //--END if(click_up)
            } // -- END if(!isTracShatsInitedFlag)
        else
        {
            click_up = 0;
            click_down = 0;
            return 0;
        } // END if(isTracShatsInitedFlag)
#endif // END  #if !defined(TKDNN)
    } // END if(handle_flag)
    return false;
} // -- END mouseHandler


void Application::keyHandler()
{

    if(key != 255) // default return of waitKey()
    {
        cout << "key = " << (int)key << endl;
    } // END if(key != 255)
    if(key == 27) // деинциализация трекера по клавише ESC
    {
        if(tracShats->isInited()) { tracShats->deinit();}
    } // END  if((int)key == 27)

    if(key == key_quit_handler)
    {
        cout << "Exit on keypress" << endl;
    } // END if(key == key_quit_handler)
    } // -- END keyHandler

void Application::draw_trac()
{
    if(demonstration_mode > 2 && tracShats->isInited())
    {
#ifdef USE_CUDA
        frame_show(Rect(Point(80, 90) - Point(7, 7), Point(80, 90) + Point(7, 7))) = color::black;
        frame_show(Rect(Point(80, 90) - Point(3, 3), Point(80, 90) + Point(3, 3))) = color::white;
#endif // END ifdef USE_CUDA
#ifdef USE_CPU
        circle(frame_show, Point(80, 90), 7, color::black, -1);
        circle(frame_show, Point(80, 90), 3, color::white, -1);
#endif // END ifdef USE_CPU
#ifdef USE_CL
        frame_show(Rect(Point(80, 90) - Point(7, 7), Point(80, 90) + Point(7, 7))) = color::white;
        frame_show(Rect(Point(80, 90) - Point(3, 3), Point(80, 90) + Point(3, 3))) = color::black;
#endif // END ifdef USE_CL
    } // -- END if(demonstration_mode > 2 && tracShats->isInited())

    // flag_zahvat = processShats(); // обработка кадра
    if(flag_zahvat)
    {
        // приведение рамки цели к абсолютным координатам
        Rect2f rectShats(aimRectShats.x * frame_show_width, aimRectShats.y * frame_show_height,
                         aimRectShats.width * frame_show_width, aimRectShats.height * frame_show_height);
        // отрисовка рамки цели
        rectangle(frame_show, rectShats, color::white, 1);
        // отрисовка центра объекта
        Point2f dimless_cent(tracShats->trac_str.obj_xy_x, tracShats->trac_str.obj_xy_y);
        Point2f center_show = Point2f(dimless_cent.x * frame_show_width, dimless_cent.y * frame_show_height);
        circle(frame_show, center_show, 2, color::white, -1);

        // добавление метки размера объекта на кадр
        if(demonstration_mode > 1)
        {
            tracShats->getSearchRect(searchRect);
            searchRect = Rect2f(searchRect.x * frame_show_width, searchRect.y * frame_show_height,
                                searchRect.width * frame_show_width, searchRect.height * frame_show_height);
            rectangle(frame_show, searchRect, color::white, 1);
            if(demonstration_mode > 3)
            {
                Point objSize(round(aimRectShats.width * frame_receive.cols),
                              round(aimRectShats.height * frame_receive.rows));
                string txt = "Object size=[" + to_string(objSize.x) + ", " + to_string(objSize.y) + "]";
                putText(frame_show, txt, Point(100, 100), FONT_ITALIC, 1, color::Green::lime, 2);
            } // -- END if(demonstration_mode > 3)
            } // -- END if(demonstration_mode > 1)
        } // END if(flag_zahvat)
    return;
} // -- END draw_trac

void Application::draw_frame_telemetry()
{
    // rs485_worker_ptr->mut_telemetry_str.lock();
    // rs485_worker_ptr->mut_telemetry_str.unlock();

    Rect rct_zoom_show(rct_zoom.x * frame_w_1 * frame_show.cols,
                       rct_zoom.y * frame_h_1 * frame_show.rows,
                       rct_zoom.width * frame_w_1 * frame_show.cols,
                       rct_zoom.height * frame_h_1 * frame_show.rows);
    rectangle(frame_show, rct_zoom_show, color::red, 3);

    //    if(gimbal_azimuth_deg > 180.0) {gimbal_azimuth_deg = 179.9;}
    //    if(gimbal_azimuth_deg < -180.0) {gimbal_azimuth_deg = -179.9;}
    //    if(gimbal_pitch_deg > 59.9) {gimbal_pitch_deg = 59.9;}
    //    if(gimbal_pitch_deg < -59.9) {gimbal_pitch_deg = -59.9;}
    double frame_pitch_rad = gimbal_pitch_deg * k_deg2rad;
    double frame_azimuth_rad = gimbal_azimuth_deg * k_deg2rad;

    /// Отрисовка направление взгляда шара по вертикали
    Point2f p0_frame_pitch = Point2f(frame_show.cols - 6 * R, frame_show.rows - 3 * R);
    Point2f p1_frame_pitch = Point2f(R * cos(frame_pitch_rad) , R * sin(frame_pitch_rad));

    circle(frame_show, p0_frame_pitch, R, color::red, thik_panel);
    rectangle(frame_show, Rect2f(p0_frame_pitch - Point2f(20, R + 10), p0_frame_pitch - Point2f(-20, R - 10)), color::blue, -1);
    line(frame_show, p0_frame_pitch + Point2f(-R,0), p0_frame_pitch + Point2f(R,0), color::red, thik_panel);

    ellipse(frame_show, p0_frame_pitch - 0.8 * p1_frame_pitch, Size(round(0.2 * R),round(0.4 * R)), gimbal_pitch_deg, 0, 360, color::red, thik_panel);
    line(frame_show, p0_frame_pitch - p1_frame_pitch, p0_frame_pitch, color::blue, thik_panel);
    putText(frame_show, "pitch", p0_frame_pitch - Point2f(R, R + 10), 3, 1, color::red, thik_panel);
    putText(frame_show, to_string((int)(gimbal_pitch_deg)) + "." + to_string((int)abs(gimbal_pitch_deg * 10) % 10) + to_string((int)abs(gimbal_pitch_deg * 100) % 10), p0_frame_pitch + Point2f(-R, R + 30), 3, 1, color::red, thik_panel);

    /// Отрисовка направление взгляда шара по горизонтали
    // heading - угол курса 0 до 36000 (0 до 360 градусов с шагом 0,01)
    Point2f p0_frame_azimuth(frame_show.cols - 3 * R, frame_show.rows - 3 * R);
    Point2f p1_frame_azimuth =   Point2f(R * cos(frame_azimuth_rad - M_PI_2) , R * sin(frame_azimuth_rad - M_PI_2));
    line(frame_show, p0_frame_azimuth + Point2f(0,-R), p0_frame_azimuth + Point2f(0,R), color::red, thik_panel);
    circle(frame_show, p0_frame_azimuth, R, color::red, thik_panel);
    rectangle(frame_show, Rect2f(p0_frame_azimuth + Point2f(-20, -20), p0_frame_azimuth + Point2f(20, 20)), color::blue, thik_panel);

    ellipse(frame_show, p0_frame_azimuth + 0.8 * p1_frame_azimuth, Size(round(0.2 * R),round(0.4 * R)), gimbal_azimuth_deg + 90, 0, 360, color::red, thik_panel);
    line(frame_show, p0_frame_azimuth + p1_frame_azimuth, p0_frame_azimuth, color::blue, thik_panel);
    putText(frame_show, "azimuth", p0_frame_azimuth - Point2f(R, R + 40), 3, 1, color::red, thik_panel);
    putText(frame_show, to_string((int)(gimbal_azimuth_deg)) + "." + to_string((int)abs(gimbal_azimuth_deg * 10) % 10) + to_string((int)abs(gimbal_azimuth_deg * 100) % 10), p0_frame_azimuth + Point2f(-R, R + 30), 3, 1, color::red, thik_panel);

} // END draw_frame_telemetry

void Application::draw_plane_telemetry()
{
    // Отрисовка телеметрии
    double ang_pitch_rad = ang_pitch_deg * k_deg2rad;
    double ang_roll_rad  = ang_roll_deg  * k_deg2rad;
    double ang_heading_rad = ang_heading_deg * k_deg2rad;
    // pitch - тангаж       -9000 до +9000      (-90 до +90 градусов с шагом 0,01) (наклон по курсу)
    Point2f p0_pitch(frame_show.cols - 9 * R, frame_show.rows - 2 * R);
    Point2f p1_pitch =   Point2f(R * cos(ang_pitch_rad) , R * sin(ang_pitch_rad));
    Point2f p2_pitch =   Point2f(0.3 * R * cos(ang_pitch_rad  - M_PI_2) , 0.3 * R * sin(ang_pitch_rad - M_PI_2));
    line(frame_show, p0_pitch + Point2f(-R,0), p0_pitch + Point2f(R,0), color::red, thik_panel);
    circle(frame_show, p0_pitch, R, color::red, 3);
    line(frame_show, p0_pitch + p1_pitch, p0_pitch - p1_pitch, color::blue, thik_plane);
    line(frame_show, p0_pitch + p1_pitch + p2_pitch, p0_pitch + p1_pitch, color::blue, thik_panel);
    putText(frame_show, "pitch", p0_pitch - Point2f(R, R + 10), 3, 1, color::red, thik_panel);
    putText(frame_show, to_string((int)ang_pitch_deg), p0_pitch + Point2f(-R, R + 30), 3, 1, color::red, thik_panel);
    putText(frame_show, to_string((int)(ang_pitch_deg)) + "." + to_string((int)abs(ang_pitch_deg * 10) % 10), p0_pitch + Point2f(-R, R + 30), 3, 1, color::red, thik_panel);

    // roll - крен          -18000 до +18000    (-90 до +90 градусов с шагом 0,01) (наклон по оси движения)
    Point2f p0_roll(frame_show.cols - 6 * R, frame_show.rows - 2 * R);
    Point2f p1_roll =   Point2f(R * cos(ang_roll_rad) , R * sin(ang_roll_rad));
    Point2f p2_roll =   Point2f(0.3 * R * cos(ang_roll_rad  - M_PI_2) , 0.3 * R * sin(ang_roll_rad - M_PI_2));

    line(frame_show, p0_roll + Point2f(-R,0), p0_roll + Point2f(R,0), color::red, thik_panel);
    circle(frame_show, p0_roll, R, color::red, thik_panel);
    circle(frame_show, p0_roll, thik_plane, color::blue, -1);
    line(frame_show, p0_roll + p1_roll, p0_roll - p1_roll, color::blue, thik_panel);
    line(frame_show, p0_roll + p2_roll, p0_roll, color::blue, thik_panel);

    putText(frame_show, "roll", p0_roll - Point2f(R, R + 10), 3, 1, color::red, thik_panel);
    putText(frame_show, to_string((int)ang_roll_deg), p0_roll + Point2f(-R, R + 30), 3, 1, color::red, thik_panel);
    putText(frame_show, to_string((int)(ang_roll_deg)) + "." + to_string((int)abs(ang_roll_deg * 10) % 10), p0_roll + Point2f(-R, R + 30), 3, 1, color::red, thik_panel);

    // heading - угол курса 0 до 36000 (0 до 360 градусов с шагом 0,01)
    Point2f p0_heading(frame_show.cols - 3 * R, frame_show.rows - 2 * R);
    Point2f p1_heading =   Point2f(R * cos(ang_heading_rad - M_PI_2) , R * sin(ang_heading_rad - M_PI_2));
    Point2f p2_heading =   Point2f(0.6 * R * cos(ang_heading_rad) , 0.6 * R * sin(ang_heading_rad));
    Point2f p3_heading =   Point2f(0.2 * R * cos(ang_heading_rad) , 0.2 * R * sin(ang_heading_rad));
    line(frame_show, p0_heading + Point2f(0,-R), p0_heading + Point2f(0,R), color::red, thik_panel);
    circle(frame_show, p0_heading, R, color::red, thik_panel);
    line(frame_show, p0_heading + p1_heading, p0_heading - p1_heading, color::blue, thik_plane);
    line(frame_show, p0_heading + p2_heading, p0_heading - p2_heading, color::blue, thik_panel);
    line(frame_show, p0_heading - p1_heading + p3_heading, p0_heading - p1_heading - p3_heading, color::blue, thik_panel);
    putText(frame_show, "head", p0_heading - Point2f(R, R + 10), 3, 1, color::red, thik_panel);
    putText(frame_show, to_string((int)(ang_heading_deg)) + "." + to_string((int)(ang_heading_deg * 10) % 10), p0_heading + Point2f(-R, R + 30), 3, 1, color::red, thik_panel);

    string data_and_time = format_num(from_bort_tlm_rs232_str.day) + ":" + format_num(from_bort_tlm_rs232_str.month) +":" + format_num(from_bort_tlm_rs232_str.year) + "  " +
                           format_num(from_bort_tlm_rs232_str.hour + 3) + ":" + format_num(from_bort_tlm_rs232_str.minute) +":" + format_num(from_bort_tlm_rs232_str.second) + "." + format_num(from_bort_tlm_rs232_str.centisecond);
    putText(frame_show, data_and_time, Point(10, frame_show.rows - 50), 1, 3, Scalar(0,0,0), 3);
} // END draw_plane_telemetry
#endif // USE_GUI

