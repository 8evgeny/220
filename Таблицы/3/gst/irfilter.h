#ifndef __GST_IR_FILTER_H__
#define __GST_IR_FILTER_H__

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

G_BEGIN_DECLS

//#define GST_TYPE_IR_FILTER (gst_ir_filter_get_type())
G_DECLARE_FINAL_TYPE (GstIRFilter, gst_ir_filter, GST, IR_FILTER, GstVideoFilter)

struct _GstIRFilter {
  GstVideoFilter parent;
  
  gdouble brightness;
};

struct _GstIRFilterClass {
  GstVideoFilterClass parent_class;
};

G_END_DECLS

#endif /* __GST_IR_FILTER_H__ */