#pragma once
#include <opencv2/core/core.hpp>
#include "devices/device.hpp"
#include "devices/iframehandler.h"
#include "INIReader.h"
#include "tools/time_keeper/time_keeper.hpp"
#include "tools/colors.h"
#include "common_data.hpp"

#include <cstring>
#include <thread>
#include <functional>
#include <condition_variable>
#include <signal.h>
#include "mat2rtsp.hpp"

#ifdef USE_FOLDER_READER
#include "devices/FolderReader/folderreader.h"
#endif // USE_FOLDER_READER

#ifdef USE_VIDEO_READER
#include "devices/VideoReader/videoreader.h"
#endif //USE_VIDEO_READER

#ifdef USE_WEB_CAMERA
#include "devices/WebCamera/webcamera.hpp"
#endif //USE_WEB_CAMERA

#ifdef USE_COLIBRI_TV
#include "devices/colibritv/colibritv.h"
#endif //USE_COLIBRI_TV

#ifdef USE_RASPBERRY_HQ_CAMERA
#include "devices/RaspberryHQCamera/factory.hpp"
#endif // USE_RASPBERRY_HQ_CAMERA

#ifdef USE_IMX219_CAMERA_MIPI
#include "devices/IMX219CameraMIPI/imx219_camera_mipi_factory.hpp"
#endif // USE_IMX219_CAMERA_MIPI

#ifdef USE_IMX477_SQUARE_CAMERA_MIPI
#include "devices/imx477squareCameraMIPI/imx477_square_camera_mipi_factory.hpp"
#endif // USE_IMX477_SQUARE_CAMERA_MIPI

#ifdef USE_SHARED_MEMORY
#include "devices/SharedMemory/sharedmemory.hpp"
#endif // USE_SHARED_MEMORY

#ifdef USE_HIKVISION
#include "devices/EthCameraRtsp/rtsp_h265_device_factory.hpp"
#endif // USE_HIKVISION


#ifdef USE_NUC_CONTROL
#include "nuc_control.hpp"
#endif // USE_NUC_CONTROL

#ifdef  USE_I2C_DMP
#include "headers/mpu6050useDMP.hpp"
#endif // USE_I2C_DMP

#ifdef USE_DBG_PLOT
#include "tools/plot/plot.hpp"
#include "tools/graph/graph.hpp"
#endif // USE_DBG_PLOT

#ifdef USE_LOG_CMD
#include "tools/log_cmd/log_cmd.hpp"
#endif // USE_LOG_CMD

#include "RS232_worker.hpp"
#include "RS485_worker.hpp"
#include "CMD_over_UDP_receiver.hpp"
#include "rs485_struct.hpp"

#ifdef USE_LOGGER
#include "logger/factory.h"
#endif //USE_LOGGER

#include "tracshats/tracshats.hpp"

#include "tools/watchdog.h"
#include <iomanip>


#include "from_bort_struct.hpp"
#include "to_bort_struct.hpp"
#include "bool_uint_transformer.hpp"

struct TrackTarget
{
    int16_t x_offset = 0;
    int16_t y_offset = 0;
}; // END struct TrackTarget

typedef unsigned char uchar;

class Application : public IFrameHandler // обработка приема фреймов
{
public:
    Application(const std::string &pathToConfig, bool &ok);
    ~Application();
    void quit(); // TODO: сделать private, т.к. небезопасен при внешнем вызове
    void stop(); // поднимаем флаг _need_quit
    /*
    Вызывает quit только при завершении работы цикла в exec();
    (но с определенным лимитом по времени ожидания watchdog_wait_close_exec_time);
    Возвращает true в случае, если удалось дождаться закрытия exec за
    выделенное лимитированное время. quit() вызывается вне зависимости от успешности операции
    ожидания.
    */
    bool quit_async();
    void start();
    void exec();    // основная функция выполнения приложения
    std::shared_ptr<common_data> common_data_ptr = nullptr;
    bool signal_flag = false;
    bool powerOFF_flag = false;

 private:

    // перечень устройств-источников видеопотока
    enum Devices
    {
        FOLDER = 0,
        VIDEO = 1,
        WEBCAMERA = 2,
        COLIBRITV = 3,
        RASPBERRY_HQ_CAMERA = 4,
        IMX477_SQUARE_CAMERA_MIPI = 5,
        IMX219_CAMERA_MIPI = 6,
        RTSP_H265 = 7,
        SHARED_MEMORY = 8,
        HVGS_GRAY_USB_CAMERA = 9,
        GOEN_400_RAW = 10,
        IMX415_CAMERA_MIPI = 11,
        RTSP_SERVER = 12
    }; // -- END enum Devices

    cv::Mat frame_process_0; // двойная буфферизация

    std::chrono::system_clock::time_point time_point_old, time_point_new;
    std::chrono::duration<double> duration1;

    int frame_send_w = 1920;
    int frame_send_h = 1080;
    float frame_send_h_1 = 1.f / frame_send_h;
    int img_send_w = 0;
    float img_send_w_1 = 0;
    cv::Rect img2send_rct = {0,0,1920,1080};
    int frame_w = 1920;
    int frame_h = 1080;
    cv::Rect crop_center = cv::Rect(960, 540, 1920, 1080);
    float frame_w_1 = 1.f / frame_w;
    float frame_h_1 = 1.f / frame_h;
    int frame_w_2 = round(0.5 * frame_w);
    int frame_h_2 = round(0.5 * frame_h);
    int dev_fps = 0;
    std::atomic<bool> _execute = false;     // флаг нахождения программы в процессе выполнения
    std::atomic<bool> _exec_complete_success = false; // флаг успешного завершения метода exec(), исполняемого в отдельном потоке
    std::atomic<bool> _quit_async_complete = false; // флаг окончания выполнения метода quit_async()

    std::atomic<bool> quit_was_called = false; // устанавливается при первом вызове метода quit() данного класса
    std::atomic<bool> _quit_async_was_called = false; // флаг устанавливается при первом вызове метода quit_async();
    std::atomic<bool> frameReady = false; // флаг готовности фрейма
    std::mutex frame_proc_0_mutex, frame_proc_1_mutex;
    cv::Mat frame_receive, frame_process_1, frame_process_tracshats, frame_send, frame_send_bgr;                 // кадр для отправки на RTSPServer
    std::atomic<int> process_frame_id = 0; // номер (0,1) текущего обрабатываемого фрейма в главном цикле программы
    float handle_flag = 1;
    // флаг готовности кадра для обработки трекером

    bool isTracShatsFirstInitedFlag = false;           // флаг инициации трекера
    bool isTracShatsInitedFlag = false;                // флаг инициации трекера
    bool tracShatsInitReqFlag = false;
    cv::Rect2f searchRect;
    cv::Rect2f rectm;

    unsigned char key = 0;
    unsigned char key_quit_handler = '`' ;
    int cross_on = 0;
    unsigned char cross_on_color = 0;

    int zahvat_size = 24;
    cv::Point2i wh_2_zahvat = cv::Point(24, 24);
    int zahvat_size_show = 24;
    cv::Point2i wh_2_zahvat_show = cv::Point(24, 24);

    cv::Rect rct_center = cv::Rect(500, 600, 500, 500);
    float frame_h_w = 1.f;

    cv::Point2i wh_2_ext[256];

    // данные для работы в режиме ROI
    int scale = 0;
    cv::Rect2f roi = cv::Rect2f(0,0,0,0);       // текущий ROI
    cv::Rect2f aimRectShats = cv::Rect2f(0,0,0,0); // текущая рамка цели
    int video_num = 1;
    std::vector<std::string> fileList;

    // устройство считвания кадров
    std::shared_ptr<Device> device = nullptr;				// источник видеопотока

    std::shared_ptr<TracShats> tracShats = nullptr;  // трекер

    int device_id = 0; // в соответствии с enum Devices
    std::string config_path = "";

    // Временные точки для нахождения времени обработки кадра.
    std::chrono::system_clock::time_point time_point_device0, time_point_device1;
    // обработка изображения (трекинг)

    int max_objects = 10;
    bool flag_zahvat = 0;
    std::unique_ptr<mat2rtsp> mat2rtsp_sender = nullptr;  //Send video stream from Gstreamer to host

