#pragma once

#include <QtGlobal>
#include <QDataStream>
#include <QByteArray>

#include <concepts>
#include <stdexcept>
#include <type_traits>

#include <goen220_types.h>

namespace Goen220 {
namespace Utils {

/**
 * @brief Возвращает бинарное смещение до указанного поля в структуре с учетом вложенности.
 * @details
 *  - Функция вычисляет смещение без использования экземпляров структуры.
 *  - Список аргументов может использовать группы аргументов соответствующие вызову offset_idx
 * @tparam T - автоматический тип текущей структуры
 * @tparam M - автоматичесий тип текущего поля в структуре
 * @tparam Rest - вариативный тип все прочих параметров
 * @param first_member - указатель на член структуры (поле) относительно которого будет вычисленно смещение
 * @param rest - вариативные параметры, соответствующие описанию вложенных полей в формате первых аргументов offset или offset_idx
 * @return смещение в байтах до указанного поля
 */
template<typename T, typename M, typename... Rest>
static inline constexpr std::size_t offset(M T::*first_member, Rest... rest);

/**
 * @brief Возвращает бинарное смещение до указанного поля-массива (с указанием индекса) в структуре с учетом вложенности.
 * @details
 *  - Функция вычисляет смещение без использования экземпляров структуры.
 *  - Список аргументов может использовать группы аргументов соответствующие вызову offset
 *  - проверка выхода index за пределы массивва вызывает исключение std::out_of_range
 * @tparam T - автоматический тип текущей структуры
 * @tparam M - автоматичесий тип текущего поля в структуре (классический C массив фиксированного размера)
 * @tparam Rest - вариативный тип все прочих параметров
 * @param array поле-массив (классический C-массив фиксированного размера)
 * @param index индекс масив
 * @param rest - вариативны параметры, соответствующие описанию вложенных полей в формате первых аргументов offset или offset_idx
 * @return смещение в байтах до искомого поля
 */
template<typename T, typename M, typename... Rest>
    requires(std::is_array_v<std::remove_reference_t<M>>)
static inline constexpr std::size_t offset_idx(M T::*array, size_t index, Rest... rest);

//====================================================================================

template<typename T, typename M, typename... Rest>
    requires(std::is_array_v<std::remove_reference_t<M>>)
static inline constexpr std::size_t offset_idx(M T::*array, size_t index, Rest... rest)
{
    auto N = std::extent<M>::value;

    if (index >= N || index < 0)
        throw std::out_of_range("array index out of range");

    if constexpr (sizeof...(Rest) != 0)
        return offset(array) + sizeof(std::remove_extent_t<M>) * index + offset(rest...);
    else
        return offset(array) + sizeof(std::remove_extent_t<M>) * index;
    return -1;
}

//====================================================================================

template<typename T, typename M, typename... Rest>
static inline constexpr std::size_t offset(M T::*first_member, Rest... rest)
{
    if constexpr (sizeof...(Rest) != 0) {
        if constexpr (std::is_array_v<std::remove_reference_t<M>>)
            return offset_idx(first_member, rest...);

        using rest_tuple_t = std::tuple<Rest...>;
        using first_of_rest_t = std::tuple_element_t<0, rest_tuple_t>;

        if constexpr (std::is_member_object_pointer_v<first_of_rest_t>)
            return offset(first_member) + offset(rest...);
        else
            throw std::invalid_argument("first of rest is not a member object pointer");

    } else {
        return reinterpret_cast<size_t>(&(reinterpret_cast<T *>(0)->*first_member));
    }

    return -1;
}

/**
 * @brief Концепт описывающий требования к структуре вариативных аргументов Args
 * на совместимость с вызовом функций Goen220::offset
 * (описание поля структуры)
 */
template<typename... Args>
concept CanCallOffset = requires(Args... args) { Goen220::Utils::offset(args...); };

/**
 * @brief примеры использования функци offset... (см. структуру Goen220::Tlm)
 * @code{.cpp}
 * // Смещение до поля Tlm::MW.FIELD_GRAD
 * int o = Goen220::Utils::offset(&Goen220::Tlm::MW, &Goen220::Tlm::Chan::FIELD_GRAD);
 *
 * // Смещение до поля Tlm::EVCAM._2NR_3NR[1]
 * int o = Goen220::Utils::offset(&Goen220::Tlm::Evcam::_2NR_3NR, 1);
 *
 * // Вывод в консоль смещения до полей Target::STATE и Target::A_X_SZ все элементов массива Tlm::TRK.TARGET[..]
 * for (int i = 0; i < 4; i++) {
 *  qDebug() << i << "STATE:" << Goen220::Utils::offset(&Goen220::Tlm::TRK, &Goen220::Tlm::Trk::TARGET, i, &Goen220::Tlm::Target::STATE);
 *  qDebug() << i << "A_X_SZ:" << Goen220::Utils::offset(&Goen220::Tlm::TRK, &Goen220::Tlm::Trk::TARGET, i, &Goen220::Tlm::Target::A_X_SZ);
 * }
 * @endcode
 */

/// @brief Концепт проверяющий тип T на признак кортежа
template<typename T>
concept IsTuple = requires { typename std::tuple_size<T>::type; }
                  && std::is_same_v<T, std::decay_t<T>>;

/// @brief Вспомогательный enum типа потока вывода
enum class StreamType { IN, OUT };

/**
 * @brief Класс потока ввода/вывода типов Goen220 в бинарный буфер
 * @tparam st тип потока (направлеие записи)
 */
template<StreamType st>
class DataStream
{
public:

    /// @brief Тип данных ссылки на бинарный буфер, в зависимости от типа потока
    using DataRef = std::conditional_t<st == StreamType::IN, const QByteArray &, QByteArray &>;


    /// @brief конструктор
    DataStream(DataRef dr)
        : m_dr{dr}
    {}

    /// @brief Оператор записи в буфер (только для типа потока OUT)
    template<typename T>
        requires (st == StreamType::OUT)
    constexpr DataStream &operator<<(const T &val)
    {
        if constexpr (std::is_enum_v<T>) {
            using ut = std::underlying_type_t<T>;
            (*this << ((const ut &) val));
        } else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, BOOL>) {
            quint8 v = (quint8)val;
            (*this << v);
        } else if constexpr (std::is_fundamental_v<T>) {
            m_dr.append((const char *) &val, sizeof(T));
        } else if constexpr (IsTuple<T>) {
            constexpr auto N = std::tuple_size_v<T>;
            [&]<std::size_t I = 0>() mutable {
                if constexpr (I < N) {
                    *this << std::get<I>(val);
                    operator()<I+1>();
                }
            }();
        } else {
            Q_ASSERT_X(false, "Goen220::Utils::DataStream", "Unsupported data type");
        }
        return *this;
    }

    /// @brief Обертка над оператором << для пака аргументов
    template<typename...Args>
        requires (st == StreamType::OUT)
    constexpr void push(Args&&...args)
    {
        (*this << ... << std::forward<Args>(args));
    }

    /// @brief Оператор чтения из буфера (только для типа потока IN)
    template<typename T>
        requires(st == StreamType::IN)
    constexpr DataStream &operator>>(T &val)
    {
        if constexpr (std::is_enum_v<T>) {
            using ut = std::underlying_type_t<T>;
            ut raw;
            *this >> raw;
            val = (T) raw;
        } else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, BOOL>) {
            quint8 raw;
            *this >> raw;
            val = (T) raw;
        } else if constexpr (std::is_fundamental_v<T>) {
            if (m_pos >= m_dr.size())
                throw std::out_of_range("pos out of range");
            val = *(const T*)(m_dr.constData() + m_pos);
            m_pos += sizeof(T);
        } else if constexpr (IsTuple<T>) {
            constexpr auto N = std::tuple_size_v<T>;
            [&]<std::size_t I = 0>() mutable {
                if constexpr (I < N) {
                    *this >> std::get<I>(val);
                    operator()<I+1>();
                }
            }();
        } else {
            Q_ASSERT_X(false, "Goen220::Utils::DataStream", "Unsupported data type");
        }
        return *this;
    }

    /// @brief Обертка над оператором >> для пака аргументов
    template<typename...Args>
        requires (st == StreamType::IN)
    constexpr void pop(Args&...args)
    {
        (*this >> ... >> args);
    }

private:
    DataRef m_dr;
    std::size_t m_pos = 0;
};

/// @brief Псевдоним для типа потока вывода
using DataOutStream = DataStream<StreamType::OUT>;

/// @brief Псевдоним для типа потока ввода
using DataInStream = DataStream<StreamType::IN>;

/// @brief Функция упаковки кортежа в буфер
template<typename T>
    requires IsTuple<T>
static inline QByteArray pack(const T &tuple)
{
    QByteArray out;
    Utils::DataOutStream(out) << tuple;
    return out;
}

/// @brief Функция распаковки кортежа в буфер
/// @note Возможен вызов исключения std::out_of_range
template<typename T>
    requires IsTuple<T>
constexpr void unpack(T &to, const QByteArray &from)
{
    DataInStream(from) >> to;
}


} // namespace Utils
} // namespace Goen220

