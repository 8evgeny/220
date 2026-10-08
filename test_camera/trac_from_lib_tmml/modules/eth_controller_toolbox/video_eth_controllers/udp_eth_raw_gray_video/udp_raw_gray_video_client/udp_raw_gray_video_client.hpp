#ifndef UDP_RAW_GRAY_VIDEO_CLIENT_HPP
#define UDP_RAW_GRAY_VIDEO_CLIENT_HPP

#include <string>
#include "video_eth_controllers/tools/ts_pipe_frame_buffer/ts_pipe_frame_buf.hpp"
#include <opencv2/opencv.hpp>

#include <netinet/in.h>

namespace udp_raw_client_gray
{
    struct Settings
    {
        std::string client_ip_v4 = "127.0.0.1";
        int client_port = 8080;
        
        int frame_width = 128;
        int frame_height = 128;

        int count_rows_in_pack = 11; // Количество строк в одном UDP пакете
        int pipebuf_total_capacity = 3; // объем фрейм-буффера на отправку

        int max_udp_pack_size = 1462;
    };

    enum CallLaunchReport : uint8_t
    {
        LAUNCH_FAILED,                              // ошибка запуска
        LAUNCH_PROCESSES_CURRENTLY,                 // в настоящий момент производится запуск сервера    
        LAUNCH_SUCCESS                              // запуск успешно завершен в результате текущего вызова или более раннего
    };

    enum CallQuitReport : uint8_t
    {
        QUIT_FAILED,                                // ошибка выхода (например, превышено допустимое время ожидания)
        QUIT_PROCESS_CURRENTLY,                     // в настоящий момент происходит выход
        QUIT_SUCCESS                                // успешно произведен выход
    };

    enum FSMState : uint8_t
    {
        STATE_PRIMARY,              // первичное состояние после исполнения конструктора
        STATE_LAUNCH_SUCCESS,       // Успешно аллоцирован pipebuf, открыт UDP-сокет и запущен цикл отправки видео. В настоящее время Server в рабочем режиме эксплуатации
        STATE_LAUNCH_FAILED,        // Неуспешный запуск Server'а
        STATE_LAUNCH_PROCESS,       // В настоящее время производится запуск 
        STATE_WORKFLOW_BREAK,       // Прервано исполнение пример, в случае обрыва Eth-соединения
        STATE_QUIT_SUCCESS,         // Успешно произведено закрытие Server'а
        STATE_QUIT_FAILED,          // Неуспешно закрыт Server (например, time_is_over ожидания корректного закрытия основного цикла)
        STATE_QUIT_PROCESS          // В настоящее время в Server запущен процесс закрытия 
    };

    class UDP_RAWGray_Client
    {
        public:
            UDP_RAWGray_Client(const Settings &_settings);
            ~UDP_RAWGray_Client();
            CallLaunchReport launch(); // (Блокирующая операция). Открытие сокета, настройка буфера и подготовка к отправке фреймов.
            cv::Mat const * get_read_frame_ptr(); // Извлечение указателя для чтения фрейма
            void release_frame_ptr(cv::Mat* frame_ptr); // Возврат указателя для чтения фрейма
            CallQuitReport quit(); // Возвращает true в случае успешного завершения потока исполнения (блокирующая операция)
            
        private:
            UDP_RAWGray_Client() = delete;
            void exec(); // поток отправки изображений
            bool check_init_new_frame_pack(int pack_size, uint8_t const * pack_data);
            static void set_all_false(std::vector<bool> &vec);
            static bool check_all_true(const std::vector<bool> &vec);
            int sockfd;
            struct sockaddr_in servaddr; // servaddr в соответствии с конфигурацией

            Settings settings;
            struct Sync
            {
                std::atomic<bool> execute = {false};
                std::atomic<bool> exec_is_close = {false};
            } sync;

            const int wait_close_exec_watchdog_time_ms = 5'000; // общее время, которое выделяется на закрытие основного цикла
            const int check_close_exec_interval_ms = 25;  
            const int check_launch_success_interval_ms = 5;

            std::shared_ptr<ts_pipe_frame_buf::TSPipeFrameBuf> pipebuf = nullptr;
            uint8_t* cmd_bytebuffer = nullptr;
            std::atomic<FSMState> fsm_state = {FSMState::STATE_PRIMARY};

            std::vector<uint8_t> start_frame_pack_data_vec = {0,255,255,0,255,255,255,255}; // 8 byte
            uint8_t start_byte_init_frame = 127; // первый байт в пакете инициализации
            uint8_t start_byte_frame_data = 169; // первый байт в пакете с данными
            int valid_init_new_frame_pack_size = -1;
            int32_t local_frame_id = -1;
    };

}

#endif //UDP_RAW_GRAY_VIDEO_CLIENT_HPP
