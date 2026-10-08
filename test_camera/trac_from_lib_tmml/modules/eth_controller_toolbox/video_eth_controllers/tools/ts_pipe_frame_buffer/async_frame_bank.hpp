#ifndef ASYNC_FRAME_BANK_HPP
#define ASYNC_FRAME_BANK_HPP

#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>
#include <map>
#include <cstdint>
#include <mutex>
#include <atomic>
#include <set>

namespace async_frame_bank
{
    /*
    1) Заведомо выделяет память для изображений в соответствии 
        с поступившей в конструктор конфигурацией; 
    2) Управляет ресурсом (массивом изображений) в режиме многопоточного доступа (чтение/запись) 
        (обеспечивается потоковая безопасность)
    3) Класс предоставляет возможность получить указатель на frame 
        и использовать данный ресурс в "аренду" некоторое время 
        (ресурс может оставаться в пользовании до момента окончания исполнения приложения)
    4) Для достижения максимальной производительности 
        ответственность в распоряжении над ресурсом, 
        предоставляемым во временное пользование настоящим банком,
        частично возлагается на внешний вызывающие объекты классов и функции.
        AsyncFrameBank (банк) возвращает указатель на объект типа cv::Mat,
        следовательно, данный ресурс нельзя захватывать в пользование smart-указателями
        и нельзя вызывать delete для них.
        Внешний код самостоятельно отслеживает прекращение вызова ресурса 
        после уведомления о возврате FrameBank, т.к. этот ресурс в это время
        потенциально может использоваться другим арендатором.
    5) Пользователь при работе с Банком отслеживает,
        что к моменту исполнения деструктора Банка все взятые
        ранее изображения в арендное пользования "возвращены".
        В ином случае будет вызвано исключение.  
    */

    struct Settings
    {
        int total_capacity = 10; // количество изображений в буфере
        cv::Size frame_size = {1920, 1080};
        int frame_type = CV_8UC1; // like CV_8UC1, CV_8UC3 etc...
    };
    
    struct ItemState
    {
        bool is_lock = true;
    };

    struct Item
    {
        cv::Mat frame;
        ItemState state;
    };

    class AsyncFrameBank
    {
        public:
            AsyncFrameBank(const Settings &_settings);
            ~AsyncFrameBank();
            bool setup(); // Выделение памяти и настройка переменных служебного назначения
            int get_count_free_items(); // количество свободных изображений
            int get_count_unfree_items(); // Количество выданных в пользование изображений
            cv::Mat* get_frame(); // Получить изображение в аренду через указатель
            void release_frame(cv::Mat* frame); // Вернуть арендованное изображение
        private:
            Settings settings;
            std::vector<Item> items_vec;
            std::vector<int> free_items_id_vec;
            // используется для разбокировки арендного ресурса вызовом release_frame(cv::Mat* frame);
            std::map<uint8_t*, int> map_framePtr_BankItemsId; // Контейнер быстрого доступа к конкретной ячейке items_buf по указателю на первый байт изображения
            
            std::mutex items_mtx; // protect: items_buf, free_items_id_buf, map_framePtr_BankItemsId

            std::atomic <bool> is_configure = {false}; // Флаг состония контейнера для регулирования доступа к методам до момента вызова bool setup();
        };
}

#endif // ASYNC_FRAME_BANK_HPP