    std::unique_ptr<RS485_worker> rs485_worker_ptr = nullptr;
    std::unique_ptr<RS232_worker> rs232_worker_ptr = nullptr;
    std::unique_ptr<CmdOverUdpReceiver> cmd_udp_ptr = nullptr;


#ifdef USE_I2C_DMP
    std::shared_ptr<MPU6050UseDMP> mpu6050_dmp_ptr = nullptr;
    float abs_yaw = 0, abs_pitch = 0, abs_roll = 0; // значения абсолютных углов с датчика MPU6050
    float abs_vel_yaw = 0, abs_vel_pitch = 0, abs_vel_roll = 0; //
    void rotate_matrix(int mode, float t_b, float a_b, float k_b, float t_s, float a_s, float& t_res, float& a_res, float& k_res);
    void get_vel(float t, float a, float k, float &Vt, float &Va, float &Vk);
    cv::Point transform_vec(cv::Point src0, cv::Point src1, cv::Point src2, cv::Point dst0, cv::Point dst1, cv::Point dst2, cv::Point vec);
    const float eps = 1e-6;
    float x_res = 0, y_res = 0, z_res = 0;
    float t_res0 = 0, a_res0 = 0, k_res0 = 0;
    const cv::Point3f tangazh_s = cv::Point3f(0, 0, 1);
    const cv::Point3f az_s = cv::Point3f(0, -1, 0);
    const cv::Point3f tangazh_b = cv::Point3f(0, 0, 1);
    const cv::Point3f kren_b = cv::Point3f(-1, 0, 0);
    const cv::Point3f az_b = cv::Point3f(0, -1, 0);
    // ====================================== Velosity:
    std::chrono::steady_clock::time_point start_get_velocity = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point stop_get_velocity = start_get_velocity;
    std::chrono::duration<double> dur;
    int num_mnk = 6; // Число точек для МНК.
    struct St4Vel
    {
        double x = 0; // Время от начала программы.
        double xx = 0; // Время от начала программы в квадрате.
        double t = 0; // Тангаж.
        double xt = 0; // Тангаж на время от начала программы.
        double a = 0; // Азимут.
        double xa = 0; // Азимут на время от начала программы.
        double k = 0; // Крен.
        double xk = 0; // Крен на время от начала программы.
    }; // END struct St4Vel
    double Sx=0, Sxx=0, St=0, Sxt=0, Sa=0, Sxa=0, Sk=0, Sxk=0; // Суммы по элементам стуктуры.
    double Zn_1 = 0;
    St4Vel St4V;
    std::list<St4Vel> lSt4V;
    // ====================================== END Velosity



#endif // USE_I2C_DMP

#ifdef USE_LOG_CMD
    std::shared_ptr<LogCmd> log_cmd_ptr = nullptr;
#endif // USE_LOG_CMD


    /// RS232_structures
    FromBortCommand from_bort_cmd_rs232_str;
    ToBortTelemetry to_bort_tlm_rs232_str;
    static const int tlm_size = sizeof(ToBortTelemetry);
    uint8_t tlm_byte_buf[sizeof(ToBortTelemetry)];
    FromBortTelemetry from_bort_tlm_rs232_str; // TODO
    ReplayToCmdToBort replay_to_cmd_to_bort_rs232_str;
    uint8_t ssr_byte_buf[sizeof(replay_to_cmd_to_bort_rs232_str)] = {0xEE, 0x19, 0x00, 0x00, 0x00};
    StatusInformationFeedback1 sif1;
    StatusInformationFeedback2 sif2;
    StatusInformationFeedback3 sif3;
    SelfInspectionResult sir;
    std::shared_ptr<BoolUintTransformer> bit_ptr = nullptr;

    std::atomic<int16_t> X = 0; //for tracking - to RS485
    std::atomic<int16_t> Y = 0;

    int16_t roll = 0;
    int16_t pitch = 0;
    int16_t roll_speed = 0;
    int16_t pitch_speed = 0;

    std::atomic<int> receivedCMD = 0;
    int receivedCMD_show = 0;
    bool f_receivedCMD = false;
    std::string f_command_str = "0";

