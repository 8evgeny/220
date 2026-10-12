#include "telemetry.h"
#include "ldc20i/ldc20i_controller.h"
#include "mini640/mini640_controller.h"
//#include "v4l2.h"
#include <QVariant>

//#define DEBUG_LOG




Telemetry::Telemetry()
    : ba_vpkt {sizeof(udp_video_pack), 0x00},
      ba_svpkt {sizeof(udp_video_service_pack), 0x00},
      ba_spkt {sizeof(udp_service_pack), 0x00},
      m_aovV{ 0.0f}
{
    vpkt_ptr = (udp_video_pack*)ba_vpkt.data();
    svpkt_ptr = (udp_video_service_pack*)ba_svpkt.data();
    spkt_ptr = (udp_service_pack*)ba_svpkt.data();

    memset(&s_telstatus, 0x00, sizeof(s_telstatus));
    qDebug()<<"status bytecnt"<< sizeof(telematic_status);

    //qDebug()<<"POS TARGET_A_Z"<<  (uint8_t*)(&spkt_ptr->service_msg.PAYLOAD.TARGET[2].TARGET_A_Z) - (uint8_t*)(&spkt_ptr->service_msg.PAYLOAD);


    s_telstatus.GSP_STATUS      = 0x01;
    s_telstatus.GSP_MODE.flags  = 0x0000;

}



QByteArray Telemetry::setData(uint16_t video_pkt_id, char* video_subframe)
{
    QByteArray out {ba_vpkt.size(), 0};

    vpkt_ptr = (udp_video_pack*)out.data();

    vpkt_ptr->pkt_preambule = 0;
    vpkt_ptr->video_pkt_flag = 0x0300;
    vpkt_ptr->video_pkt_id = video_pkt_id;

    memcpy(vpkt_ptr->video_pkt_data,video_subframe, udp_payload_len);

    return out;
}

QByteArray Telemetry::setVideoServiceData(uint16_t video_frame_num, uint16_t video_width, uint16_t video_height, uint16_t video_frame_type)
{
    QByteArray out {ba_svpkt.size(), 0};

    svpkt_ptr = (udp_video_service_pack*)out.data();

    svpkt_ptr->pkt_preambule = 0;
    svpkt_ptr->service_video_pkt_flag = 0x0200;
    svpkt_ptr->video_frame_num = video_frame_num;   
    svpkt_ptr->video_width = video_width;
    svpkt_ptr->video_height=video_height;
    svpkt_ptr->video_frame_type =video_frame_type;

    return out;
}

typedef struct {
    unsigned int x;     // Input value (decimal)
    float zoom;        // Zoom level (1.0 to 360.0)
    float aovH;        // Horizontal angle of view (degrees)
    float aovV;
} ZoomEntry;

