#include "corsair400raw.hpp"

#include <thread>
#include <chrono>

#include "global_defines.h"

namespace devices
{

Corsair400raw::Corsair400raw(const corsair_400_raw::Settings &_settings, bool &report) : settings(_settings)
{
#if not defined(CCM_8UC1)
    throw std::runtime_error(
                "\033[1;31m Not supported color space for output format!\n"
                "Corsair400raw support only GRAYSCALE (CCM_8UC1) output (for RGB used BAYER, need unpack after device), \n"
                "check CMakeLists define \"USE_COLOR_CHANNEL_MODE\"\033[0m");
#endif // !CCM_8UC1
    report = start_backend();
} // -- constructor Corsair400raw

Corsair400raw::~Corsair400raw()
{
    std::cout << "---Destructor Corsair400raw" << std::endl;
    quit();
} // -- destructor  Corsair400raw

void Corsair400raw::setup()
{
    std::cout << "WARNING: Corsair400raw::setup doing nothing" << std::endl;
} // -- END setup

void Corsair400raw::start()
{
    std::cout << "WARNING: Corsair400raw::start doing nothing" << std::endl;
} // -- END start

uint8_t* Corsair400raw::receiveFrame(int &w, int &h, int &id, int &num)
{
    std::cout << "ERROR: not support method receiveFrame" << std::endl;
    return nullptr;
} // -- END receiveFrame

void Corsair400raw::getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image)
{
#if defined(CCM_8UC1)
    memcpy(image.data, f, image.total());
#else // if(!CCM_8UC1)
    throw std::runtime_error("\033[1;31m Not supported 4/3 channels color space for output format! Only CV_8UC1!\033[0m");
#endif // if(!CCM_8UC1)
} // -- END getFormatedImage

int Corsair400raw::getColorChannels()
{
    return 1;
} // -- END getColorChannels

bool Corsair400raw::isBayerColorChannel()
{
    return false;
} // -- END isBayerColorChannel

void Corsair400raw::keyHandler(unsigned char &key)
{

} // -- END keyHandler

bool Corsair400raw::getState()
{
    return true;
} // -- END getState

void Corsair400raw::workflow()
{

} // -- END workflow

bool Corsair400raw::start_backend()
{
    std::cout << "BEGIN " << settings.section_name << "::setup\n";

    SocketAddress _rawReceiving(settings.net.rcv_port, settings.net.rcv_ip.c_str()); // адрес и порт приемнкиа видео (текущая машина)
    SocketAddress _rawSource(39379, "172.27.240.1"); // адрес и порт источника видео

    std::cout << "DBG:: Corsair400raw:: rcv_port=" << _rawReceiving.port() << "; ip=" << _rawReceiving.ip() << std::endl;

    UDPPacketParametrs _srvMsgPar({UDP_SRV_FLAG, UDP_SRV_LEN, UDP_SRV_SRV_LEN, UDP_SRV_DATA_LEN});
    UDPPacketParametrs _vidMsgPar({UDP_VID_FLAG, UDP_VID_LEN, UDP_VID_SRV_LEN, UDP_VID_DATA_LEN});

    _rawRcv = std::make_unique<RAWVideoServer>(_srvMsgPar, _vidMsgPar, (int)FRAME_BUF_LEN);

    _rawRcv->open(_rawReceiving, _rawSource);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    bool connStatus = _rawRcv->isConnected();
    if(!connStatus)
    {
        std::cout << "ERROR: device corsair400raw error open!" << std::endl;
        return false;
    } // END if(!connStatus)

    sync.execute.store(true);

    std::thread(&Corsair400raw::exec, this).detach();

    std::cout << "END " << settings.section_name << "::setup\n";
    return true;
} // -- END start_backend

