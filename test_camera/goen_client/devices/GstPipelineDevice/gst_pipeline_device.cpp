#include "gst_pipeline_device.hpp"

using namespace std;
using namespace devices;

namespace devices
{
void newData(GstElement *appsink, GST_PIPELINE_device *client)
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
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *structure = gst_caps_get_structure(caps, 0);
        client->fps = 0;
        client->width = g_value_get_int(gst_structure_get_value(structure, "width"));
        client->height = g_value_get_int(gst_structure_get_value(structure, "height"));
        const GValue *fr = gst_structure_get_value(structure, "framerate");
        gint fps_n = 0;
        gint fps_d = 0;

        if (fr)
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
            client->fps = gst_value_get_fraction_numerator(fr);
        } // END if (fr)
        if (!fps_d)
        {
            /* unspecified or zero denominator is variable framerate */
            fps_n = 0;
            fps_d = 1;
        }
        cout << "gst_pipeline_device:: undifined x-raw data: new data:" << endl
             << "      width:\t" << client->width << endl
             << "      height:\t" << client->height << endl
             << "      fps:\t" << client->fps << endl;

        gst_caps_unref(caps);
        client->frame_byte_length = client->width * client->height * client->getColorChannels();
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
        f_state.store(false);
        break;
    } // END GST_MESSAGE_ERROR

    case GST_MESSAGE_EOS: //  end-of-stream
    {
        g_print("Exit pipeline!\n");
        f_state.store(false);
        break;
    } // END GST_MESSAGE_EOS

    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState oldstate;
        GstState newstate;
        GstState pending;
        string oldstate_str, newstate_str, pending_str;
        gst_message_parse_state_changed(message, &oldstate, &newstate, &pending);
        switch(oldstate)
        {
        case 0: {oldstate_str = "VOID_PENDING"; break;}
        case 1: {oldstate_str = "NULL"; break;}
        case 2: {oldstate_str = "READY"; break;}
        case 3: {oldstate_str = "PAUSED"; break;}
        case 4: {oldstate_str = "PLAYING"; break;}
        } // END switch(oldstate)
        switch(newstate)
        {
        case 0: {newstate_str = "VOID_PENDING"; break;}
        case 1: {newstate_str = "NULL"; break;}
        case 2: {newstate_str = "READY"; break;}
        case 3: {newstate_str = "PAUSED"; break;}
        case 4: {newstate_str = "PLAYING"; break;}
        } // END switch(newstate)
        switch(pending)
        {
        case 0: {pending_str = "VOID_PENDING"; break;}
        case 1: {pending_str = "NULL"; break;}
        case 2: {pending_str = "READY"; break;}
        case 3: {pending_str = "PAUSED"; break;}
        case 4: {pending_str = "PLAYING"; break;}
        } // END switch(pending)

        g_print ("STATE_CHANGED(%i): ", (int)message->type);
        printf (" change state from: %s to %s, pending: %s\n", oldstate_str.c_str(), newstate_str.c_str(), pending_str.c_str());
        break;
    } // END case GST_MESSAGE_STATE_CHANGED:

    case GST_MESSAGE_NEW_CLOCK:
    {
        GstClock * clock;
        gst_message_parse_new_clock(message, &clock);
        g_print("GST_MESSAGE_NEW_CLOCK(%i): clock = %c\n", (int)message->type, clock);
        break;
    } // END case GST_MESSAGE_NEW_CLOCK:

    case GST_MESSAGE_STREAM_STATUS:
    {
        g_print("GST_MESSAGE_STREAM_STATUS(%i): ", (int)message->type);
        GstStreamStatusType type;
        GstElement * owner;
        string type_str;

        gst_message_parse_stream_status(message, &type, &owner);
        switch(type)
        {
        case 0: {type_str = "CREATE"; break;}
        case 1: {type_str = "ENTER"; break;}
        case 2: {type_str = "LEAVE"; break;}
        case 3: {type_str = "DESTROY"; break;}
        case 8: {type_str = "START"; break;}
        case 9: {type_str = "PAUSE"; break;}
        case 10: {type_str = "STOP"; break;}
        } // END switch(type)
        g_print ("  Type: %s ;\n", type_str.c_str());
        break;
    } // END case GST_MESSAGE_STREAM_STATUS:

    case GST_MESSAGE_PROGRESS:
    {
        GstProgressType type;
        gchar *code = NULL;
        gchar *text = NULL;
        gst_message_parse_progress(message, &type, &code, &text);
        g_print ("GST_MESSAGE_PROGRESS(%i): ", (int)message->type);
        g_print ("  Type: %d ;", type);
        g_print ("  Code: %s ;", code);
        g_print ("  Text: %s ;\n", text);
        break;
    } // END case GST_MESSAGE_PROGRESS:

    default:    //         unhandled message
    {
        cout << "my_bus_callback::message type: " << (int)message->type << endl << endl;
        break;
    } // END default
    } // END switch(GST_MESSAGE_TYPE(message))
    return true;
} // END my_bus_callback
} // -- END namespace devices

