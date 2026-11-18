#include "tlm_data_model_impl.h"
#include "goen220_tlm.h"
#include "tlm_data_category_filter_model.h"

#include <type_traits>
#include <QDebug>

#include <algorithm>

namespace Goen220 {

/**
 * @brief Шаблонная по типу T функция преобразования бинарных данных к QVariant
 * @param ptr указатель на бинарные данные
 * @return QVariant по типу T
 */
template<typename T>
static QVariant toVariant(const void *ptr)
{
    return *(const T*)ptr;
}

template<typename T, size_t sz>
static void fromVariant(void *dst, const QVariant &src)
{
    auto v = src.value<T>();
    std::memcpy(dst, &v, sz);
}

/// Класс доступа к бинарным данным структуры Telemetry
class TlmData {

public:

    /// Тип указателя на функцию преобразования бинарных данных
    using ToVariantFptr = QVariant (*)(const void*);

    /// Тип указателя на функцию преобразования бинарных данных
    using FromVariantFptr = void (*)(void *, const QVariant &);

    /// Структура описания поля в структуре Telemetry
    struct FieldItem {

        /// Категория поля по умолчанию (используется при создании новых FieldItem)
        static TlmDataModel::Category s_defaultCat;

        /**
         * @brief Конструктор
         * @param _offset смещение до поля
         * @param _cnvTo Указатель на функцию преобразования в QVariant
         * @param _cnvFrom Указатель на функцию преобразования из QVariant
         */
        FieldItem(size_t _offset, ToVariantFptr _cnvTo, FromVariantFptr _cnvFrom)
            : offset {_offset}
            , cnvToVar {_cnvTo}
            , cnvFromVar {_cnvFrom}
            , cat {s_defaultCat}
        {}

        /**
         * @brief Устанавливает имя поля, отображаемое в модели данных
         * @param _name имя поля
         * @return ссылку на this
         */
        FieldItem& setName(const QString &_name)
        {
            name = _name;
            return *this;
        }

        // bool operator < (const FieldItem &other)
        // {
        //     return offset < other.offset;
        // }

        size_t offset = 0;          ///< Смещение до поля
        ToVariantFptr cnvToVar = nullptr;      ///< Указатель на функцию преобразования в QVariant
        FromVariantFptr cnvFromVar = nullptr; ///< Указатель на функцию преобразования из QVariant
        QString name;               ///< Имя поля
        TlmDataModel::Category cat;   ///< Категория
    };

    /// @brief вспомогательная структура для выделения базового типа по типу указателя на объект член класса
    /// обобщенныый вариант шаблона
    template<typename T>
    struct _member_type_info;

    /// @brief вспомогательная структура для выделения базового типа по типу указателя на объект член класса
    /// рабочая специализация
    template<typename T, typename M>
    struct _member_type_info<M T::*> {
        using type = M;
        using class_type = T;
    };

    using CnvFptr = std::tuple<ToVariantFptr, FromVariantFptr>;

    /**
     * @brief Возвращает указатель на функцию преобразования для последнего поля структуры на которое ссылается список аргументов
     * @details
     * - Требует список аргументов совместимый с вызово функции Goen220::Utils::offset
     * - Выводит тип последнего поля структуры (либо прямое указание, либо элемента массива)
     * - Требует, что требуемый элемент (поле) имеел фундаментальный тип данных)
     * @return указатель на конкретную реализацию функции toVariant по выведеному типу
     */
    template<typename... Args>
        requires Goen220::Utils::CanCallOffset<Args...>
    static CnvFptr getCnvFunc(Args...)
    {
        constexpr auto N = sizeof...(Args);
        static_assert(N > 0, "Arguments required!!!!");

        using args_tuple_t = std::tuple<Args...>;
        using last_arg_t = std::tuple_element_t<N - 1, args_tuple_t>;

        if constexpr (std::is_member_object_pointer_v<last_arg_t>) {
            using member_type = _member_type_info<last_arg_t>::type;
            static_assert(std::is_fundamental_v<member_type>, "last member object pointer must be of fundamental type");
            return {toVariant<member_type>, fromVariant<member_type, sizeof(member_type)>};
        }

        if constexpr (std::is_integral_v<last_arg_t>) {
            static_assert(N > 1, "last arg is an array index. second to last argument required");
            using second_last_arg_t = std::tuple_element_t<N - 2, args_tuple_t>;

            static_assert(std::is_member_object_pointer_v<second_last_arg_t>, "second to last argument is not a member object");

            if constexpr (std::is_member_object_pointer_v<second_last_arg_t>) {
                using member_2_type = _member_type_info<second_last_arg_t>::type;
                static_assert(std::is_array_v<member_2_type>, "second to last argument must be of array type");
                using a_type = std::remove_extent_t<member_2_type>;
                static_assert(std::is_fundamental_v<a_type>, "fundamental type array required");
                return {toVariant<a_type>, fromVariant<a_type, sizeof(a_type)>};
            }
        }

        throw std::invalid_argument("incompatible argument pack");
    }

