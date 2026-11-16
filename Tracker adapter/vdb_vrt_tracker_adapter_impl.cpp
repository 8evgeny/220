#include "vdb_vrt_tracker_adapter_impl.hpp"
#include "math.h"
#include <thread>

#include "vdb_gsp.hpp"
#include "vdb_registers.h"
#include "cli_log_tools.hpp"
#include "tracker_adapter_pack_utils.hpp"

// namespace idFunc = vdb::tr_adapter;
using namespace vdb::tr_adapter;
using namespace vdb::tr_adapter_common;

TrackerAdapter::TrackerAdapter(const Cfg &_cfg, int &rep): VDBNode(_cfg.vdb_dev_id), m_cfg(_cfg)
{
    //m_pixSize = (m_params.resX/m_params.angleField)*180/3.14f; //размер пикселя в радианах
    //сейчас задаем для всех рассчетов коэффициентов экспоненциальный закон
    m_expFiltAlfa.setAlpha(0.85);
    m_expFiltBetta.setAlpha(0.85);
    m_slewRateLimitterAlfa = new SlewRateLimiter(10.0f);
    m_slewRateLimitterBetta = new SlewRateLimiter(10.0f);
    //m_coefs.expCalcDistFromCenter = new CoefficientLaw<LawType::Exponential, float, float, false>(m_coefs.kDistFromCenter, m_trackerFrameBase.distTotal);
    //m_coefs.expCalcSizeTrFr = new CoefficientLaw<LawType::Exponential, float, float, true>(m_coefs.kSizeTrFr, m_trackerFrameBase.area);
    //m_coefs.expCalcAngleField = new CoefficientLaw<LawType::Exponential, float, float, true>(m_coefs.kFOV, m_FOVCur.x);
    std::vector<VDBRegHandleData> hndl_data_vec =
    {
        {
            .cmd_type       = vdb::CMD_TYPE_REQ_WRITE, 
            .cmd_status     = vdb::STATUS_TYPE_REQ_WITHOUT_RSP, 
            .func_id        = FUNC_TRACKER_STATUS, 
            .handler        = std::bind(&TrackerAdapter::handle_f_tracker_status_wr_norsp, this, std::placeholders::_1, std::placeholders::_2)
        },
        {
            .cmd_type       = vdb::CMD_TYPE_REQ_WRITE, 
            .cmd_status     = vdb::STATUS_TYPE_REQ_WITHOUT_RSP, 
            .func_id        = FUNC_START_TRACKING, 
            .handler        = std::bind(&TrackerAdapter::handle_f_start_wr_norsp, this, std::placeholders::_1, std::placeholders::_2)
        },
        {
            .cmd_type       = vdb::CMD_TYPE_REQ_WRITE, 
            .cmd_status     = vdb::STATUS_TYPE_REQ_WITHOUT_RSP, 
            .func_id        = FUNC_STOP_TRACKING, 
            .handler        = std::bind(&TrackerAdapter::handle_f_stop_wr_norsp, this, std::placeholders::_1, std::placeholders::_2)
        },
        {
            .cmd_type       = vdb::CMD_TYPE_REQ_WRITE,
            .cmd_status     = vdb::STATUS_TYPE_REQ_WITHOUT_RSP,
            .func_id        = FUNC_LENS_STATUS,
            .handler        = std::bind(&TrackerAdapter::handle_f_lens_status_wr_norsp, this, std::placeholders::_1, std::placeholders::_2)
        },
        {
            .cmd_type       = vdb::CMD_TYPE_REQ_READ,
            .cmd_status     = vdb::STATUS_TYPE_REQ_WITH_RSP,
            .func_id        = FUNC_STATUS_COMPLEX,
            .handler        = std::bind(&TrackerAdapter::handle_f_status_complex_rd_rsp, this, std::placeholders::_1, std::placeholders::_2)
        }
    };
    vdb_disp.register_handlers_batch(hndl_data_vec);

    std::thread processCoefs(&TrackerAdapter::runtime, this);
    processCoefs.detach();
    // m_pidAlfa.set_coefs(50.0f, 0.1f, 20.4f, 60.0f);
    // m_pidBetta.set_coefs(50.0f, 0.1f, 20.4f, 60.0f);

    m_stopwatchProgramExecute.restart();
    m_stopwatchTrackingSessionExecute.stop();
    
    rep = 0;
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::handle_msg(VDBMsgHdr hdr, const VDBMsgPld &pld)
{
    vdb_disp.dispath(hdr,pld);
} /* handle_msg*/
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setCurFOV(float v_xFov, float v_yFOV)
{
    //v_xFov = v_xFov / 180.f * M_PI;
    //v_yFOV = v_yFOV / 180.f * M_PI;
    if(m_FOVCur.x != v_xFov || m_FOVCur.y != v_yFOV) {
        m_FOVCur.x = v_xFov;
        m_FOVCur.y = v_yFOV;
        // TODO настроечные параметры привязанные к конкретному профилю.  
        float k_min_x = 0.8;
        float k_min_y = 0.95;
        float k_min_xy_lim = 0.6;
        float k_max = 1;
        /* угловые поля каналов мин и макс. TODO: передавать при старте (init).  */
        float fov_min_x = 0.19;
        float fov_max_x = 11;
        float fov_min_y = 0.33;
        float fov_max_y = 20;
        m_k_pd_fov_x = k_min_x + (k_max - k_min_x)*(m_FOVCur.x - fov_min_x)/(fov_max_x-fov_min_x);
        m_k_pd_fov_y = k_min_y + (k_max - k_min_y)*(m_FOVCur.y - fov_min_y)/(fov_max_y-fov_min_y);
        m_k_lim_fov_x = k_min_xy_lim + (k_max - k_min_xy_lim)*(m_FOVCur.x - fov_min_x)/(fov_max_x-fov_min_x);
        m_k_lim_fov_y = k_min_xy_lim + (k_max - k_min_xy_lim)*(m_FOVCur.y - fov_min_y)/(fov_max_y-fov_min_y);
        m_FOVCur.newVal = true;
    }
    //m_pixSize = (m_params.resX/m_params.angleField)*180/3.14; //размер пикселя в радианах
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setCurTrackerFrameSize(float v_sizeX,float v_sizeY)
{
    if( m_trackerFrameCur.sizeX != v_sizeX ||  m_trackerFrameCur.sizeY != v_sizeY){
    m_trackerFrameCur.sizeX = v_sizeX;
    m_trackerFrameCur.sizeY = v_sizeY;
    m_trackerFrameCur.area = m_trackerFrameCur.sizeX*m_trackerFrameCur.sizeY;
    m_trackerFrameCur.newValSize = true;
    }
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::setCurTrackerFrameDist(float v_distX, float v_distY)
{
    if( m_trackerFrameCur.distX != v_distX ||  m_trackerFrameCur.distY != v_distY){
    m_trackerFrameCur.distX = m_trackerFrameCur.sizeX/2 + v_distX - 0.5f;
    m_trackerFrameCur.distY = m_trackerFrameCur.sizeY/2 + v_distY - 0.5f;
    m_trackerFrameCur.distTotal = sqrt((m_trackerFrameCur.distX*m_trackerFrameCur.distX)+
                                      (m_trackerFrameCur.distY*m_trackerFrameCur.distY));
    m_trackerFrameCur.newValDist = true;
    }
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::handle_f_tracker_status_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld)
{
    // LOG_NOTE("handle");
    func::tracker_status::Pld s_params;
    if(ser_tools::deser(pld, s_params) != 0)
    {
        LOG_ERR("failed parse pld");
        return;
    }
    setTrackerStatusUpdate(s_params);
    m_presetController_ptr->setTrackerStatus(TrackerStatusCommon{
        .target_left_norm = s_params.distX,
        .target_top_norm = s_params.distY,
        .target_width_norm = s_params.sizeX,
        .target_height_norm = s_params.sizeY
    });
} /* handle_f_tracker_status_wr_norsp */
//-----------------------------------------------------------------------------------------------------------------

void TrackerAdapter::handle_f_start_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld)
{
    LOG_NOTE("handle");
    tracking = true;
    m_currentState = STATE_TRACKING;
    m_stopwatchTrackingSessionExecute.restart();
} /* handle_f_start_wr_norsp */

void TrackerAdapter::handle_f_stop_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld)
{
    LOG_NOTE("handle");
    tracking = false;
    m_currentState = STATE_STANDBY;
    m_stopwatchTrackingSessionExecute.stop();
    m_pidAlfa.reset();
    m_pidBetta.reset();
    send_speed_to_gsp(0,0);
} /* handle_f_stop_wr_norsp */

void TrackerAdapter::handle_f_lens_status_wr_norsp(VDBMsgHdr hdr, const VDBMsgPld &pld)
{
    // LOG_NOTE("handle");
    func::lens_status::Pld s_pld;
    if(ser_tools::deser(pld, s_pld) != ser_tools::RES_OK)
    {
        LOG_ERR("failed parse pld");
        return;
    }
    float fov_hor_deg = s_pld.data.fov_deg._cur.hor;
    float fov_ver_deg = s_pld.data.fov_deg._cur.ver;

    // xFov - fov_ver_deg // TODO сделать однозначный нейминг 
    // yFov - fov_hor_deg
    // setCurFOV(v_params.xFOV, v_params.yFOV);
    setCurFOV(fov_ver_deg, fov_hor_deg);

    m_presetController_ptr->setLensStatus(s_pld.data);
} /* handle_f_channel_status_wr_norsp */

void TrackerAdapter::handle_f_status_complex_rd_rsp(VDBMsgHdr hdr, const VDBMsgPld &pld)
{
    LOG_NOTE("handle");

    PresetDataPacked presetDataPacked;
    m_presetController_ptr->getPresetDataPacked(presetDataPacked);
    LensStatusExtendCommon lensStatus_tmp;
    m_presetController_ptr->getLensStatus(lensStatus_tmp);
    TrackerStatusCommon trackerStatus_tmp;
    m_presetController_ptr->getTrackerStatus(trackerStatus_tmp);
    TrackerAdapterPresetController::Solution presetControllerSolution_tmp;
    if(tracking)
    {
        m_presetController_ptr->getLastSolution(presetControllerSolution_tmp);
    }
    else
    {
        presetControllerSolution_tmp = m_presetController_ptr->getSolution();
    }

    func::status_complex::Pld s_pld_out;
    s_pld_out.state                                             = m_currentState;
    s_pld_out.response_cntr                                     = m_statusComplexResponseCounter.get();
    s_pld_out.timestamp_ms                                      = m_stopwatchProgramExecute.elapsed(StopwatchTS::en_time_unit::MS);
    s_pld_out.cur_track_session_duration_ms                     = m_stopwatchTrackingSessionExecute.elapsed(StopwatchTS::en_time_unit::MS);
    s_pld_out.cur_preset_data                                   = presetDataPacked;
    s_pld_out.cur_rcv_tracker_status_update_interval_mean_us    = 0; // TODO реализовать расчет с усреднением
    s_pld_out.cur_rcv_tracker_status                            = trackerStatus_tmp;
    s_pld_out.cur_rcv_lens_status_update_interval_mean_us       = 0; // TODO реализовать расчет с усреднением
    s_pld_out.cur_rcv_lens_status                               = lensStatus_tmp;
    s_pld_out.cur_snd_gsp_speed_pitch                           = m_lastGspSpeedCtrlPitch.load();
    s_pld_out.cur_snd_gsp_speed_yaw                             = m_lastGspSpeedCtrlYaw.load();
    s_pld_out.cur_pid_res.x                                     = packPidCoefs(presetControllerSolution_tmp.axis_x.pid_result);
    s_pld_out.cur_pid_res.y                                     = packPidCoefs(presetControllerSolution_tmp.axis_y.pid_result);
    s_pld_out.cur_pid_fovcorr.x                                 = packPidCoefs(presetControllerSolution_tmp.axis_x.pid_fov_corrector);
    s_pld_out.cur_pid_fovcorr.y                                 = packPidCoefs(presetControllerSolution_tmp.axis_y.pid_fov_corrector);
    s_pld_out.cur_pid_bboxszcorr.x                              = packPidCoefs(presetControllerSolution_tmp.axis_x.pid_bboxsz_corrector);
    s_pld_out.cur_pid_bboxszcorr.y                              = packPidCoefs(presetControllerSolution_tmp.axis_y.pid_bboxsz_corrector);

    VDBMsgUnion msg_un_out = make_hdr_rsp_to_rd_req(hdr, s_pld_out);
    snd_msg_inside(msg_un_out.hdr, msg_un_out.pld);
    m_statusComplexResponseCounter.inc();
} /* handle_f_status_complex_rd_rsp */

void TrackerAdapter::setTrackerStatusUpdate(func::tracker_status::Pld v_params)
{
//   setCurFOV(v_params.xFOV, v_params.yFOV);
    setCurTrackerFrameSize(v_params.sizeX, v_params.sizeY);
    setCurTrackerFrameDist(v_params.distX, v_params.distY);
    m_trackerPrepareScoreNorm.store(v_params.trackerPrepareScoreNorm);
    bool enableSlowdownByPrepareScore_bool = v_params.enableSlowdownByPrepareScore == vdb_registers::en_value_on;
    m_enableSlowdownByPrepareScore.store(enableSlowdownByPrepareScore_bool); 
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::calcCoefs()
{
    uint8_t counter = 0;
    //1 этап расчет текущих коэффициентов от размера рамки трекера
    // за коэффициент равный 1 принимаем размер рамки равный m_trackerFrameBase.area
    //с увеличением рамки коэффициент уменьшается
    //if(m_trackerFrameCur.newValSize){
     //   m_coefs.kSizeTrFr = m_coefs.expCalcSizeTrFr->operator()(m_trackerFrameCur.area);
     //   m_trackerFrameCur.newValSize = false;
     //   counter = counter+1;
    //}
    //2 этап расчет текущих коэффициентов от величины удаленности от центра кадра
//    if(m_trackerFrameCur.newValDist){
//        m_coefs.kDistFromCenter = m_coefs.expCalcDistFromCenter->operator()(m_trackerFrameCur.distTotal);
//        m_trackerFrameCur.newValDist = false;
//        counter = counter+1;
//    }
    //3 этап расчет текущих коэффициентов от величины углового поля
    //при уменьшении углового поля коэффициент уменьшается
//    if(m_FOVCur.newVal){
//        m_coefs.kFOV = m_coefs.expCalcAngleField->operator()(m_FOVCur.x);
//        m_FOVCur.newVal = false;
//        counter = counter+1;
//    }
//    if(counter){
//        m_coefs.kFinal = m_coefs.kSizeTrFr * m_coefs.kDistFromCenter * m_coefs.kFOV;
//    }
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::calcAngles()
{
    // m_angErrors.alfa = atan(m_trackerFrameCur.distX * tan(m_FOVCur.x/2)) * 2;
    // m_angErrors.betta =  atan(m_trackerFrameCur.distY * tan(m_FOVCur.y/2)) * 2;
    //m_angErrors.alfa = m_trackerFrameCur.distX * m_FOVCur.x;
    //m_angErrors.betta = m_trackerFrameCur.distY * m_FOVCur.y;
    m_angErrors.alfa = m_trackerFrameCur.distX * 20;
    m_angErrors.betta = m_trackerFrameCur.distY * 20;
    m_angErrors.alfa = m_expFiltAlfa.update(m_angErrors.alfa);
    m_angErrors.betta = m_expFiltBetta.update(m_angErrors.betta); // здесь не должен ли быть m_expFiltBetta?
}
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::calcVelocities()
{
    // m_angErrors.alfa = atan(m_trackerFrameCur.distX * tan(m_FOVCur.x/2)) * 2;
    // m_angErrors.betta =  atan(m_trackerFrameCur.distY * tan(m_FOVCur.y/2)) * 2;
    m_angErrors.alfa = m_trackerFrameCur.distX * m_FOVCur.x;
    m_angErrors.betta = m_trackerFrameCur.distY * m_FOVCur.y;
    // m_angErrors.alfa = m_trackerFrameCur.distX * 20;
    // m_angErrors.betta = m_trackerFrameCur.distY * 20;
    m_angErrors.alfa = m_expFiltAlfa.update(m_angErrors.alfa);
    m_angErrors.betta = m_expFiltBetta.update(m_angErrors.betta);
    //std::cout << "DBG: [TrackerAdapter]: err_alpha = " << m_angErrors.alfa << "; err_beta = " << m_angErrors.betta << std::endl;
}

void TrackerAdapter::setPresetController(std::shared_ptr<TrackerAdapterPresetController> presetController_ptr)
{
    m_presetController_ptr = presetController_ptr;
} /* setPresetController */
//-----------------------------------------------------------------------------------------------------------------
void TrackerAdapter::runtime()
{
    while(1)
    {
        if(tracking)
        {
            if(m_FOVCur.newVal || m_trackerFrameCur.newValDist || m_trackerFrameCur.newValSize)
            {
                calcVelocities();
                // std::cout << "DBG: call getSolution" << std::endl;
                auto solTmp = m_presetController_ptr->getSolution();
                
                float squarekoef = 1;
                if(m_enableSlowdownByPrepareScore.load())
                {
                    squarekoef = m_trackerPrepareScoreNorm*m_trackerPrepareScoreNorm;
                }
 
                m_pidAlfa.set_coefs(
                    solTmp.axis_x.pid_result.kp*squarekoef,
                    solTmp.axis_x.pid_result.ki*squarekoef*squarekoef,
                    solTmp.axis_x.pid_result.kd*squarekoef,
                    solTmp.axis_x.pid_result.outputLim*squarekoef,
                    solTmp.axis_x.pid_result.integralLim*squarekoef*squarekoef 
                );
                m_pidBetta.set_coefs(
                    solTmp.axis_y.pid_result.kp*squarekoef,
                    solTmp.axis_y.pid_result.ki*squarekoef*squarekoef,
                    solTmp.axis_y.pid_result.kd*squarekoef,
                    solTmp.axis_y.pid_result.outputLim*squarekoef,
                    solTmp.axis_y.pid_result.integralLim*squarekoef*squarekoef
                );
                // float speed_alpha = m_pidAlfa.calc(m_angErrors.alfa);
                // float speed_beta  = m_pidBetta.calc(m_angErrors.betta);
                float speed_alpha = m_slewRateLimitterAlfa->limit(m_pidAlfa.calc(m_angErrors.alfa))*squarekoef;
                float speed_beta  = m_slewRateLimitterBetta->limit(m_pidBetta.calc(m_angErrors.betta))*squarekoef;
                //std::cout << "DBG: [TrackerAdapter]: speedAlfa = " << speed_alpha << "; speedBetta = " << speed_beta << std::endl;
                m_FOVCur.newVal = false;
                m_trackerFrameCur.newValDist = false;
                m_trackerFrameCur.newValSize = false;

                // if(m_enableSlowdownByPrepareScore.load())
                // {
                //     speed_beta = speed_beta*m_trackerPrepareScoreNorm*m_trackerPrepareScoreNorm;
                //     speed_alpha = speed_alpha*m_trackerPrepareScoreNorm*m_trackerPrepareScoreNorm;
                // }
                send_speed_to_gsp(-speed_beta, -speed_alpha);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void TrackerAdapter::send_speed_to_gsp(float pitch, float yaw)
{
    vdb::gsp::func::control_speed::Pld s_pld;
    s_pld.spd_yaw_degsec            = yaw;
    s_pld.spd_pitch_degsec          = pitch;
    m_lastGspSpeedCtrlPitch.store(pitch);
    m_lastGspSpeedCtrlYaw.store(yaw);
    snd_vdb_msg_wr_norsp_var_pld(vdb::dev_id::ID_GSP, vdb_registers::en_vdb_gsp_controlSpeed, s_pld);
} /* send_speed_to_gsp */
