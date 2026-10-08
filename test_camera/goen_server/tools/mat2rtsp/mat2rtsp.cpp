#include "mat2rtsp.hpp"

using namespace std ;
using namespace cv;

mat2rtsp::mat2rtsp(const string &pathToConfig, bool &ok, int &frame_w, int &frame_h, int &dev_fps)
{
    ok = get_ini_params(pathToConfig);
    if(work == 1)
    {
        if(!ok){cout << "NOT get_ini_params!\n"; return;}
        mat2rtsp_sender = make_unique<VideoWriter>();

#ifdef ARCH_X86_64
        string pipeline = "appsrc ! videoconvert ! x" + codec + "enc speed-preset=" + to_string(gop) + " key-int-max=25  bitrate=" + to_string(bps * 1000);
#endif // ARCH_X86_64
#ifdef ARCH_ARM
        string pipeline = "appsrc ! videoconvert ! video/x-raw, format=RGB ! mpph" + codec + "enc gop=" + to_string(gop) + " bps=" + to_string(bps * 1000);
#endif // ARCH_ARM

#ifdef USE_RTP
        pipeline += " ! rtph" + codec + "pay mtu=" + to_string(mtu) + " config-interval=" + to_string(config_interval) + " ! udpsink host=" + send_ip + " port=" + to_string(port);
#else // USE_RTP
        pipeline += " ! rtspclientsink location=rtsp://localhost:" + port_mountpoint;
#endif // !USE_RTP
        cout << "pipeline=" << pipeline << endl;

        int fourcc = 0;
        mat2rtsp_sender->open(pipeline, fourcc, dev_fps, Size(frame_w, frame_h), 1);
        if(mat2rtsp_sender->isOpened())
        {
            cout << "Video writer Open OK\n";
        } // END if (mat2rtsp_sender->isOpened())
        else
        {
            cout << "!!! ERROR can't create video writer!\n";
            ok = 0;
            return;
        } // END if(!mat2rtsp_sender->isOpened())
    } // END if (work == 1)
} // END mat2rtsp

mat2rtsp::~mat2rtsp()
{
    cout << "Destructor mat2rtsp" << endl;
} // END ~mat2rtsp

bool mat2rtsp::get_ini_params(const string &config)
{
    cout << "BEGIN get_ini_params mat2rtsp" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    INIReader reader(config);

#ifdef USE_RTP
    std::string section = "rtp_sender";
#else
    std::string section = "mat2rtsp";
#endif // USE_RTP
    cout << "\n[" << section << "]:\n";
    work = reader.GetInteger(section, "work", -111111111);
    if(work == -111111111)
    {
        cout << "\twork not declared!\n";
        return false;
    } // END  if(work == -111111111)

    send_ip = reader.Get("NETWORK", "send_ip", "oops");
    if(send_ip == "oops")
    {
        cout << "\tsend_ip not declared!\n";
        return false;
    } // END  if(send_ip == "oops")

    bps = reader.GetInteger(section, "bps", -111111111);
    if(bps == -111111111)
    {
        cout << "\tbps not declared!\n";
        return false;
    } // END  if(bps == -111111111)

    mtu = reader.GetInteger(section, "mtu", -111111111);
    if(mtu == -111111111)
    {
        cout << "\tmtu not declared!\n";
        return false;
    } // END  if(mtu == -111111111)

    gop = reader.GetInteger(section, "gop", -111111111);
    if(gop == -111111111)
    {
        cout << "\tgop not declared!\n";
        return false;
    } // END  if(gop == -111111111)


    config_interval = reader.GetInteger(section, "config_interval", -111111111);
    if(config_interval == -111111111)
    {
        cout << "\tconfig_interval not declared!\n";
        return false;
    } // END  if(config_interval == -111111111)

    codec = reader.Get(section, "codec", "oops");
    if(codec == "oops")
    {
        cout << "\tcodec not declared!\n";
        return false;
    } // END  if(codec == "oops")

#ifndef USE_RTP // USE_RTP
    port_mountpoint = reader.Get(section, "port_mountpoint", "oops");
    if(port_mountpoint == "oops")
    {
        cout << "\tport_mountpoint not declared!\n";
        return false;
    } // END  if(port_mountpoint == "oops")

#else // !USE_RTP
    port = reader.GetInteger(section, "port", -111111111);
    if(port == -111111111)
    {
        cout << "\tParameterport not declared!\n";
        return false;
    } // END  if(port == -111111111)
#endif // USE_RTP
    return true;
} // END get_ini_params

void mat2rtsp::sendToRTSPServer(Mat & frame_receive)
{
    if (work == 1)
    {        
        mat2rtsp_sender->operator << (frame_receive);
    }// END if (work == 1)
} // END sendToHost