    /**
     * @brief Создает и добавляет в список структуру описания поля
     * @param args пак аругментов описания поля (@see Goen220::Utils::offset)
     * @return ссылка на добавленную структуру
     */
    template<typename... Args>
        requires Goen220::Utils::CanCallOffset<Args...>
    FieldItem &appendItem(Args... args)
    {
        auto o = Utils::offset(args...);
        auto [to, from] = getCnvFunc(args...);
        m_fields.append({o, to, from});

        return m_fields.last();
    }

    int fieldNumber() const {return m_fields.size();}


    void setValue(Tlm* tlm, int fi, const QVariant &value)
    {
        if (fi < 0 || fi >= fieldNumber())
            return;

        m_fields[fi].cnvFromVar((void*)((const quint8*)tlm + m_fields[fi].offset), value);
    }

    void setValue(int fi, const QVariant &value)
    {
        setValue(&m_tlm, fi, value);
    }

    QVariant value(const Tlm *tlm, int fi) const
    {
        if (fi < 0 || fi >= fieldNumber())
            return {};

        return m_fields[fi].cnvToVar((const void*)((const quint8*)tlm + m_fields[fi].offset));
    }


    QVariant value(int fi) const
    {
        return value(&m_tlm, fi);
    }

    const QString& name(int fi) const
    {
        static const QString stub;
        if (fi < 0 || fi >= fieldNumber())
            return stub;

        return m_fields[fi].name;
    }

    TlmDataModel::Category category(int fi) const
    {
        if (fi < 0 || fi >= fieldNumber())
            return TlmDataModel::Category::UNDEF;

        return m_fields[fi].cat;
    }

    void setRawData(const Tlm *raw)
    {
        std::memcpy(&m_tlm, raw, sizeof(Tlm));
    }

    int row(int offset)
    {
        int left = 0;
        int right = m_fields.size() - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            int co = m_fields[mid].offset;
            if (co == offset)
                return mid;
            else if (co < offset)
                left = mid + 1;
            else
                right = mid - 1;
        }

        return (right >= 0 && (m_fields[right].offset == offset)) ? right : -1;
    }

    void sort() { std::ranges::sort(m_fields, std::less{}, &FieldItem::offset); }

    const Tlm& rawData() const {return m_tlm;}

    Tlm& rawData() {return m_tlm;}

private:
    QList<FieldItem> m_fields;
    Tlm m_tlm;
};

TlmDataModel::Category TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::UNDEF;

//============================================================================================