// Updated table with correct AOV values
const ZoomEntry zoom_table[] = {
    // Wide to mid zoom (1×-30×) - AOV decays exponentially
    // {0,     1.0f,  58.1f}, {4881,   2.0f,  45.0f}, {7219,   3.0f,  35.0f},
    // {8706,   4.0f,  28.0f}, {9763,   5.0f,  22.0f}, {10574,  6.0f,  18.0f},
    // {11229,  7.0f,  15.0f}, {11775,  8.0f,  12.5f}, {12258,  9.0f,  10.5f},
    // {12648, 10.0f,   9.0f}, {13006, 11.0f,   7.8f}, {13327, 12.0f,   6.8f},
    // {13617, 13.0f,   6.0f}, {13880, 14.0f,   5.3f}, {14122, 15.0f,   4.8f},
    // {14344, 16.0f,   4.3f}, {14549, 17.0f,   3.9f}, {14741, 18.0f,   3.6f},
    // {14919, 19.0f,   3.3f}, {15085, 20.0f,   3.1f}, {15242, 21.0f,   2.9f},
    // {15389, 22.0f,   2.7f}, {15528, 23.0f,   2.6f}, {15660, 24.0f,   2.5f},
    // {15785, 25.0f,   2.4f}, {15906, 26.0f,   2.3f}, {16024, 27.0f,   2.3f},
    // {16141, 28.0f,   2.3f}, {16261, 29.0f,   2.3f}, {16384, 30.0f,   2.3f},

    {0,      1.0f,  58.1000f, 45.1145f},
    {4881,   2.0f,  33.6568f, 25.6100f},
    {7219,   3.0f,  23.5204f, 17.7830f},
    {8706,   4.0f,  18.0371f, 13.6029f},
    {9763,   5.0f,  14.6157f, 11.0091f},
    {10574,  6.0f,  12.2812f,  9.2442f},
    {11229,  7.0f,  10.5879f,  7.9663f},
    {11775,  8.0f,   9.3041f,  6.9983f},
    {12258,  9.0f,   8.2974f,  6.2399f},
    {12648, 10.0f,   7.4870f,  5.6297f},

    {13006, 11.0f,   6.8207f,  5.1281f},
    {13327, 12.0f,   6.2632f,  4.7086f},
    {13617, 13.0f,   5.7898f,  4.3524f},
    {13880, 14.0f,   5.3830f,  4.0464f},
    {14122, 15.0f,   5.0295f,  3.7805f},
    {14344, 16.0f,   4.7195f,  3.5474f},
    {14549, 17.0f,   4.4456f,  3.3414f},
    {14741, 18.0f,   4.2016f,  3.1580f},
    {14919, 19.0f,   3.9831f,  2.9937f},
    {15085, 20.0f,   3.7861f,  2.8456f},

    {15242, 21.0f,   3.6077f,  2.7115f},
    {15389, 22.0f,   3.4454f,  2.5894f},
    {15528, 23.0f,   3.2970f,  2.4779f},
    {15660, 24.0f,   3.1609f,  2.3755f},
    {15785, 25.0f,   3.0356f,  2.2813f},
    {15906, 26.0f,   2.9198f,  2.1943f},
    {16024, 27.0f,   2.8125f,  2.1137f},
    {16141, 28.0f,   2.7128f,  2.0388f},
    {16261, 29.0f,   2.6200f,  1.9690f},
    {16384, 30.0f,   2.3000f,  1.9038f},

    // Extended zoom (30×-360×) - AOV scales linearly
    {24576,  60.0f,  1.15f,  0.8642f},
    {27264,  90.0f,  0.77f,  0.5787f},
    {28672, 120.0f,  0.58f,  0.4359f},
    {29440, 150.0f,  0.46f,  0.3457f},
    {30016, 180.0f,  0.38f,  0.2856f},
    {30400, 210.0f,  0.33f,  0.2480f},
    {30720, 240.0f,  0.29f,  0.2179f},
    {30912, 270.0f,  0.26f,  0.1954f},
    {31104, 300.0f,  0.23f,  0.1728f},
    {31232, 330.0f,  0.21f,  0.1578f},
    {31424, 360.0f,  0.19f,  0.1428f}
};

const int table_size = sizeof(zoom_table) / sizeof(ZoomEntry);

// Linear interpolation helper
float lerp(float x, float x0, float x1, float y0, float y1) {
    return y0 + ((y1 - y0) * (x - x0)) / (x1 - x0);
}

