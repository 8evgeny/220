#ifndef GOEN220_TLM_DATA_MODEL_H
#define GOEN220_TLM_DATA_MODEL_H


#include <QAbstractTableModel>
#include <QPointer>

#include <libgoen220data.h>

namespace Goen220 {

/// @brief Табличная модель данных телеметрии в системе QtMVC
class TlmDataModel : public QAbstractTableModel
{
    Q_OBJECT

protected:

    explicit TlmDataModel(QObject *parent = nullptr)
        : QAbstractTableModel(parent)
    {}

public:

    /// @brief Пересичление категорий, данных
    enum class Category {
        UNDEF,
        TV_CHAN,
        IR_CHAN,
        RF,
        GSP,
        CTRL,
        SRVC,
        TRACKING,
        EVCAM,
        MINI640,
        CH_EXT_ST,
        LDC
    };

    /// @brief Коды колонок данных в таблице
    enum class Col {
        UNDEF = -1,
        NAME = 0,
        VALUE = 1
    };

public:

    /**
     * @brief Загружает (копирует) в модель бинарные данные структуры Tlm
     * @details Если текущие данные отличаются от загружаемых, то
     * модель уведомляет через сигнал dataChanged
     * @param tlm указательно на объект типа Tlm
     */
    virtual void setRaw(const Tlm *tlm) = 0;

    /**
     * @brief Возвращает константную ссылку на структуру типа Goen220::Tlm
     * @details Структура является внутренним источником данных модели
     * Ссылка валидна на всем этапе существования модели.
     */
    virtual const Goen220::Tlm& rawData() const = 0;

    /**
     * @brief Возвращает ссылку на структуру типа Goen220::Tlm
     * @details Структура является внутренним источником данных модели
     * @note Внимание. Изменения внесенные по данной ссылке не отслеживаются моделью автоматически
     * Ссылка валидна на всем этапе существования модели.
     */
    virtual Goen220::Tlm& rawData() = 0;

    /**
     * @brief Возврашает индекс модели
     * @param row номер ряда в модеи
     * @param col Тип колонки модели
     * @return индекс модели
     */
    virtual QModelIndex index(int row, Col col) const = 0;

    /**
     * @brief Возвращает номер ряда в модели, по смещениею
     * @param offset бинарное смещение поля в структуре типа Goen220::Tlm
     * @return номер ряда или -1, если смещение не валидно
     */
    virtual int row(size_t offset) const = 0;

    /**
     * @brief Создает вторичную прокси-модель с фильтрацией по категории
     * @param cat требуемя категория
     * @return указатель на прокси модель.
     */
    virtual QPointer<QAbstractItemModel> createCategoryFilterModel(Category cat) = 0;

    /**
     * @brief Возвращает имя поля по номеру ряда
     * @param row номер ряда в модели
     * @return имя поля, используемое в модели
     */
    QString name(int row) const { return data(index(row, Col::NAME)).toString(); }

    /**
     * @brief Возвращатет значение поля из модели по номеру ряда
     * @param row номер ряда в модели
     * @return значение поля
     */
    QVariant value(int row) const { return data(index(row, Col::VALUE)); }

    /**
     * @brief Возвращает номер ряда для поля описываемого в Args
     * @tparam Args Вариативный тип аргументов описания поля
     * @param args аргументы описания поля, должны соответвовать требования функций Goen220::Tlm::offset
     * @return номер поля
     */
    template <typename... Args>
    requires Utils::CanCallOffset<Args...>
    int row(Args...args)
    {
        return row(Utils::offset(args...));
    }

    /**
     * @brief Возвращает имя поля описываемого в Args
     * @tparam Args Вариативный тип аргументов описания поля
     * @param args аргументы описания поля, должны соответвовать требования функций Goen220::Tlm::offset
     * @return имя поля в модели
     */
    template<typename... Args>
    requires Utils::CanCallOffset<Args...>
    QString name(Args... args)
    {
        return name(row(Utils::offset(args...)));
    }

    /**
     * @brief Возвращает значение поля описываемого в Args
     * @tparam Args Вариативный тип аргументов описания поля
     * @param args аргументы описания поля, должны соответвовать требования функций Goen220::Tlm::offset
     * @return значение поля в модели
     */
    template<typename... Args>
    requires Utils::CanCallOffset<Args...>
    QVariant value(Args... args)
    {
        return value(row(Utils::offset(args...)));
    }

    //--------------------------------------------------------------------------------------------

    /**
     * @brief Создает объект модели
     * @note удаление созданного объекта реализауется на вызывающей стороне
     * @param parent указатель на parent в иерархии объектов Qt
     * @return указатель на созданный объект
     */
    static TlmDataModel* createModel(QObject* parent);

    /**
     * @brief Создает объект модели, в которой доступна установка значений колонки VALUE
     * @note удаление созданного объекта реализауется на вызывающей стороне
     * @param parent указатель на parent в иерархии объектов Qt
     * @return указатель на созданный объект
     */
    static TlmDataModel* createEditableModel(QObject* parent);

private:
};

} // namespace Goen220

#endif // GOEN220_TLM_DATA_MODEL_H