TlmDataModelImpl::TlmDataModelImpl(bool editable, QObject *parent)
    : TlmDataModel(parent)
    , m_data{new TlmData()}
    , m_eflag (editable ? Qt::ItemIsEditable : Qt::NoItemFlags)
{
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::TV_CHAN;
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::SENSOR_STATUS).setName("TV SENSOR_STATUS");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::LENS_STATUS).setName("TV LENS_STATUS");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::FPS).setName("TV FPS");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::VRES).setName("TV VRES");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::HRES).setName("TV HRES");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::EXP).setName("TV EXP");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::ZOOM_DRIVE).setName("TV ZOOM_DRIVE");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::FOCUS_DRIVE).setName("TV FOCUS_DRIVE");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::LENS_TEMP).setName("TV LENS_TEMP");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::FOCUS_MM).setName("TV FOCUS_MM");
    m_data->appendItem(&Tlm::TV, &Tlm::Chan::FIELD_GRAD).setName("TV FIELD_GRAD");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::IR_CHAN;
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::SENSOR_STATUS).setName("MW SENSOR_STATUS");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::LENS_STATUS).setName("MW LENS_STATUS");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::FPS).setName("MW FPS");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::VRES).setName("MW VRES");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::HRES).setName("MW HRES");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::EXP).setName("MW EXP");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::ZOOM_DRIVE).setName("MW ZOOM_DRIVE");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::FOCUS_DRIVE).setName("MW FOCUS_DRIVE");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::LENS_TEMP).setName("MW LENS_TEMP");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::FOCUS_MM).setName("MW FOCUS_MM");
    m_data->appendItem(&Tlm::MW, &Tlm::Chan::FIELD_GRAD).setName("MW FIELD_GRAD");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::RF;
    m_data->appendItem(&Tlm::RF, &Tlm::Rf::STATUS).setName("RF STATUS");
    m_data->appendItem(&Tlm::RF, &Tlm::Rf::MEAS_STATE).setName("RF MEAS_STATE");
    m_data->appendItem(&Tlm::RF, &Tlm::Rf::LAST_DISTANCE).setName("RF LAST_DISTANCE");
    m_data->appendItem(&Tlm::RF, &Tlm::Rf::AVG_DISTANCE).setName("RF AVG_DISTANCE");
    m_data->appendItem(&Tlm::RF, &Tlm::Rf::RMS_DISTANCE).setName("RF RMS_DISTANCE");
    m_data->appendItem(&Tlm::RF, &Tlm::Rf::STATE).setName("RF STATE");
    m_data->appendItem(&Tlm::RF, &Tlm::Rf::BDT).setName("RF BDT");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::GSP;
    m_data->appendItem(&Tlm::GSP, &Tlm::Gsp::STATUS).setName("GSP STATUS");
    m_data->appendItem(&Tlm::GSP, &Tlm::Gsp::MODE).setName("GSP MODE");
    m_data->appendItem(&Tlm::GSP, &Tlm::Gsp::POS_Y).setName("GSP POS_Y");
    m_data->appendItem(&Tlm::GSP, &Tlm::Gsp::POS_Z).setName("GSP POS_Z");
    m_data->appendItem(&Tlm::GSP, &Tlm::Gsp::ANGLE_Y).setName("GSP ANGLE_Y");
    m_data->appendItem(&Tlm::GSP, &Tlm::Gsp::SPEED_Y).setName("GSP SPEED_Y");
    m_data->appendItem(&Tlm::GSP, &Tlm::Gsp::SPEED_Z).setName("GSP SPEED_Z");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::CTRL;
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::AF_STATE).setName("AF_STATE");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::REGULAR_FIELD_SYNC_STATE).setName("REGULAR_FIELD_SYNC_STATE");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TRACK_STATE).setName("TRACK_STATE");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TARGET_STATE).setName("TARGET_STATE");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TRK_STR_X).setName("TRK_STR_X");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TRK_STR_Y).setName("TRK_STR_Y");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TRK_STR_XSZ).setName("TRK_STR_XSZ");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TRK_STR_YSZ).setName("TRK_STR_YSZ");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TARGET_Z).setName("TARGET_Z");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TARGET_X).setName("TARGET_X");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TARGET_Z_SZ).setName("TARGET_Z_SZ");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::TARGET_X_SZ).setName("TARGET_X_SZ");
    m_data->appendItem(&Tlm::CTRL, &Tlm::Ctrl::ETH_STATUS).setName("ETH_STATUS");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::SRVC;
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ID_YEAR).setName("ID_YEAR");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ID_MONTH).setName("ID_MONTH");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ID_ID).setName("ID_ID");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ID_HARD_VER).setName("ID_HARD_VER");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ID_SOFT_VER).setName("ID_SOFT_VER");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ID_HOURS).setName("ID_HOURS");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ID_MINUTES).setName("ID_MINUTES");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ALIGNMENT_POS_Y).setName("ALIGNMENT_POS_Y");
    m_data->appendItem(&Tlm::SRVC, &Tlm::Srvc::ALIGNMENT_POS_Z).setName("ALIGNMENT_POS_Z");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::TRACKING;
    m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TRACKER_CHANNEL).setName("TRACKER_CHANNEL");
    m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET_ID).setName("TARGET_ID");
    for (size_t i = 1; i <= 4; i++) {
        auto idx = i - 1;
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::STATE).setName(QString("TARGET %1 STATE").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::X).setName(QString("TARGET %1 X").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::Y).setName(QString("TARGET %1 Y").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::X_SZ).setName(QString("TARGET %1 X_SZ").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::YS_Z).setName(QString("TARGET %1 YS_Z").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::A_Z).setName(QString("TARGET %1 A_Z").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::A_X).setName(QString("TARGET %1 A_X").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::A_Z_SZ).setName(QString("TARGET %1 A_Z_SZ").arg(i));
        m_data->appendItem(&Tlm::TRK, &Tlm::Trk::TARGET, idx, &Tlm::Target::A_X_SZ).setName(QString("TARGET %1 A_X_SZ").arg(i));
    }
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::EVCAM;
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::FlickerReduction).setName("EVCAM FlickerReduction");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::BackLight).setName("EVCAM BackLight");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::Gamma).setName("EVCAM Gamma");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::GammaOffset).setName("EVCAM GammaOffset");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::ExExpComp_Pos).setName("EVCAM ExExpComp_Pos");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::Stabilizer).setName("EVCAM Stabilizer");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::Stabilizer_Level).setName("EVCAM Stabilizer_Level");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::HighSensitivity).setName("EVCAM HighSensitivity");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::Defog).setName("EVCAM Defog");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::WideD).setName("EVCAM WideD");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::WideD_Parameter, 0).setName("EVCAM WideD_Parameter 0");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::WideD_Parameter, 1).setName("EVCAM WideD_Parameter 1");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::WideD_Parameter, 2).setName("EVCAM WideD_Parameter 2");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::NR).setName("EVCAM NR");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::_2NR_3NR, 0).setName("EVCAM 2NR_3NR 0");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::_2NR_3NR, 1).setName("EVCAM 2NR_3NR 1");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::HLC).setName("EVCAM HLC");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::ICR).setName("EVCAM ICR");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::AutoICR_Threshold).setName("EVCAM AutoICR_Threshold");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::PictureFlip).setName("EVCAM PictureFlip");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::AutoICR).setName("EVCAM AutoICR");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::Autofocus).setName("EVCAM Autofocus");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::MinShutter).setName("EVCAM MinShutter");
    m_data->appendItem(&Tlm::EVCAM, &Tlm::Evcam::MinShutter_Limit).setName("EVCAM MinShutter_Limit");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::MINI640;
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::INT_PERIOD_TIME).setName("MINI640 INT_PERIOD_TIME");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::INT_HIGH_LEVEL).setName("MINI640 INT_HIGH_LEVEL");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::FPA_CONF).setName("MINI640 FPA_CONF");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::PCBIT_TSTAT_PSTAT).setName("MINI640 PCBIT_TSTAT_PSTAT");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::MONITOR_REF25V).setName("MINI640 MONITOR_REF25V");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::MONITOR_REF33V).setName("MINI640 MONITOR_REF33V");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::MONITOR_REF5V_N4V).setName("MINI640 MONITOR_REF5V_N4V");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::CCE_VERSIONSFT).setName("MINI640 CCE_VERSIONSFT");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::CCE_VERSIONFPGA).setName("MINI640 CCE_VERSIONFPGA");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::COOLER_EN).setName("MINI640 COOLER_EN");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::COOLER_TIMER).setName("MINI640 COOLER_TIMER");
    m_data->appendItem(&Tlm::MINI640, &Tlm::Mini640::IRSensT).setName("MINI640 IRSensT");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::CH_EXT_ST;
    m_data->appendItem(&Tlm::CH_EXT_ST, &Tlm::ChExtSt::FCB_EV9500M, 0).setName("FCB_EV9500M 0");
    m_data->appendItem(&Tlm::CH_EXT_ST, &Tlm::ChExtSt::FCB_EV9500M, 1).setName("FCB_EV9500M 1");
    m_data->appendItem(&Tlm::CH_EXT_ST, &Tlm::ChExtSt::MINI640_IDDCA, 0).setName("MINI640 IDDCA 0");
    m_data->appendItem(&Tlm::CH_EXT_ST, &Tlm::ChExtSt::MINI640_IDDCA, 1).setName("MINI640 IDDCA 1");
    TlmData::FieldItem::s_defaultCat = TlmDataModel::Category::LDC;
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::EMIT_STATE_CODE).setName("LDC_EMIT_STATE_CODE");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::RAD_TC_STATE_CODE).setName("LDC_RAD_TC_STATE_CODE");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::LD_TC_STATE_CODE).setName("LDC_LD_TC_STATE_CODE");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::TEMP_POWER_SW).setName("LDC_TEMP_POWER_SW");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::TEMP_RAD).setName("LDC_TEMP_RAD");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::TEMP_LD).setName("LDC_TEMP_LD");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::RAD_HEATER_CURR).setName("LDC_RAD_HEATER_CURR");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::VOLTAGE).setName("LDC_VOLTAGE");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::TEM_LD_CURR).setName("LDC_TEM_LD_CURR");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::TEM_LD_VOLTAGE_DROP).setName("LDC_TEM_LD_VOLTAGE_DROP");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::TEMP_FD).setName("LDC_TEMP_FD");
    m_data->appendItem(&Tlm::LDC20, &Tlm::Ldc20::Frequency_LRF).setName("Frequency_LRF");

    m_data->sort();
}

