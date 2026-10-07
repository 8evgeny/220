#ifndef TRSH_WRAPPER_H
#define TRSH_WRAPPER_H

#include <opencv2/opencv.hpp>

class TrShWrapperPrivate;


/**
 * @brief Оболочка для трекера Шатского в openCL версии
 */
class TrShWrapper
{

public:

    //---------------------------------------------------------------

    /**
     * @brief Прямоугольник (пиксели)
     */
    struct Rect {
        unsigned short cx; ///< Центр X (пикс.)
        unsigned short cy; ///< Центр Y (пикс.)
        unsigned short w; ///< Ширина (пикс.)
        unsigned short h; ///< Высота (пикс.)

        /**
         * @brief Возвращает true если ширина и высота больше 1 пикс
         */
        bool valid() const { return w > 1 && h > 1; }

        /**
         * @brief Возврашает левый (минимальный) X (пикс.)
         */
        inline unsigned short left() const { return cx - (w >> 1); }

        /**
         * @brief Возвращате верхний (минимальный) Y (пикс.)
         */
        inline unsigned short top() const { return cy - (h >> 1); }
    };

    //---------------------------------------------------------------

    /**
     * @brief Прямоугольник (в относительных величинах [0..1])
     */
    struct RectF {
        float cx; ///< Центр X
        float cy; ///< Центр Y
        float w; ///< Ширина
        float h; ///< Высота
    };

    //---------------------------------------------------------------

    TrShWrapper();

    TrShWrapper(const TrShWrapper&) = delete;

    ~TrShWrapper();

    TrShWrapper& operator=(const TrShWrapper&) = delete;

    //---------------------------------------------------------------

    /**
     * @brief Задает INI файл конфигурации для трекера
     * @details Файл используется один раз при инициализации
     * @param fileNamePath полный путь к файлу, включая имя файла
     */
    void setIniFileNamePath(const std::string &fileNamePath);

    /**
     * @brief Возвращает коэффициент расширение рамки ROI
     * (относительно размеров объекта)
     */
    float roiExt() const;

    /**
     * @brief Устанавливает коэффициент расширения рамки ROI
     * (относительно размера объекта)
     * @param ext коэффициент
     */
    void setRoiExt(float ext);

    /**
     * @brief Устанавливает целевое положение и размер объекта (пикс.)
     * @details Остановка и перезапуск трекинга происходит автоматически
     * @param objectRect рамка объекта
     */
    void setTargetObject(const Rect &objectRect);

    /**
     * @brief Остановка трекинга
     */
    void stopTracking();

    /**
     * @brief Процедура трекинга по текущему кадру, возврашает признак успешного захвата
     * @details
     * - Автоматически создает трекер по параметрам самого первого кадра.
     * - Обеспечивает очередность остановки/инициализации трекера
     * - Передает кадр на трекинг, формирует выходные данные
     *
     * @param frame текущий кадр
     * @return true если выполнен захват объекта, @see lockedOn()
     */
    bool trackFrame(cv::Mat &frame);

    /**
     * @brief Вовзращает признак активности трекига.
     * @see setTargetObject
     * @see stopTracking
     */
    bool active() const;

    /**
     * @brief Возвращет признак успешного захвата объекта
     */
    bool lockedOn() const;

    /**
     * @brief Возвращает текущий ROI, по итогу трекинга
     */
    const Rect &roiRect() const;

    /**
     * @brief Возврашает текущее положение и размер захваченного объекта
     * (пикс.), по итогу трекинга
     */
    const Rect &objectRect() const;

    /**
     * @brief Возврашает текущее положение и размер захваченного объекта
     * (в относительных величинах), по итогу трекинга
     */
    const RectF &objectRectF() const;

    /**
     * @brief Возвращает число кадров переданных в @see trackFrame
     * @details Инкремент происходит в конце работы trackFrame
     */
    long long frameCounter() const;

    /**
     * @brief Возвращате указатель на trac_struct
     * @note для отладочных целей.
     */
    void* tctl();


private:
    TrShWrapperPrivate *m_d;
};

#endif // TRSH_WRAPPER_H
