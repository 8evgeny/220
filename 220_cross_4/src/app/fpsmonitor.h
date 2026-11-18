#include <gst/gst.h>
#include <glib.h>

G_BEGIN_DECLS

#define GST_TYPE_FPS_MONITOR (gst_fps_monitor_get_type())
    G_DECLARE_FINAL_TYPE(GstFpsMonitor, gst_fps_monitor, GST, FPS_MONITOR, GstElement)

    typedef void (*FpsCallback)(gdouble fps, gboolean stream_active, gpointer user_data);

struct _GstFpsMonitor {
    GstElement element;

    /* Properties */
    guint interval;
    FpsCallback callback;
    gpointer user_data;

    /* State */
    GstClockTime last_time;
    guint frame_count;
    guint timer_id;
    GMutex lock;
    gboolean stream_active;
};

GType gst_fps_monitor_get_type(void);

G_END_DECLS