    int show_command_interval = 0;
    int frame_counter = 0;
    std::string item0_232 = "";
    std::string item1_232 = "";
    int rs232_speed = 0;
    int rs485_speed = 0;
    std::string SERIAL_PORT_RS232 = "";
    std::string SERIAL_PORT_RS485 = "";
    bool port_232_find = false;
    bool port_485_find = false;
    int send_CMD_BLOCK_OFF = -1;

    std::thread thread_rs232_485;
    std::thread check_press_button_power;
    std::thread thread_only_rs232;

    uint8_t buf[32];
    std::string array_uint8_to_string(uint8_t * request, int num);
    void show_command_RS232(cv::Mat& img);
    void check_press_powerOFF();
    void work_rs232_rs485();
    void work_only_rs232();
    void reset_goen();
    void get_status_goen();
    void get_version_goen();
    void set_mode_goen(uint8_t newMode);
    // void control_speed_goen(int16_t X, int16_t Y);
    void rs232_work(std::thread&);
    void rs485_work(std::thread&);
    void rs485_send_request_status(std::thread&);
    void copy_workRect();
    void get_system_result(const std::string& system_str, std::string& cmd_result);
    void init_ports(const string& config);
    void set_ports();
    void calibrateDrift();

    /// RS232_sender
    void set_from_goen_telemetry();
    void rs232_tlm_send_to_Board();
    bool send_single_status_return(uint8_t control_param);
    void exec_rcv_rs232_cmd();

    /// Блок обработки изображения перед отправкой клиенту
    bool f_ir_black_heat = 0;
    bool f_image_enchance = 0;

#ifdef USE_LOGGER // Логгирование время преобразования изображений и fps отправки изображений на RTSP сервер (он же fps полного цикла в exec())
    std::chrono::system_clock::time_point time_point_cv0, time_point_cv1, tp_full0, tp_full1, time_point_tracker0, time_point_tracker1;
#endif // USE_LOGGER
    double clip_limit = 1 ; //0.32 ; // ограничение значения контраста изображения
    cv::Size clahe_grid_size = {8,8}; // размер сетки разбиения изображения для адаптивной эквализации гистограммы
    cv::Ptr<cv::CLAHE> clahe_cv = cv::createCLAHE(clip_limit, clahe_grid_size); // CONTRAST LIMITED ADAPTIVE HISTOGRAMM EQUALIZATION
    void transparent_image();

    /// PTZ Telemetry
    float gestue_pitch_deg = 0;
    //    float gestue_roll_deg = 0;
    float gestue_azimuth_deg = 0;

    /// PTZ commands
    float digital_zoom_value = 1.f;
    float digital_zoom_value_1 = 1.f;

#ifdef USE_TPV_cam
    int zoom_index = 0;
    std::vector<float> v_zoom_value = { 1.0, 1.1, 1.4, 1.6, 2.0, 2.3, 2.8, 3.3, 4.0};
    float k_rel2field_x = 12 * 20.f;
    float k_rel2field_y = 9.6 * 20.f;

    float k_field_x2grad100 = 12 * 100.f;
    float k_field_y2grad100 = 9.6 * 100.f;

#ifdef USE_NUC_CONTROL
    std::shared_ptr<NucControl> nuc_control_ptr = nullptr;
    std::string serial_port_rstpv = "/dev/ttyUSB2";
#endif // USE_NUC_CONTROL
#endif // END USE_TPV_cam
#ifdef USE_TV
    int zoom_index = 1;
    std::vector<float> v_zoom_value = {0.5, 1.0, 1.1, 1.4, 1.6, 2.0, 2.3, 2.8, 3.3, 4.0};
    float k_rel2field_x = 6 * 20.f;
    float k_rel2field_y = 3.375 * 20.f;

    float k_field_x2grad100 = 6 * 100.f;
    float k_field_y2grad100 = 3.375 * 100.f;
#endif // END USE_TV
    cv::Rect rct_zoom = {0, 0, frame_w, frame_h};

