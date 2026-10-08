#include "cv_gst_send.hpp"
#include "iostream"
#include "INIReader.h"
using namespace std ;
using namespace cv;

cv_gst_send::cv_gst_send(const std::string &pathToConfig, bool &ok, int &frame_w, int &frame_h, int &dev_fps)
{
    cout <<"gst_send frame_w="<<frame_w<<endl;
    cout <<"gst_send frame_h="<<frame_h<<endl;
    cout <<"gst_send dev_fps="<<dev_fps<<endl;
    ok = get_ini_params(pathToConfig);
    if(!ok){cout << "NOT get_ini_params!\n"; return;}
    int send_fps = 0;
    if (dev_fps > 0)
    {
        send_fps = dev_fps;
        cout << "send_fps = " << send_fps << endl;
    }
    else
    {
        send_fps = fps;
        cout << "device fps incorrect!!!\nsend_fps = " << send_fps << endl;
    }
    cv_gst_sender = make_unique<VideoWriter>();
    string pipeline = pipeline1 + " " + pipeline2 + " " + pipeline3;

    cv_gst_sender->open(pipeline, 0, (double)send_fps, cv::Size(frame_w, frame_h),
#if defined(CCM_8UC1)
    false
#elif defined(CCM_8UC3)
    true
#endif // END defined(CCM_8UC1)
    );
    if(cv_gst_sender->isOpened())
    {
        cout << "Video writer Open OK\n";
    } // END if (cv_gst_sender->isOpened())
    else
    {
        cout << "#############  ERROR can't create video writer!\n";
        ok = 0;
        return;
    } // END if(!cv_gst_sender->isOpened())
} // END cv_gst_send

cv_gst_send::~cv_gst_send()
{
    cout << "Dectructor cv_gst_send" << endl;
} // END ~cv_gst_send

bool cv_gst_send::get_ini_params(const string &config)
{
    cout << "BEGIN get_ini_params cv_gst_send" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    INIReader reader(config);

    pipeline1 = reader.Get("cv_gst_send", "pipeline1", "");
    if(pipeline1 == "")
    {
        cout << "cv_gst_send pipeline1 not declared!\n";
        return false;
    } // END  if(pipeline1 == "")

    pipeline2 = reader.Get("cv_gst_send", "pipeline2", "");
    if(pipeline2 == "")
    {
        cout << "cv_gst_send pipeline2 not declared!\n";
        return false;
    } // END  if(pipeline2 == "")

    pipeline3 = reader.Get("cv_gst_send", "pipeline3", "");
    if(pipeline3 == "")
    {
        cout << "cv_gst_send pipeline3 not declared!\n";
        return false;
    } // END  if(pipeline3 == "")

    fps = reader.GetInteger("cv_gst_send", "fps", -1);
    if(fps == -1)
    {
        cout << "cv_gst_send fps not declared!\n";
        return false;
    } // END  if(send_frame_h == -1)

    return true;
} // END get_ini_params

void cv_gst_send::sendToHost(Mat & frame_receive)
{
    cv_gst_sender->operator << (frame_receive);
} // END sendToHost


