#ifndef VDB_VRT_TRACKER_ADAPTER_FUNC_HPP
#define VDB_VRT_TRACKER_ADAPTER_FUNC_HPP

#include <cstdint>
#include <unordered_map>
#include "../../common_data_types.hpp"
#include "tracker_adapter_packed_types.hpp"
#include "tracker_adapter_def.hpp"

namespace vdb::tr_adapter
{

enum en_func_id : uint8_t
{
    FUNC_START_TRACKING                 = 0x01  ,
    FUNC_STOP_TRACKING                  = 0x02  ,
    FUNC_TRACKER_STATUS                 = 0x03  ,   /* автомат сопровождения отправляет на этот адрес выполненное предсказание положеняи цели */
    FUNC_LENS_STATUS                    = 0x04  ,   /* регулярное поступление телеметрии объектива текущего канала на этот адрес */
    
    FUNC_STATUS_HEAVY                   = 0xA0  ,   /* Запрос "тяжелого" блока телеметрии */
    FUNC_STATUS_SLIM                    = 0xA1  ,   /* запрос "облегченной" части телеметрии - только быстро меняющиеся (и связанные с ними) параметры */
    FUNC_STATUS_COMPLEX                 = 0xA2  ,   /* весь статус: heavy + slim */
};
} /* nms vdb::tr_adapter */

namespace vdb::tr_adapter::func
{
    namespace tracker_status
    {
        #pragma pack(push,1)
        //ткущие парметры углового поля и рамки трекера
        struct Pld
        {
            float sizeX{0.1f};      //размер рамки по X [относительные координаты][0;1]
            float sizeY{0.1f};      //размер рамки по Y [относительные координаты][0;1]
            float distX{0.1f};      //расстояние центра рамки от центра кадра по X [относительные координаты][-0.5;0.5]
            float distY{0.1f};      //расстояние центра рамки от центра кадра по Y [относительные координаты][-0.5;0.5]
            float trackerPrepareScoreNorm{0.0f};
            uint8_t enableSlowdownByPrepareScore{0x00};
        };
        #pragma pack(pop)
    } /* nms tracker_status */

    namespace lens_status
    {
        #pragma pack(push,1)
        struct Pld
        {
            LensStatusExtendCommon data;
        };
        #pragma pack(pop)
    } /* channel_telemetry */

    namespace status_heavy
    {
        #pragma pack(push,1)
        struct Pld
        {

        };
        #pragma pack(pop)
    } /* nms status_heavy */

    namespace status_slim
    {
        #pragma pack(push,1)
        struct Pld
        {

        };
        #pragma pack(pop)
    } /* nms status_slim */

    namespace status_complex
    {
        #pragma pack(push,1)    
        struct Pld
        {
            /* статусы рантайма адаптера */
            tr_adapter_common::en_tracker_adapter_state     state;                                                  // текущее состояние адаптера 
            int64_t                                         response_cntr;                                          // счетчик ответов       
            int64_t                                         timestamp_ms;                                           // текущая относительная метка времени 
            int64_t                                         cur_track_session_duration_ms;                          // продолжительность текущей сесии сопровождения 

            /* текущие настройки контура слежения (пресет) */
            tr_adapter_common::PresetDataPacked             cur_preset_data;

            /* приходящие регулярные статусы модулей извне */
            float                                           cur_rcv_tracker_status_update_interval_mean_us;         // средний интервал прихода данных от трекера  
            TrackerStatusCommon                             cur_rcv_tracker_status;                                 // последние поступпишие данные от трекера 
            float                                           cur_rcv_lens_status_update_interval_mean_us;            // средний интервал прихода данных от объектива 
            LensStatusExtendCommon                          cur_rcv_lens_status;                                    // последняя поступившая телеметрия объектива 
            
            /* выходные управляющие данные на платформу */
            float                                           cur_snd_gsp_speed_pitch;                                // последняя переданная упр. угл. скорость на платформу
            float                                           cur_snd_gsp_speed_yaw;     
            
            /* текущие PID */
            tr_adapter_common::PidCoefsXYPacked             cur_pid_res;                                            // результирующий pid вследствие воздействия поправок на pid_base
            tr_adapter_common::PidCoefsXYPacked             cur_pid_fovcorr;                                        // мультипликативная поправка pid по угловому полю
            tr_adapter_common::PidCoefsXYPacked             cur_pid_bboxszcorr;                                     // мультипликативная попправка pid по размеру рамки цели трекера 
            
            uint8_t reserve[8];
        }; /* Pld */
        #pragma pack(pop)
    } /* nms telemetry */

} /* vdb::tr_adapter::func */

#endif /* VDB_VRT_LW_CAM_PIPE_FUNC_HPP */