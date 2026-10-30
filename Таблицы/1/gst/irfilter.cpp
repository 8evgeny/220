#include "irfilter.h"

GST_DEBUG_CATEGORY_STATIC (ir_filter_debug);
#define GST_CAT_DEFAULT ir_filter_debug


#define PACKAGE "irfilter-plugin"
#define VERSION "1.0.0"
#define GST_PACKAGE_NAME "IR Filter Plugin"
#define GST_PACKAGE_ORIGIN "http://example.com"

//GType gst_ir_filter_get_type(void) { return GST_TYPE_IR_FILTER; }

enum {
  PROP_0,
  PROP_BRIGHTNESS,
  N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE (
  "sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("GRAY16_LE"))
);

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE (
  "src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("GRAY8"))
);

static void
gst_ir_filter_set_property (GObject *object, guint prop_id,
                            const GValue *value, GParamSpec *pspec)
{
  GstIRFilter *self = GST_IR_FILTER (object);

  switch (prop_id) {
    case PROP_BRIGHTNESS:
      self->brightness = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_ir_filter_get_property (GObject *object, guint prop_id,
                            GValue *value, GParamSpec *pspec)
{
  GstIRFilter *self = GST_IR_FILTER (object);

  switch (prop_id) {
    case PROP_BRIGHTNESS:
      g_value_set_double (value, self->brightness);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_ir_filter_transform_frame (GstVideoFilter *filter,
                               GstVideoFrame *in_frame,
                               GstVideoFrame *out_frame)
{
  GstIRFilter *self = GST_IR_FILTER (filter);

  gint width = GST_VIDEO_FRAME_WIDTH (in_frame);
  gint height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  cv::Mat src(height, width, CV_16UC1, GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0));
  cv::Mat dst(height, width, CV_8UC1, GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0));

  double alpha = self->brightness * (255.0 / 65535.0);
  cv::convertScaleAbs(src, dst, alpha, 0);

  return GST_FLOW_OK;
}

static void
gst_ir_filter_class_init (GstIRFilterClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstVideoFilterClass *vfilter_class = GST_VIDEO_FILTER_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gobject_class->set_property = gst_ir_filter_set_property;
  gobject_class->get_property = gst_ir_filter_get_property;

  properties[PROP_BRIGHTNESS] = 
    g_param_spec_double ("brightness", "Brightness",
                        "Brightness multiplier (0.0 - N)",
                        0.0, G_MAXDOUBLE, 1.0,
                        (GParamFlags)(G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  g_object_class_install_properties (gobject_class, N_PROPERTIES, properties);

  gst_element_class_set_static_metadata (element_class,
    "IR Filter Processor", "Filter/Video/IR",
    "Processes IR video streams with brightness adjustment",
    "Your Name <your.email@example.com>");

  gst_element_class_add_pad_template (element_class,
    gst_static_pad_template_get (&sink_template));
  gst_element_class_add_pad_template (element_class,
    gst_static_pad_template_get (&src_template));

  vfilter_class->transform_frame = 
    GST_DEBUG_FUNCPTR (gst_ir_filter_transform_frame);
}

static void
gst_ir_filter_init (GstIRFilter *self)
{
  self->brightness = 1.0;
}


extern "C" {
static gboolean plugin_init(GstPlugin *plugin);
};

static gboolean
plugin_init (GstPlugin *plugin)
{
  GST_DEBUG_CATEGORY_INIT (ir_filter_debug, "irfilter",
                           0, "IR Video Filter");

  return gst_element_register (plugin, "irfilter",
                              GST_RANK_NONE, NULL);  //GST_TYPE_IR_FILTER);
}

// Finally the GST_PLUGIN_DEFINE
GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    irfilter,
    "IR Video Filter Processor",
    plugin_init,
    VERSION,
    "LGPL",
    GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN)
