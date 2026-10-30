#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h> // For GstVideoFrame and GstVideoInfo
#include <opencv2/opencv.hpp>

#define PLUGIN_NAME "gray16togray8"

#define GST_TYPE_GRAY16TOGRAY8 (Gray16ToGray8::get_type())
#define GST_GRAY16TOGRAY8(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_GRAY16TOGRAY8, Gray16ToGray8))
#define GST_IS_GRAY16TOGRAY8(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_GRAY16TOGRAY8))

// Forward declarations
struct Gray16ToGray8;
struct Gray16ToGray8Class;

// Fully define the class structure
struct Gray16ToGray8Class : public GstBaseTransformClass {};

// Define the plugin class
struct Gray16ToGray8 : public GstBaseTransform {
    static GType get_type() {
        static GType type = 0;
        if (!type) {
            static const GTypeInfo info = {
                sizeof(Gray16ToGray8Class), // Size of the class structure
                nullptr,
                nullptr,
                (GClassInitFunc)class_init,
                nullptr,
                nullptr,
                sizeof(Gray16ToGray8), // Size of the instance structure
                0,
                (GInstanceInitFunc)instance_init,
            };
            type = g_type_register_static(GST_TYPE_BASE_TRANSFORM, PLUGIN_NAME, &info, (GTypeFlags)0);
        }
        return type;
    }

public:




    static gboolean set_caps(GstBaseTransform *trans, GstCaps *incaps, GstCaps *outcaps)
    {
        Gray16ToGray8 *self = GST_GRAY16TOGRAY8(trans);
        GST_ERROR_OBJECT(self, "Failed to parse input cap323s");

        GstVideoInfo in_info;
        if (!gst_video_info_from_caps(&in_info, incaps))
        {
            GST_ERROR_OBJECT(self, "Failed to parse input caps");
            return FALSE;
        }

        // Configure output caps (GRAY8 with same resolution/framerate)
        GstVideoInfo out_info;
        gst_video_info_init(&out_info);
        gst_video_info_set_format(&out_info, GST_VIDEO_FORMAT_GRAY8,
                                  GST_VIDEO_INFO_WIDTH(&in_info),
                                  GST_VIDEO_INFO_HEIGHT(&in_info));

        // Preserve framerate
        out_info.fps_n = GST_VIDEO_INFO_FPS_N(&in_info);
        out_info.fps_d = GST_VIDEO_INFO_FPS_D(&in_info);

        GstCaps *out_caps = gst_video_info_to_caps(&out_info);
        gst_pad_set_caps(GST_BASE_TRANSFORM_SRC_PAD(trans), out_caps);
        gst_caps_unref(out_caps);
        GstStructure *structure = gst_caps_get_structure(incaps, 0);
        const gchar *format;
        // Extract the format from the caps
        format = gst_structure_get_string(structure, "format");
        if (g_strcmp0(format, "GRAY16_LE") == 0)
        {
            GST_DEBUG("Plugin received GRAY16_LE format");
            // Perform any initialization specific to GRAY16_LE
            return TRUE;
        }

        return TRUE;
    }

    static GstCaps *
    gst_ir_filter_transform_caps(GstBaseTransform *trans,
                                 GstPadDirection direction,
                                 GstCaps *caps,
                                 GstCaps *filter)
    {
        GstCaps *result = gst_caps_new_empty();
        guint i, n;

        GST_DEBUG_OBJECT(trans, "Transforming caps direction: %d, caps: %" GST_PTR_FORMAT,
                         direction, caps);

        n = gst_caps_get_size(caps);
        for (i = 0; i < n; i++)
        {
            GstStructure *structure = gst_caps_get_structure(caps, i);
            GstStructure *new_structure = gst_structure_copy(structure);

            /* Handle format transformation based on direction */
            if (direction == GST_PAD_SINK)
            {
                /* Transforming sink caps (input) to src caps (output) */
                gst_structure_set_name(new_structure, "video/x-raw");
                gst_structure_set(new_structure,
                                  "format", G_TYPE_STRING, "GRAY8",
                                  NULL);
            }
            else
            {
                /* Transforming src caps (output) to sink caps (input) */
                gst_structure_set_name(new_structure, "video/x-raw");
                gst_structure_set(new_structure,
                                  "format", G_TYPE_STRING, "GRAY8",
                                  NULL);
            }

            /* Preserve other fields */
            if (gst_structure_has_field(new_structure, "width"))
                gst_structure_remove_field(new_structure, "width");
            if (gst_structure_has_field(new_structure, "height"))
                gst_structure_remove_field(new_structure, "height");
            if (gst_structure_has_field(new_structure, "framerate"))
                gst_structure_remove_field(new_structure, "framerate");

            gst_caps_append_structure(result, new_structure);
        }

        if (filter)
        {
            GstCaps *tmp = result;
            result = gst_caps_intersect(tmp, filter);
            gst_caps_unref(tmp);
        }

        GST_DEBUG_OBJECT(trans, "Transformed caps: %" GST_PTR_FORMAT, result);
        return result;
    }