GST_PIPELINE_device::GST_PIPELINE_device(std::string & pipeline) : pipeline_str(pipeline)
{
    cout << "GST_PIPELINE_device::pipeline = " << pipeline << endl;
    f_state.store(true);
} // END GST_PIPELINE_device

GST_PIPELINE_device::~GST_PIPELINE_device()
{
    quit();
} // END GST_PIPELINE_device

void GST_PIPELINE_device::setup()
{
    cout << "std:: pipeline: \n" << pipeline_str << endl;
    gst_init(NULL, NULL);

    sync.frameReady.store(false);
    gchar *descr = g_strdup(pipeline_str.c_str());
    // Check pipeline
    error = nullptr;
    pipeline = gst_parse_launch(descr, &error);

    if(error) {
        g_print("could not construct pipeline: %s\n", error->message);
        g_error_free(error);
        f_state.store(false);
        exit(-1);
    } // END if(error)

    // Get sink
    sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

    g_signal_connect(sink, "new-sample", (GCallback)newData, this);

    state.inited = true;
} // END setup()

void GST_PIPELINE_device::start()
{
    if (!state.inited)
    {
        setup();
        if (!state.inited)
            return;
    } // END if (!state.inited)

    sync.handleFrameExecute.store(true, std::memory_order_release);
    std::thread(&GST_PIPELINE_device::exec, this).detach();
    std::thread(&GST_PIPELINE_device::runHandleFrame, this).detach();
    //    this_thread::sleep_for(chrono::milliseconds(1000));


} // END start

void GST_PIPELINE_device::quit()
{
    sync.handleFrameExecute.store(false, std::memory_order_acquire);
    sync.frameReadyCv.notify_one();

    if(pipeline)
    {
        cout << "Pipeline GST_STATE_NULL" << endl;
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(pipeline));
        pipeline = nullptr;
    }
    else
    {
        cout << "pipeline was nullptr" << endl;
    } // END else
} // END quit

uint8_t *GST_PIPELINE_device::receiveFrame(int &w, int &h, int &id, int &num)
{
    if (sync.frameReady.load())
    {
        w = width;
        h = height;
        id = 0;
        num = frameCnt;
        sync.frameReady.store(false);
        return map.data;
    };
    return nullptr;
} // END receiveFrame

void GST_PIPELINE_device::getFormatedImage(uint8_t * dat, int w, int h, int id, cv::Mat & image)
{
    memcpy(image.data, dat, 3 * w * h);
    sync.frameReady.store(false);
} // END getFormatedImage

int GST_PIPELINE_device::getColorChannels()
{
    return 3;
} // END getColorChannels

bool GST_PIPELINE_device::isBayerColorChannel()
{
    return false;
} // END isBayerColorChannel

void GST_PIPELINE_device::keyHandler(unsigned char & key)
{

} // END keyHandler

void GST_PIPELINE_device::workflow()
{

} // END workflow

void GST_PIPELINE_device::exec()
{
    GstBus *bus;
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, my_bus_callback, this);
    gst_object_unref(bus);

    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
    cout << " END EXEC PIPELINE! " << endl;
} // END exec

void GST_PIPELINE_device::runHandleFrame()
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
            handler->handle(map.data, width,
                            height, 0, frameCnt);
        } // -- END for(auto handler : frame_handlers)
        lk.unlock();
    } // -- END while
    cout << "END RUN HANDLE_FRAME_EXECUTE" << endl;
} // -- END runHandleFrame
