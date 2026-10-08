#include "hvgs_gray_usb_camera.hpp"

#include <thread>
#include <chrono>

#define MAX_SUPPORT_SIMULTANEOUSLY_HV_CAM_COUNT 5 /* максимальное количество одновременно подключаемых HuatengVision камер */

namespace devices
{

HVGS_Gray_USB_Camera::HVGS_Gray_USB_Camera(const hvgs_gray_usb_camera::Settings &_settings, bool &report) : settings(_settings)
{
#if not defined(CCM_8UC1)
    throw std::runtime_error(
                "\033[1;31m Not supported color space for output format!\n"
                "HVGS_Gray_USB_Camera support only GRAYSCALE (CCM_8UC1), \n"
                "check CMakeLists define \"USE_COLOR_CHANNEL_MODE\"\033[0m");
#endif
    report = start_backend();
} // -- constructor HVGS_Gray_USB_Camera


bool HVGS_Gray_USB_Camera::getState(ParitelTelemetryGen & state_frame)
{
    state_frame.exposure_ms = fr_meta.exposure_ms;
//    std::cout << "dbg:: Call getState in HVGS_gray_USB_Camera!" << std::endl;
    return 0;
} // END getState


HVGS_Gray_USB_Camera::~HVGS_Gray_USB_Camera()
{
    std::cout << "---Destructor HVGS_Gray_USB_Camera" << std::endl;
    quit();
} // -- destructor  HVGS_Gray_USB_Camera

void HVGS_Gray_USB_Camera::setup()
{

} // -- END setup

void HVGS_Gray_USB_Camera::start()
{

} // -- END start

uint8_t* HVGS_Gray_USB_Camera::receiveFrame(int &w, int &h, int &id, int &num)
{
    return nullptr;
} // -- END receiveFrame

void HVGS_Gray_USB_Camera::getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image)
{
#if defined(CCM_8UC1)
    memcpy(image.data, f, image.total());
#else
    throw std::runtime_error("\033[1;31m Not supported color space for output format\033[0m");
#endif
} // -- END getFormatedImage

float HVGS_Gray_USB_Camera::getExposureMs()
{
    return fr_meta.exposure_ms;
} // -- END getExposureMs

int HVGS_Gray_USB_Camera::getColorChannels()
{
    return 1;
} // -- END getColorChannels

bool HVGS_Gray_USB_Camera::isBayerColorChannel()
{
    return false;
} // -- END isBayerColorChannel

void HVGS_Gray_USB_Camera::keyHandler(char &key)
{

} // -- END keyHandler

void HVGS_Gray_USB_Camera::workflow()
{

} // -- END workflow

