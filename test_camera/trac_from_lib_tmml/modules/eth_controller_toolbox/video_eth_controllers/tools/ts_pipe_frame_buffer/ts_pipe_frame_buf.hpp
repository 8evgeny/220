#ifndef TS_PIPE_FRAME_BUF_HPP
#define TS_PIPE_FRAME_BUF_HPP

#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>
#include <map>
#include <cstdint>
#include <mutex>
#include <atomic>
#include <set>
#include <deque>
#include "async_frame_bank.hpp"

// Thread-Safe Pipe like Frame buffer
namespace ts_pipe_frame_buf
{
/**
 * Удаление конкретного элемента из вектора "vec" на позиции "pos"
*/
template <class T>
void vec_remove(std::vector<T>& vec, size_t pos)
{
    if(vec.size() <= pos)
    {
        return;
    }
    typename std::vector<T>::iterator it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}

struct Settings
{
    int total_capacity          = 1;            // количество аллоцируемых изображений
    cv::Size frame_size         = {1920,1080};  // фиксированный размер одного изображения в буфере 
    int frame_type              = CV_8UC1;      // формат цветового пространства 

    // Необходимое условие total_capacity >= minCountThresholdOverwritePrepareFrame
    // минимальное количество фреймов в очереди готовых, 
    // при котором допустима перезапись последнего.
    int minCountThresholdOverwritePrepareFrame = 1; 
}; // -- Settings

/**
 * Возможные состояния MatSmartID объекта
*/
enum MSidStates
{
    STATE_STORAGE_EMPTY,        // в ожидании записи
    STATE_STORAGE_PREPARED,     // записан и ожидает чтения
    STATE_PROCESS_READ,         // находится в состоянии чтения
    STATE_PROCESS_WRITE,        // находится в состоянии записи
    STATE_NONE                  // неопределенное состояние
};

struct MSid // cv::Mat Smart ID
{
    cv::Mat* frame_ptr = nullptr;
    int id = -1; 
    MSidStates state = STATE_NONE;
};

class TSPipeFrameBuf
{
public:
    TSPipeFrameBuf(
        const Settings &_settings);
    ~TSPipeFrameBuf();

    /**
     * Настройка буфера для дальнейшего взаимодействия.
     * Возвращает true в случае успешного ввода в эксплуатацию.
     * Возвращает false при возникновении ошибок.
     * Сообщения о некоторых ошибках логируются в терминал.
     * @return setup buffer success status
    */
    bool setup();       

    /**
     * Корректное завершение работы буфера (деинициализация).
     * Возвращает true в случае успешного завершения работы буфера.
     * Возвращает false при возникновении ошибок.
     * Сообщения о некоторых ошибках логируются в терминал.
     * После вызова quit (успешного или неуспешного)
     * дальнейшая корректная работа с буфером не предусмотрена.
     * При возврате false предусмотрена возможность
     * повторного вызова.
     * @return quit buffer success status
    */
    bool quit();                        

    /**
     * Запрос указателя с целью записи фрейма.
     * Указатель может быть выделен из различных доступных "банков".
     * Для записи указатель может быть выделен одним из следующих банков:
     * 1. "Ожидающие записи", 2. "Записанные (готовые)".
     * Первый в приоритете банк "Ожидающие записи".
     * Если свободных "ожидающих" фреймов нет, 
     * то запрос производится у следующего в 
     * порядке приоритета банка.
     * Среди "записанных" фреймов в банке есть порядок
     * хранения в структуре данных типа "очередь".
     * Наименее актуальным является первый в очереди на чтение.
     * Наиболее актуальным является последний поступивший.
     * Если соблюдаются условия минимальной населенности
     * (см. minCountThresholdOverwritePrepareFrame),
     * то возвращается указатель на наименее актуальный.
     * Если банк "готовых" фреймом не может выдать указатель,
     * то возвращается nullptr.
     * 
     * 
     * !!!Ограничения!!!:
     * В случае многопоточной записи в очередь готовых фреймов 
     * новые фреймы поступают, когда конкретный поток
     * вызывает "защелку" frame_write_complete_success(...).
     * Вследствие автоматического распределения 
     * вычислительных ресурсов конкретного вычислителя
     * (в т.ч. планировщик), нет гарантии сохранения
     * исходной последовательности. Например, в одном потоке
     * в настоящий момент времени записывается в буфер фрейм
     * (т.е. несколько ранее был выполнен запрос get_ptr_write_frame).
     * Во другом потоке тоже начинается запись фрейма. 
     * Но вследствие влияния различных факторов запись второго фрейма
     * завершается быстрее (вызывается frame_write_complete_success 
     * во втором потоке). Следовательно, в pipebuf фрейм 
     * второго потока будет выдан на чтение раньше, 
     * чем фрейм первого потока (когда подойдет очередь).
     * Для некоторых обстоятельств будет корректнее судить о порядке фреймов
     * не по событию вызова "защелки" (frame_write_complete_success), 
     * а по событию запроса указателя на чтение (get_ptr_write_frame).
     * Однако, такая механика поведения потребует введения
     * сортировочных мероприятий в банке готовых фреймов при 
     * вызовах "защелки". 
     * @return pointer to frame for write
    */
    cv::Mat* get_ptr_write_frame();