    /// PLANE TELEMETRY
    double k_deg2rad = M_PI / 180.f;
    double k_rad2deg = 1.f / k_deg2rad;
    double k_deg2rad_100 = 0.01 * k_deg2rad;
    float ang_pitch_deg = 0.f;
    float ang_roll_deg = 0.f;
    float ang_heading_deg = 0.f;
    double k_sec2deg_100 = 1.f / 36;
    /// GIMBAL TELEMETRY
    float gimbal_pitch_deg = 0.f;
    float gimbal_azimuth_deg = 0.f;
    float sec2grad = 1.f / 3600.f;
    float Vx_dreyf0 = 0.f;
    float Vy_dreyf0 = 0.f;

    std::atomic<bool> f_need_send_tlm = {false};

#ifdef USE_UDP_TLM
    void set_extension_udp_tlm();
    Tlm4AVAX tlm_ap_str;
    float valid_ap_step_decrease = 0.01 ;
    float f_valid = 0;
    float f_attack = 0;
#endif // USE_UDP_TLM

#ifdef USE_DBG_PLOT
    std::chrono::system_clock::time_point tp_now, tp_startapp;
    float time = 0;
    uchar add6(float z_, float y_, float x_, float vz_, float vy_, float vx_, float t_, bool use_kalman = false); // -- END add6
    std::shared_ptr<Plot> plot_ptr = nullptr;
    std::shared_ptr<Graph> graph_ptr = nullptr;
#endif // USE_DBG_PLOT


#ifdef USE_GUI
    std::string app_win_name = "Application";
    // results demonstration
    cv::Mat frame_show;                 // кадр для показа на экране
    cv::Scalar clr_scaling = color::green;
    int frame_show_width = 640;         // ширина отображаемого кадра
    int demonstration_mode = 0;         // признак режима показа
    // данные для захвата цели мышью
    static bool click_down;
    static bool click_up;
    static cv::Point p_mouse;
    static cv::Point p_down;
    float show2originImgRatio = 1.f;
    float orig2show = 1.f/show2originImgRatio;
    int frame_show_height = 1;
    int frame_show_width_2, frame_show_height_2;
    float frame_show_w_1, frame_show_h_1;
    bool first_click_down = 1;

    float R = 50 ; // радиус окружности индикатора
    float thik_panel = 3;
    float thik_plane = 8;

    // колл-бэк метод обработки событий мыши
    static void aimFrameCallback(int event, int x, int y, int flags, void* param);
    // обработчик событий мыши (основной регулярный цикл)
    void frame_show_setup();
    bool mouseHandler(cv::Mat& img, cv::Rect2f& object_rect);
    // обработчик событий клавиатуры
    void keyHandler();
    void draw_frame_telemetry();
    void draw_plane_telemetry();
    void draw_trac();
#endif //USE_GUI

    bool processShats();
    void workflowShats();
    void next_frame_async();
    bool ethHandler(cv::Rect2f & object_rect);
    // загрузка рамки цели из файла
    bool loadRectFromFile(std::string);
    // подготовка изображения для tracShats
    inline void prepareFrameForTracShats();
    bool get_ini_params(const std::string& config);
    // обработчик события поступления кадра при приёме от устройства
    void handleDeviceFrame(uint8_t * f, int w, int h, int num, int id);
    bool FileIsExist(const std::string& filePath);
    bool DirContent(std::string& path, std::vector<std::string>& fileList);
    void work_TKDNN();
    void calc_to_goen_tlm_rs232_str_params();
    void handle_treatment(cv::Rect2f & rectm_rel);

    // Различные варианты реализации передачи навигационных данных ГОЭН-60
#ifdef USE_TLM_MODE_CHANGER
    int tm_xy_enable = 0; // 0 - disable, 1 - enable target_miss sending
    int abs_angle_enable = 1; // 0 - disable, 1 - complex abs angle absolute angles sending
    int angular_velocity_enable = 1; // 0 - disable; 1 - enable - angular velocity for yaw, pitch, roll
#endif // USE_TLM_MODE_CHANGER
    int ini_err_value = -1111111111;
    void set_target_miss();
    void set_absolute_navigation();
    void set_status_fields();
}; // -- END class Application
