#include <gst/gst.h>
#include <glib.h>
#include <opencv2/imgproc/imgproc.hpp>

G_BEGIN_DECLS

#define GST_TYPE_GOEN_TRACK (gst_goen_track_get_type())
    G_DECLARE_FINAL_TYPE(GstGoenTrack, gst_goen_track, GST, GOEN_TRACK, GstElement)

#define GST_IS_GOEN_TRACK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GOEN_TRACK))
#define GST_IS_GOEN_TRACK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GOEN_TRACK))

    typedef void (*FpsCallback)(gdouble fps, gboolean stream_active, gpointer user_data);

struct _GstGoenTrack {
    GstElement element;

    /* Properties */
    FpsCallback callback;
    gpointer user_data;
    guint interval;
    gdouble brightness;

    float roiExt;
    bool bTrackIsLockedOn = false;
    bool bTrackIsActive = false;
    bool bIsNewTargetObj = false;
    bool bIsStopTracking = true;
    char sConfigFile[20] = "tracking.ini";
    /* State */
    GstClockTime last_time;
    guint frame_count;
    guint timer_id;
    GMutex lock;
    gboolean stream_active;





};

void handle_get_active_lock(GstElement *element, bool &bActive, bool &bLock);
void handle_get_out_roi(GstElement *element,  ushort &cx, ushort &cy, ushort &w, ushort &h);
void handle_get_out_object(GstElement *element, ushort &cx, ushort &cy, ushort &w, ushort &h);
void handle_set_stop_tracking(GstElement *element);
void handle_set_roi_ext(GstElement *element, gdouble new_roi_ext);
void handle_set_target_object( GstElement *element, ushort cx, ushort cy, ushort w, ushort h);
void handle_set_cfg_filename( GstElement *element, char sFileName[20]);

G_END_DECLS
