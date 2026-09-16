#include "gst/video/video-info.h"
#include "gstgoentrack.h"
#include "trsh_wrapper.h"

GST_DEBUG_CATEGORY_STATIC(goen_track_debug);
#define GST_CAT_DEFAULT goen_track_debug

/* Forward declarations */
static GstFlowReturn gst_goen_track_chain(GstPad *pad, GstObject *parent, GstBuffer *buf);
static gboolean gst_goen_track_sink_event(GstPad *pad, GstObject *parent, GstEvent *event);
static gboolean on_interval_timeout(gpointer user_data);
static void gst_goen_track_finalize(GObject *object);



static TrShWrapper* track_wrapper;

TrShWrapper::Rect rectObject;
TrShWrapper::Rect rectTargetObject;
TrShWrapper::Rect rectROI;




/* Plugin registration */
static gboolean plugin_init(GstPlugin *plugin) {
    GST_DEBUG_CATEGORY_INIT(goen_track_debug, "goentrack", 0, "GOEN Track");
    return gst_element_register(plugin, "goentrack", GST_RANK_NONE, GST_TYPE_GOEN_TRACK);
}
#ifndef PACKAGE
#define PACKAGE "goentrack"
#endif


GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    goentrack,
    "GOEN Tracker plugin",
    plugin_init,
    "1.0",
    "LGPL",
    "GStreamer",
    "https://gstreamer.net/"
    )

G_DEFINE_TYPE(GstGoenTrack, gst_goen_track, GST_TYPE_ELEMENT)

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE(
    "sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE(
    "src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static void gst_goen_track_class_init(GstGoenTrackClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

    gobject_class->finalize = gst_goen_track_finalize;

    gst_element_class_set_static_metadata(element_class,
                                          "GOEN Tracker",
                                          "Filter",
                                          "Monitors frame rate with or without active stream",
                                          "Your Name <your.email@example.com>");

    gst_element_class_add_static_pad_template(element_class, &sink_template);
    gst_element_class_add_static_pad_template(element_class, &src_template);


   //track_wrapper.setIniFileNamePath("tracking.ini");


}

static void gst_goen_track_init(GstGoenTrack *self) {
    GstPad *sinkpad = gst_pad_new_from_static_template(&sink_template, "sink");
    GstPad *srcpad = gst_pad_new_from_static_template(&src_template, "src");

    gst_pad_set_chain_function(sinkpad, gst_goen_track_chain);
    gst_pad_set_event_function(sinkpad, gst_goen_track_sink_event);

    gst_element_add_pad(GST_ELEMENT(self), sinkpad);
    gst_element_add_pad(GST_ELEMENT(self), srcpad);
   // std::cout<<"TrackerConfigFile"<<std::endl;
    //track_wrapper.setIniFileNamePath(self->sConfigFile);

    track_wrapper = new TrShWrapper();

    self->interval = 1000; // 1 second updates
    self->callback = NULL;
    self->user_data = NULL;
    self->last_time = 0;
    self->frame_count = 0;
    self->stream_active = FALSE;
   // self->timer_id = g_timeout_add(self->interval, on_interval_timeout, self);
    g_mutex_init(&self->lock);


}


static void gst_goen_track_finalize(GObject *object) {
    GstGoenTrack *self = GST_GOEN_TRACK(object);

    std::cerr << "freeing track_wrapper...\r\n";

    delete(track_wrapper);
    std::cerr << "freeing track_wrapper completed\r\n";
    if (self->timer_id) {
        g_source_remove(self->timer_id);
        self->timer_id = 0;
    }

    g_mutex_clear(&self->lock);
    G_OBJECT_CLASS(gst_goen_track_parent_class)->finalize(object);
}

// static gboolean on_interval_timeout(gpointer user_data) {
//     GstGoenTrack *self = GST_GOEN_TRACK(user_data);
//     gdouble fps = 0.0;
//     gboolean active;

//     g_mutex_lock(&self->lock);
//     active = self->stream_active;

//     if (active && self->last_time > 0) {
//         GstClockTime current_time = gst_util_get_timestamp();
//         GstClockTimeDiff diff = current_time - self->last_time;

