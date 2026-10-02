#include <gstgoentrack.h>

#include "main_app.h"
#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h> // For GstVideoFrame and GstVideoInfo
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <fstream>
#include <iostream>
#include <gio/gio.h>
#include "trsh_wrapper.h"
#include <opencv2/imgproc/imgproc.hpp>

#define PLUGIN_NAME "goentrack"

#define GST_TYPE_GOENTRACK (GoenTrack::get_type())
#define GST_GOENTRACK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_GOENTRACK, GoenTrack))
#define GST_IS_GOENTRACK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_GOENTRACK))

// D-Bus constants
#define DBUS_NAME "org.istok.GoenTrack"
#define DBUS_PATH "/org/istok/GoenTrack"
#define DBUS_INTERFACE "org.istok.GoenTrack"

// Forward declarations

static int iter =0;
static bool isInit =false;



static gdouble m_K [640*512];
static gdouble m_B [640 * 512];

static const uint32_t L = 65536;
static double n_k_original[L];
static double n_k_equalized[L];
static double p_k_original[L];
static double p_k_equalized[L];
static double s_k[L];

static __int16_t m_Ki[640 * 512];
static __int16_t m_Bi[640 * 512];
static bool init_ok;

static TrShWrapper track_wrapper;

#define frameT1File "/home/khadas/gst/ir_calib/frameT1.png"
#define frameT2File "/home/khadas/gst/ir_calib/frameT2.png"
#define kTableFile "/home/khadas/gst/ir_calib/kTable.txt"
#define bTableFile "/home/khadas/gst/ir_calib/bTable.txt"


static cv::Mat m_frameT1;
static cv::Mat m_frameT2;
static cv::Mat m_frame;
static cv::Mat m_frame8bit;
static cv::Mat m_map;
#define W 640
#define H 512
#define imgSize 640 * 512

#define m_threshold 0x40
static const gchar introspection_xml[] =
    "<node>"
    "  <interface name='org.istok.GoenTrack'>"
    "    <method name='SetBrightness'>"
    "      <arg type='d' name='brightness' direction='in'/>"
    "    </method>"
    "    <method name='GetOutObject'>"
    "      <arg type='u' name='cx' direction='out'/>"
    "      <arg type='u' name='cy' direction='out'/>"
    "      <arg type='u' name='w' direction='out'/>"
    "      <arg type='u' name='h' direction='out'/>"
    "    </method>"
    "    <method name='GetOutRoi'>"
    "      <arg type='u' name='cx' direction='out'/>"
    "      <arg type='u' name='cy' direction='out'/>"
    "      <arg type='u' name='w' direction='out'/>"
    "      <arg type='u' name='h' direction='out'/>"
    "    </method>"
    "    <method name='SetTargetObject'>"
    "      <arg type='u' name='cx' direction='in'/>"
    "      <arg type='u' name='cy' direction='in'/>"
    "      <arg type='u' name='w' direction='in'/>"
    "      <arg type='u' name='h' direction='in'/>"
    "    </method>"
    "    <method name='SetStopTracking'>"
    "    </method>"
    "    <method name='GetActiveLockTracking'>"
    "      <arg type='b' name='active' direction='out'/>"
    "      <arg type='b' name='locked' direction='out'/>"
    "    </method>"
    "   <method name='SetRoiExt'>"
    "      <arg type='d' name='ext' direction='in'/>"
    "    </method>"
    "  </interface>"
    "</node>";

// D-Bus method handler


static GDBusInterfaceVTable dbus_vtable ;

// Fully define the class structure
struct GoenTrackClass : public GstBaseTransformClass {};

// Define the plugin class
struct GoenTrack : public GstBaseTransform {
    gdouble brightness;

    TrShWrapper::Rect rectObject;
    TrShWrapper::Rect rectTargetObject;
    TrShWrapper::Rect rectROI;
    float roiExt;
    bool bTrackIsLockedOn = false;
    bool bTrackIsActive = false;
    bool bIsNewTargetObj = false;
    bool bIsStopTracking = true;

    GDBusConnection *dbus_connection;

    static GType get_type() {
        static GType type = 0;
        if (!type) {
            static const GTypeInfo info = {
                sizeof(GoenTrackClass), // Size of the class structure
                nullptr,
                nullptr,
                (GClassInitFunc)class_init,
                nullptr,
                nullptr,
                sizeof(GoenTrack), // Size of the instance structure
                0,
                (GInstanceInitFunc)instance_init,
            };
            type = g_type_register_static(GST_TYPE_BASE_TRANSFORM, PLUGIN_NAME, &info, (GTypeFlags)0);
        }
        return type;
    }

public:




    // D-Bus introspection XML

    static void on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
    {
        GoenTrack *self = GST_GOENTRACK(user_data);
        GError *error = NULL;

        // Export the D-Bus object
        guint registration_id = g_dbus_connection_register_object(
            connection,
            DBUS_PATH,
            g_dbus_node_info_new_for_xml(introspection_xml, &error)->interfaces[0],
            &dbus_vtable,
            self,
            NULL,  // Pass NULL as the GDestroyNotify function
            &error // Pass the GError** here
            );

        if (registration_id == 0)
        {
            GST_ERROR_OBJECT(self, "Failed to register D-Bus object: %s", error->message);
            g_error_free(error);
        }
        else
        {
            GST_INFO_OBJECT(self, "D-Bus object registered successfully");
        }

        self->dbus_connection = connection;
    }

    static void goen_track_dispose(GObject *object)
    {
        GoenTrack *self = GST_GOENTRACK(object);

        // Clean up D-Bus resources
        if (self->dbus_connection)
        {
            g_object_unref(self->dbus_connection);
            self->dbus_connection = NULL;
        }

        // G_OBJECT_CLASS(goen_track_parent_class)->dispose(object);
    }

    static gboolean set_caps(GstBaseTransform *trans, GstCaps *incaps, GstCaps *outcaps)
    {
        GoenTrack *self = GST_GOENTRACK(trans);
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
        gst_video_info_set_format(&out_info, GST_VIDEO_FORMAT_RGB,
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



    /*
    static void gst_badPixMap(GoenTrackClass *klass)
    {
        m_frameT1 = cv::imread(frameT1File, cv::IMREAD_ANYDEPTH);
        m_frameT2 = cv::imread(frameT2File, cv::IMREAD_ANYDEPTH);

        cv::Mat difFrame(m_frameT1.size(), CV_16U);
        cv::absdiff(m_frameT2, m_frameT1, difFrame);
        auto itDif = difFrame.begin<ushort>();
        auto itDifEnd = difFrame.end<ushort>();
        auto itNew = m_map.begin<uchar>();
        auto itNewEnd = m_map.end<uchar>();
        for (; (itDif != itDifEnd) || (itNew != itNewEnd); ++itDif, ++itNew)
        {
            if (*itDif < m_threshold)
            {
                *itNew = std::numeric_limits<uchar>::max();
            }
            else
                *itNew = std::numeric_limits<uchar>::min();
        }
    }
        */
    static gboolean gst_ocl_init()
    {
        const char *kernel_code = R"(
            __kernel void cal_kernel(__global ushort* img,
                                    __global const ushort* m,
                                    __global const ushort* b,
                                    int imgSize)
            {
                int i = get_global_id(0);
                if (i < imgSize) {
                    uint product = (uint)m[i] * (uint)img[i];
                    uint shifted = product >> 13;
                    img[i] = (ushort)(shifted + b[i]);
                }
            }
        )";
        cv::String errmsg;
        // Build OpenCL program and create kernel
        cv::ocl::ProgramSource program_source(kernel_code);
        cv::String build_err;
        cv::ocl::Program program = cv::ocl::Program(program_source, "", build_err);

        if (program.ptr() == nullptr)
        {
            CV_Error(cv::Error::OpenCLApiCallError, "Kernel compilation error: " + build_err);
        }

        cv::ocl::Kernel kernel("cal_kernel", program);
        if (kernel.empty())
        {
            CV_Error(cv::Error::OpenCLApiCallError, "Failed to create kernel");
        }
        return true;
    }

    static gboolean gst_readKBTablesFromFile()
    {

        std::ifstream inK(kTableFile, std::ios::in | std ::ios::binary);
        if (!inK)
        {
            std::cout << "\nCannot open file and read m_K table.\n\n";
            return false;
        }

        inK.read((char *)&m_K, sizeof m_K); // считываем блок данных
        if (inK.good())
            std::cout << "\nm_K table was sucessfully read from file!\n\n";
        inK.close();

        std::ifstream inB(bTableFile, std::ios::in | std ::ios::binary);
        if (!inB)
        {
            std::cout << "\nCannot open file and read m_B table.\n\n";
            return false;
        }

        inB.read((char *)&m_B, sizeof m_B); // считываем блок данных
        if (inB.good())
            std::cout << "\nm_B table was sucessfully read from file!\n\n";
        inB.close();


        for(int i = 0; i< imgSize; i++)
        {
            m_Ki[i] = static_cast<int16_t>(round(m_K[i] * (1 << 13)));
            m_Bi[i] = static_cast<int16_t>(round(m_B[i]));

            // std::cout << "m_Ki: " << m_Ki[i] << " m_K: " << m_K[i] << "\n\n" ;
            // std::cout << "m_Bi: " << m_Bi[i] << " m_B: " << m_B[i] << "\n\n";
        }


        return true;
    }
    static void gst_badPixMap()
    {
        cv::Mat difFrame(m_frameT1.size(), CV_16U);
        cv::absdiff(m_frameT2, m_frameT1, difFrame);
        auto itDif = difFrame.begin<ushort>();
        auto itDifEnd = difFrame.end<ushort>();
        auto itNew = m_map.begin<uchar>();
        auto itNewEnd = m_map.end<uchar>();
        for (; (itDif != itDifEnd) || (itNew != itNewEnd); ++itDif, ++itNew)
        {
            if (*itDif < m_threshold)
            {
                *itNew = std::numeric_limits<uchar>::max();
            }
            else
                *itNew = std::numeric_limits<uchar>::min();
        }

        // cv::imwrite(mapFile, m_map, {cv::IMWRITE_PNG_COMPRESSION, 0});
        // cv::imshow("map", m_map);
    }



    static void gst_correction(cv::Mat &img)
    {
        auto it = img.begin<uint16_t>();
        // auto itEnd = img.end<uint16_t>();
        int32_t pix;
        for (int i = 0; i < imgSize; ++i, ++it)
        {
            pix = static_cast<int32_t>((m_K[i] * (*it)) + m_Bi[i]);

            if(pix < 0)
                pix = 0;
            if(pix > 65535)
                pix = 65535;
            (*it) = static_cast<uint16_t>(pix);
            //  (*it) = static_cast<uint16_t>(((m_Ki[i] * (*it)) >> 13) + m_Bi[i]);

        }
    }

    static void gst_calcOrigHist(cv::Mat &m_frame)
    {
        auto it = m_frame.begin<uint16_t>();
        auto itEnd = m_frame.end<uint16_t>();
        for (; it != itEnd; ++it)
        {
            n_k_original[*it]++;
        }
        for (int i = 0; i < L; i++)
        {
            p_k_original[i] = (n_k_original[i] / (imgSize));
        }
    }

    static void gst_histEqualization16bit(cv::Mat &img)
    {
        cv::Mat img_equ(img.size(), img.type()); //

        // массив s_k:
        double sum = 0;
        for (int i = 0; i < L; i++)
        {
            s_k[i] = std::round((L - 1) * sum);
            sum += p_k_original[i];
        }

        auto itEqu = img_equ.begin<uint16_t>();
        auto itEquEnd = img_equ.end<uint16_t>();
        auto it = img.begin<uint16_t>();
        auto itEnd = img.end<uint16_t>();

        for (; (it != itEnd) || (itEqu != itEquEnd); ++it, ++itEqu)
        {
            *itEqu = s_k[*it];
        }

        auto itEqual = img_equ.begin<uint16_t>();
        auto itEqualEnd = img_equ.end<uint16_t>();
        for (; (itEqual != itEqualEnd); ++itEqual)
        {
            ++n_k_equalized[*itEqual];
        }

        for (int i = 0; i < L; i++)
        {
            p_k_equalized[i] = (n_k_equalized[i] / (H * W));
        }

        img_equ.copyTo(img);
    }



    static void handle_set_brightness(GDBusConnection *connection,
                                      const gchar *sender,
                                      const gchar *object_path,
                                      const gchar *interface_name,
                                      const gchar *method_name,
                                      GVariant *parameters,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data)
    {
        GoenTrack *self = GST_GOENTRACK(user_data);
        gdouble new_brightness;

        // Extract the brightness value from the parameters
        g_variant_get(parameters, "(d)", &new_brightness);

        // Clamp the brightness value to the valid range (-255 to 255)
        /* if (new_brightness < -255)
             new_brightness = -255;
         if (new_brightness > 255)
             new_brightness = 255;*/

        // Update the brightness property
        GST_OBJECT_LOCK(self);
        self->brightness = new_brightness;
        GST_OBJECT_UNLOCK(self);

        std::cout << "OU, YEAH "
                  << self->brightness << "\n\n";
        // Respond to the caller
        g_dbus_method_invocation_return_value(invocation, NULL);
    }

    static void handle_set_roi_ext(GDBusConnection *connection,
                                      const gchar *sender,
                                      const gchar *object_path,
                                      const gchar *interface_name,
                                      const gchar *method_name,
                                      GVariant *parameters,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data)
    {
        GoenTrack *self = GST_GOENTRACK(user_data);
        gdouble new_roi_ext;

        // Extract the brightness value from the parameters
        g_variant_get(parameters, "(d)", &new_roi_ext);

        // Clamp the brightness value to the valid range (-255 to 255)
        if ((new_roi_ext > 0) &&  (new_roi_ext < 100))

         {
            // Update the brightness property
            GST_OBJECT_LOCK(self);
            track_wrapper.setRoiExt(new_roi_ext);
            GST_OBJECT_UNLOCK(self);

            std::cout << "handle_set_roi_ext " << new_roi_ext << "\n\n";
         }
        else
             std::cout << "handle_set_roi_ext " << new_roi_ext << " DATA ERROR!\n\n";
        // Respond to the caller
        g_dbus_method_invocation_return_value(invocation, NULL);
    }

    static void handle_set_stop_tracking(GDBusConnection *connection,
                                      const gchar *sender,
                                      const gchar *object_path,
                                      const gchar *interface_name,
                                      const gchar *method_name,
                                      GVariant *parameters,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data)
    {
        GoenTrack *self = GST_GOENTRACK(user_data);

        // Update the brightness property
        GST_OBJECT_LOCK(self);
        self->bIsStopTracking = true;
        GST_OBJECT_UNLOCK(self);

        std::cout << "STOP TRACKING \n";
        // Respond to the caller
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    static void handle_get_out_object(GDBusConnection *connection,
                                const gchar *sender,
                                const gchar *object_path,
                                const gchar *interface_name,
                                const gchar *method_name,
                                GVariant *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer user_data) {
        // Serialize the Rect fields into a D-Bus response
        GoenTrack *self = GST_GOENTRACK(user_data);
        GST_OBJECT_LOCK(self);
        TrShWrapper::Rect outObjectD= self->rectObject;
        GST_OBJECT_UNLOCK(self);

        g_dbus_method_invocation_return_value(
            invocation,
            g_variant_new("(uuuu)", outObjectD.cx, outObjectD.cy, outObjectD.w, outObjectD.h)
            );
    }

    static void handle_get_out_roi(GDBusConnection *connection,
                                      const gchar *sender,
                                      const gchar *object_path,
                                      const gchar *interface_name,
                                      const gchar *method_name,
                                      GVariant *parameters,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data) {
        // Serialize the Rect fields into a D-Bus response
        GoenTrack *self = GST_GOENTRACK(user_data);
        GST_OBJECT_LOCK(self);
        TrShWrapper::Rect outRoidD= self->rectROI;
        GST_OBJECT_UNLOCK(self);

        g_dbus_method_invocation_return_value(
            invocation,
            g_variant_new("(uuuu)", outRoidD.cx, outRoidD.cy, outRoidD.w, outRoidD.h)
            );
    }

    static void handle_get_active_lock(GDBusConnection *connection,
                                   const gchar *sender,
                                   const gchar *object_path,
                                   const gchar *interface_name,
                                   const gchar *method_name,
                                   GVariant *parameters,
                                   GDBusMethodInvocation *invocation,
                                   gpointer user_data) {
        // Serialize the Rect fields into a D-Bus response
        GoenTrack *self = GST_GOENTRACK(user_data);
        GST_OBJECT_LOCK(self);
        bool bActive= self->bTrackIsActive;
        bool bLock= self->bTrackIsLockedOn;
        GST_OBJECT_UNLOCK(self);

        g_dbus_method_invocation_return_value(
            invocation,
            g_variant_new("(bb)", bActive, bLock)
            );
    }


    // D-Bus method handler for SetRect
    static void handle_set_target_object(GDBusConnection *connection,
                                const gchar *sender,
                                const gchar *object_path,
                                const gchar *interface_name,
                                const gchar *method_name,
                                GVariant *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer user_data) {
        // Extract the Rect fields from the parameters
        guint cx, cy, w, h;
        g_variant_get(parameters, "(uuuu)", &cx, &cy, &w, &h);
        std::cout<<"handle_set_target_object"<<std::endl;
        TrShWrapper::Rect target_obj;
        // Update the static Rect variable
        target_obj.cx = (unsigned short)cx;
        target_obj.cy = (unsigned short)cy;
        target_obj.w = (unsigned short)w;
        target_obj.h = (unsigned short)h;

        GoenTrack *self = GST_GOENTRACK(user_data);
        GST_OBJECT_LOCK(self);
        self->rectTargetObject= target_obj;
        self->bIsNewTargetObj = true;
        GST_OBJECT_UNLOCK(self);

        GST_INFO("Updated Rect: cx=%u, cy=%u, w=%u, h=%u", target_obj.cx, target_obj.cy, target_obj.w, target_obj.h);

        // Respond to the caller
        g_dbus_method_invocation_return_value(invocation, NULL);
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
                                  "format", G_TYPE_STRING, "RGB",
                                  NULL);
            }
            else
            {
                /* Transforming src caps (output) to sink caps (input) */
                gst_structure_set_name(new_structure, "video/x-raw");
                gst_structure_set(new_structure,
                                  "format", G_TYPE_STRING, "RGB",
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
    static void class_init(GoenTrackClass* klass) {
        GstElementClass* element_class = GST_ELEMENT_CLASS(klass);
        GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
        gobject_class->dispose = goen_track_dispose;

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
                "format = (string) RGB, "         // Accept GRAY16_LE format
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
                "format = (string) RGB, "     // Accept GRAY16_LE format
                "width = (int) [ 1, MAX ], "        // Width range
                "height = (int) [ 1, MAX ], "       // Height range
                "framerate = (fraction) [ 0, MAX ]" // Frame rate range
                ));

        dbus_vtable = {
                       handle_set_brightness,
                       NULL,
                       NULL};

        dbus_vtable = {
            .method_call = [](GDBusConnection *connection,
                              const gchar *sender,
                              const gchar *object_path,
                              const gchar *interface_name,
                              const gchar *method_name,
                              GVariant *parameters,
                              GDBusMethodInvocation *invocation,
                              gpointer user_data) {
                if (g_strcmp0(method_name, "SetBrightness") == 0) {
                    handle_set_brightness(connection, sender, object_path, interface_name, method_name, parameters, invocation, user_data);
                } else if (g_strcmp0(method_name, "GetOutObject") == 0) {
                    handle_get_out_object(connection, sender, object_path, interface_name, method_name, parameters, invocation, user_data);
                } else if (g_strcmp0(method_name, "GetOutRoi") == 0) {
                    handle_get_out_roi(connection, sender, object_path, interface_name, method_name, parameters, invocation, user_data);
                } else if (g_strcmp0(method_name, "SetRoiExt") == 0) {
                    handle_set_roi_ext(connection, sender, object_path, interface_name, method_name, parameters, invocation, user_data);
                } else if (g_strcmp0(method_name, "SetTargetObject") == 0) {
                    handle_set_target_object(connection, sender, object_path, interface_name, method_name, parameters, invocation, user_data);
                } else if (g_strcmp0(method_name, "SetStopTracking") == 0) {
                    handle_set_stop_tracking(connection, sender, object_path, interface_name, method_name, parameters, invocation, user_data);
                } else if (g_strcmp0(method_name, "GetActiveLockTracking") == 0) {
                    handle_get_active_lock(connection, sender, object_path, interface_name, method_name, parameters, invocation, user_data);
                }

            },
        };

        gst_element_class_add_pad_template(element_class,
                                           gst_static_pad_template_get(&source_template));

       // gst_ocl_init();
       // gst_badPixMap();
       // gst_readKBTablesFromFile();
        track_wrapper.setIniFileNamePath("tracking.ini");

        // Set the transform_ip function
        GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);
        base_transform_class->set_caps = GST_DEBUG_FUNCPTR(set_caps);
        base_transform_class->transform = GST_DEBUG_FUNCPTR(transform);
        base_transform_class->transform_caps =
            GST_DEBUG_FUNCPTR(gst_ir_filter_transform_caps);
    }

