using float32_t = float;
#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h> // For GstVideoFrame and GstVideoInfo
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/xphoto/bm3d_image_denoising.hpp>

#include <fstream>
#include <iostream>
#include <gio/gio.h>
//#include <arm_neon.h>
//#include <omp.h>
#define PLUGIN_NAME "gray16togray8"

#define GST_TYPE_GRAY16TOGRAY8 (Gray16ToGray8::get_type())
#define GST_GRAY16TOGRAY8(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_GRAY16TOGRAY8, Gray16ToGray8))
#define GST_IS_GRAY16TOGRAY8(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_GRAY16TOGRAY8))

// D-Bus constants
#define DBUS_NAME "org.example.Gray16ToGray8"
#define DBUS_PATH "/org/example/Gray16ToGray8"
#define DBUS_INTERFACE "org.example.Gray16ToGray8"

// Forward declarations
struct Gray16ToGray8;
struct Gray16ToGray8Class;

static int iter =0;

static gdouble m_K [640*512];
static gdouble m_B [640 * 512];
static gfloat m_Kf [640*512];
static gfloat m_Bf [640 * 512];
static int8_t m_bp[640*512];


static const uint32_t L = 65536>>2;
static uint64_t n_k_original[L];

// static double n_k_equalized[L];
 static uint64_t p_k_original[L];
// static double p_k_equalized[L];
static float32_t s_k[L];

static __int16_t m_Ki[640 * 512];
static __int16_t m_Bi[640 * 512];
static __uint8_t m_BPi[640 * 512];



#define frameT1File "/home/khadas/gst/ir_calib/frameT1.png"
#define frameT2File "/home/khadas/gst/ir_calib/frameT2.png"
#define kTableFile "/home/khadas/gst/ir_calib/kTable.bin"
#define bTableFile "/home/khadas/gst/ir_calib/bTable.bin"
#define bpTableFile "/home/khadas/gst/ir_calib/bpTable.bin"



static cv::Mat m_frameT1;
static cv::Mat m_frameT2;
static cv::Mat m_frame;
static cv::Mat m_frame8bit;
static cv::Mat m_map;
#define W 640
#define H 512
#define imgSize (640 * 512)

#define m_threshold 0x40

const float32_t eq_k= (L - 1) * 4 / imgSize;


/** Размер рамки игнорирования обработки */
static constexpr int32_t m_ignoreBorder = 2;
/** Размер ROI для медианной фильтрации */
static constexpr int32_t m_medianSize = 3;
const cv::Size m_roiSize = cv::Size(m_medianSize, m_medianSize);
static constexpr int32_t m_arraySize = m_medianSize * m_medianSize;
static constexpr int32_t m_cross = m_arraySize / 2;
std::array<uint16_t, m_arraySize> m_array;
std::array<uint16_t, m_arraySize> m_array8bit;

static const gchar introspection_xml[] =
    "<node>"
    "  <interface name='org.example.Gray16ToGray8'>"
    "    <method name='SetBrightness'>"
    "      <arg type='d' name='brightness' direction='in'/>"
    "    </method>"
    "  </interface>"
    "</node>";

// D-Bus method handler


static GDBusInterfaceVTable dbus_vtable ;

// Fully define the class structure
struct Gray16ToGray8Class : public GstBaseTransformClass {};

// Define the plugin class
struct Gray16ToGray8 : public GstBaseTransform {
    gdouble brightness;
    GDBusConnection *dbus_connection;

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




    // D-Bus introspection XML

