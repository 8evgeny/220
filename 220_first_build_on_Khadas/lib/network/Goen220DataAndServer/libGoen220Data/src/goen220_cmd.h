#pragma once

#include <goen220_types.h>
#include <goen220_utils.h>

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QMetaEnum>



namespace Goen220 {

#define __S(x) #x
#define _S(x) __S(x)

/// @brief Генератор имени структуры параметров
#define _CMD_PARAM_NAME(_cmd) CmdParam_##_cmd

/// @brief Создает константу содержащую значение Cmd
#define _CMD_PARAM_DECL_STATIC_CMD(_cmd)\
static constexpr Cmd cmd = Cmd::_cmd;

/// @brief Гененатор констант лимитов
#define _NL \
static constexpr bool has_limits = false;

/// @brief Гененатор констант лимитов
#define _L(t, _min, _max) \
static constexpr bool has_limits = true; \
static constexpr t min = _min; \
static constexpr t max = _max;

/// @brief Создает поле m типа t, с инициализацией нулем
#define _CMD_PARAM_FLD(t, m) t m = (t) 0;

/// @brief Базовый шаблон доступа к структуре параметров по типу команды ()
template<Cmd cmd>
struct CmdParamTy;

/// @brief Генератор специализаций шаблона CmdParam<Cmd>
#define _CMD_PARAM_T(cmd) \
template<> \
    struct CmdParamTy<Cmd::cmd> { \
        using type = _CMD_PARAM_NAME(cmd); \
}

/// @brief Щаблонная обертка структуры параметров команды по значению команды
template<Cmd cmd>
using CmdParamType = CmdParamTy<cmd>::type;

/// @brief Щаблонная обертка мета структуры параметров команды по значению команды
template<Cmd cmd>
using CmdParamMeta = CmdParamTy<cmd>::type::Meta;

/**
 * @name Макросы генерации метаинформации
 * @def _CMD_META_x(...tx, mx)
 * @brief Создает вложенную структуру Meta с типом кортежа имеющего типы tx,
 *      константу N с числом параметров и константный строковой массив names
 *      с именами полей структуры
 *      @{
*/

#define _CMD_META_0 \
struct Meta { \
    using Types = std::tuple<>; \
    static constexpr const char**names = nullptr; \
    static constexpr auto N = std::tuple_size_v<Types>; \
};

#define _CMD_META_1(t1, m1) \
struct Meta { \
    using Types = std::tuple<t1>; \
    static constexpr std::size_t N = std::tuple_size_v<Types>;\
    static constexpr const char*names[N] = {_S(m1)}; \
};

#define _CMD_META_2(t1, m1, t2, m2) \
struct Meta { \
        using Types = std::tuple<t1, t2>; \
        static constexpr auto N = std::tuple_size_v<Types>;\
        static constexpr const char*names[N] = {_S(m1), _S(m2)}; \
};

#define _CMD_META_3(t1, m1, t2, m2, t3, m3) \
struct Meta { \
        using Types = std::tuple<t1, t2, t3>; \
        static constexpr auto N = std::tuple_size_v<Types>;\
        static constexpr const char*names[N] = {_S(m1), _S(m2), _S(m3)}; \
};

#define _CMD_META_4(t1, m1, t2, m2, t3, m3, t4, m4) \
struct Meta { \
        using Types = std::tuple<t1, t2, t3, t4>; \
        static constexpr auto N = std::tuple_size_v<Types>;\
        static constexpr const char*names[N] = {_S(m1), _S(m2), _S(m3), _S(m4)}; \
};

#define _CMD_META_5(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5) \
struct Meta { \
        using Types = std::tuple<t1, t2, t3, t4, t5>; \
        static constexpr auto N = std::tuple_size_v<Types>;\
        static constexpr const char*names[N] = {_S(m1), _S(m2), _S(m3), _S(m4), _S(m5)}; \
};

#define _CMD_META_6(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6) \
struct Meta { \
        using Types = std::tuple<t1, t2, t3, t4, t5, t6>; \
        static constexpr auto N = std::tuple_size_v<Types>;\
        static constexpr const char*names[N] = {_S(m1), _S(m2), _S(m3), _S(m4), _S(m5), _S(m6)}; \
};

#define _CMD_META_7(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6, t7, m7) \
struct Meta { \
        using Types = std::tuple<t1, t2, t3, t4, t5, t6, t7>; \
        static constexpr auto N = std::tuple_size_v<Types>;\
        static constexpr const char*names[N] = {_S(m1), _S(m2), _S(m3), _S(m4), _S(m5), _S(m6), _S(m7)}; \
};

/** @} */

/// @brief Генератор полей структуры (0 полей)
#define _CMD_PARAM_FLDS_0

/// @brief Генератор полей структуры (1 поле)
#define _CMD_PARAM_FLDS_1(t1, m1) _CMD_PARAM_FLD(t1, m1)

/// @brief Генератор полей структуры (2 поля)
#define _CMD_PARAM_FLDS_2(t1, m1, t2, m2) \
    _CMD_PARAM_FLD(t1, m1) \
    _CMD_PARAM_FLD(t2, m2)

/// @brief Генератор полей структуры (3 поля)
#define _CMD_PARAM_FLDS_3(t1, m1, t2, m2, t3, m3) \
    _CMD_PARAM_FLD(t1, m1) \
    _CMD_PARAM_FLD(t2, m2) \
    _CMD_PARAM_FLD(t3, m3)

/// @brief Генератор полей структуры (4 поля)
#define _CMD_PARAM_FLDS_4(t1, m1, t2, m2, t3, m3, t4, m4) \
    _CMD_PARAM_FLD(t1, m1) \
    _CMD_PARAM_FLD(t2, m2) \
    _CMD_PARAM_FLD(t3, m3) \
    _CMD_PARAM_FLD(t4, m4)

/// @brief Генератор полей структуры (5 полей)
#define _CMD_PARAM_FLDS_5(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5) \
    _CMD_PARAM_FLD(t1, m1) \
    _CMD_PARAM_FLD(t2, m2) \
    _CMD_PARAM_FLD(t3, m3) \
    _CMD_PARAM_FLD(t4, m4) \
    _CMD_PARAM_FLD(t5, m5)

/// @brief Генератор полей структуры (6 полей)
#define _CMD_PARAM_FLDS_6(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6) \
    _CMD_PARAM_FLD(t1, m1) \
    _CMD_PARAM_FLD(t2, m2) \
    _CMD_PARAM_FLD(t3, m3) \
    _CMD_PARAM_FLD(t4, m4) \
    _CMD_PARAM_FLD(t5, m5) \
    _CMD_PARAM_FLD(t6, m6)

/// @brief Генератор полей структуры (7 полей)
#define _CMD_PARAM_FLDS_7(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6, t7, m7) \
    _CMD_PARAM_FLD(t1, m1) \
    _CMD_PARAM_FLD(t2, m2) \
    _CMD_PARAM_FLD(t3, m3) \
    _CMD_PARAM_FLD(t4, m4) \
    _CMD_PARAM_FLD(t5, m5) \
    _CMD_PARAM_FLD(t6, m6) \
    _CMD_PARAM_FLD(t7, m7)

/// @brief Сигнатура метода загрузки данных из кортежа
#define _CMD_PARAM_FROM_TUPLE_SIGN constexpr void from(const Meta::Types &tuple)

/// @brief Сигнатура метода выгрузки данных в кортеж
#define _CMD_PARAM_TO_TUPLE_SIGN constexpr void to(Meta::Types &tuple)

/// @brief Сигнатура метода упаковки
#define _CMD_PARAM_PACK_SIGN inline QByteArray pack() const

/// @brief Сигнатура метода распасковки
#define _CMD_PARAM_UNPACK_SIGN inline bool unpack(const QByteArray &from)

/// @brief Генератор конструктора по кортежу типа Meta::Types
#define _CMD_PARAM_CTR_TPL(_cmd) \
    _CMD_PARAM_NAME(_cmd)(const Meta::Types &tuple) \
        : _CMD_PARAM_NAME(_cmd)() \
    { \
        from(tuple); \
    }

/// @brief Генератор конструктора по-умолчанию без параметров
#define _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_NAME(_cmd)() = default;

/// @brief Генератор конструктора с 1 параметром
#define _CMD_PARAM_CTR_1(_cmd, t1, m1) \
    _CMD_META_1(t1, m1) \
    _CMD_PARAM_NAME(_cmd)(t1 _##m1) \
        : m1{_##m1} \
    {} \
    _CMD_PARAM_CTR_TPL(_cmd)

/// @brief Генератор конструктора с 2 параметрами
#define _CMD_PARAM_CTR_2(_cmd, t1, m1, t2, m2) \
    _CMD_META_2(t1, m1, t2, m2) \
    _CMD_PARAM_NAME(_cmd)(t1 _##m1, t2 _##m2) \
        : m1{_##m1} \
        , m2{_##m2} \
    {} \
    _CMD_PARAM_CTR_TPL(_cmd)

/// @brief Генератор конструктора с 3 параметрами
#define _CMD_PARAM_CTR_3(_cmd, t1, m1, t2, m2, t3, m3) \
    _CMD_META_3(t1, m1, t2, m2, t3, m3) \
    _CMD_PARAM_NAME(_cmd)(t1 _##m1, t2 _##m2, t3 _##m3) \
        : m1{_##m1} \
        , m2{_##m2} \
        , m3{_##m3} \
    {} \
    _CMD_PARAM_CTR_TPL(_cmd)

/// @brief Генератор конструктора с 4 параметрами
#define _CMD_PARAM_CTR_4(_cmd, t1, m1, t2, m2, t3, m3, t4, m4) \
    _CMD_META_4(t1, m1, t2, m2, t3, m3, t4, m4) \
    _CMD_PARAM_NAME(_cmd)(t1 _##m1, t2 _##m2, t3 _##m3, t4 _##m4) \
        : m1{_##m1} \
        , m2{_##m2} \
        , m3{_##m3} \
        , m4{_##m4} \
    {} \
    _CMD_PARAM_CTR_TPL(_cmd)

/// @brief Генератор конструктора с 5 параметрами
#define _CMD_PARAM_CTR_5(_cmd, t1, m1, t2, m2, t3, m3, t4, m4, t5, m5) \
    _CMD_META_5(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5) \
    _CMD_PARAM_NAME(_cmd)(t1 _##m1, t2 _##m2, t3 _##m3, t4 _##m4, t5 _##m5) \
        : m1{_##m1} \
        , m2{_##m2} \
        , m3{_##m3} \
        , m4{_##m4} \
        , m5{_##m5} \
    {} \
    _CMD_PARAM_CTR_TPL(_cmd)

/// @brief Генератор конструктора с 6 параметрами
#define _CMD_PARAM_CTR_6(_cmd, t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6) \
    _CMD_META_6(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6) \
    _CMD_PARAM_NAME(_cmd)(t1 _##m1, t2 _##m2, t3 _##m3, t4 _##m4, t5 _##m5, t6 _##m6) \
        : m1{_##m1} \
        , m2{_##m2} \
        , m3{_##m3} \
        , m4{_##m4} \
        , m5{_##m5} \
        , m6{_##m6} \
    {} \
    _CMD_PARAM_CTR_TPL(_cmd)

/// @brief Генератор конструктора с 7 параметрами
#define _CMD_PARAM_CTR_7(_cmd, t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6, t7, m7) \
    _CMD_META_7(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6, t7, m7) \
    _CMD_PARAM_NAME(_cmd)(t1 _##m1, t2 _##m2, t3 _##m3, t4 _##m4, t5 _##m5, t6 _##m6, t7 _##m7) \
        : m1{_##m1} \
        , m2{_##m2} \
        , m3{_##m3} \
        , m4{_##m4} \
        , m5{_##m5} \
        , m6{_##m6} \
        , m7{_##m7} \
    {} \
    _CMD_PARAM_CTR_TPL(_cmd)

/// @brief Генератор метода упаковки
#define _CMD_PARAM_PACK(...) \
_CMD_PARAM_PACK_SIGN \
{ \
    QByteArray to; \
    Utils::DataOutStream(to).push(__VA_ARGS__); \
    return to; \
}

/// @brief Генератор метода распаковки
#define _CMD_PARAM_UNPACK(...) \
_CMD_PARAM_UNPACK_SIGN \
{ \
    try { \
        Utils::DataInStream(from).pop(__VA_ARGS__); \
    } catch (const std::out_of_range &) { \
        return false; \
    } \
    return true; \
}

/// @brief Генератор методов упаковки/распаковки
#define _CMD_PARAM_PACK_UNPACK(...) \
_CMD_PARAM_PACK(__VA_ARGS__) \
_CMD_PARAM_UNPACK(__VA_ARGS__)

/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 0
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_0 \
_CMD_PARAM_FROM_TUPLE_SIGN {} \
_CMD_PARAM_TO_TUPLE_SIGN {}


/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 1
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_1(m1) \
_CMD_PARAM_FROM_TUPLE_SIGN \
{ \
    m1 = std::get<0>(tuple); \
} \
_CMD_PARAM_TO_TUPLE_SIGN \
{ \
    std::get<0>(tuple) = m1; \
}

/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 2
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_2(m1, m2) \
_CMD_PARAM_FROM_TUPLE_SIGN \
{ \
    m1 = std::get<0>(tuple); \
    m2 = std::get<1>(tuple); \
} \
_CMD_PARAM_TO_TUPLE_SIGN \
{ \
    std::get<0>(tuple) = m1; \
    std::get<1>(tuple) = m2; \
}

/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 3
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_3(m1, m2, m3) \
_CMD_PARAM_FROM_TUPLE_SIGN \
{ \
    m1 = std::get<0>(tuple); \
    m2 = std::get<1>(tuple); \
    m3 = std::get<2>(tuple); \
} \
_CMD_PARAM_TO_TUPLE_SIGN \
{ \
    std::get<0>(tuple) = m1; \
    std::get<1>(tuple) = m2; \
    std::get<2>(tuple) = m3; \
}

/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 4
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_4(m1, m2, m3, m4) \
_CMD_PARAM_FROM_TUPLE_SIGN \
{ \
    m1 = std::get<0>(tuple); \
    m2 = std::get<1>(tuple); \
    m3 = std::get<2>(tuple); \
    m4 = std::get<3>(tuple); \
} \
_CMD_PARAM_TO_TUPLE_SIGN \
{ \
    std::get<0>(tuple) = m1; \
    std::get<1>(tuple) = m2; \
    std::get<2>(tuple) = m3; \
    std::get<3>(tuple) = m4; \
}

/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 5
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_5(m1, m2, m3, m4, m5) \
_CMD_PARAM_FROM_TUPLE_SIGN \
{ \
    m1 = std::get<0>(tuple); \
    m2 = std::get<1>(tuple); \
    m3 = std::get<2>(tuple); \
    m4 = std::get<3>(tuple); \
    m5 = std::get<4>(tuple); \
} \
_CMD_PARAM_TO_TUPLE_SIGN \
{ \
    std::get<0>(tuple) = m1; \
    std::get<1>(tuple) = m2; \
    std::get<2>(tuple) = m3; \
    std::get<3>(tuple) = m4; \
    std::get<4>(tuple) = m5; \
}

/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 6
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_6(m1, m2, m3, m4, m5, m6) \
_CMD_PARAM_FROM_TUPLE_SIGN \
{ \
    m1 = std::get<0>(tuple); \
    m2 = std::get<1>(tuple); \
    m3 = std::get<2>(tuple); \
    m4 = std::get<3>(tuple); \
    m5 = std::get<4>(tuple); \
    m6 = std::get<5>(tuple); \
} \
_CMD_PARAM_TO_TUPLE_SIGN \
{ \
    std::get<0>(tuple) = m1; \
    std::get<1>(tuple) = m2; \
    std::get<2>(tuple) = m3; \
    std::get<3>(tuple) = m4; \
    std::get<4>(tuple) = m5; \
    std::get<5>(tuple) = m6; \
}

/// @brief Генератор методов загрузки/выгрузки данныз в кортеж. Размернсть 7
#define _CMD_PARAM_TUPLE_LOAD_UNLOAD_7(m1, m2, m3, m4, m5, m6, m7) \
_CMD_PARAM_FROM_TUPLE_SIGN \
{ \
    m1 = std::get<0>(tuple); \
    m2 = std::get<1>(tuple); \
    m3 = std::get<2>(tuple); \
    m4 = std::get<3>(tuple); \
    m5 = std::get<4>(tuple); \
    m6 = std::get<5>(tuple); \
    m7 = std::get<6>(tuple); \
} \
_CMD_PARAM_TO_TUPLE_SIGN \
{ \
    std::get<0>(tuple) = m1; \
    std::get<1>(tuple) = m2; \
    std::get<2>(tuple) = m3; \
    std::get<3>(tuple) = m4; \
    std::get<4>(tuple) = m5; \
    std::get<5>(tuple) = m6; \
    std::get<6>(tuple) = m7; \
}


/// @brief Генератор структуры с без параметров
#define _CMD_PARAM_DEF_STRUCT_0(_cmd) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_META_0 \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    _NL \
    _CMD_PARAM_FLDS_0 \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_TPL(_cmd) \
    _CMD_PARAM_PACK_SIGN { return {}; } \
    _CMD_PARAM_UNPACK_SIGN { return true; } \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_0 \
}; \
_CMD_PARAM_T(_cmd)

/// @brief Генератор структуры с 1 параметром
#define _CMD_PARAM_DEF_STRUCT_1(lim, _cmd, t1, m1) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    lim \
    _CMD_PARAM_FLDS_1(t1, m1) \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_1(_cmd, t1, m1) \
    _CMD_PARAM_PACK_UNPACK(m1) \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_1(m1) \
}; \
_CMD_PARAM_T(_cmd)

/// @brief Генератор структуры с 2 параметрами
#define _CMD_PARAM_DEF_STRUCT_2(lim, _cmd, t1, m1, t2, m2) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    lim \
    _CMD_PARAM_FLDS_2(t1, m1, t2, m2) \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_2(_cmd, t1, m1, t2, m2) \
    _CMD_PARAM_PACK_UNPACK(m1, m2) \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_2(m1, m2) \
}; \
_CMD_PARAM_T(_cmd)

/// @brief Генератор структуры с 3 параметрами
#define _CMD_PARAM_DEF_STRUCT_3(lim, _cmd, t1, m1, t2, m2, t3, m3) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    lim \
    _CMD_PARAM_FLDS_3(t1, m1, t2, m2, t3, m3) \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_3(_cmd, t1, m1, t2, m2, t3, m3) \
    _CMD_PARAM_PACK_UNPACK(m1, m2, m3) \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_3(m1, m2, m3) \
}; \
_CMD_PARAM_T(_cmd)

/// @brief Генератор структуры с 4 параметрами
#define _CMD_PARAM_DEF_STRUCT_4(lim, _cmd, t1, m1, t2, m2, t3, m3, t4, m4) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    lim \
    _CMD_PARAM_FLDS_4(t1, m1, t2, m2, t3, m3, t4, m4) \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_4(_cmd, t1, m1, t2, m2, t2, m3, t4, m4) \
    _CMD_PARAM_PACK_UNPACK(m1, m2, m3, m4) \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_4(m1, m2, m3, m4) \
}; \
_CMD_PARAM_T(_cmd)

/// @brief Генератор структуры с 5 параметрами
#define _CMD_PARAM_DEF_STRUCT_5(lim, _cmd, t1, m1, t2, m2, t3, m3, t4, m4, t5, m5) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    lim \
    _CMD_PARAM_FLDS_5(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5) \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_5(_cmd, t1, m1, t2, m2, t2, m3, t4, m4, t5, m5) \
    _CMD_PARAM_PACK_UNPACK(m1, m2, m3, m4, m5) \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_5(m1, m2, m3, m4, m5) \
}; \
_CMD_PARAM_T(_cmd)

/// @brief Генератор структуры с 6 параметрами
#define _CMD_PARAM_DEF_STRUCT_6(lim, _cmd, t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    lim \
    _CMD_PARAM_FLDS_6(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6) \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_6(_cmd, t1, m1, t2, m2, t2, m3, t4, m4, t5, m5, t6, m6) \
    _CMD_PARAM_PACK_UNPACK(m1, m2, m3, m4, m5, m6) \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_6(m1, m2, m3, m4, m5, m6) \
}; \
_CMD_PARAM_T(_cmd)

/// @brief Генератор структуры с 7 параметрами
#define _CMD_PARAM_DEF_STRUCT_7(lim, _cmd, t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6, t7, m7) \
struct _CMD_PARAM_NAME(_cmd) \
{ \
    _CMD_PARAM_DECL_STATIC_CMD(_cmd) \
    lim \
    _CMD_PARAM_FLDS_7(t1, m1, t2, m2, t3, m3, t4, m4, t5, m5, t6, m6, t7, m7) \
    _CMD_PARAM_CTR_DFLT(_cmd) \
    _CMD_PARAM_CTR_7(_cmd, t1, m1, t2, m2, t2, m3, t4, m4, t5, m5, t6, m6, t7, m7) \
    _CMD_PARAM_PACK_UNPACK(m1, m2, m3, m4, m5, m6, m7) \
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_7(m1, m2, m3, m4, m5, m6, m7) \
}; \
_CMD_PARAM_T(_cmd)

//=====================================================================

/**
 * @name Создание структур параметров команд управления
 * @def CmdParam_...
 * @brief  Создание структур параметров команд управления под каждую команду
 * @details Создаются структуры следующего вида
 * (пример: условная команда Goen220::Cmd::CMD_X параметрами bool, int, int,
 *  где для параметров int существуют допустимый диапазон значений 1..42)
 *
 *  _CMD_PARAM_DEF_STRUCT_2(_L(1, 42), CMD_X, bool, param_0, int, param_1, int param_2);
 *
 *  =>
 *
 * struct CmdParam_CMD_X
 * {
 *     struct Meta {
 *        using Types = std::tuple<bool, int, int>;
 *        static constexpr std::size_t N = std::tuple_size_v<Types>;
 *        static constexpr const char*names[N] = {"param_0", "param_1", "param_2"};
 *     };
 *
 *     static constexpr Goen220::Cmd cmd = Goen220::Cmd::CMD_X;
 *     static constexpr bool has_limits = true;
 *     static constexpr int min = 1;
 *     static constexpr int max = 42;
 *
 *     bool param_0 = (bool) 0;
 *     int param_1 = (int) 0;
 *     int param_2 = (int) 0;
 *
 *     CmdParam_CMD_X() = default;
 *     CmdParam_CMD_X(bool _param_0, int _param_1, int _param_2) {...}
 *     CmdParam_CMD_X(const Meta::Types &tuple) {...}
 *
 *     QByteArray pack() const {...}
 *     bool unpack(QByteArray &to) {...}
 *
 *     void from(const Meta::Types &tuple) {...}
 *     void to(Meta::Types &tuple) {...}
 * };
 *
 *  Так же доступена шаблонная структура CmdParamTy<Cmd cmd>
 *  и специализация для каждой Cmd вида
 *
 *  template<>
 *  struct CmdParamTy<Goen220::CMD_X> {
 *     using type = CmdParam_CMD_X;
 *  }
 *
 *  Так же доступны два шаблонных псевдонима
 *
 *  template <Goen220::Cmd cmd>
 *  CmdParamType = CmdParamTy<cmd>::type;
 *
 *  template <Goen220::Cmd cmd>
 *  CmdParamMeta = CmdParamTy<cmd>::type::Meta;
 *
 *  @note структура параметров для TR_EN_4 имеет резеврное поле, не отображаемое в конструкторе и Meta::Types
 *  @{
 */

_CMD_PARAM_DEF_STRUCT_2(_NL, CH_ENABLE, CHANNEL, channel, bool, enabled);
_CMD_PARAM_DEF_STRUCT_2(_NL, CH_MAN_ZOOM_IN, CHANNEL, channel, quint8, speed);
_CMD_PARAM_DEF_STRUCT_2(_NL, CH_MAN_ZOOM_OUT, CHANNEL, channel, quint8, speed);
_CMD_PARAM_DEF_STRUCT_1(_NL, CH_MAN_ZOOM_STOP, CHANNEL, channel);
_CMD_PARAM_DEF_STRUCT_2(_NL, CH_MAN_ZOOM_POSITION, CHANNEL, channel, quint16, pos);
_CMD_PARAM_DEF_STRUCT_2(_NL, CH_MAN_FOCUS_NEAR, CHANNEL, channel, quint8, speed);
_CMD_PARAM_DEF_STRUCT_2(_NL, CH_MAN_FOCUS_FAR, CHANNEL, channel, quint8, speed);
_CMD_PARAM_DEF_STRUCT_1(_NL, CH_MAN_FOCUS_STOP, CHANNEL, channel);
_CMD_PARAM_DEF_STRUCT_2(_NL, CH_MAN_FOCUS_POSITION, CHANNEL, channel, quint16, pos);
_CMD_PARAM_DEF_STRUCT_1(_NL, CH_FIELD_SYNC, CHANNEL, channel);
_CMD_PARAM_DEF_STRUCT_2(_NL, CH_REGULAR_FSYNC, CHANNEL, channel, bool, enabled);
_CMD_PARAM_DEF_STRUCT_0(CH_SYNC_NARROW_FIELDS);

_CMD_PARAM_DEF_STRUCT_1(_NL, LFR_RADIATION, LFR_MODE, mode);
_CMD_PARAM_DEF_STRUCT_1(_NL, LFR_POWER_ON, bool, enabled);
_CMD_PARAM_DEF_STRUCT_1(_L(quint8, 1, 30), LFR_FREQ, quint8, freq);

_CMD_PARAM_DEF_STRUCT_1(_NL, GYRO_ARR, bool, set);
_CMD_PARAM_DEF_STRUCT_1(_NL, GYRO_STAB_EN, bool, enabled);
_CMD_PARAM_DEF_STRUCT_1(_NL, GYRO_POS, GYRO_DEF_POS, pos);
_CMD_PARAM_DEF_STRUCT_2(_L(float, -60.0f, 60.0f), GYRO_SPEED_DEG, float, speedZ, float, speedX);
_CMD_PARAM_DEF_STRUCT_2(_L(qint16, -179, 179), GYRO_RELATIVE_ANGLE, qint16, angZ, qint16, angX);
_CMD_PARAM_DEF_STRUCT_2(_L(float, -179.0f, 179.0f), GYRO_ABS_ANGLE_DEG, float, angZ, float, angX);

_CMD_PARAM_DEF_STRUCT_5(_L(quint16, 0, 1920),
                        TR_EN_SINGLE,
                        CHANNEL,
                        channel,
                        quint16,
                        centerX,
                        quint16,
                        centerY,
                        quint16,
                        width,
                        quint16,
                        height);
_CMD_PARAM_DEF_STRUCT_0(TR_DIS_SINGLE);
_CMD_PARAM_DEF_STRUCT_1(_NL, TR_CHS, TR_ID, id);

struct _CMD_PARAM_NAME(TR_EN_4)
{
    _CMD_PARAM_DECL_STATIC_CMD(TR_EN_4)
    _L(quint16, 0, 1920)
    _CMD_PARAM_FLDS_7(CHANNEL,
                      channel,
                      TR_ID,
                      id,
                      quint8,
                      rsrv,
                      quint16,
                      centerX,
                      quint16,
                      centerY,
                      quint16,
                      width,
                      quint16,
                      height)
    _CMD_PARAM_CTR_DFLT(TR_EN_4)
    _CMD_PARAM_CTR_6(TR_EN_4,
                     CHANNEL,
                     channel,
                     TR_ID,
                     id,
                     quint16,
                     centerX,
                     quint16,
                     centerY,
                     quint16,
                     width,
                     quint16,
                     height)
    _CMD_PARAM_PACK_UNPACK(channel, id, rsrv, centerX, centerY, width, height)
    _CMD_PARAM_TUPLE_LOAD_UNLOAD_6(channel, id, centerX, centerY, width, height)
};
_CMD_PARAM_T(TR_EN_4);

_CMD_PARAM_DEF_STRUCT_1(_NL, TR_DIS, TR_ID, id);

_CMD_PARAM_DEF_STRUCT_1(_NL, SRV_ALIGNMENT, quint16, id);

_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_FlickerReduction, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_BackLight, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_Gamma, EVCAM_GAMMA, mode);
_CMD_PARAM_DEF_STRUCT_1(_L(qint8, -64, 64), EVCAM_GammaOffset, qint8, offset);
_CMD_PARAM_DEF_STRUCT_0(EVCAM_ExExpComp_Reset);
_CMD_PARAM_DEF_STRUCT_1(_L(quint8, 0x01, 0x7F), EVCAM_ExExpComp_UP, quint8, step);
_CMD_PARAM_DEF_STRUCT_1(_L(quint8, 0x01, 0x7F), EVCAM_ExExpComp_DOWN, quint8, step);
_CMD_PARAM_DEF_STRUCT_1(_L(quint8, 0x01, 0x7F), EVCAM_ExExpComp_DIRECT, quint8, step);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_Stabilizer, EVCAM_STABILIZER, stabilizer);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_Stabilizer_Level, EVCAM_STABILIZER_LEVEL, level);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_HighSensitivity, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_Defog, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_WideD, BOOL, on);
_CMD_PARAM_DEF_STRUCT_3(_NL,
                        EVCAM_WideD_SetParameter,
                        EVCAM_HDR_BRTNSS,
                        brightness,
                        EVCAM_HDR_BRTNSS_COMP_LVL,
                        brightnessCompLevel,
                        EVCAM_HDR_COMP_LVL,
                        compLevel);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_NR, EVCAM_NR_LVL, level);
_CMD_PARAM_DEF_STRUCT_2(_NL, EVCAM_2NR_3NR, EVCAM_NR_LVL, level2D, EVCAM_NR_LVL, level3D);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_HLC, quint8, value);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_ICR, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_AutoICR, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_AutoICR_Threshold, quint8, level);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_MinShutter, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_L(quint8, 0x05, 0x14), EVCAM_MinShutter_Limit, quint8, lim);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_PictureFlip, BOOL, on);
_CMD_PARAM_DEF_STRUCT_1(_NL, EVCAM_AutoFocus, EVCAM_FOCUS_MODE, mode);

_CMD_PARAM_DEF_STRUCT_1(_L(quint32, 0, 92583), Mini640_INT_PERIOD_TIME, quint32, value);
_CMD_PARAM_DEF_STRUCT_1(_L(quint32, 0, 92583), Mini640_INT_HIGH_LEVEL, quint32, level);
_CMD_PARAM_DEF_STRUCT_1(_NL, Mini640_FPA_CONF, MINI640_FPA_CONF, mode);
_CMD_PARAM_DEF_STRUCT_1(_NL, Mini640_COOLER_EN, BOOL, on);

/** @} */

} // namespace Goen220