    /**
     * Запрос указателя с целью чтения фрейма.
     * Указатель может быть выделен только из банка "готовых" фреймов.
     * Если есть готовые фреймы, выделяется наименее актуальный.
     * Если готовых фреймов нет, возвращается nullptr.
     * С целью оптимизации операции в Real-Time приложениях
     * изменение содержимого изображения (полученного для чтения)
     * может производиться с использованием указателя на данные (.data).
     * @return pointer to frame for read
    */
    cv::Mat const * get_ptr_read_frame(); 

    /**
     * Уведомление буфера о том, что фрейм успешно записан.
     * Фрейм попадает в банк "готовых".
     * Фрейм проверяется на предмет внесения изменений:
     * 1. width, 2. height, 3. type, 4. data (start ptr)
     * @param frame_ptr - pointer to successfully written frame
    */
    void frame_write_complete_success(cv::Mat const *frame_ptr);

    /**
     * Уведомление буфера о том, что фрейм записан с ошибкой.
     * Фрейм попадает в банк "ожидающих". (Не учитывается как готовый).
     * Фрейм проверяется на предмет внесения изменений:
     * 1. width, 2. height, 3. type, 4. data (start ptr)
     * @param frame_ptr - pointer to an unsuccessfully written frame
    */
    void frame_write_complete_failure(cv::Mat const *frame_ptr); 
    
    /**
     * Уведомление буфера о том, что чтение фрейма завершено.
     * Фрейм попадает в банк "ожидающих".
     * Фрейм проверяется на предмет внесения изменений:
     * 1. width, 2. height, 3. type, 4. data (start ptr)
     * @param frame_ptr - pointer to an successfully read frame
    */
    void frame_read_complete(cv::Mat const *frame_ptr); 

    /**
     * Возвращает количество "готовых" к чтению фреймов.
     * Внимание! Поскольку буфер асинхронный и потокобезопасный,
     * то значение количества готовых фреймов является мгновенным.
     * Истинная населенность банка "готовых" фреймов изменяется 
     * быстрее, чем максимальная из частот запроса и уведомления 
     * для чтения и записи.
     * @return current count prepared frames in buffer (instance value)
    */ 
    int get_count_prepared();

    /**
     * Возвращает максимальную вместимость буфера.
     * @return maximum possible number of images in buffer
    */
    int get_total_capacity();
private:

    /**
     * Конструктор по умолчанию запрещен и удален,
     * т.к. поведение буфера определено только при 
     * использовании конструктора с аргументом Settings
    */
    TSPipeFrameBuf() = delete;

    /**
     * Отчет о проверке приходящего указателя на cv::Mat в методы:
     * 1) frame_write_complete_success
     * 2) frame_write_complete_failure
     * 3) frame_read_complete
    */
    enum CheckOwnershipReturnedFrameReports
    {
        REPORT_FRAME_VALID,         // фрейм прошел проверки и будет обработан
        REPORT_FRAME_NOT_OWNING,    // поступивший фрейм не относится к реестру собственных ресурсов
        REPORT_FRAME_WRONG_STATE,   // фрейм принадлежит буферу, но числится с другим текущим состоянием 
        REPORT_FRAME_WAS_CRUSHED    // фрейм принадлежит буферу, но был изменен (размер, тип)
    };

