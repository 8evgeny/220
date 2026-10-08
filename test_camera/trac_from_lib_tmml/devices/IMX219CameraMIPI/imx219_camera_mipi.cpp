#include "imx219_camera_mipi.hpp"
#include <thread>
#include <opencv2/core.hpp>

#ifdef GUI_OK
#include <opencv2/highgui.hpp>
#endif //GUI_OK
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#ifdef USE_CUDA_RESIZE_DEVICE_IMX219
    #include <opencv2/core/cuda.hpp>
    #include <opencv2/cudawarping.hpp>
    #include <opencv2/cudaimgproc.hpp>
#endif //USE_CUDA_RESIZE_DEVICE_IMX219

using namespace devices;
using namespace devices::imx219_camera_mipi;

IMX219CameraMIPI::IMX219CameraMIPI(const devices::imx219_camera_mipi::Settings &settings) : settings(settings)
{
    frame_request_delay_ms = 1000 / settings.output.fps;
    if(settings.video.width != settings.output.width || settings.video.height != settings.output.height)
    {
        need_io_resize = true;
    }
    else
    {
        need_io_resize = false;
    }
} // -- IMX219CameraMIPI::IMX219CameraMIPI

IMX219CameraMIPI::~IMX219CameraMIPI()
{
    quit();
} // -- IMX219CameraMIPI::~IMX219CameraMIPI()