//         if (diff > 0) {
//             fps = (gdouble)self->frame_count * GST_SECOND / diff;
//             self->frame_count = 0;
//             self->last_time = current_time;

//         }
//     }

//     if (self->callback) {
//         self->callback(fps, active, self->user_data);
//     }
//     g_mutex_unlock(&self->lock);

//     return G_SOURCE_CONTINUE;
// }

static gboolean gst_goen_track_sink_event(GstPad *pad, GstObject *parent, GstEvent *event) {

    GstGoenTrack *self = GST_GOEN_TRACK(parent);
    GstPad *srcpad = gst_element_get_static_pad(GST_ELEMENT(self), "src");
    gboolean ret;

    switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CAPS: {
        GstCaps *caps;
        gst_event_parse_caps(event, &caps);
        ret = gst_pad_push_event(srcpad, gst_event_new_caps(caps));

        g_mutex_lock(&self->lock);
        self->stream_active = TRUE;
        g_mutex_unlock(&self->lock);
        break;
    }
    case GST_EVENT_EOS: {
        g_mutex_lock(&self->lock);
        self->stream_active = FALSE;
        g_mutex_unlock(&self->lock);
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    default:
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }

    gst_object_unref(srcpad);
    return ret;
}

void handle_set_roi_ext(GstElement *element, gdouble new_roi_ext)
{

    g_printerr("HANDE SET ROI EXT");
    g_return_if_fail(GST_IS_GOEN_TRACK(element));

    GstGoenTrack *self = GST_GOEN_TRACK(element);


    // Clamp the brightness value to the valid range (-255 to 255)
    if ((new_roi_ext > 0) &&  (new_roi_ext < 100))

    {
        // Update the brightness property
        g_mutex_lock(&self->lock);
        track_wrapper->setRoiExt(new_roi_ext);
        g_mutex_unlock(&self->lock);

        std::cout << "handle_set_roi_ext " << new_roi_ext << "\n\n";
    }
    else
        std::cout << "handle_set_roi_ext " << new_roi_ext << " DATA ERROR!\n\n";
}

void handle_set_stop_tracking(GstElement *element)
{
    g_return_if_fail(GST_IS_GOEN_TRACK(element));

    GstGoenTrack *self = GST_GOEN_TRACK(element);

    // Update the brightness property
    g_mutex_lock(&self->lock);
    self->bIsStopTracking = true;
    g_mutex_unlock(&self->lock);

    std::cout << "STOP TRACKING \n";
}
void handle_get_out_object(GstElement *element, ushort &cx, ushort &cy, ushort &w, ushort &h) {
    g_return_if_fail(GST_IS_GOEN_TRACK(element));

    // Serialize the Rect fields into a D-Bus response
    GstGoenTrack *self = GST_GOEN_TRACK(element);
    g_mutex_lock(&self->lock);
    TrShWrapper::Rect outObjectD= rectObject;
    g_mutex_unlock(&self->lock);

    cx=outObjectD.cx;
    cy=outObjectD.cy;
    w=outObjectD.w;
    h=outObjectD.h;
}

void handle_get_out_roi(GstElement *element,  ushort &cx, ushort &cy, ushort &w, ushort &h) {
    g_return_if_fail(GST_IS_GOEN_TRACK(element));

    // Serialize the Rect fields into a D-Bus response
    GstGoenTrack *self = GST_GOEN_TRACK(element);
    GST_OBJECT_LOCK(self);
    TrShWrapper::Rect outRoidD= rectROI;
    GST_OBJECT_UNLOCK(self);
    cx=outRoidD.cx;
    cy=outRoidD.cy;
    w=outRoidD.w;
    h=outRoidD.h;

}

void handle_get_active_lock(GstElement *element, bool &bActive, bool &bLock) {
     g_return_if_fail(GST_IS_GOEN_TRACK(element));
    // Serialize the Rect fields into a D-Bus response
    GstGoenTrack *self = GST_GOEN_TRACK(element);
    g_mutex_lock(&self->lock);
    bActive= self->bTrackIsActive;
    bLock= self->bTrackIsLockedOn;
    g_mutex_unlock(&self->lock);

}