bool HVGS_Gray_USB_Camera::start_backend()
{
    std::cout << "BEGIN " << settings.section_name << "::setup\n";
    int                     iCameraCounts = 1;
    int                     iStatus = -1;
    tSdkCameraDevInfo       tCameraEnumList[MAX_SUPPORT_SIMULTANEOUSLY_HV_CAM_COUNT];

    if(CameraSdkInit(0) != CAMERA_STATUS_SUCCESS)
    {
        std::cout << "\tERROR: CameraSdkInit failed" << std::endl;
        return false;
    }

    // Нумерация девайсов и создание листа девайсов
    iStatus = CameraEnumerateDevice(tCameraEnumList, &iCameraCounts);
    if(iStatus != CAMERA_STATUS_SUCCESS)
    {
        std::cout << "\tERROR: fail CameraEnumerateDevice" << std::endl;
        return false;
    }
    std::cout << "\tcam_counts = " << iCameraCounts << std::endl;
    
    if(iCameraCounts == 0)
    {
        std::cout << "\tERROR: no free cameras" << std::endl;
        return false;
    }

    int select_cam_id = settings.camera.id;
    if(select_cam_id < 0)
    {
        std::cout << "WARNING: settings.camera.sensor_id=0, select_cam_id set equal \"0\"" << std::endl;
        select_cam_id = 0;
    }

    if(select_cam_id >= iCameraCounts)
    {
        std::cout << "WARNING: select_cam_id >= iCameraCounts => select_cam_id = iCameraCounts - 1;" << std::endl;
        select_cam_id = iCameraCounts - 1;
    }

    // Camera initialization.
    // Only after successful initialization can any other camera-related operation interfaces be called.
    iStatus = CameraInit(&tCameraEnumList[select_cam_id], -1, -1, &hCamera);
    if(iStatus!=CAMERA_STATUS_SUCCESS)
    {
        std::cout << "\tERROR: CameraInit failed" << std::endl;
        return false;
    }

    // Get the camera's feature description structure.
    // This structure contains range information of
    // various parameters that the camera can set.
    // determines the parameters of the relevant functions
    if(CameraGetCapability(hCamera, &tCapability) != CAMERA_STATUS_SUCCESS)
    {
        std::cout << "\tERROR: CameraGetCapability failed" << std::endl;
        return false;
    }
    
    /* Let the SDK enter working mode and start receiving images sent from the camera data.
    If the current camera is in trigger mode, it needs to receive
    The image will not be updated until the frame is triggered. */
    if(CameraPlay(hCamera) != CAMERA_STATUS_SUCCESS)
    {
        std::cout << "\tERROR: CameraPlay failed" << std::endl;
        return false;
    }

    if(CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_MONO8) != CAMERA_STATUS_SUCCESS)
    {
        quit_d();
        std::cout << "\tERROR: CameraSetIspOutFormat failed" << std::endl;
        return false;
    }

    if(CameraGetImageResolution(hCamera, &img_res) != CAMERA_STATUS_SUCCESS)
    {
        quit_d();
        std::cout << "\tERROR: CameraGetImageResolution failed" << std::endl;
        return false;
    }
    
    receive_frame_size.width = img_res.iWidth;
    receive_frame_size.height = img_res.iHeight;

    std::cout << "\treceive_frame_size = " << receive_frame_size << std::endl;
    if(settings.camera.fps > 0)
    {
        if(CameraSetFrameRate(hCamera, settings.camera.fps) != CAMERA_STATUS_SUCCESS)
        {
            quit_d();
            std::cout << "\tERROR: CameraSetFrameRate failed" << std::endl;
            return false;
        }
    }

    if(settings.exposure_mode == hvgs_gray_usb_camera::EXPOSURE_AUTO)
    {
        if(CameraSetAeState(hCamera, true) != CAMERA_STATUS_SUCCESS)
        {
            quit_d();
            std::cout << "\tERROR: CameraSetAeState failed" << std::endl;
            return false;
        }
        if(CameraSetAeTarget(hCamera, settings.auto_exposure.brightness_target) != CAMERA_STATUS_SUCCESS)
        {
            quit_d();
            std::cout << "\tERROR: CameraSetAeTarget failed" << std::endl;
            return false;
        }
    }
    else if(settings.exposure_mode == hvgs_gray_usb_camera::EXPOSURE_MANUAL)
    {
        if(CameraSetAeState(hCamera, false) != CAMERA_STATUS_SUCCESS)
        {
            quit_d();
            std::cout << "\tERROR: CameraSetAeState failed" << std::endl;
            return false;
        }
        if(CameraSetAnalogGainX(hCamera, settings.manual_exposure.analog_gain) != CAMERA_STATUS_SUCCESS)
        {
            quit_d();
            std::cout << "\tERROR: CameraSetAnalogGainX failed" << std::endl;
            return false;
        }
        if(CameraSetExposureTime(hCamera, settings.manual_exposure.exposure_time_ms) != CAMERA_STATUS_SUCCESS)
        {
            quit_d();
            std::cout << "\tERROR: CameraSetExposureTime failed" << std::endl;
            return false;
        }
    }

    if(CameraSetSharpness(hCamera, settings.preprocess.sharpness) != CAMERA_STATUS_SUCCESS)
    {
        quit_d();
        std::cout << "\tERROR: CameraSetSharpness failed" << std::endl;
        return false;
    }

    if(CameraSetGamma(hCamera, settings.preprocess.lut.gamma) != CAMERA_STATUS_SUCCESS)
    {
        quit_d();
        std::cout << "\tERROR: CameraSetGamma failed" << std::endl;
        return false;
    }

    if(CameraSetContrast(hCamera, settings.preprocess.lut.contrast) != CAMERA_STATUS_SUCCESS)
    {
        quit_d();
        std::cout << "\tERROR: CameraSetContrast failed" << std::endl;
        return false;
    }

    if(CameraSetMirror(
                hCamera, MIRROR_DIRECTION_HORIZONTAL, settings.preprocess.mirror_horizontal)
            != CAMERA_STATUS_SUCCESS)
    {
        quit_d();
        std::cout << "\tERROR: CameraSetMirror failed" << std::endl;
        return false;
    }

    if(CameraSetMirror(
                hCamera, MIRROR_DIRECTION_VERTICAL, settings.preprocess.mirror_vertical)
            != CAMERA_STATUS_SUCCESS)
    {
        quit_d();
        std::cout << "\tERROR: CameraSetMirror failed" << std::endl;
        return false;
    }

    frame_receive = cv::Mat(receive_frame_size, CV_8UC1, cv::Scalar::all(0));

    sync.exec.execute.store(true);

    std::thread(&HVGS_Gray_USB_Camera::exec, this).detach();

    std::cout << "END " << settings.section_name << "::setup\n";
    return true;
} // -- END start_backend