void Corsair400raw::quit()
{
    if(!sync.execute.load())
    {
        return;
    } // END if(!sync.execute.load())
    _rawRcv->close();
    sync.execute.store(false);

    std::this_thread::sleep_for(std::chrono::microseconds(250));

    std::cout << "END " << settings.section_name << "::quit" << std::endl;
    return;
} // -- END quit


/*
channels from global_defines.hpp

#define CHANNEL_ID_TVRGB        (0x00u)
#define CHANNEL_ID_FUSION       (0x04u)
#define CHANNEL_ID_MWIR         (0x01u)
#define CHANNEL_ID_TVGRAY       (0x03u)
#define CHANNEL_ID_TVGRAYMWIR   (0x02u)
#define CHANNEL_ID_TEST         (0x07u)

*/

std::string ch_id_to_ch_alias(int ch_id)
{
    std::string ch_alias = "";
    switch(ch_id)
    {
        case CHANNEL_ID_TVRGB:
            ch_alias = "CHANNEL_ID_TVRGB";
            break;
        case CHANNEL_ID_MWIR:
            ch_alias = "CHANNEL_ID_MWIR";
            break;
        case CHANNEL_ID_TVGRAY:
            ch_alias = "CHANNEL_ID_MWIR";
            break;
        default:
            ch_alias = "NONE";
            break;
    }; // END switch(ch_id)
    return ch_alias;
} // -- ch_id_to_ch_alias

void Corsair400raw::exec()
{
    int32_t len = 0;
    FrameParametrs framePar;
    uint8_t* frame_ptr = nullptr;

    while(sync.execute.load())
    {
        frame_ptr = _rawRcv->get(len, framePar);
        if(frame_ptr)
        {
            first_frame_sz.width = framePar.width;
            first_frame_sz.height = framePar.height;
            first_frame_ch_id = framePar.id;
            frame_len_bytes = framePar.width * framePar.height * 1; // 1 channel cv mat only
            std::cout << "\nLOG [Corsair400raw] Img param: \n" <<
                "\t channel(alias) = " << ch_id_to_ch_alias(first_frame_ch_id) << "\n" <<
                "\t width = " << first_frame_sz.width << "\n" <<
                "\t height = " << first_frame_sz.height << "\n\n" << std::endl;  
            break;
        } // END if(frame_ptr)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } // END while(sync.execute.load())

    // cv::Mat fr_rcv(first_frame_sz.height, first_frame_sz.width, CV_8UC1, cv::Scalar(0));

    while(sync.execute.load())
    {
        frame_ptr = _rawRcv->get(len, framePar);
        if(frame_ptr)
        {
            if(framePar.width != first_frame_sz.width || framePar.height != first_frame_sz.height)
            {
                std::cout << "WARNING: invalid incoming frame size. Select valid video channel" << std::endl;
            } // END if(framePar.width != first_frame_sz.width || framePar.height != first_frame_sz.height)
            else
            {
                // memcpy(fr_rcv.data, frame_ptr, frame_len_bytes);
                cv::Mat fr_rcv(first_frame_sz.height, first_frame_sz.width, CV_8UC1, frame_ptr);
                if(first_frame_ch_id == CHANNEL_ID_TVRGB)
                {
                    cv::cvtColor(fr_rcv, fr_rcv, cv::COLOR_BayerGR2GRAY);
                } // END if(first_frame_ch_id == CHANNEL_ID_TVRGB)
                for(auto handler : frame_handlers)
                {
                    handler->handle(fr_rcv.data, fr_rcv.cols,
                                    fr_rcv.rows, 0, frame_id);
                } // -- END for(auto handler : frame_handlers)
                ++frame_id;
                if(frame_id >= frame_id_cycle_reset)
                {
                    frame_id = 0;
                } // END if(frame_id >= frame_id_cycle_reset)
            } // END else
        } // END if(frame_ptr)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } // -- while(sync.execute.load())
    std::cout << "Corsair400raw::exec() success complete" << std::endl;
} // -- END exec

} // -- namespace devices