    static void on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
    {
        Gray16ToGray8 *self = GST_GRAY16TOGRAY8(user_data);
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

    static void gray16_to_gray8_dispose(GObject *object)
    {
        Gray16ToGray8 *self = GST_GRAY16TOGRAY8(object);

        // Clean up D-Bus resources
        if (self->dbus_connection)
        {
            g_object_unref(self->dbus_connection);
            self->dbus_connection = NULL;
        }

       // G_OBJECT_CLASS(gray16_to_gray8_parent_class)->dispose(object);
    }

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


        std::ifstream inBP(bpTableFile, std::ios::in | std ::ios::binary);
        if (!inBP)
        {
            std::cout << "\nCannot open file and read m_BP table.\n\n";
            return false;
        }

        inBP.read((char *)&m_BPi, sizeof m_BPi); // считываем блок данных
        if (inBP.good())
            std::cout << "\nm_BP table was sucessfully read from file!\n\n";
        inBP.close();



        for(int i = 0; i< imgSize; i++)
        {
            m_Ki[i] = static_cast<int16_t>(round(m_K[i] * (1 << 13)));
            m_Bi[i] = static_cast<int16_t>(round(m_B[i]));

            m_Kf[i] = static_cast<float>(m_K[i]);
            m_Bf[i] = static_cast<float>(m_B[i]);
           // m_BPi[i] = static_cast<int8_t>(m_BPi[i]);
          //  if(m_BPi[i] !=0)  std::cout << "mBP: " <<i<< "\n\n" << std::endl;

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

      static uint16_t gst_median16bit(cv::Mat img)
    {
        cv::Mat roi(m_roiSize, CV_16U, m_array8bit.data());
        img.copyTo(roi);
        std::sort(m_array8bit.begin(), m_array8bit.end());
        return m_array8bit.at(m_cross);
    }


    // Optimized implementation with partial sort
    static uint16_t optimized_median_3x3_u16(const uint16_t* ptr) {
        uint16_t values[9];

        // Properly copy 3x3 ROI data accounting for possible row padding
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                values[y*3 + x] = ptr[y*(m_roiSize.width) + x];
            }
        }

        // Partial sort to get median (more efficient than full sort)
        for (int i = 0; i < 5; ++i) {
            int min_idx = i;
            for (int j = i+1; j < 9; ++j) {
                if (values[j] < values[min_idx]) {
                    min_idx = j;
                }
            }
            std::swap(values[i], values[min_idx]);
        }

        return values[4];
    }



    static uint16_t optimized_median_3x3_u16_neon(const uint16_t* ptr) {
        // Load the 3x3 neighborhood
        uint16x4_t row0 = vld1_u16(ptr);
        uint16x4_t row1 = vld1_u16(ptr + m_roiSize.width);
        uint16x4_t row2 = vld1_u16(ptr + 2 * m_roiSize.width);

        // Combine all 9 elements (we only need first 3 from each 4-element vector)
        uint16_t values[9] = {
            vget_lane_u16(row0, 0), vget_lane_u16(row0, 1), vget_lane_u16(row0, 2),
            vget_lane_u16(row1, 0), vget_lane_u16(row1, 1), vget_lane_u16(row1, 2),
            vget_lane_u16(row2, 0), vget_lane_u16(row2, 1), vget_lane_u16(row2, 2)
        };

        // NEON-optimized partial sort (find 5 smallest elements)
        for (int i = 0; i < 5; i++) {
            // Load remaining elements into NEON registers
            uint16x8_t remaining = vld1q_u16(values + i);

            // Find minimum
            uint16_t min_val = vminvq_u16(remaining);

            // Find and swap minimum
            for (int j = i; j < 9; j++) {
                if (values[j] == min_val) {
                    uint16_t tmp = values[i];
                    values[i] = values[j];
                    values[j] = tmp;
                    break;
                }
            }
        }

        return values[4];
    }


    static void gst_patching16bit(cv::Mat &img)
    {
        const int32_t xBorder = img.size().width - m_ignoreBorder;
        const int32_t yBorder = img.size().height - m_ignoreBorder;
        const int32_t xWidth = img.size().width;

        auto itMap        = m_BPi;
        auto it           = img.begin<uint16_t>();
        const auto itEnd  = img.end<uint16_t>();
//#pragma omp parallel for collapse(4) schedule(dynamic)


        for(; (it != itEnd); ++it) {
            const auto point = it.pos();
            if( (itMap[xWidth*point.y+point.x ] != 0) &&
                point.x > m_ignoreBorder &&
                point.y > m_ignoreBorder &&
                point.x < xBorder &&
                point.y < yBorder)
            {
                cv::Mat roi_mat(img, cv::Rect(point, m_roiSize));
                cv::Mat roi = roi_mat.clone();
                uint32_t p2 = gst_median16bit(roi);
               // uint32_t p2 =   optimized_median_3x3_u16((uint16_t*)roi.data);
              // if (p2!=gst_median16bit(roi))

                 // uint16_t p2 = optimized_median_3x3_u16_neon((uint16_t*)roi.data);
                img.at<uint16_t>(point) = p2;// 0xFFFF;// p2;
               // std::cout<<"X: "<<  point.x <<" Y: " <<  point.y<<std::endl;

                //   std::cout<<"neon_error" << (uint32_t) p2<< " median8b: "<< (uint32_t)gst_median16bit(roi)<< std::endl;
              //  img.at<uint16_t>(point) = p2;
            }
        }
    }