    /**
     * Проверка приходящего указателя на cv::Mat:
     * 1) проверка на nullptr
     * 2) проверка на предмет наличия в реестре
     * 3) проверка на предмет внесения изменения 
     *      (актуально для frame_write_complete_success/failure)
    */
    CheckOwnershipReturnedFrameReports checkValidReturnedFramePtr(
        cv::Mat const *     frame_ptr,
        MSidStates          required_state
        );

    /**
     * sorting/sort/arranging/ordering/order_regulation etc...
     * Возможная механика работы с многопоточной поставкой в буффер фреймов.
    */
    // void organizing();

    Settings settings;

    /**
     * Общий банк изображений, 
     * которые в начале запрашиваются, 
     * в конце возвращаются на баланс.
    */
    std::shared_ptr<async_frame_bank::AsyncFrameBank> frame_bank;

    /**
     * Общий вектор MSid выделяется при вызове setup один раз,
     * далее указатели на объекты MSid 
     * мигрируют копированием между 4 контейнерами:
     * 1) items_procStoragePrepared_queue, 
     * 2) items_procRead_vec, 
     * 3) items_procWrite_vec, 
     * 4) items_procStorageEmpty_vec
     * 
     * items_all_buffer:
     * Общий буфер всех "MSid" (cv::Mat Smart ID)
     * Выделяется в стеке, другие контейнеры получают указатель через &
    */
    std::vector<MSid> items_all_buffer; 

    /**
     * 1) items_procStoragePrepared_queue:
     *      Банк готовых к поставке пользовательскому (вызывающему) коду
     *      смарт-указателей на фреймы 
     * 
     * 2) items_procRead_vec:
     *      Банк смарт-указателей фреймы, 
     *      находящиеся в процессе чтения извне 
     *      (TODO: для большей оптимизации лучше использовать 
     *      std::set для Read и Write контейнеров)
     * 
     * 3) items_procWrite_vec:
     *      Банк смарт-указателей на фреймы,
     *      находящихся в процессе записи извне 
     * 
     * 4) items_procStorageEmpty_vec:
     *      Банк смарт-указателей на фреймы,
     *      находящиеся в режиме ожидания записи
    */
    std::deque<MSid*>   items_procStoragePrepared_queue; // why decue? maybe list?
    std::vector<MSid*>  items_procRead_vec; 
    std::vector<MSid*>  items_procWrite_vec; 
    std::vector<MSid*>  items_procStorageEmpty_vec;

    /**
     * Быстрое преобразование из указателя на cv::Mat данных в указатель на msid.
     * Может быть использовано для проверки состояния MSidStates и т.д.
    */
    std::unordered_map<cv::Mat const *, MSid*> dispatch_framesPtr2mqsidPtr; 
    // std::unordered_map<

    /**
     * Быстрое преобразование из id изображения 
     * (уникальная характеристика-признак) 
     * в порядковый номер (позицию) в одном из 4х векторов-банков
    */
    std::vector<int> dispatch_frameID2procReadVecID;
    std::vector<int> dispatch_frameID2procWriteVecID; 
    std::vector<int> dispatch_frameID2procStorageEmptyID;
    std::vector<int> dispatch_frameID2procStoragePreparedID;

    /**
     * TODO: Обеспечение квазиоднопоточности процесса записи фреймов в буфер.
    */
    std::mutex write_frame_mtx; 

    /**
     * Потоковая безопасность при работе с "банками"
    */
    std::mutex items_mtx;

    struct SyncQuit
    {
        std::atomic<bool> current_called    = {false}; // в настоящий момент вызван
        std::atomic<bool> success           = {false}; // успешно закрыт
        std::atomic<bool> _was_call         = {false}; // хотя бы один раз был вызван
    };

    struct Sync
    {
        std::atomic<bool>   is_configure    = {false};
        std::atomic<bool>   setup_was_call  = {false};
        SyncQuit            quit;
    } sync;

    struct WatchDogEvent
    {
        int wait_total_time     = 1'000;    // общее время ожидания
        int check_delay_time    = 30;       // интервал проверки
        WatchDogEvent(int _wait_total_time, int _check_delay_time)
            : wait_total_time(_wait_total_time), check_delay_time(_check_delay_time)
        {

        };

    };

    struct WatchDog // настройки временных интервалов для внутриклассовых процессов 
    {
        WatchDogEvent release_wr_frames_pts = {3'000, 30};
    } watchdog;
}; // -- class TSPipeFrameBuf

} // -- namespace ts_pipe_frame_buf

#endif //TS_PIPE_FRAME_BUF_HPP
