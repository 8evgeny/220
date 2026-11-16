#ifndef ABSTRACTCHANNEL_H
#define ABSTRACTCHANNEL_H

#include <QObject>
#include <QPainter>
#include <mutex>
#include <opencv2/opencv.hpp>

#include "device_parts/cmd/telemetry.h"

#define CHANNEL_W   (1920)
#define CHANNEL_H   (1080)


#define CHANNEL_NAME_TVRGB              ("TVRGB")
#define CHANNEL_SHOW_NAME_TVRGB         ("ТВ(цвет.)")
#define CHANNEL_NAME_FUSION             ("FUSION")
#define CHANNEL_SHOW_NAME_FUSION        ("Комплексирование")
#define CHANNEL_NAME_SWIR               ("SWIR")
#define CHANNEL_SHOW_NAME_SWIR          ("КВИК")
#define CHANNEL_NAME_MWIR               ("MWIR")
#define CHANNEL_SHOW_NAME_MWIR          ("ТПВ-С")
#define CHANNEL_NAME_TVGRAY             ("TVGRAY")
#define CHANNEL_SHOW_NAME_TVGRAY        ("ТВ(ч/б)")
#define CHANNEL_NAME_TVGRAYMWIR         ("TVGRAYMWIR")
#define CHANNEL_SHOW_NAME_TVGRAYMWIR    ("ТВЧБ+ТПВ-С")
#define CHANNEL_NAME_TEST               ("TEST")
#define CHANNEL_SHOW_NAME_TEST          ("Тест")
#define CHANNEL_NAME_CPU                ("CPU")
#define CHANNEL_SHOW_NAME_CPU           ("CPU")
#define CHANNEL_NAME_VLX_STREAM_1       ("TV_CLR_STREAM")
#define CHANNEL_SHOW_NAME_VLX_STREAM_1  ("ТВ(цв.)/RTP")
#define CHANNEL_NAME_VLX_STREAM_2       ("MWIR_STREAM")
#define CHANNEL_SHOW_NAME_VLX_STREAM_2  ("ТПВ-С/RTP")

#define CHANNEL_ID_TVRGB        (0x00u)
#define CHANNEL_ID_FUSION       (0x01u)
#define CHANNEL_ID_SWIR         (0x02u)
#define CHANNEL_ID_MWIR         (0x03u)
#define CHANNEL_ID_TVGRAY       (0x04u)
#define CHANNEL_ID_CPU          (0x05u)
#define CHANNEL_ID_TVGRAYMWIR   (0x06u)
#define CHANNEL_ID_TEST         (0x07u)
#define CHANNEL_ID_VLX_STREAM_1 (0x80u)
#define CHANNEL_ID_VLX_STREAM_2 (0x81u)
#define CHANNEL_ID_NONE         (0xFFu)

#define CHANNEL_TV_PIX_SIZE     (0.00345)
#define CHANNEL_SWIR_PIX_SIZE   (0.020)
#define CHANNEL_MWIR_PIX_SIZE   (0.015)

#define CHANNEL_TV_MIN_FOCUS_MM    (15) // 400
#define CHANNEL_TV_MAX_FOCUS_MM    (510)
// #define CHANNEL_TV_MIN_FOCUS_MM    (8.57) // 350
// #define CHANNEL_TV_MAX_FOCUS_MM    (120)
#define CHANNEL_MWIR_MIN_FOCUS_MM  (45) // 400
#define CHANNEL_MWIR_MAX_FOCUS_MM  (600)
// #define CHANNEL_MWIR_MIN_FOCUS_MM  (15)  // 350
// #define CHANNEL_MWIR_MAX_FOCUS_MM  (330)
#define CHANNEL_SWIR_MIN_FOCUS_MM  (32)
#define CHANNEL_SWIR_MAX_FOCUS_MM  (320)

class ETLD;
class CmdManager;
class GSPController;
class EthController;