void IMX219CameraMIPI::setup()
{
    std::ostringstream camAdvPropPipePartOS; // camAdvPropPipePart -- camera advanced propeties pipeline part

    camAdvPropPipePartOS << " wbmode=" << settings.camera.wbmode << " ";

    if(!settings.camera.enable_auto_exposure_range)
    {
        camAdvPropPipePartOS << " exposuretimerange=\"" <<
            settings.camera.manual_exposure_range_ns.first << " " <<
            settings.camera.manual_exposure_range_ns.second << "\" ";
    }

    std::string camProps = camAdvPropPipePartOS.str();
#if defined(CCM_8UC1)
    pipeline = "nvarguscamerasrc " + camProps + " ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(settings.video.width) + ", height=(int)" +
        std::to_string(settings.video.height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(settings.video.fps) +
        "/1 ! nvvidconv ! video/x-raw, format=(string)GRAY8 ! appsink max-buffers=" + std::to_string(settings.camera.max_buffers) + " drop=True";
#elif defined(CCM_8UC3)
    /*
    Преобразование цвета COLOR_YUV2BGR_I420 I420->BGR не интегрировано в pipeline,/
    т.к. получается большая задержка между входом и выходом, что приводит к замедлению отклика
    на изменение состояния регистрируемой сцены пространства предметов;
    */
    pipeline = "nvarguscamerasrc " + camProps + " ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(settings.video.width) + ", height=(int)" +
        std::to_string(settings.video.height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(settings.video.fps) +
        "/1 ! nvvidconv ! video/x-raw, format=(string)I420 ! appsink max-buffers=" + std::to_string(settings.camera.max_buffers) +  " drop=True";
#else
    throw std::runtime_error("Not supported color space for output format");
#endif
    std::cout << "Pipeline: " << pipeline << std::endl;
} // -- END IMX219CameraMIPI::setup()

void IMX219CameraMIPI::start()
{
    gst_cap.open(pipeline, cv::CAP_GSTREAMER);
    if(!gst_cap.isOpened()) {
	    std::cout << "Error: Failed to open pipeline." << std::endl;
        return;
    }
    std::cout << "IMX219CameraMIPI::start() -- Pipeline success create!\n";
    sync.handleFrameExecute.store(true, std::memory_order_release);
    sync.frame_ready.store(false);
    std::thread(&IMX219CameraMIPI::exec, this).detach(); // поток извлечения кадров из pipeline
    std::thread(&IMX219CameraMIPI::runHandleFrame, this).detach(); // отправка фреймов подписчикам (application'у)
} // -- END IMX219CameraMIPI::start()

void IMX219CameraMIPI::exec()
{
    //считываем первый кадр и определяем его параметры, сравниваем с запрашиваемыми
    cv::Mat first_receive_frame;
    if(!gst_cap.read(first_receive_frame))
    {
        std::cout << " --- Pipeline stop frame flow" << std::endl;
        quit();
        return;
    }

    /*
        Для GRAY8 CV_8UC1 и для I420 CV_8UC1 
        (для I420 высота изображения x1.5, т.к. содержит сегменты, 
        в которых отображаются параметры цветности. 
        см. техническую документацию I420 формата изображения)
    */
    int input_frame_type = CV_8UC1;
    if(first_receive_frame.type() != input_frame_type)
    {
        throw std::runtime_error("Error: required and received frames color types not equal!");
    }

    /*
    Сравнение резрешения запрашиваемых в gstreamer конвейере и полученных изображений
    */
#if defined(CCM_8UC1)
    if(first_receive_frame.cols != settings.video.width || 
        first_receive_frame.rows != settings.video.height)
#elif defined(CCM_8UC3)
    if(first_receive_frame.cols != settings.video.width ||
        first_receive_frame.rows != 1.5 * settings.video.height)
#endif
    {
        throw std::runtime_error("Error: required and received frames sizes not equal");
    }

    cv::Size input_frame_size(settings.video.width, settings.video.height); // gstreamer to device
    cv::Size output_frame_size(settings.output.width, settings.output.height); // device to application

    frame_receive = cv::Mat(input_frame_size, input_frame_type); // выделяем память под рабочий принимаемый перезаписываемый кадр
    
#if defined(CCM_8UC1)
    int required_output_frame_type_cv = CV_8UC1;
#elif defined(CCM_8UC3)
    int required_output_frame_type_cv = CV_8UC3;
#else
    throw std::runtime_error("Not supported color space for output format");
#endif


    //frame_receive ==> frame_intermediate ==> frame_prepared (для некоторых конфигурация CCM и need_io_resize)
#if defined(CCM_8UC1)
    if(need_io_resize)
    {
        frame_prepared = cv::Mat(output_frame_size, required_output_frame_type_cv); 
    }
    else
    {
        frame_prepared = frame_receive;
    }
#elif defined(CCM_8UC3) 
    if(need_io_resize)// преобразование производится в два этапа: 1) cvtColor 2) resize
    {
        frame_intermediate = cv::Mat(input_frame_size, required_output_frame_type_cv);
    }
    frame_prepared = cv::Mat(output_frame_size, required_output_frame_type_cv);
#else
    throw std::runtime_error("Not supported color space for output format");
#endif

    while(sync.handleFrameExecute.load(std::memory_order_acquire))
    {
        std::chrono::system_clock::time_point timePoint =
                std::chrono::system_clock::now() +
                std::chrono::milliseconds(frame_request_delay_ms);
        gst_cap >> frame_receive;
        if(frame_receive.empty()) 
        {
            std::cout << " --- Pipeline stop frame flow" << std::endl;
                quit();
            break;
        }
        sync.frameMtx.lock();
#if defined(CCM_8UC1)
        if(need_io_resize)
        {
            #if defined(USE_CUDA_RESIZE_DEVICE_IMX219)
                static cv::cuda::GpuMat gpuframe_receive(input_frame_size, input_frame_type);
                static cv::cuda::GpuMat gpuframe_prepared(output_frame_size, required_output_frame_type_cv);
                gpuframe_receive.upload(frame_receive);
                cv::cuda::resize(gpuframe_receive, gpuframe_prepared, output_frame_size, cv::InterpolationFlags::INTER_NEAREST);
                gpuframe_prepared.download(frame_prepared);
            #else // !USE_CUDA_RESIZE_DEVICE_IMX219
                cv::resize(frame_receive, frame_prepared, output_frame_size, cv::InterpolationFlags::INTER_NEAREST);
            #endif // USE_CUDA_RESIZE_DEVICE_IMX219

        }
#elif defined(CCM_8UC3) 
        if(need_io_resize)
        {
            // Для флага преобразования цветового пространства COLOR_YUV2BGR_I420 не существует реализации на cuda -> исполнение на cpu
            cv::cvtColor(frame_receive, frame_intermediate, cv::COLOR_YUV2BGR_I420); // Для преобразования COLOR_YUV2BGR_I420 необходимо первоначально cvtColor, затем resize
            #if defined(USE_CUDA_RESIZE_DEVICE_IMX219)
                static cv::cuda::GpuMat gpuframe_intermediate(input_frame_size, required_output_frame_type_cv);
                static cv::cuda::GpuMat gpuframe_prepared(output_frame_size, required_output_frame_type_cv);
                gpuframe_intermediate.upload(frame_intermediate);
                cv::cuda::resize(gpuframe_intermediate, gpuframe_prepared, output_frame_size);
                gpuframe_prepared.download(frame_prepared);
            #else // !USE_CUDA_RESIZE_DEVICE_IMX219
                cv::resize(frame_intermediate, frame_prepared, output_frame_size, cv::InterpolationFlags::INTER_NEAREST);
            #endif // USE_CUDA_RESIZE_DEVICE_IMX219
        }
        else
        {
            cv::cvtColor(frame_receive, frame_prepared, cv::COLOR_YUV2BGR_I420);            
        }
#else
    throw std::runtime_error("Not supported color space for output format");
#endif

        frame_counter++;
        sync.frameMtx.unlock();
        sync.frameReadyCv.notify_one();
        sync.frame_ready.store(true);
        std::this_thread::sleep_until(timePoint);
    } // -- END while(sync.handleFrameExecute.load(std::memory_order_acquire))
} // -- END IMX219CameraMIPI::exec()

void IMX219CameraMIPI::runHandleFrame()
{
    while(sync.handleFrameExecute.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lk(sync.frameMtx);
        sync.frameReadyCv.wait(lk);
        if(!sync.handleFrameExecute.load(std::memory_order_acquire))
        {
            break;
        }
        for(auto handler : frame_handlers)
        {
            handler->handle(frame_prepared.data, frame_prepared.cols,
                            frame_prepared.rows, 0, frame_counter);
        } // -- END for(auto handler : frame_handlers)
        sync.frame_ready.store(false);
        lk.unlock();
    } // -- END (sync.handleFrameExecute.load(std::memory_order_acquire))
} // -- END RaspberryHQCamera::runHandleFrame()

void IMX219CameraMIPI::quit()
{
    sync.handleFrameExecute.store(false, std::memory_order_acquire);
    sync.frameReadyCv.notify_one();
    gst_cap.release();
    std::cout << "IMX219CameraMIPI::quit() -- gst pipeline success closed\n";
} // -- IMX219CameraMIPI::quit()

uint8_t* IMX219CameraMIPI::receiveFrame(int &w, int &h, int &id, int &num)
{
    if(sync.frame_ready.load())
    {
        sync.frameMtx.lock();
        w = frame_prepared.cols; 
        h = frame_prepared.rows;
        id = 0;
        num = frame_counter;
        sync.frameMtx.unlock();
        sync.frame_ready.store(false);
        return frame_prepared.data;
    }
    return nullptr;    
}

void IMX219CameraMIPI::keyHandler(uchar &key){return void();} // END keyHandler

void IMX219CameraMIPI::getFormatedImage(uint8_t * dat, int w, int h, int id, cv::Mat & image)
{
#if defined(CCM_8UC1)
    memcpy(image.data, dat, image.total());
#elif defined(CCM_8UC3)
    memcpy(image.data, dat, 3*image.total());
#else
    throw std::runtime_error("Not supported color space for output format");
#endif
} // END getFormatedImage

bool IMX219CameraMIPI::isBayerColorChannel()
{
    return false;
} // -- END IMX219CameraMIPI::isBayerColorChannel()

void IMX219CameraMIPI::keyHandler(char &key)
{

} // -- END IMX219CameraMIPI::keyHandler(char &key)

void IMX219CameraMIPI::workflow()
{

} // -- END IMX219CameraMIPI::workflow()

int IMX219CameraMIPI::getColorChannels()
{
#if defined(CCM_8UC1)
    return 1;
#elif defined(CCM_8UC3)
    return 3;
#else
    return 0;
#endif
} // -- END IMX219CameraMIPI::getColorChannels()