    // Initialize the class
    static void class_init(Gray16ToGray8Class* klass) {
        GstElementClass* element_class = GST_ELEMENT_CLASS(klass);

        gst_element_class_set_metadata(
            element_class,
            "Gray16 to Gray8 Converter with Brightness Enhancement",
            "Filter/Converter",
            "Converts GRAY16 to GRAY8 and enhances brightness",
            "Your Name <your.email@example.com>"
        );
// Define the sink pad template
/*gst_element_class_add_pad_template(
    element_class,
    gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                         gst_caps_new_simple("video/x-raw",
                                             "format", G_TYPE_STRING, "GRAY16_LE",
                                             "width", GST_TYPE_INT_RANGE, 1, G_MAXINT,
                                             "height", GST_TYPE_INT_RANGE, 1, G_MAXINT,
                                             "framerate", GST_TYPE_FRACTION_RANGE, 0, 1, G_MAXINT, 1,
                                             nullptr))
);
*/
        // Sink pad template (accept ANY resolution/framerate as long as format=GRAY16_LE)
        // Sink pad template (accept any GRAY16_LE video)

        GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE(
            "sink",          // Pad name
            GST_PAD_SINK,    // Pad direction (sink)
            GST_PAD_ALWAYS,  // Pad presence (always available)
            GST_STATIC_CAPS( // Supported caps
                "video/x-raw, "
                "format = (string) GRAY8, "         // Accept GRAY16_LE format
                "width = (int) [ 1, MAX ], "        // Width range
                "height = (int) [ 1, MAX ], "       // Height range
                "framerate = (fraction) [ 0, MAX ]" // Frame rate range
                ));

        gst_element_class_add_pad_template(element_class,
                                           gst_static_pad_template_get(&sink_template));

        // Source pad template (output GRAY8)

        GstStaticPadTemplate source_template = GST_STATIC_PAD_TEMPLATE(
            "src",          // Pad name
            GST_PAD_SRC,    // Pad direction (sink)
            GST_PAD_ALWAYS,  // Pad presence (always available)
            GST_STATIC_CAPS( // Supported caps
                "video/x-raw, "
                "format = (string) GRAY8, "     // Accept GRAY16_LE format
                "width = (int) [ 1, MAX ], "        // Width range
                "height = (int) [ 1, MAX ], "       // Height range
                "framerate = (fraction) [ 0, MAX ]" // Frame rate range
                ));

        gst_element_class_add_pad_template(element_class,
                                           gst_static_pad_template_get(&source_template));

        // Set the transform_ip function
        GstBaseTransformClass* base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);
        base_transform_class->set_caps = GST_DEBUG_FUNCPTR(set_caps);
        base_transform_class->transform = GST_DEBUG_FUNCPTR(transform);
        base_transform_class->transform_caps =
            GST_DEBUG_FUNCPTR(gst_ir_filter_transform_caps);
    }

    // Initialize the instance
    static void instance_init(Gray16ToGray8* self) {}

    // Transform function (must be static)
    static GstFlowReturn transform(GstBaseTransform *trans, GstBuffer *inbuf, GstBuffer *outbuf)
    {
        Gray16ToGray8 *self = GST_GRAY16TOGRAY8(trans);

        GstMapInfo in_map, out_map;
        if (!gst_buffer_map(inbuf, &in_map, GST_MAP_READ) ||
            !gst_buffer_map(outbuf, &out_map, GST_MAP_WRITE))
        {
            GST_ERROR_OBJECT(self, "Failed to map buffers");
            return GST_FLOW_ERROR;
        }

        // Get dimensions from caps
        GstVideoInfo in_info;
        gst_video_info_from_caps(&in_info, gst_pad_get_current_caps(GST_BASE_TRANSFORM_SINK_PAD(trans)));
        int width = GST_VIDEO_INFO_WIDTH(&in_info);
        int height = GST_VIDEO_INFO_HEIGHT(&in_info);

        // Convert GRAY16_LE to GRAY8
        cv::Mat gray16(height, width/2, CV_16UC1, in_map.data);
        cv::Mat gray8(height, width/2, CV_8UC1, out_map.data);
        gray16.convertTo(gray8, CV_8UC1, 1.0 / 256.0);

        // Brightness enhancement
        //gray8.convertTo(gray8, -1, 1.5, 50); // alpha=1.5, beta=50

        gst_buffer_unmap(inbuf, &in_map);
        gst_buffer_unmap(outbuf, &out_map);
        return GST_FLOW_OK;
    }
};

// Register the plugin
static gboolean plugin_init(GstPlugin* plugin) {
    gboolean ret = gst_element_register(plugin, PLUGIN_NAME, GST_RANK_NONE, Gray16ToGray8::get_type());
    if (!ret) {
        g_printerr("Failed to register plugin '%s'\n", PLUGIN_NAME);
    } else {
        g_print("Successfully registered plugin '%s'\n", PLUGIN_NAME);
    }

    return ret;
}

#ifndef PACKAGE
#define PACKAGE "gray16togray8"
#endif

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    gray16togray8,
    "Converts GRAY16 to GRAY8 and enhances brightness",
    plugin_init,
    "1.0",
    "LGPL",
    "Custom Plugins",
    "http://example.com"
)