    // Initialize the instance
    static void instance_init(GoenTrack *self)
    {
        self -> brightness = 0.5;
        // Acquire the D-Bus session bus
        guint owner_id = g_bus_own_name(
            G_BUS_TYPE_SESSION,
            DBUS_NAME,
            G_BUS_NAME_OWNER_FLAGS_NONE,
            on_bus_acquired, // Called when the bus is acquired
            NULL,            // Called when the name is acquired (optional)
            NULL,            // Called when the name is lost (optional)
            self,            // User data passed to callbacks
            NULL             // No cleanup function for user_data
            );

        if (owner_id == 0)
        {
            GST_ERROR_OBJECT(self, "Failed to acquire D-Bus name");
        }

    }
    // Transform function (must be static)
    static GstFlowReturn transform(GstBaseTransform *trans, GstBuffer *inbuf, GstBuffer *outbuf)
    {
        GoenTrack *self = GST_GOENTRACK(trans);
       // double yOrig[L], yEqu[L], x[L];

        isInit = true;

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

        //std::cout<< "height" << height <<" width" << width<<std::endl;
        cv::Mat rgb_in(height, width, CV_8UC3, in_map.data);
        cv::Mat rgb_out(height, width, CV_8UC3, out_map.data);

        rgb_in.copyTo(rgb_out);


        cv::Mat greyMat;
        cv::cvtColor(rgb_in, greyMat, cv::COLOR_RGBA2GRAY);

        //rgb_in.convertTo(rgb_in, -1, self->brightness, 50);

        for (int i = 0; i < L; i++)
        {
            n_k_original[i] = 0;
            n_k_equalized[i] = 0;
        }

        //gst_calcOrigHist(gray16);
        //gst_correction(gray16);
        // gst_histEqualization16bit(gray16);

        // cv::UMat cv_source_frameU = cv::UMat(gray16.getUMat(cv::ACCESS_FAST, cv::USAGE_ALLOCATE_DEVICE_MEMORY));
        // cv::UMat cv_dst_frameU;

        // gray16(cv_source_frameU);
         GST_OBJECT_LOCK(self);
         if(self->bIsNewTargetObj)
         {
             track_wrapper.setTargetObject(self->rectTargetObject);
         }

         if(self->bIsStopTracking)
         {
             track_wrapper.stopTracking();
         }


         self->bIsStopTracking =false;
         self->bIsNewTargetObj = false;
         GST_OBJECT_UNLOCK(self);


        //std::cerr << "before track_wrapper.trackFrame"<< std::endl;
       //  rgb_out=rgb_in;//.copyTo(rgb_out);

        track_wrapper.trackFrame(greyMat);
        //std::cerr << "after track_wrapper.trackFrame"<< std::endl;



        GST_OBJECT_LOCK(self);
        self->rectObject= track_wrapper.objectRect();
        self->rectROI = track_wrapper.roiRect();
        self->bTrackIsActive = track_wrapper.active();
        self->bTrackIsLockedOn = track_wrapper.lockedOn();
        GST_OBJECT_UNLOCK(self);

        // cv::Rect rect;
        // rect.x = self->rectObject.cx;
        // rect.y = self->rectObject.cy;
        // rect.width = self->rectObject.w;
        // rect.height = self->rectObject.h;
        //cv::rectangle(rgb_out, rect, cv::Scalar(0, 255, 0),2);


        //cv_source_frameU.convertTo(cv_dst_frameU, CV_8UC1, 1.0 / 256.0);

        // memcpy(out_map.data, cv_dst_frameU.getMat(cv::ACCESS_FAST).data, imgSize);

        // Brightness enhancement
        //gray8.convertTo(gray8, -1, 1.5, 50); // alpha=1.5, beta=50

        gst_buffer_unmap(inbuf, &in_map);
        gst_buffer_unmap(outbuf, &out_map);
        return GST_FLOW_OK;
    }
};

// Register the plugin
static gboolean plugin_init(GstPlugin* plugin) {
    gboolean ret = gst_element_register(plugin, PLUGIN_NAME, GST_RANK_NONE, GoenTrack::get_type());
    if (!ret) {
        g_printerr("Failed to register plugin '%s'\n", PLUGIN_NAME);
    } else {
        g_print("Successfully registered plugin '%s'\n", PLUGIN_NAME);
    }

    return ret;
}

#ifndef PACKAGE
#define PACKAGE "goentrack"
#endif

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    goentrack,
    "Converts GRAY16 to GRAY8 and enhances brightness",
    plugin_init,
    "1.0",
    "LGPL",
    "Custom Plugins",
    "http://example.com"
    )
