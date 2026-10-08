#include "rtsp.hpp"
#include <thread>
#include <gst/app/gstappsink.h>
#ifdef GUI_OK
  #include <opencv2/highgui.hpp>
#endif //GUI_OK
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace devices;
using namespace devices::rtsp;

namespace devices
{
  void newData(GstElement *appsink, RTSP *client)
  {
      std::lock_guard lock(client->sync.frameMtx);
      GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
      if (sample == NULL)
      {
          fprintf(stderr, "gst_app_sink_pull_sample returned null\n");
          return;
      }
      if(client->first_frame)
      {
          if(!client->settings->video.width || ! client->settings->video.height || !client->settings->video.fps)
          {
              GstCaps *caps = gst_sample_get_caps(sample);
              GstStructure *structure = gst_caps_get_structure(caps, 0);
              client->settings->video.width = g_value_get_int(gst_structure_get_value(structure, "width"));
              client->settings->video.height = g_value_get_int(gst_structure_get_value(structure, "height"));
              const GValue *fr = gst_structure_get_value(structure, "framerate");
              gint fps_n = 0;
              gint fps_d = 0;
              if(fr)
              {
                  if (G_VALUE_TYPE (fr) == GST_TYPE_FRACTION_RANGE)
                  {
                      fr = gst_value_get_fraction_range_min(fr);

                  } // END  if (G_VALUE_TYPE (fr) == GST_TYPE_FRACTION_RANGE)
              } // END if (fr)
              if (fr)
              {
                  fps_n = gst_value_get_fraction_numerator(fr);
                  fps_d = gst_value_get_fraction_numerator(fr);
                  client->settings->video.fps = gst_value_get_fraction_numerator(fr);
              } // END if (fr)
              if (!fps_d)
              {
                  /* unspecified or zero denominator is variable framerate */
                  fps_n = 0;
                  fps_d = 1;
              }
              cout << "rtsp_h265_device:: undifined x-raw data: new data:" << endl
                   << "      width: " << client->settings->video.width << endl
                   << "      height: " << client->settings->video.height << endl
                   << "      fps: " << client->settings->video.fps << endl;

              gst_caps_unref(caps);
          } // END if (!param: w,h,fps)
          client->frame_byte_length = client->settings->video.width * client->settings->video.height * client->getColorChannels();
          client->first_frame = false;
          std::cout << "      frame_byte_length: " << client->frame_byte_length << endl;
      } // END if client first_frame
      GstBuffer *buffer = gst_sample_get_buffer(sample);
      gst_buffer_map(buffer, &client->map, GST_MAP_READ);
      gst_buffer_unmap(buffer, &client->map);
      gst_sample_unref(sample);
      client->frameCnt++;
      client->sync.frameReady.store(true);
      client->sync.frameReadyCv.notify_one();
  } // END newData

  static gboolean my_bus_callback(GstBus *bus, GstMessage *message, gpointer data)
  {
      // Debug message
      //g_print("Got %s message\n", GST_MESSAGE_TYPE_NAME(message));
      switch(GST_MESSAGE_TYPE(message)) {
      case GST_MESSAGE_ERROR:
      {
          GError *err;
          gchar *debug;
          gst_message_parse_error(message, &err, &debug);
          g_print("Error: %s\n", err->message);
          g_error_free(err);
          g_free(debug);
          break;
      } // END GST_MESSAGE_ERROR
      case GST_MESSAGE_EOS: //  end-of-stream
      {
          g_print("Exit pipeline!\n");


          break;
      } // END GST_MESSAGE_EOS
      default:    //         unhandled message
      {

          break;
      } // END default
      } // END switch(GST_MESSAGE_TYPE(message))
      return true;
  } // END my_bus_callback
} // -- END namespace devices

RTSP::RTSP(SettingsPtr &settings) : settings(settings)
{
#if defined(CCM_8UC3)
    settings->output.format_out = "RGB";
#elif defined(CCM_8UC1)
    settings->output.format_out = "GRAY8";
#else
    throw std::runtime_error("Not supported color space for output format");
#endif
} // END RTSP

RTSP::~RTSP()
{
    quit();
} // END ~RTSP

void RTSP::get_system_result(const string &system_str, string &cmd_result)
{
    cmd_result = "";
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(system_str.c_str(), "r"), pclose);
    if(!pipe){throw std::runtime_error("popen() failed!");}
    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr){cmd_result += buffer.data();}
} // END get_system_result

void RTSP::isCameraMIPI()
{
    cameraMIPI = false;
    string cmd = "v4l2-ctl --list-formats -d /dev/video";
    cmd += to_string(settings->video.camera_id) + " | grep MJPG" ;
    string result = "";
    get_system_result(cmd, result);
    if (result == "") { cameraMIPI = true; }
} // END void RTSP::isCameraMIPI()