TlmDataModelImpl::~TlmDataModelImpl()
{
    delete m_data;
}

TlmDataModel::Category TlmDataModelImpl::category(int r) const
{
    return m_data->category(r);
}

QVariant TlmDataModelImpl::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal) {
        if (section == 0)
            return "ПОЛЕ";
        if (section == 1)
            return "ЗНАЧЕНИЕ";
    }
    return {};
}

int TlmDataModelImpl::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_data->fieldNumber();
}

int TlmDataModelImpl::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

bool TlmDataModelImpl::hasChildren(const QModelIndex &parent) const
{
    return false;
}

QVariant TlmDataModelImpl::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (role == Qt::DisplayRole) {
        if (index.column() == (int)Col::NAME)
            return m_data->name(index.row());
        if (index.column() == (int)Col::VALUE)
            return m_data->value(index.row());
    }
    return {};
}

Qt::ItemFlags TlmDataModelImpl::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == (int)Col::VALUE)
        return QAbstractItemModel::flags(index) | m_eflag;

    return QAbstractItemModel::flags(index);
}

bool TlmDataModelImpl::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() != (int)Col::VALUE)
        return false;

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return false;

    if (index.row() >= m_data->fieldNumber())
        return false;

    m_data->setValue(index.row(), value);

    return true;
}

