#ifndef UDP_RAW_GRAY_VIDEO_SERVER_HPP
#define UDP_RAW_GRAY_VIDEO_SERVER_HPP

#include <string>
#include <atomic>
#include <cstdint>

#include "video_eth_controllers/tools/ts_pipe_frame_buffer/ts_pipe_frame_buf.hpp"
#include <netinet/in.h>
#include <chrono>

// асинхронная отправка несжатого gray видео
namespace udp_raw_server_gray
{
    struct Settings
    {
        std::string client_ip_v4 = "127.0.0.1";
        int client_port = 8080;

        int frame_width = 1920;
        int frame_height = 1080;

        uint32_t cross_frame_interval_mcs = 1'000; // mcs - микросекунды
        uint32_t cross_frame_interval_check_buffer_empty_mcs = 100; // интервал проверки наполненности буфера
        uint32_t cross_package_interval_mcs = 10; // mcs - микросекунды


        /*
            Определяется через MTU текущего адаптера и количество отправляемых строк изображения
            за один пакет за вычетом байтов служебной информации (например, 
            если MTU=1500, то максимальная длина пакета 1432, следовательно, 
            т.к. отправка ведется построчно, то максимальная ширина изображения составляет 1432)
        */
        int max_udp_pack_size = 1432;

        int count_rows_in_pack = 11; // Количество строк в одном UDP пакете


        /*
            Максимальная кадровая частота поступления новых фреймов в pipebuf 
            вызовом метода send_frame(const cv::Mat &frame). 
            Если частота запросов send_frame превышает установленное значение параметра,
            то производится выход из функции 
        */
        int max_fps_push_buffer_frames = 11; 
        int pipebuf_total_capacity = 3; // объем фрейм-буффера на отправку
    };

    enum CallLaunchReport : uint8_t
    {
        LAUNCH_FAILED,                              // ошибка запуска
        LAUNCH_PROCESSES_CURRENTLY,                 // в настоящий момент производится запуск сервера    
        LAUNCH_SUCCESS                              // запуск успешно завершен в результате текущего вызова или более раннего
    };
    
    enum CallSendFrameReport : uint8_t
    {
        SEND_FRAME_ERROR,                           // ошибка отправки фрейма
        SEND_FRAME_SKIP_MAX_REQUEST_RATE_EXCEED,    // отправка фрейма пропущена, т.к. превышена максимальная частота запросов на отправку (max_fps_push_buffer_frames)
        SEND_FRAME_SUCCESS_PUSH_FRAME_QUEUE_TAIL,   // добавление в конец очереди, буфер не переполнен
        SEND_FRAME_SUCCESS_OVERWRITE_QUEUE_HEAD,    // буфер переполнен: поступления изображений чаще отправки
    };

    enum CallQuitReport : uint8_t
    {
        QUIT_FAILED,                                // ошибка выхода (например, превышено допустимое время ожидания)
        QUIT_PROCESS_CURRENTLY,                     // в настоящий момент происходит выход
        QUIT_SUCCESS                                // успешно произведен выход
    };

    enum LaunchFailType : uint8_t // LFT - Launch Fail Type
    {
        LFT_NONE,
        LFT_ERROR_LAUNCH_PIPEBUF,
        LFT_ERROR_OPEN_SOCKET
    };

    enum WorkflowBreakType : uint8_t // WBT - Workflow Break Type
    {
        WBT_NONE,
        WBT_CALL_QUIT, // остановка исполнения основного цикла в результате внешнего вызова функции quit
        WBT_ETH_CONNECTION_ERROR // остановка вследствие ошибки eth соединения
    };

    enum QuitFailType : uint8_t // QFT - Quit Fail Type
    {
        QFT_NONE, 
        QFT_WATCHDOG_TIME_IS_OVER // время ожидания завершения исполнения основного цикла вышло
    };