    static void gst_correction(cv::Mat &img)
    {
        auto it = img.begin<uint16_t>();
        // auto itEnd = img.end<uint16_t>();
        int32_t pix;
        uint16_t* data = img.ptr<uint16_t>();
        float sum = 0;
#pragma omp parallel for collapse(1) schedule(static)

        for (int i = 0; i < imgSize; ++i)
        {

            // Prefer integer arithmetic when possible
            int32_t pix = static_cast<int32_t>((m_Kf[i] * data[i]) + m_Bf[i]);

            // Branchless clamping using bitwise operations
            pix = pix & ~(pix >> 31);              // Clamp negative to 0
            pix = (pix | ((65535 - pix) >> 31)) & 65535; // Clamp above 65535

            data[i] = static_cast<uint16_t>(pix);
            //sum += *it;

        }


        // (*it) = static_cast<uint16_t>(pix);
        //    it++;

        //  (*it) = static_cast<uint16_t>(((m_Ki[i] * (*it)) >> 13) + m_Bi[i]);


    }


//     static void gst_correction(cv::Mat &img)
//     {
//         auto it = img.begin<uint16_t>();
//         auto itEnd = img.end<uint16_t>();
//         float sum = 0;
//         for (; it != itEnd; ++it)
//         {
//             p_k_original[*it>>2]++;
//             sum += *it;
//         }

//         std::cout<< "Median IR: " << sum / imgSize <<" Black: "<<p_k_original[0]+p_k_original[1]+p_k_original[2]+p_k_original[3] <<" Overlight: "<<p_k_original[L-1] <<std::endl;

//         const int imgSizeConst = img.rows * img.cols;
//         uint16_t* data = img.ptr<uint16_t>();

//         // Process 8 pixels at a time
//         const int neonSize = imgSizeConst & ~7;

// #pragma omp parallel for schedule(static)
//         for (int i = 0; i < neonSize; i += 8)
//         {
//             // Load 8 pixels
//             uint16x8_t pixels = vld1q_u16(data + i);

//             // Convert to 32-bit integers
//             uint32x4_t pixels_low = vmovl_u16(vget_low_u16(pixels));
//             uint32x4_t pixels_high = vmovl_u16(vget_high_u16(pixels));

//             // Load 8 K coefficients and convert to 32-bit float
//             float32x4_t k_low = vld1q_f32(m_Kf + i);
//             float32x4_t k_high = vld1q_f32(m_Kf + i + 4);

//             // Load 8 Bi coefficients
//             int16x8_t bi = vld1q_s16(m_Bi + i);
//             int32x4_t bi_low = vmovl_s16(vget_low_s16(bi));
//             int32x4_t bi_high = vmovl_s16(vget_high_s16(bi));

//             // Convert pixels to float
//             float32x4_t pix_float_low = vcvtq_f32_u32(pixels_low);
//             float32x4_t pix_float_high = vcvtq_f32_u32(pixels_high);

//             // Multiply by K
//             float32x4_t res_low = vmulq_f32(pix_float_low, k_low);
//             float32x4_t res_high = vmulq_f32(pix_float_high, k_high);

//             // Add Bi (convert to float for addition)
//             res_low = vaddq_f32(res_low, vcvtq_f32_s32(bi_low));
//             res_high = vaddq_f32(res_high, vcvtq_f32_s32(bi_high));

//             // Convert back to 32-bit integers with rounding
//             int32x4_t resi_low = vcvtnq_s32_f32(res_low);
//             int32x4_t resi_high = vcvtnq_s32_f32(res_high);

//             // Clamp negative values to 0
//             resi_low = vmaxq_s32(resi_low, vdupq_n_s32(0));
//             resi_high = vmaxq_s32(resi_high, vdupq_n_s32(0));

//             // Clamp values above 65535 to 65535
//             resi_low = vminq_s32(resi_low, vdupq_n_s32(65535));
//             resi_high = vminq_s32(resi_high, vdupq_n_s32(65535));

//             // Convert back to 16-bit
//             uint16x8_t result = vcombine_u16(
//                 vmovn_u32(vreinterpretq_u32_s32(resi_low)),
//                 vmovn_u32(vreinterpretq_u32_s32(resi_high)));

//             // Store results
//             vst1q_u16(data + i, result);
//         }

//         // Process remaining pixels (if imgSize not multiple of 8)
//         for (int i = neonSize; i < imgSize; ++i)
//         {
//             int32_t pix = static_cast<int32_t>((m_K[i] * data[i]) + m_Bi[i]);
//             pix = pix & ~(pix >> 31);              // Clamp negative to 0
//             pix = (pix | ((65535 - pix) >> 31)) & 65535; // Clamp above 65535
//             data[i] = static_cast<uint16_t>(pix);
//         }
//     }


