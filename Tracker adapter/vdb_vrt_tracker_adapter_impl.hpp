#ifndef ADAPTIVETRACKERADAPTERIMPL_HPP
#define ADAPTIVETRACKERADAPTERIMPL_HPP

#include "vdb_msg_dispatch.hpp"
#include "vdb_vrt_tracker_adapter_func.hpp"
#include "vdb_node.hpp"
#include "stdint.h"
#include "coefficientsLaws.hpp"
#include "simple_exponential_filter.hpp"
#include "pid_regulator/pid_regulator.hpp"
#include "tracker_adapter_def.hpp"
#include "vdb_vrt_tracker_adapter_preset_controller.hpp"
#include "time_tools/stopwatch_ts.hpp"
#include "counters/numeric_counter/num_counter_ts.hpp"
#include <memory>
#include <chrono>
#include "slewRatelimiter.hpp"


class TrackerAdapter: public VDBNode
{
public:
    struct Cfg
    {
        uint8_t vdb_dev_id = 0xFF;
    };

    //pid
    // struct pid{
    //     float kP{0};
    //     float kI{0};
    //     float kD{0};
    //     float limitIntegral{0};
    //     float limitOutput{0};
    // };

    // ошибки по углу тангажа и курса (рассогласование между центро рамки трекера и центром кадра)
    struct anglesErrors{
        float betta{0.f};//угол тангажа
        float alfa{0.f};//угол курса
    };
public:
    TrackerAdapter(const Cfg &_cfg, int &rep);

    // handlers: TODO перенести в private
    void handle_f_tracker_status_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld);
    void handle_f_start_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld);
    void handle_f_stop_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld);
    void handle_f_lens_status_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld);
    void handle_f_status_complex_rd_rsp(VDBMsgHdr hdr, const VDBMsgPld &pld);

    void setTrackerStatusUpdate(vdb::tr_adapter::func::tracker_status::Pld v_params);
    void calcAngles();
    void calcVelocities();
    float getAngleErrorPitch() {return m_angErrors.betta;}
    float getAngleErrorYaw()   {return m_angErrors.alfa;}

    void setPresetController(std::shared_ptr<TrackerAdapterPresetController> presetController_ptr);
protected:
    void handle_msg(VDBMsgHdr hdr, const VDBMsgPld &pld) override;
    
private:
    //структура с информацией о рамке трекера
    struct trackerFrame{
        float sizeX{0.0f};
        float sizeY{0.0f};
        float area{0.00f};
        float distX{0.0f};
        float distY{0.0f};
        float distTotal{0.0f};
        bool  newValSize{false};
        bool  newValDist{false};
    };
//структура с информацией о FOV
    struct FOV{
        float x{7};
        float y{7};
        bool  newVal{true};
    };


//    struct coefs{
//        CoefficientLaw<LawType::Exponential, float, float, true>*       expCalcSizeTrFr{nullptr};
//        CoefficientLaw<LawType::Exponential, float, float, true>*       expCalcAngleField{nullptr};
//        CoefficientLaw<LawType::Exponential, float, float, false>*      expCalcDistFromCenter{nullptr};
//        float kSizeTrFr{1.0f};
//        float kFOV{1.0f};
//        float kDistFromCenter{1.0f};
//        float kFinal{1.0f};
//    };

private:
    void setCurFOV(float v_xFov, float v_yFOV);
    void setCurTrackerFrameSize(float sizeX,float sizeY);
    void setCurTrackerFrameDist(float distX, float distY);
    void calcCoefs();
    void runtime();

    void send_speed_to_gsp(float pitch, float yaw);


    //необходимо сделать много коэффициентов
    //в зависимости от углового поля (чем оно больше, тем коэффициенты меньше)
    //в зависимости от размера рамки трекера
    PIDRegulator                m_pidAlfa;
    PIDRegulator                m_pidBetta;
    SlewRateLimiter*             m_slewRateLimitterAlfa;
    SlewRateLimiter*             m_slewRateLimitterBetta;
    //FOV                       m_FOVBase;
    FOV                         m_FOVCur;
    SimpleExponentialFilter     m_expFiltBetta;
    SimpleExponentialFilter     m_expFiltAlfa;
    //trackerFrame              m_trackerFrameBase;
    trackerFrame                m_trackerFrameCur;
    anglesErrors                m_angErrors;
    //coefs                     m_coefs;
    Cfg                         m_cfg;
    VDBMsgDispath               vdb_disp;
    bool                        tracking = {false};
    float                       m_k_pd_fov_x{1};
    float                       m_k_pd_fov_y{1};
    float                       m_k_lim_fov_x{1};
    float                       m_k_lim_fov_y{1};


    // обеспечение работы PresetController и формирования статуса для терминала 
    vdb::tr_adapter_common::en_tracker_adapter_state        m_currentState                          = vdb::tr_adapter_common::STATE_STANDBY;
    std::shared_ptr<TrackerAdapterPresetController>         m_presetController_ptr                  = nullptr;
    StopwatchTS                                             m_stopwatchProgramExecute;                                                          // с момента начала программы
    StopwatchTS                                             m_stopwatchTrackingSessionExecute;                                                  // с момента начала последней сессии сопровождения для измерения длительности сессии сопровождения 
    NumCounterTS                                            m_statusComplexResponseCounter;                                                     // счетчик ответов на запрос status_complex
    std::atomic<float>                                      m_lastGspSpeedCtrlPitch                 = 0.0f;
    std::atomic<float>                                      m_lastGspSpeedCtrlYaw                   = 0.0f;
    std::atomic<float>                                      m_trackerPrepareScoreNorm               = {0.0f};
    std::atomic<bool>                                       m_enableSlowdownByPrepareScore          = {false}; 
};

#endif // ADAPTIVETRACKERCONTROLLER_H