    enum FSMState : uint8_t
    {
        STATE_PRIMARY,          // первичное состояние после исполнения конструктора
        STATE_LAUNCH_SUCCESS,   // Успешно аллоцирован pipebuf, открыт UDP-сокет и запущен цикл отправки видео. В настоящее время Server в рабочем режиме эксплуатации
        STATE_LAUNCH_FAILED,    // Неуспешный запуск Server'а
        STATE_LAUNCH_PROCESS, // В настоящее время производится запуск 
        STATE_WORKFLOW_BREAK,   // Прервано исполнение пример, в случае обрыва Eth-соединения
        STATE_QUIT_SUCCESS,     // Успешно произведено закрытие Server'а
        STATE_QUIT_FAILED,      // Неуспешно закрыт Server (например, time_is_over ожидания корректного закрытия основного цикла)
        STATE_QUIT_PROCESS    // В настоящее время в Server запущен процесс закрытия 
    };

    class UDP_RAWGray_Server
    {
        public:
            UDP_RAWGray_Server(const Settings &_settings);
            ~UDP_RAWGray_Server();
            CallLaunchReport launch(); // (Блокирующая операция). Открытие сокета, настройка буфера и подготовка к отправке фреймов.
            
            // TODO send_frame заменить на двухэтапную работу get_ptr_write_frame(), write_frame_success/write_frame_fail
            CallSendFrameReport send_frame(const cv::Mat &frame); // (Квазиблокирующая операция: Блокируется на время пере)
            CallQuitReport quit(); // Возвращает true в случае успешного завершения потока исполнения (блокирующая операция)

        private:
            UDP_RAWGray_Server() = delete;
            void exec(); // поток отправки изображений
            enum SFDReport : uint8_t // SendFrameDirectReport
            {
                SFDR_SUCCESS,
                SFDR_PIPEBUF_EMPTY,
                SFDR_ETH_CONNECTION_ERROR
            };
            SFDReport send_frame_direct(); // блокирующая операция отправки фрейма, вызывается строго из exec() с соблюдением таймингов конфига
            Settings settings;

            int sockfd;
            struct sockaddr_in servaddr; // servaddr в соответствии с конфигурацией

            struct Sync
            {
                std::atomic<bool> execute = {false};
                std::atomic<bool> exec_is_close = {false};
                std::mutex snd_mtx; // обеспечение квазиоднопоточного характера запросов внешнего кода к send_frame
                // std::mutex quit_mtx; 
                // std::mutex launch_mtx;
            } sync;
            const int wait_close_exec_watchdog_time_ms = 5'000; // общее время, которое выделяется на закрытие основного цикла
            const int check_close_exec_interval_ms = 25;  
            const int check_launch_success_interval_ms = 5;

            std::shared_ptr<ts_pipe_frame_buf::TSPipeFrameBuf> pipebuf = nullptr;

            uint8_t* cmd_bytebuffer = nullptr;

            std::atomic<FSMState> fsm_state = {FSMState::STATE_PRIMARY};

            LaunchFailType launch_fail_type = LaunchFailType::LFT_NONE;
            WorkflowBreakType workflow_break_type = WorkflowBreakType::WBT_NONE;
            QuitFailType quit_fail_type = QuitFailType::QFT_NONE;

            int push_frame_min_delay_mcs = 1'000; // минимальная задержка между вызовами send_frame 
            std::chrono::system_clock::time_point tp_watchdog_push_delay; // watchdog delay push frames to pipebuf 
            
            std::vector<uint8_t> start_frame_pack_data_vec = {0,255,255,0,255,255,255,255}; // 8 byte
            uint8_t start_byte_init_frame = 127; // первый байт в пакете инициализации
            uint8_t start_byte_frame_data = 169; // первый байт в пакете с данными

            double inst_real_stream_fps = 0; // мгновенный действительный fps отправки видео
            double inst_real_stream_single_duration_mcs = 0; // мгновенное время отправки одного фрейма
            std::chrono::system_clock::time_point tp_real_fps;
            std::chrono::system_clock::time_point inst_tp_real_fps;

            double total_stream_exec_time_mcs = 0;
            double average_real_stream_fps = 0;
            size_t total_send_frames_by_session = 0;

            int32_t frame_cnt_local = 0;
            int32_t max_frame_cnt_local = 1e9;
    };
}

#endif //UDP_RAW_GRAY_VIDEO_SERVER_HPP