// D-Bus method handler for SetRect
void handle_set_cfg_filename( GstElement *element, char sFileName[20])
{
    g_return_if_fail(GST_IS_GOEN_TRACK(element));

    GstGoenTrack *self = GST_GOEN_TRACK(element);
    g_mutex_lock(&self->lock);

    strcpy(self->sConfigFile, sFileName);
    track_wrapper->setIniFileNamePath(self->sConfigFile);
    std::cout<<"handle_set_cfg_filename "<<self->sConfigFile<<std::endl;

    g_mutex_unlock(&self->lock);

    GST_INFO("Updated ConfigFileName: filename=%s", sFileName);

}



// D-Bus method handler for SetRect
void handle_set_target_object(GstElement *element,  ushort cx, ushort cy, ushort w, ushort h) {

    g_return_if_fail(GST_IS_GOEN_TRACK(element));
    std::cout<<"handle_set_target_object"<<std::endl;
    TrShWrapper::Rect target_obj;
    // Update the static Rect variable
//    target_obj.cx = (unsigned short)cx;
//    target_obj.cy = (unsigned short)cy;
    target_obj.cx = (unsigned short)(cx - (w/2));
    target_obj.cy = (unsigned short)(cy - (h/2));

    target_obj.w = (unsigned short)w;
    target_obj.h = (unsigned short)h;

    GstGoenTrack *self = GST_GOEN_TRACK(element);
    g_mutex_lock(&self->lock);
    rectTargetObject= target_obj;
    self->bIsNewTargetObj = true;
    g_mutex_unlock(&self->lock);

    GST_INFO("Updated Rect: cx=%u, cy=%u, w=%u, h=%u", target_obj.cx, target_obj.cy, target_obj.w, target_obj.h);

}



static GstFlowReturn gst_goen_track_chain(GstPad *pad, GstObject *parent, GstBuffer *buf) {
    GstGoenTrack *self = GST_GOEN_TRACK(parent);
    GstClockTime timestamp = GST_BUFFER_PTS(buf);
    GstPad *srcpad = gst_element_get_static_pad(GST_ELEMENT(self), "src");
    GstFlowReturn ret;

    g_mutex_lock(&self->lock);
    // if (GST_CLOCK_TIME_IS_VALID(timestamp)) {
    //     if (self->last_time == 0) {
    //         self->last_time = timestamp;
    //     }
    //     self->frame_count++;
    // }




    GstMapInfo in_map;
    if (!gst_buffer_map(buf, &in_map, GST_MAP_READ))
    {
        GST_ERROR_OBJECT(self, "Failed to map buffers");
        return GST_FLOW_ERROR;
    }
    GstVideoInfo in_info;
    gst_video_info_from_caps(&in_info, gst_pad_get_current_caps(srcpad));
    int width = GST_VIDEO_INFO_WIDTH(&in_info);
    int height = GST_VIDEO_INFO_HEIGHT(&in_info);
    GST_VIDEO_INFO_IS_RGB(&in_info);
    cv::Mat rgb_in(height, width, CV_8UC3, in_map.data);
    cv::Mat greyMat;
    cv::cvtColor(rgb_in, greyMat, cv::COLOR_RGBA2GRAY);
    if(self->bIsNewTargetObj)
    {
        track_wrapper->setTargetObject(rectTargetObject);
    }

    if(self->bIsStopTracking)
    {
        track_wrapper->stopTracking();
    }
    self->bIsStopTracking =false;
    self->bIsNewTargetObj = false;

    track_wrapper->trackFrame(greyMat);


    rectObject= track_wrapper->objectRect();
    rectROI = track_wrapper->roiRect();
    self->bTrackIsActive = track_wrapper->active();
    self->bTrackIsLockedOn = track_wrapper->lockedOn();



    g_mutex_unlock(&self->lock);

    gst_buffer_unmap(buf, &in_map);

    ret = gst_pad_push(srcpad, buf);
    gst_object_unref(srcpad);
    return ret;
}
