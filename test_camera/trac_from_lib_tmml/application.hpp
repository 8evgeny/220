#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <opencv2/core/core.hpp>
#include "devices/device.hpp"
#include "devices/iframehandler.h"
#include "INIReader.h"
#include "tools/time_keeper/time_keeper.hpp"
#include "tools/colors.h"
#ifdef CV_GST_SEND
    #include "cv_gst_send.hpp"
#endif // END #ifdef CV_GST_SEND
#ifdef MAT2RTSP
#include "mat2rtsp.hpp"
#endif // END #ifdef MAT2RTSP
#include "tracshats/tracshats.hpp"

#include <cstring>
#include <thread>
#include <functional>
#include <condition_variable>
#include <signal.h>

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

#ifdef USE_IMX415_CAMERA_MIPI
#include "devices/imx415CameraMIPI/imx415_camera_mipi_factory.hpp"
#endif // USE_IMX415_CAMERA_MIPI

#ifdef USE_SHARED_MEMORY
#include "devices/SharedMemory/sharedmemory.hpp"
#endif // USE_SHARED_MEMORY

#ifdef USE_HIKVISION
   #include "devices/EthCameraRtsp/rtsp_h265_device_factory.hpp"
#endif // USE_HIKVISION

#ifdef USE_CORSAIR_400_RAW
	#include "devices/Corsair400raw/corsair400raw_factory.hpp"
#endif // USE_CORSAIR_400_RAW

#ifdef USE_CORSAIR_400_RAW
#include "modules/vtrack_data_sender/vtrack_data_sender_factory.hpp"
#include "modules/eth_controller_toolbox/command_processing/commands_zoo/cmd400/eth_cmd_400_init.hpp"
#include "modules/eth_controller_toolbox/command_processing/commands_zoo/cmd400/eth_cmd_400_deinit.hpp"
#include "modules/eth_controller_toolbox/command_processing/commands_zoo/cmd400/eth_cmd_400_record_on.hpp"
#include "modules/cmd400/cmd400_keeper_factory.hpp"
#endif // USE_CORSAIR_400_RAW

#ifdef USE_RTSP_SERVER
#include "rtsp_factory.hpp"
#endif // USE_RTSP_SERVER

#ifdef DBG_VIDEO_SENDER
#include "modules/fenix2meta/fenix2meta_struct.hpp"
#include "video_eth_controllers/gstc/rtp_server_x264/rtp_server.hpp"
#include "video_eth_controllers/gstc/rtp_server_x264/rtp_server_settings.hpp"
#endif // DBG_VIDEO_SENDER

#ifdef USE_LOGGER
#include "logger/factory.h"
#endif //USE_LOGGER

#ifdef USE_VERTELKA_CMD
#include "modules/cmd_vertelka/cmd_vertelka_keeper_factory.hpp"
#endif // USE_VERTELKA_CMD

#include "tools/watchdog.h"
#include <iomanip>
typedef unsigned char uchar;