void TlmDataModelImpl::setRaw(const Tlm *tlm)
{
    QList<int> changes;

    for(int r = 0; r < m_data->fieldNumber(); r++) {
        auto v_a = m_data->value(r);
        auto v_b = m_data->value(tlm, r);

        if (v_a != v_b) {
            changes.append(r);
            // qDebug() << " " << m_data->name(r) << "old:" << v_a.toString() << "new" << v_b.toString();
        }
    }

    if (changes.isEmpty())
        return;

    m_data->setRawData(tlm);

    auto emitDataChanges = [this](int s, int e) {
        auto tl = createIndex(s, (int)Col::VALUE);
        auto br = createIndex(e, (int)Col::VALUE);
        emit dataChanged(tl, br, {Qt::DisplayRole});
    };

    int si = 0;
    for (int i = 1; i < changes.size(); i++) {
        if ((changes[i] - (changes[i - 1])) != 1 ) {
            emitDataChanges(si, i - 1);
            si = i;
        }
    }

    emitDataChanges(si, changes.size() - 1);
}

const Tlm &TlmDataModelImpl::rawData() const
{
    return m_data->rawData();
}

Tlm &TlmDataModelImpl::rawData()
{
    return m_data->rawData();
}

QModelIndex TlmDataModelImpl::index(int row, Col col) const
{
    return QAbstractTableModel::index(row, (int)col);
}

int TlmDataModelImpl::row(size_t offset) const
{
    return m_data->row(offset);
}

QPointer<QAbstractItemModel> TlmDataModelImpl::createCategoryFilterModel(Category cat)
{
    return new TlmDataCategoryFilterModel(cat, this);
}

//==================================

TlmDataModel* TlmDataModel::createModel(QObject *parent = nullptr)
{
    return new TlmDataModelImpl(false, parent);
}

TlmDataModel* TlmDataModel::createEditableModel(QObject *parent = nullptr)
{
    return new TlmDataModelImpl(true, parent);
}

} // namespace Goen220
