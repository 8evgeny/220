#ifndef G16G8_IR_H
#define G16G8_IR_H
#include <gst/gst.h>
#include <glib.h>

G_BEGIN_DECLS

#define GST_TYPE_FPS_MONITOR (gst_fps_monitor_get_type())
    G_DECLARE_FINAL_TYPE(GstGray16ToGray8, gst_fps_monitor, GST, FPS_MONITOR, GstElement)

struct _GstFpsMonitor {
    GstElement element;

    /* Properties */
    guint interval;
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

#endif // G16G8_IR_H
