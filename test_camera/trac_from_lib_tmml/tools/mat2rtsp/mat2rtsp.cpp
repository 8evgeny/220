#include "mat2rtsp.hpp"

using namespace std ;
using namespace cv;

mat2rtsp::mat2rtsp(const string &pathToConfig, bool &ok, int &frame_w, int &frame_h, int &dev_fps)
{
    ok = get_ini_params(pathToConfig);
    if (work == 1)
    {
        //Start rtsp server (tools)
        string startRTSP = "./mat2rtsp_bin " + to_string(port); // argv[1]
        startRTSP += " " + mountpoint + " \"";  // argv[2]
        startRTSP += "udpsrc port=61111 ! queue ! application/x-rtp, media=video, clock-rate=90000, "; //argv[3]
        startRTSP += "encoding-name=H" + codec + ", payload=96 ! rtph" + codec + "depay ! queue ! rtph" + codec + "pay name=pay0 pt=96" ; //argv[3]
        startRTSP += "\"";
        cout << "startRTSP\n" << startRTSP << endl << endl;
        thread([&](){system(startRTSP.c_str());}).detach();

        if(!ok){cout << "NOT get_ini_params!\n"; return;}
        mat2rtsp_sender = make_unique<VideoWriter>();
        string pipeline = "";

    #ifndef RAW_VIDEO_TO_UDP
        pipeline += "appsrc ! videoconvert ! video/x-raw";
        #ifdef ARCH_ARM
            pipeline += ", format=RGB ! mpph" + codec + "enc gop=10 bps=" + to_string(bps * 1000);
        #endif  // END #ifndef ARCH_ARM
        #ifdef ARCH_X86_64
            pipeline += " ! x" + codec + "enc key-int-max=10 tune=zerolatency bitrate=" + to_string(bps);
        #endif  // END #ifndef ARCH_X86_64
        pipeline += " ! rtph" + codec + "pay  config-interval=1 ! udpsink host=127.0.0.1 port=61111 ";
    #endif // END #ifndef RAW_VIDEO_TO_UDP

    #ifdef RAW_VIDEO_TO_UDP  //to future develop
        pipeline += "appsrc ! videoconvert ! video/x-raw, format=UYVY, width=1920, height=1080  ! rtpvrawpay  !  ";
        pipeline += "queue ! application/x-rtp, media=(string)video, ";
        pipeline += "encoding-name=(string)RAW ! udpsink host=127.0.0.1 port=9999  buffer-size=1000000";
    #endif // END #ifdef RAW_VIDEO_TO_UDP  //to future develop
        int fourcc = 0;
//        if (codec == "264")
//        {
//            fourcc = VideoWriter::fourcc('H','2','6','4');
//        } // END if (codec == "264")
        mat2rtsp_sender->open(pipeline, fourcc, dev_fps, Size(frame_w, frame_h), isColor);
        if(mat2rtsp_sender->isOpened())
        {
            cout << "Video writer Open OK\n";
        } // END if (cv_gst_sender->isOpened())
        else
        {
            cout << "#############  ERROR can't create video writer!\n";
            ok = 0;
            return;
        } // END if(!mat2rtsp_sender->isOpened())
    } // END if (work == 1)
} // END mat2rtsp

void mat2rtsp::get_system_result(const string& system_str, string& cmd_result)
{
    cmd_result = "";
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(system_str.c_str(), "r"), pclose);
    if(!pipe){throw std::runtime_error("popen() failed!");}
    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr){cmd_result += buffer.data();}
} // END get_system_result

void mat2rtsp::kill_system_process(const string & killName)
{
    string result = "";
    get_system_result("ps -e -T | grep " + killName, result);
    int first_dig = -1;
    int last_dig = -1;
    for(int i0 = 0; i0 < result.length(); ++i0)
    {
        if(isdigit(result[i0])){first_dig = i0; break;}
    } // END for(int i0 = 0; i0 < result.length(); ++i0)
    for(int i1 = first_dig + 1; i1 < result.length(); ++i1)
    {
        if(!isdigit(result[i1])){last_dig = i1; break;}
    } // END for(int i1 = first_dig + 1; i1 < result.length(); ++i1)
    string pid = result.substr(first_dig, last_dig - first_dig);
    cout << "pid=" << pid << endl;
    string cmd = "kill " + pid;
    system(cmd.c_str());
} // END kill_system_process

mat2rtsp::~mat2rtsp()
{
    cout << "Dectructor mat2rtsp" << endl;
    if (work == 1)
    {
        kill_system_process("mat2rtsp_bin");
    } // END if (work == 1)
} // END ~mat2rtsp