void RTSP::setup()
{
    isCameraMIPI();
    //Start rtsp server
    string startRTSP = "";
    if(cameraMIPI)
    { //MIPI Camera
            startRTSP = "./rtsp_server_device " +to_string(settings->src.port) + " "; //argv[1]
            startRTSP += settings->video.mountpoint + " ";                       //argv[2]
            startRTSP += "\"v4l2src device=/dev/video" + to_string(settings->video.camera_id) + " io-mode=dmabuf ";  //argv[3]
            startRTSP += "! video/x-raw, width=" + to_string(settings->video.width);
            startRTSP += ", height=" + to_string(settings->video.height) + ", framerate=";
            startRTSP += to_string(settings->video.fps) + "/1 ! videoconvert ";
            startRTSP += "! mpp" + settings->src.codec + "enc ";
            startRTSP += "min-force-key-unit-interval=1000 ";
            startRTSP += "bps=" + to_string(settings->video.bps) + " ";
            startRTSP += "level=40 "; // (1080p@30fps)
            startRTSP += "! rtp" + settings->src.codec + "pay name=pay0 pt=96\"";

            cout <<"startRTSP\n"<<startRTSP<<endl<<endl;
            thread ([&](){system(startRTSP.c_str());}).detach();

    } // END if(cameraMIPI)
    else
    { //USB camera
        cout <<"+++++++ USB Camera ++++++++\n";
//For future work
//
//






    } // END if(!cameraMIPI)

    std::string video_settings_str = "";
    if(settings->video.fps && settings->video.width && settings->video.height)
    {
        video_settings_str = ",width=" + to_string(settings->video.width) +
                ",height=" + to_string(settings->video.height) +
                ",framerate=" + to_string(settings->video.fps) + "/1 ";
    } // END if(settings->video.fps && settings->video.width && settings->video.height)

    pipeline_str = "rtspsrc location=rtsp://" + settings->src.ip +  ":";
    pipeline_str += to_string(settings->src.port) + "/" + settings->video.mountpoint;
    pipeline_str += " latency=10 ";
    pipeline_str += "! application/x-rtp, payload=96 ";
    pipeline_str += "! rtp" + settings->src.codec + "depay ! " + settings->src.codec + "parse ";
    pipeline_str += "! avdec_" + settings->src.codec + " ";
    pipeline_str += "! videoconvert ! video/x-raw, format=(string)";
    pipeline_str += settings->output.format_out + video_settings_str.c_str() + "! videoconvert ";
#if defined(CCM_8UC3)
    pipeline_str += "! video/x-raw, format=(string)BGR ";
#endif // END #if defined(CCM_8UC3)
    pipeline_str += "! appsink name=" + settings->sink.name + " ";
    pipeline_str += "emit-signals=" + settings->sink.emit_signals + " ";
    pipeline_str += "sync=" + settings->sink.sync + " ";
    pipeline_str += "max-buffers=" + to_string(settings->sink.max_buffers) + " ";
    pipeline_str += "drop=" + settings->sink.drop;
    cout << "GST pipeline: \n" << pipeline_str << endl;

    gst_init(NULL, NULL);

    sync.frameReady.store(false);
    gchar *descr = g_strdup(pipeline_str.c_str());
    // Check pipeline
    error = nullptr;
    pipeline = gst_parse_launch(descr, &error);

    if(error)
    {
        g_print("could not construct pipeline: %s\n", error->message);
        g_error_free(error);
        exit(-1);
    } // END if(error)

    // Get sink
    sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    g_signal_connect(sink, "new-sample", (GCallback)newData, this);
    state.inited = true;
} // END setup()

void RTSP::start()
{
    if(!state.inited)
    {
        setup();
        if (!state.inited){return;}
    }
    sync.handleFrameExecute.store(true, std::memory_order_release);
    std::thread(&RTSP::exec, this).detach();
    std::thread(&RTSP::runHandleFrame, this).detach();
} // END RTSP::start()

void RTSP::quit()
{
    sync.handleFrameExecute.store(false, std::memory_order_acquire);
    sync.frameReadyCv.notify_one();
    if(pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(pipeline));
        pipeline = nullptr;
    } // END if(pipeline)
} // END RTSP::quit()

uint8_t *RTSP::receiveFrame(int &w, int &h, int &id, int &num)
{
    if (sync.frameReady.load())
    {
        w = settings->video.width;
        h = settings->video.height;
        id = 0;
        num = frameCnt;
        sync.frameReady.store(false);
        return map.data;
    } // END if (sync.frameReady.load())
    return nullptr;
} // END receiveFrame

void RTSP::getFormatedImage(uint8_t * dat, int w, int h, int id, cv::Mat & image)
{
#if defined(CCM_8UC1)
    memcpy(image.data, dat, image.total());
#elif defined(CCM_8UC3)
    memcpy(image.data, dat, 3*image.total());
#else
    throw std::runtime_error("Not supported color space for output format");
#endif
    sync.frameReady.store(false);
} // END getFormatedImage

int RTSP::getColorChannels()
{
#if defined(CCM_8UC3)
    return 4;
#elif defined(CCM_8UC1)
    return 1;
#else
    return 0;
#endif
} // END RTSP::getColorChannels()

bool RTSP::isBayerColorChannel() { return false; }

void RTSP::keyHandler(unsigned char & key) {}

void RTSP::workflow() {}

void RTSP::exec()
{
    GstBus *bus;
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, my_bus_callback, nullptr);
    gst_object_unref(bus);

    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
} // END exec

void RTSP::runHandleFrame()
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
            handler->handle(map.data, settings->video.width,
                            settings->video.height, 0, frameCnt);
        } // -- END for(auto handler : frame_handlers)
        lk.unlock();
    } // -- END while
} // END runHandleFrame