void HVGS_Gray_USB_Camera::quit()
{
    if(sync.quit.was_call)
    {
        return;
    }
    sync.quit.was_call.store(true);

    std::cout << "BEGIN " << settings.section_name << "::quit" << std::endl;

    // если exec-цикл исполнялся до вызова quit
    bool close_exec_report = true;
    if(sync.exec.execute.load())
    {
        sync.exec.execute.store(false);
        sync.exec.wait_close.store(true);
        int watchdog_time_counter_ms = 0;
        while(!sync.exec.close.load())
        {
            if(watchdog_time_counter_ms > settings.watchdog.exec_complete_watchdog_ms)
            {
                std::cout << "\tERROR: wait exec complete done time is over" << std::endl;
                close_exec_report = false;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(settings.watchdog.check_exec_complete_delay_ms));
            watchdog_time_counter_ms += settings.watchdog.check_exec_complete_delay_ms;
        }
        if(sync.exec.close.load())
        {
            sync.exec.wait_close.store(false);
        }
    } // -- if(sync.exec.execute.load())
    bool quit_direct_report = quit_d();
    bool report_final = close_exec_report & quit_direct_report;
    sync.quit.success.store(report_final);
    if(report_final)
    {
        std::cout << "\tSUCCESS HVGS_Gray_USB_Camera::quit" << std::endl;
    }
    else
    {
        std::cout << "\tFAILED HVGS_Gray_USB_Camera::quit" << std::endl;
    }
    std::cout << "END " << settings.section_name << "::quit" << std::endl;
    return;
} // -- END quit

bool HVGS_Gray_USB_Camera::quit_d() // quit direct
{
    bool cam_stop_report = true;
    if(CameraStop(hCamera) != CAMERA_STATUS_SUCCESS)
    {
        std::cout << "\tERROR: CameraStop return != 0" << std::endl;
        cam_stop_report = false;
    }
    bool cam_uninit_report = true;
    if(CameraUnInit(hCamera) != CAMERA_STATUS_SUCCESS)
    {
        std::cout <<"\tERROR: CameraUnInit return != 0" << std::endl;
        cam_uninit_report = false;
    }
    return cam_stop_report & cam_uninit_report;
}; // -- END quit_d

void HVGS_Gray_USB_Camera::exec()
{
    tSdkFrameHead sFrameInfo;
    BYTE* pbyBuffer;
    while(sync.exec.execute.load())
    {
        if(CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, settings.watchdog.read_frame_watchdog_ms) == CAMERA_STATUS_SUCCESS)
        {
            CameraImageProcess(hCamera, pbyBuffer, frame_receive.data, &sFrameInfo);

            fr_meta.exposure_ms = 1e-3 * sFrameInfo.uiExpTime;
            for(auto handler : frame_handlers)
            {
                handler->handle(frame_receive.data, frame_receive.cols,
                                frame_receive.rows, 0, frame_id);
            } // -- END for(auto handler : frame_handlers)
            
            ++frame_id;
            // std::cout << "frame arrive:" << std::endl;
            //     std::cout << "\ttime_stamp=" << sFrameInfo.uiTimeStamp << "\n" <<
            //     "\texposure_time=" << sFrameInfo.uiExpTime << "\n" <<
            //     "\tanalog_gain=" << sFrameInfo.fAnalogGain << "\n" <<
            //     "\tcontrast=" << sFrameInfo.iContrast << "\n" <<
            //     "\tgamma=" << sFrameInfo.iGamma << "\n" << std::endl;

            /*
            After successfully calling CameraGetImageBuffer,
            CameraReleaseImageBuffer must be called to release the obtained buffer.

            Otherwise, when CameraGetImageBuffer is called again,
            the program will be suspended and blocked until
            CameraReleaseImageBuffer is called in other threads to release the buffer.

            */
            CameraReleaseImageBuffer(hCamera,pbyBuffer);
        }
        else
        {
            std::cout << "WARNING: wait frame time is over (timeout)" << std::endl;
//            break;
        }
    } // -- while(sync.execute.load())
    std::cout << "NOTE: HVGS_Gray_USB_Camera::exec() complete done" << std::endl;
    sync.exec.close.store(true);
} // -- END exec

} // -- namespace devices