class AbstractChannel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractChannel(QObject *parent = nullptr,
                             const QString &_name = "Abstract",
                             const QString &_show_name = "Abstract",
                             const uint32_t &_id = 0,
                             const int &_w = CHANNEL_W,
                             const int &_h = CHANNEL_H);
    virtual ~AbstractChannel();

    inline QString name() const {return _name;}
    inline QString show_name() const {return _show_name;}
    inline uint32_t id() const {return _id;}
    inline int w() const {return _w;}
    inline int h() const {return _h;}
    inline int n() const {return _n;}
    inline double const_zoom() const {return _const_zoom;}
    cv::Mat origin_img() const {return _origin_img;}
    cv::Mat display_img() const {return _display_img;}
    inline double digital_zoom() const {return _digital_zoom;}
    inline bool pic_in_pic_on() const {return _pic_in_pic_on;}
    inline int swir_gain() const {return _swir_gain;}
    inline bool agc_on() const {return _agc_on;}
    inline bool inversion_on() const {return _inversion_on;}
    inline int brightness() const {return _brightness;}
    inline double contrast() const {return _contrast;}
    inline bool autofocus() const {return _autofocus;}
    inline bool reg_field_sync() const {return _reg_field_sync;}
    inline double focus_mm() const {return _focus_mm;}
    inline double min_focus_mm() const {return _min_focus_mm;}
    inline double max_focus_mm() const {return _max_focus_mm;}
    inline double pixel_size_mm() const {return _pixel_size_mm;}
    inline double field_angle() const {return _field_angle;}
    inline bool x_inv() const {return _x_inv;}
    inline bool y_inv() const {return _y_inv;}

    inline void set_const_zoom(const double & v){_const_zoom = v;}
    inline void set_digital_zoom(const double & v){_digital_zoom = v > 8 ? 8 : (v < 1 ? 1 : v);}
    inline void set_pic_in_pic_on(const bool & v){_pic_in_pic_on = v;}
    inline void set_swir_gain(const int & v){_swir_gain = v;}
    inline void set_agc_on(const bool & v){_agc_on = v;}
    inline void set_inversion_on(const bool & v){_inversion_on = v;}
    inline void set_brightness(const int & v){_brightness = v > 127 ? 127 : (v < -127 ? -127 : v);}
    inline void set_contrast(const double & v){_contrast = v > 2 ? 2 : (v < 0. ? 0. : v);}
    inline void set_autofocus(const bool v){_autofocus = v;}
    inline void set_reg_field_sync(const bool v){_reg_field_sync = v;}
    inline void set_focus_mm(const double & v){_focus_mm = v;}
    inline void set_min_focus_mm(const double & v){_min_focus_mm = v;}
    inline void set_max_focus_mm(const double & v){_max_focus_mm = v;}
    inline void set_pixel_size_mm(const double & v){_pixel_size_mm = v;}
    inline void set_field_angle(const double & v){_field_angle = v;}
    inline void set_x_inv(const bool & v){_x_inv = v;}
    inline void set_y_inv(const bool & v){_y_inv = v;}

    void parse_raw_img(uint8_t * img_data, const int & img_w, const int & img_h){parse_raw_img(cv::Mat_<uint8_t>(img_h, img_w, img_data));}
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) = 0;
    virtual void selected() {};
    virtual void deselected() {};


    void decount_object(int & x, int & y, int & w, int & h, const int & W, const int & H);
    void recount_object(int & x, int & y, int & w, int & h, const int & W, const int & H);

protected:
    void load();
    void save();

protected:
    const QString _name;
    const QString _show_name;
    const uint32_t _id;
    const int _w;
    const int _h;
    const int _n;
    double _const_zoom;
    cv::Mat _origin_img;
    cv::Mat _display_img;
    std::mutex img_mutex;
    // Video
    double _digital_zoom;
    bool   _pic_in_pic_on;
    int    _swir_gain;
    bool   _agc_on;
    bool   _inversion_on;
    int    _brightness;
    double _contrast;
    bool   _autofocus;
    bool   _reg_field_sync;
    //Objective
    double _focus_mm;
    double _min_focus_mm;
    double _max_focus_mm;
    double _pixel_size_mm;
    double _field_angle;
    //Position
    bool _x_inv;
    bool _y_inv;
};

#endif // ABSTRACTCHANNEL_H
