#ifndef CORSAIRPAINTER_HPP
#define CORSAIRPAINTER_HPP

#include <opencv2/core/mat.hpp>

#include <QObject>
#include <controller.h>
#include <mutex>

class AbstractChannel;
class VideoRegistrator;

class CorsairPainter : public QObject
{
    Q_OBJECT
public:
    explicit CorsairPainter(QObject *parent = nullptr);
    ~CorsairPainter();
    void setFrameImage(cv::Mat img);
    const QImage &outputQImage() const;
    const QTransform& frameToDispTransform() const;
    const QSize& frameSize() const;

    void setCurrentChannel(const AbstractChannel *ch) {channel = ch;}

    void video_record_switch();
    void video_record_on() {_video_record = true;}
    void video_record_off();

    void mwirSensorStatus(int32_t val) {m_mwirSensorStatus = val;};
    void mwirWaitingEn(bool val)       {m_mwirWaitingEn =val;};
    void mwirDumpEn(bool val)          {m_mwirDumpEn = val; };

    bool video_record() {return _video_record;}
    void video_record(bool newVideo_record) {_video_record = newVideo_record;}
    void infographics_on_screen(bool newInfographics_on_screen) {_infographics_on_screen = newInfographics_on_screen;}
    void infographics_on_record(bool newInfographics_on_record) {_infographics_on_record = newInfographics_on_record;}
    void infographics_on_picture(bool newInfographics_on_picture) {_infographics_on_picture = newInfographics_on_picture;}
    void infographics_angleInversion_X_changed(bool angleInversion) {_infographics_angleInversion_X = angleInversion;}
    void infographics_angleInversion_Z_changed(bool angleInversion) {_infographics_angleInversion_Z = angleInversion;}

    void setTarget(bool en, int size, int id, bool fast);
    void setRangefinder(const Controller::t_rangefinder &newRangefinder) {_rangefinder = newRangefinder;}
    void setGsp(const Controller::t_gsp &newGsp, double z_shift, bool GSPControlEnable);
    void setGspAngleRange(bool zAngleRange180) {m_zAngleRange180 = zAngleRange180;}
    void setTracker(uint8_t state, uint8_t channel) {tracker_state = state; tracker_channel = channel;}
    void setTrackerObject(Controller::TrackerObject *obj) {memcpy(objects, obj, 4 * sizeof(Controller::TrackerObject));}

    bool image_save() {return _image_save;}
    void image_save_switch() {_image_save = !_image_save;}
    void image_save_on() {_image_save = true;}
    void image_save_off() {_image_save = false;}

    void setCurrentChannelID(int32_t newCurrentChannelID, int32_t currIdChannelW, int32_t currIdChannelH);

    void setEnable(bool newEnable) {m_enable = newEnable;}

    void setShipPointer();
    void setHelicopterPointer();

private:
    void writeVideoFrame(cv::Mat &frame);
    void writeImage(cv::Mat &frame);
    void drawTracker(const AbstractChannel *channel, QPainter &p);


    QTransform buildFrameToDispTransform(const cv::Mat& original,
                                         double digital_zoom,
                                         const QSize& dst_size,
                                         bool x_inv, bool y_inv);

public slots:
    void sl_paint();

signals:
    void sg_ready();
    void sg_sendGSPAngles(const double angleX, const double angleZ);

private:
    const AbstractChannel *channel{nullptr};
    bool m_enable{true};

    int32_t m_currentChannelID{0};
    int32_t m_currIdChannelW{0};
    int32_t m_currIdChannelH{0};

    cv::Mat m_image;
    cv::Mat inverted_image;
    cv::Mat zoomed_image;
    cv::Mat fitted_image;
    QImage *m_disp_q_img;
    QTransform m_frame_to_disp_tr;
    QSize m_frame_size;

    std::mutex _video_record_mutex;
   bool _video_record;
   bool _image_save;
   bool _infographics_on_screen;
   bool _infographics_on_record;
   bool _infographics_on_picture;
   bool _infographics_angleInversion_X;
   bool _infographics_angleInversion_Z;

   bool _alignmentCrossEn = false;

   // VideoRegistrator
   VideoRegistrator *videoRegistrator;
   const std::string PATH = "media";
   const std::string FILE_NAME_PREFIX = "corsair";
   const int VIDEO_REGISTRATOR_W     = 1920;
   const int VIDEO_REGISTRATOR_H     = 1080;
   const int VIDEO_REGISTRATOR_FPS   = 25;
   const int VIDEO_REGISTRATOR_LEN_S = 250;
   const int VIDEO_REGISTRATOR_CNT   = 10;

   int32_t m_mwirSensorStatus{};
   bool m_mwirWaitingEn {false};
   bool m_mwirDumpEn{false};

   // Target
   bool target_en;
   int target_size;
   int target_id;
   bool target_fast;
   // Object
   uint8_t tracker_state;
   uint8_t tracker_channel;

   Controller::t_rangefinder _rangefinder;
   Controller::t_gsp _gsp;
   Controller::TrackerObject objects[4];

   double z_shift_angle;
   bool m_GSPControlEnable { true };
   bool m_zAngleRange180 { true };
};

#endif // CORSAIRPAINTER_HPP