bool mat2rtsp::get_ini_params(const string &config)
{
    cout << "BEGIN get_ini_params mat2rtsp" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    INIReader reader(config);
    port = reader.GetInteger("mat2rtsp", "port", -1);
    if(port == -1)
    {
        cout << "mat2rtsp port not declared!\n";
        return false;
    } // END  if(port == -1)
    mountpoint = reader.Get("mat2rtsp", "mountpoint", "oops");
    if(mountpoint == "oops")
    {
        cout << "mat2rtsp mountpoint not declared!\n";
        return false;
    } // END  if(mountpoint == "oops")
    work = reader.GetInteger("mat2rtsp", "work", -1);
    if(work == -1)
    {
        cout << "mat2rtsp work not declared!\n";
        return false;
    } // END  if(work == -1)
    bps = reader.GetInteger("mat2rtsp", "bps", -1);
    if(bps == -1)
    {
        cout << "mat2rtsp bps not declared!\n";
        return false;
    } // END  if(bps == -1)
    codec = reader.Get("mat2rtsp", "codec", "oops");
    if(codec == "oops")
    {
        cout << "mat2rtsp codec not declared!\n";
        return false;
    } // END  if(codec == "oops")

    return true;
} // END get_ini_params

void mat2rtsp::sendToRTSPServer(Mat & frame_receive)
{
    if (work == 1)
    {
        mat2rtsp_sender->operator << (frame_receive);
    }// END if (work == 1)
} // END sendToHost

/*
OK
"videotestsrc ! mpph264enc ! rtph264pay name=pay0 pt=96 ";

OK
./mat2rtsp 8888 imx415 "v4l2src device=/dev/video42 io-mode=dmabuf ! \
video/x-raw, width=1920, height=1080, framerate=30/1 ! videoconvert ! \
mpph264enc min-force-key-unit-interval=1000 bps=4000000 level=40 ! rtph264pay name=pay0 pt=96"

OK
./mat2rtsp 8888 imx415 "v4l2src device=/dev/video60 ! \
mpph264enc min-force-key-unit-interval=1000 bps=8000000 level=40 ! rtph264pay name=pay0 pt=96"

OK encoding VIDEO - work variant
./mat2rtsp 8888 imx415 "udpsrc port=9999 ! queue ! application/x-rtp, media=video, clock-rate=90000, \
encoding-name=H264, payload=96 ! rtph264depay ! queue ! rtph264pay name=pay0 pt=96"


sudo sysctl -w net.core.rmem_max=1000000 (for RAW VIDEO stream)
server RAW OK
gst-launch-1.0 -v v4l2src device=/dev/video42 ! video/x-raw, format=UYVY, width=1920, height=1080 ! \
queue ! rtpvrawpay ! 'application/x-rtp, media=(string)video, encoding-name=(string)RAW' ! udpsink host=127.0.0.1 port=9999 buffer-size=1000000

client RAW OK
gst-launch-1.0 udpsrc port="9999" buffer-size=1000000   caps = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, \
sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)1920, height=(string)1080, colorimetry=(string)SMPTE240M, payload=(int)96, \
ssrc=(uint)4163085018, timestamp-offset=(uint)470302868, seqnum-offset=(uint)12496, a-framerate=(string)30" ! \
rtpvrawdepay ! videoconvert ! queue ! autovideosink sync=false

server RAW OK
gst-launch-1.0 -v videotestsrc pattern=ball ! queue ! rtpvrawpay ! udpsink host="127.0.0.1" port="9999"

client RAW OK
gst-launch-1.0 udpsrc port="9999" caps = "application/x-rtp, media=(string)video, clock-rate=(int)90000, \
encoding-name=(string)RAW, sampling=(string)YCbCr-4:4:4, depth=(string)8, width=(string)320, height=(string)240, \
colorimetry=(string)BT601-5, payload=(int)96, ssrc=(uint)1056592822, timestamp-offset=(uint)3565672070, seqnum-offset=(uint)23533, \
a-framerate=(string)30" ! rtpvrawdepay ! videoconvert ! queue ! autovideosink sync=false

server h264  OK
gst-launch-1.0 -v videotestsrc pattern=ball ! videoconvert ! video/x-raw, format=RGB ! \
mpph264enc ! rtph264pay  config-interval=1 ! udpsink host=127.0.0.1 port=9999

client h264  OK
gst-launch-1.0 -v udpsrc port=9999 ! queue ! application/x-rtp, media=video, clock-rate=90000, \
encoding-name=H264, payload=96 ! rtph264depay ! h264parse ! avdec_h264 ! autovideosink

server RAW
gst-launch-1.0 -v videotestsrc pattern=ball ! tee name=t  t. ! queue ! autovideosink \
t. ! queue ! rtpvrawpay ! udpsink host="127.0.0.1" port="9999"
*/
