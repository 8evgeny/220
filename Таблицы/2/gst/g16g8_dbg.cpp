#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

GST_DEBUG_CATEGORY_STATIC (gray16_to_gray8_debug);
#define GST_CAT_DEFAULT gray16_to_gray8_debug

#define VIDEO_FORMATS "{ GRAY16_LE }"

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE (
    "sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE (VIDEO_FORMATS))
);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE (
    "src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("GRAY8"))
);

typedef struct _GstGray16ToGray8 {
  GstVideoFilter parent;
} GstGray16ToGray8;

typedef struct _GstGray16ToGray8Class {
  GstVideoFilterClass parent_class;
} GstGray16ToGray8Class;

G_DEFINE_TYPE(GstGray16ToGray8, gst_gray16_to_gray8, GST_TYPE_VIDEO_FILTER);

static GstFlowReturn
gst_gray16_to_gray8_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * in_frame, GstVideoFrame * out_frame)
{
  const guint8 *in_data;
  guint8 *out_data;
  gint width, height;
  gint in_stride, out_stride;
  gint x, y;

  in_data = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  out_data = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);
  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);
  in_stride = GST_VIDEO_FRAME_PLANE_STRIDE (in_frame, 0);
  out_stride = GST_VIDEO_FRAME_PLANE_STRIDE (out_frame, 0);

  for (y = 0; y < height; y++) {
    const guint8 *in_row = in_data + y * in_stride;
    guint8 *out_row = out_data + y * out_stride;

    for (x = 0; x < width; x++) {
      guint16 pixel = GST_READ_UINT16_LE (in_row + (x * 2));
      out_row[x] = (guint8)(pixel >> 8);
    }
  }

  return GST_FLOW_OK;
}

static void
gst_gray16_to_gray8_class_init (GstGray16ToGray8Class * klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS (klass);

  video_filter_class->transform_frame =
      GST_DEBUG_FUNCPTR (gst_gray16_to_gray8_transform_frame);

  gst_element_class_set_static_metadata (element_class,
      "Gray16 to Gray8 converter",
      "Filter/Converter/Video",
      "Converts 16-bit grayscale video to 8-bit",
      "Your Name <your.email@example.com>");

  gst_element_class_add_static_pad_template (element_class, &sink_factory);
  gst_element_class_add_static_pad_template (element_class, &src_factory);
}

static void
gst_gray16_to_gray8_init (GstGray16ToGray8 * filter)
{
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gray16_to_gray8_debug, "gray16togray8",
      0, "Gray16 to Gray8 converter");

  return gst_element_register (plugin, "gray16togray8", GST_RANK_NONE,
      GST_TYPE_GRAY16_TO_GRAY8);
}

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    gray16togray8,
    "Converts GRAY16_LE to GRAY8",
    plugin_init,
    "1.0",
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)