class Application : public IFrameHandler // обработка приема фреймов
{
public:
	Application(const std::string &pathToConfig, bool &ok);
	~Application();
	void quit(); // TODO: сделать private, т.к. небезопасен при внешнем вызове
    void stop(); // поднимаем флаг need_quit
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
    int dev_fps = 0;
    static const unsigned char key_exit = '/';

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
        CORSAIR_400_RAW = 10,
        IMX415_CAMERA_MIPI = 11,
        RTSP = 12
    }; // -- END enum Devices

    cv::Mat frame_process_0; // двойная буфферизация

    int frame_w = 1920;
    int frame_h = 1080;
    float frame_w_1 = 1.f / frame_w;
    float frame_h_1 = 1.f / frame_h;

	std::atomic<bool> _execute = false;     // флаг нахождения программы в процессе выполнения
	std::atomic<bool> _exec_complete_success = false; // флаг успешного завершения метода exec(), исполняемого в отдельном потоке 
	std::atomic<bool> _quit_async_complete = false; // флаг окончания выполнения метода quit_async()

	std::atomic<bool> quit_was_called = false; // устанавливается при первом вызове метода quit() данного класса
	std::atomic<bool> _quit_async_was_called = false; // флаг устанавливается при первом вызове метода quit_async();
	
	std::atomic<bool> frameReady = false; // флаг готовности фрейма

	std::mutex frame_proc_0_mutex, frame_proc_1_mutex;
	
    cv::Mat frame_receive, frame_receive0;	
	cv::Mat frame_process_1;

    cv::Mat frame_process_tracshats;    // кадр для обработки трекером
	cv::Mat frame_show;                 // кадр для показа на экране
	
	std::atomic<int> process_frame_id = 0; // номер (0,1) текущего обрабатываемого фрейма в главном цикле программы
    float handle_flag = 1;
	// флаг готовности кадра для обработки трекером

    bool isTracShatsFirstInitedFlag = false;           // флаг инициации трекера
    bool isTracShatsInitedFlag = false;                // флаг инициации трекера
    bool tracShatsInitReqFlag = false;
    cv::Rect2f searchRect;
    cv::Rect2f rectm;

    unsigned char key = 0;
    int cross_on = 0;
    unsigned char cross_on_color = 0;

    // данные для захвата цели мышью
    int get_rect_manual = 1;       // признак разрешения ручного захвата цели
    static bool click_down;
    static bool click_up;
    static cv::Point p_mouse;
    static cv::Point p_down;
    int zahvat_size = 24;
    cv::Point2i wh_2_zahvat = cv::Point(24, 24);
    bool first_click_down = 1;

    cv::Rect rct_center = cv::Rect(500, 600, 500, 500);
    float show2originImgRatio = 1.f;
    float orig2show = 1.f/show2originImgRatio;
    int frame_show_height = 1;    
    int frame_show_width_2, frame_show_height_2;
    float frame_show_w_1, frame_show_h_1;
    float frame_h_w = 1.f;

    cv::Point2i wh_2_ext[256];

    // данные для работы в режиме ROI         
	int roi_mode = 0;				// признак работы в режиме ROI
    bool isRoiSetted = false;           // признак установки ROI
    int scaleROI = 5;                   // масштаб ROI относительно размера цели
    int scale = 0;
    cv::Scalar clr_scaling = color::green;
    cv::Rect2f roi = cv::Rect2f(0,0,0,0);       // текущий ROI
    cv::Rect2f aimRectShats = cv::Rect2f(0,0,0,0); // текущая рамка цели

    // results demonstration
    int frame_show_width = 640;         // ширина отображаемого кадра
    int demonstration_mode = 0;         // признак режима показа
    int video_on = 0;
    int rec_on = 0;
    int video_num = 1;
    std::string path4frames = "";
    int save_frames_skip = 0;
    int save_frames_prefix = 10000000;
    int max_frames = 0;
    std::vector<std::string> fileList;

	// устройство считвания кадров
	std::shared_ptr<Device> device = nullptr;				// источник видеопотока

    std::shared_ptr<TracShats> tracShats = nullptr;  // трекер

	int device_id = 0; // в соответствии с enum Devices

	std::string config_path;

	bool parallel = 0;
	bool need_quit = false;		// флаг запроса на выход из приложения, устанавливается методами 
	
 #ifdef GUI_OK
	std::string app_win_name = "Application";
 #endif // GUI_OK

	// Временные точки для нахождения времени обработки кадра.
	std::chrono::system_clock::time_point 
		time_point_tracker0, time_point_tracker1,
		time_point_device0, time_point_device1;
	// обработка изображения (трекинг)
	inline bool processShats();
	// рабочий цикл трекеров
	inline void workflowShats();


	#ifdef GUI_OK
	// колл-бэк метод обработки событий мыши
	static void aimFrameCallback(int event, int x, int y, int flags, void* param);
	#endif // GUI_OK
	
	#ifdef GUI_OK
	// обработчик событий мыши (основной регулярный цикл)
    bool mouseHandler(cv::Mat& img, cv::Rect2f& object_rect);
	#endif // GUI_OK
	
 #ifdef GUI_OK
	// обработчик событий клавиатуры
    void keyHandler();
 #endif //GUI_OK

    int max_objects = 10;
    bool flag_zahvat = 0;

#ifdef CV_GST_SEND
    std::unique_ptr<cv_gst_send> cv_gst_sender = nullptr;  //Send video stream from Gstreamer to host
#endif // CV_GST_SEND

#ifdef MAT2RTSP
    std::unique_ptr<mat2rtsp> mat2rtsp_sender = nullptr;  //Send video stream from Gstreamer to host
#endif // MAT2RTSP

#ifdef DBG_VIDEO_SENDER
    std::shared_ptr<rtp::RtpServer> fenix2frame_sender_ptr = nullptr;
    rtp::server::SettingsPtr settings_srv_ptr = nullptr;
    std::string rtp_server_section = "gstc_rtp_server_h264";
    int frame_send_w = 1920;
    int frame_send_h = 1080;
    Fenix2Meta fenix2meta_data2send;

#endif // DBG_VIDEO_SENDER

#ifdef USE_CORSAIR_400_RAW
    std::shared_ptr<track_pos_sender::vtrackDataSender> sender_ptr = nullptr;
    track_pos_sender::vtrackDataStruct track_pos_data2send;
    track_pos_sender::StateCmdStruct app_state;
    track_pos_sender::trackPos tr_pos;


    bool ethHandler(cv::Rect2f & object_rect);
    cv::Rect rect_init_eth = {-1,-1,-1,-1};

    ///cmd400_keeper
    std::shared_ptr<cmd400_keeper::Cmd400Keeper> cmd_keeper_ptr = nullptr;
    cmd400_keeper::Cmd400_list cmd_list2keep;
    cmd400_keeper::Settings settings_keeper;
#endif // USE_CORSAIR_400_RAW
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
}; // -- END class Application
#endif // APPLICATION_H