    static void gst_calcOrigHist(cv::Mat &m_frame)
    {
        auto it = m_frame.begin<uint16_t>();
        auto itEnd = m_frame.end<uint16_t>();
        float sum = 0;
        uint16_t  max=0, min = 0xFFFF;
        for (; it != itEnd; ++it)
        {
            n_k_original[*it>>2]++;
            sum += *it;
            if((max<*it) && (*it< 65535))
                max=*it;
            if(min>*it)
                min=*it;


        }
      //  std::cout<< "Median IR: " << sum / imgSize << " Min"<< min<< " Max:"<< max <<" Black: "<<n_k_original[0]+n_k_original[1]+n_k_original[2]+n_k_original[3] <<" Overlight: "<<n_k_original[L-1] <<std::endl;

        // for (int i = 0; i < L; i++)
        // {
        //     p_k_original[i] = (n_k_original[i] / (imgSize));
        // }
    }


    static void gst_histEqualization16bit(cv::Mat &img)
    {
        cv::Mat img_equ(img.size(), img.type()); //

        // массив s_k:
        float32_t sum = 0;
        for (int i = 0; i < L; i++)
        {
            s_k[i] = std::round((L - 1) * sum * 4 / imgSize   );
            sum += n_k_original[i] ;
        }

        auto itEqu = img_equ.begin<uint16_t>();
        auto itEquEnd = img_equ.end<uint16_t>();
        auto it = img.begin<uint16_t>();
        auto itEnd = img.end<uint16_t>();

        for (; (it != itEnd) || (itEqu != itEquEnd); ++it, ++itEqu)
        {
            *itEqu = s_k[*it>>2] ;
        }

       // auto itEqual = img_equ.begin<uint16_t>();
        //auto itEqualEnd = img_equ.end<uint16_t>();
        // for (; (itEqual != itEqualEnd); ++itEqual)
        // {
        //     ++n_k_equalized[*itEqual];
        // }

        // for (int i = 0; i < L; i++)
        // {
        //     p_k_equalized[i] = (n_k_equalized[i] / (H * W));
        // }

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
        Gray16ToGray8 *self = GST_GRAY16TOGRAY8(user_data);
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
        GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
        gobject_class->dispose = gray16_to_gray8_dispose;

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

        dbus_vtable = {
            handle_set_brightness,
            NULL,
            NULL};

        gst_element_class_add_pad_template(element_class,
                                           gst_static_pad_template_get(&source_template));

       //gst_badPixMap();
        gst_readKBTablesFromFile();

        // Set the transform_ip function
        GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);
        base_transform_class->set_caps = GST_DEBUG_FUNCPTR(set_caps);
        base_transform_class->transform = GST_DEBUG_FUNCPTR(transform);
        base_transform_class->transform_caps =
        GST_DEBUG_FUNCPTR(gst_ir_filter_transform_caps);
    }

    // Initialize the instance
    static void instance_init(Gray16ToGray8 *self)
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
        Gray16ToGray8 *self = GST_GRAY16TOGRAY8(trans);
        double yOrig[L], yEqu[L], x[L];

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
        cv::Mat gray16_orig(height, width / 2, CV_16UC1, in_map.data);
        cv::Mat gray16 = gray16_orig.clone();
        cv::Mat gray8_1;
        cv::Mat gray8(height, width/2, CV_8UC1, out_map.data);

        // for (int i = 0; i < L; i++)
        // {
        //     n_k_original[i] = 0;
        //     n_k_equalized[i] = 0;
        // }
        memset(n_k_original, 0x00, sizeof(n_k_original));
        memset(p_k_original, 0x00, sizeof(p_k_original));
        memset(m_bp, 0x00, sizeof(m_bp));


        cv::Mat laplacian, gaussian;

        // Apply Laplacian operator

       gst_correction(gray16);


       gst_patching16bit(gray16);
        gst_calcOrigHist(gray16);
       //
       gst_histEqualization16bit(gray16);
       // cv::GaussianBlur(gray16, gaussian, cv::Size(5, 5), 0, 0);
        //cv::Laplacian(gaussian, laplacian, CV_16UC1, 5);

        //memset(gray16.data, 0x00,height* width *2);

        gray16.convertTo(gray8, CV_8UC1, 1.0 / 256.0);
       // cv::xphoto::bm3dDenoising(gray8_1, gray8);


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