// Get zoom and AOV for input value `x`
static void get_zoom_and_aov( unsigned int x, float *zoom, float *aovH, float *aovV)
{
    if( x == 0)
    {
        *zoom = 1.0f;
        *aovH = 58.1000f;
        *aovV = 45.1145f;
        return;
    }

    if( x >= zoom_table[table_size-1].x)
    {
        *zoom = 360.0f;
        *aovH = 0.19f;
        *aovV = 0.1428f;
        return;
    }

    // Binary search
    int low = 0, high = table_size - 1;
    while( low <= high)
    {
        int mid = (low + high) / 2;
        if( zoom_table[mid].x == x)
        {
            *zoom = zoom_table[mid].zoom;
            *aovH = zoom_table[mid].aovH;
            *aovV = zoom_table[mid].aovV;
            return;
        }
        else if( zoom_table[mid].x < x)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    *zoom = lerp(x, zoom_table[high].x, zoom_table[low].x,
                 zoom_table[high].zoom, zoom_table[low].zoom);

    *aovH = lerp(x, zoom_table[high].x, zoom_table[low].x,
                zoom_table[high].aovH, zoom_table[low].aovH);

    *aovV = lerp( 	x, zoom_table[high].x, zoom_table[low].x,
                    zoom_table[high].aovV, zoom_table[low].aovV);
}


void Telemetry::procVICAPstatus(CameraStatus status)
{
    mStatus.lock();

   s_telstatus.TV_HRES =1920;
    s_telstatus.TV_VRES =1080;

    s_telstatus.TV_LENS_TEMP = status.temperature;
    s_telstatus.TV_ZOOM_DRIVE= status.zoom_val;
    float zoom_lvl, aovH, aovV;
    get_zoom_and_aov( status.zoom_val, &zoom_lvl, &aovH, &aovV);
    // qDebug() << Q_FUNC_INFO << status.zoom_val << zoom_lvl << aov;
    m_aovV = aovV;
    s_telstatus.TV_FIELD_GRAD = aovH;
    s_telstatus.TV_EXP = status.gain;
    s_telstatus.TV_FOCUS_MM = status.focus_val;
    if(status.focus_val<=4096)
        s_telstatus.TV_FOCUS_MM = INFINITY;
    if((status.focus_val>4096) && (status.focus_val<=45056) )
        s_telstatus.TV_FOCUS_MM= 49152.0/(status.focus_val-4096.0)*1000.0;
    if(status.focus_val>45056)
        s_telstatus.TV_FOCUS_MM= 0.61* pow(0.55, ((status.focus_val-49152.0)/4096.0))*1000.0;

    mStatus.unlock();
    //s_telstatus.TV_I_DRIVE = status.temperature;
}

void Telemetry::procJ200status(J200_DEVSTATE ds)
{
    mStatus.lock();
    s_telstatus.MW_ZOOM_DRIVE = ds.zoomPosition;
    s_telstatus.MW_FOCUS_DRIVE = ds.focusPosition;


  //  s_telstatus.MW_LENS_TEMP = ds.temperature;
    if(ds.temperature<0)
        qDebug()<<" j200 temperature" << ds.temperature;
    mStatus.unlock();
}


void Telemetry::procLDCstatus(LDC_STATE dev_state)
{
    mStatus.lock();
    s_telstatus.RF_LAST_DISTANCE = dev_state.dist.distance;
    s_telstatus.RF_STATE.radiation = dev_state.emit_en;
 //   s_telstatus.RF_STATE. = dev_state.emit_en;
    s_telstatus.RF_STATE.power_on = dev_state.treg_state.temp_reg_rad==0x03;
    s_telstatus.RF_STATE.ready = dev_state.treg_state.temp_reg_ld==0x03;
    s_telstatus.RF_STATE.ready_reserve = !(dev_state.treg_state.temp_reg_ld==0x03);

    //s_telstatus.RF_STATE.power_on = dev_state.treg_state.temp_reg_ld==0x03;
  //  s_telstatus.RF_STATE. = dev_state.treg_state.temp_reg_ld == 0x03;
    s_telstatus.RF_BDT = dev_state.meas.temp_ld;//temp_power_sw;
    s_telstatus.RF_STATUS = 0xFF;
    s_telstatus.RF_MEAS_STATE = dev_state.dist.impulse_emit_cnt_total & 0xFF;
    s_telstatus.RF_AVG_DISTANCE= dev_state.dist.distance_mean;
    s_telstatus.RF_RMS_DISTANCE= dev_state.dist.distance_stddev;
#ifdef DEBUG_LOG
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    qDebug() << ((dev_state.treg_state.temp_reg_ld == 3) ? "LDC LD is Ready": "LDC LD is not Ready");
    qDebug() << "Distance "<<  dev_state.dist.distance;
    qDebug() << "Emit pulse cnt "<<  dev_state.dist.impulse_emit_cnt_total;
    qDebug() << "Recv pulse cnt "<<  dev_state.dist.impulse_recieve_cnt_total;

    qDebug() << "Temp ld "<<  dev_state.meas.temp_ld;
    qDebug() << "Temp sw "<<  dev_state.meas.temp_power_sw;
    // qDebug() << "Temp fd "<<  dev_state.meas.temp_fd;
    //  qDebug() << "Temp rad "<<  dev_state.meas.temp_rad;
    qDebug() << "Treg ld "<<  dev_state.treg_state.temp_reg_ld;
    qDebug() << "Treg rad "<<  dev_state.treg_state.temp_reg_rad;
#endif

    mStatus.unlock();
    //s_telstatus.TV_I_DRIVE = status.temperature;
}




// void VideoFrame::updateServiceData(frame_t sFrame)
// {
//     mStatus.lock();

//     if(sFrame.devId == 0)
//     {
//         s_telstatus.TV_FPS = sFrame.fps; //float to char
//         s_telstatus.TV_HRES = sFrame.width; //uint32 to uint16
//         s_telstatus.TV_VRES = sFrame.height; //uint32 to uint16
//     }
//     mStatus.unlock();

// }


void Telemetry::proc_MINI640_State(MINI640_DEV_STATE dev_state)
{
    // qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    // qDebug() << "power_on_fpa" << dev_state.power_on_fpa;
    // qDebug() << "cce_temp" << dev_state.mon.cce_temp;
    // qDebug() << "ref2_5V" << dev_state.mon.ref2_5V;
    // qDebug() << "video_inv" << dev_state.video_inv;

    // qDebug() << "cooler en" << dev_state.isCoolerEn;
    // qDebug() << "cooler timer" << dev_state.cooler_timer ;
    // qDebug() << " INT_HIGH_LEVEL" << dev_state.exp.INT_HIGH_LEVEL ;
    // qDebug() << "INT_PERIOD_TIME" << dev_state.exp.INT_PERIOD_TIME ;
    // qDebug() << "GPOL" << dev_state.mon.gpol_DAC;
    mStatus.lock();
//  s_telstatus.SELF_TEST.
    s_telstatus.MW_HRES = 640;
    s_telstatus.MW_VRES = 512;
    s_telstatus.MW_LENS_TEMP = round(dev_state.mon.cce_temp-273.15);
    s_telstatus.MW_EXP= round(dev_state.exp.INT_HIGH_LEVEL * (9/50E3));
    s_telstatus.MW_FIELD_GRAD = dev_state.cooler_timer ;
    s_telstatus.MW_FOCUS_MM = round(dev_state.mon.cce_temp-273.15);
    //qDebug() <<dev_state.exp.INT_HIGH_LEVEL * (9/50E3);
    s_telstatus.SELF_TEST.conn_sensor_mw_uart=dev_state.isTimeout;
    mStatus.unlock();


}


void Telemetry::proc_UpdateTrackerActiveLockTracking(bool active, bool tracking)
{
    mStatus.lock();

    s_telstatus.TRACK_STATE = (active)? 0x80 : 0xFF;
    s_telstatus.TARGET_STATE = !tracking;
    s_telstatus.TARGET_1_STATE = !tracking;

    mStatus.unlock();

    // qDebug() << Q_FUNC_INFO << active << tracking;
}


void Telemetry::proc_UpdateTrackerObject(ushort x, ushort y, ushort width, ushort heigth)
{
    mStatus.lock();

    s_telstatus.TRACKER_CHANNEL = 0x80;
    s_telstatus.TARGET_1_X = x;
    s_telstatus.TARGET_1_Y = y;
    s_telstatus.TARGET_1_X_SZ = width;
    s_telstatus.TARGET_1_Y_SZ = heigth;

    mStatus.unlock();
//  TODO:   !!!!!!!!!!!!!!!!!!
    // qDebug() << Q_FUNC_INFO << x << y << width << heigth;
}


void Telemetry::procGspStatus( const bool connected)
{
    mStatus.lock();
    s_telstatus.GSP_STATUS = (connected ? 0x00 : 0x01);
    mStatus.unlock();
}


void Telemetry::procGspPositionMode( const bool zero, const bool pohod, const bool pilot, const bool park, const bool stabilization)
{
    mStatus.lock();

    s_telstatus.GSP_MODE.zero           = zero;
    s_telstatus.GSP_MODE.pohod          = pohod;
    s_telstatus.GSP_MODE.pilot          = pilot;
    s_telstatus.GSP_MODE.park           = park;
    s_telstatus.GSP_MODE.stabilization  = stabilization;

    mStatus.unlock();
}


void Telemetry::procGspMotorAngle(const float z, const float y)
{
    mStatus.lock();

    s_telstatus.ANGLE_Z     = z;
    s_telstatus.ANGLE_Y     = y;

    mStatus.unlock();
}


void Telemetry::procGspMotorSpeed( const float z, const float y)
{
    // qDebug() << Q_FUNC_INFO << z << y;
    mStatus.lock();

    s_telstatus.SPEED_Z     = z;
    s_telstatus.SPEED_Y     = y;

    mStatus.unlock();
}


float Telemetry::getTvFieldGrad( float *const aovV)
{
    QMutexLocker mutex( &mStatus);
    if( aovV)
        *aovV = m_aovV;

    return s_telstatus.TV_FIELD_GRAD;
}




// void Telemetry::procUpdateTracker()
// {

//     bool active;
//     bool tracking;
//     if(m_gst_wrapper == nullptr)
//         return;
//     m_gst_wrapper->proc_getOutObject();
//     m_gst_wrapper->proc_getActiveLockTracking();
// //    memset(&s_telstatus,0x1, sizeof(s_telstatus));





// }



QByteArray Telemetry::setServiceData()
{
    QByteArray out {ba_spkt.size(), 0};

    spkt_ptr = (udp_service_pack*)out.data();

    static uint32_t frame_number;
    spkt_ptr->service_pkt_flag = 0x0100;
    udp_service_msg* srv_ptr = &spkt_ptr->service_msg;
    memset(&srv_ptr->PAYLOAD, 0x01,  (uint8_t*)(&spkt_ptr->service_msg.PAYLOAD.FrameNumber) - (uint8_t*)(&spkt_ptr->service_msg.PAYLOAD));


    srv_ptr->CMD = 0x00;
    srv_ptr->STATUS = 0x00;
    srv_ptr->SRC=   0x0103;
    srv_ptr->DST  = 0x0000;
    srv_ptr->WORD_COUNT = 126;
    // srv_ptr->PAYLOAD.TV_SENSOR_STATUS=1;
    // srv_ptr->PAYLOAD.MesID = 20;
    // srv_ptr->PAYLOAD.DevID = 20;
    //  srv_ptr->PAYLOAD.FrameNumber=frame_number++;
    // srv_ptr->PAYLOAD.ID_YEAR = 24;
    // srv_ptr->PAYLOAD.GSP_MODE = 8;
    // srv_ptr->PAYLOAD.TARGET_ID = 2;
    // srv_ptr->PAYLOAD.TRACKER_CHANNEL = 1;
    mStatus.lock();

    //procUpdateTracker();

    //s_telstatus.TARGET_1_X
    srv_ptr->PAYLOAD=s_telstatus;
    mStatus.unlock();

    // for(int i=0; i<4; i++)
    // {
    //     srv_ptr->PAYLOAD.TARGET[i].TARGET_STATE = 1;
    //     srv_ptr->PAYLOAD.TARGET[i].TARGET_X = 200+i;
    //     srv_ptr->PAYLOAD.TARGET[i].TARGET_Y = 300+i;
    // }
    //srv_ptr->PAYLOAD[0]=1;
    //srv_ptr->PAYLOAD[1]=2;
    return out;
}
