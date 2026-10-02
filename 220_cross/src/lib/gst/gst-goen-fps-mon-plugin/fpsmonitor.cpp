#include "fpsmonitor.h"

GST_DEBUG_CATEGORY_STATIC(fps_monitor_debug);
#define GST_CAT_DEFAULT fps_monitor_debug

/* Forward declarations */
static GstFlowReturn gst_fps_monitor_chain(GstPad *pad, GstObject *parent, GstBuffer *buf);
static gboolean gst_fps_monitor_sink_event(GstPad *pad, GstObject *parent, GstEvent *event);
static gboolean on_interval_timeout(gpointer user_data);
static void gst_fps_monitor_finalize(GObject *object);

/* Plugin registration */
static gboolean plugin_init(GstPlugin *plugin) {
    GST_DEBUG_CATEGORY_INIT(fps_monitor_debug, "fpsmonitor", 0, "FPS Monitor");
    return gst_element_register(plugin, "fpsmonitor", GST_RANK_NONE, GST_TYPE_FPS_MONITOR);
}
#ifndef PACKAGE
#define PACKAGE "fpsmonitor"
#endif


GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    fpsmonitor,
    "FPS monitoring plugin",
    plugin_init,
    "1.0",
    "LGPL",
    "GStreamer",
    "https://gstreamer.net/"
    )

G_DEFINE_TYPE(GstFpsMonitor, gst_fps_monitor, GST_TYPE_ELEMENT)

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

static void gst_fps_monitor_class_init(GstFpsMonitorClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

    gobject_class->finalize = gst_fps_monitor_finalize;

    gst_element_class_set_static_metadata(element_class,
                                          "FPS Monitor",
                                          "Filter",
                                          "Monitors frame rate with or without active stream",
                                          "Your Name <your.email@example.com>");

    gst_element_class_add_static_pad_template(element_class, &sink_template);
    gst_element_class_add_static_pad_template(element_class, &src_template);
}

static void gst_fps_monitor_init(GstFpsMonitor *self) {
    GstPad *sinkpad = gst_pad_new_from_static_template(&sink_template, "sink");
    GstPad *srcpad = gst_pad_new_from_static_template(&src_template, "src");

    gst_pad_set_chain_function(sinkpad, gst_fps_monitor_chain);
    gst_pad_set_event_function(sinkpad, gst_fps_monitor_sink_event);

    gst_element_add_pad(GST_ELEMENT(self), sinkpad);
    gst_element_add_pad(GST_ELEMENT(self), srcpad);

    self->interval = 1000; // 1 second updates
    self->callback = NULL;
    self->user_data = NULL;
    self->last_time = 0;
    self->frame_count = 0;
    self->stream_active = FALSE;
    self->timer_id = g_timeout_add(self->interval, on_interval_timeout, self);
    g_mutex_init(&self->lock);
}

static void gst_fps_monitor_finalize(GObject *object) {
    GstFpsMonitor *self = GST_FPS_MONITOR(object);

    if (self->timer_id) {
        g_source_remove(self->timer_id);
        self->timer_id = 0;
    }

    g_mutex_clear(&self->lock);
    G_OBJECT_CLASS(gst_fps_monitor_parent_class)->finalize(object);
}

static gboolean on_interval_timeout(gpointer user_data) {
    GstFpsMonitor *self = GST_FPS_MONITOR(user_data);
    gdouble fps = 0.0;
    gboolean active;

    g_mutex_lock(&self->lock);
    active = self->stream_active;

    if (active && self->last_time > 0) {
        GstClockTime current_time = gst_util_get_timestamp();
        GstClockTimeDiff diff = current_time - self->last_time;

        if (diff > 0) {
            fps = (gdouble)self->frame_count * GST_SECOND / diff;
            self->frame_count = 0;
            self->last_time = current_time;

        }
    }

    if (self->callback) {
        self->callback(fps, active, self->user_data);
    }
    g_mutex_unlock(&self->lock);

    return G_SOURCE_CONTINUE;
}

static gboolean gst_fps_monitor_sink_event(GstPad *pad, GstObject *parent, GstEvent *event) {
    GstFpsMonitor *self = GST_FPS_MONITOR(parent);
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

static GstFlowReturn gst_fps_monitor_chain(GstPad *pad, GstObject *parent, GstBuffer *buf) {
    GstFpsMonitor *self = GST_FPS_MONITOR(parent);
    GstClockTime timestamp = GST_BUFFER_PTS(buf);
    GstPad *srcpad = gst_element_get_static_pad(GST_ELEMENT(self), "src");
    GstFlowReturn ret;

    g_mutex_lock(&self->lock);
    if (GST_CLOCK_TIME_IS_VALID(timestamp)) {
        if (self->last_time == 0) {
            self->last_time = timestamp;
        }
        self->frame_count++;
    }
    g_mutex_unlock(&self->lock);

    ret = gst_pad_push(srcpad, buf);
    gst_object_unref(srcpad);
    return ret;
}
