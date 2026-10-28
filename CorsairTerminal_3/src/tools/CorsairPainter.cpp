#include "CorsairPainter.hpp"
#include "videoregistrator.h"
#include "tools/drawp.h"
#include "tools/imgp.h"

#include <device_parts/channel/abstractchannel.h>

#include "vlx_settings.h"
#include "vlx_display_parameters_dialog.h"

#include <QDebug>

CorsairPainter::CorsairPainter(QObject *parent)
    : QObject{parent}
    , m_disp_q_img(new QImage(CHANNEL_W, CHANNEL_H, QImage::Format_RGB888))
    , _video_record(false)
    , _image_save(false)
    , _infographics_on_screen(true)
    , _infographics_on_record(true)
    , _infographics_on_picture(true)
{
    videoRegistrator = new VideoRegistrator(PATH, FILE_NAME_PREFIX, cv::Size(VIDEO_REGISTRATOR_W, VIDEO_REGISTRATOR_H),
                                            cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                            VIDEO_REGISTRATOR_FPS, VIDEO_REGISTRATOR_LEN_S, VIDEO_REGISTRATOR_CNT);


    _alignmentCrossEn = VlxSettings::i().dispAlignmentCross();

    connect(VlxDisplayParametersDialog::inst(), &VlxDisplayParametersDialog::settingsSaved, this,
            [this]() {_alignmentCrossEn = VlxSettings::i().dispAlignmentCross();});
}

CorsairPainter::~CorsairPainter()
{
    delete m_disp_q_img;
    delete videoRegistrator;
}

void CorsairPainter::setFrameImage(cv::Mat img)
{
    m_image = img;
}

const QImage &CorsairPainter::outputQImage() const
{
    return *m_disp_q_img;
}

const QTransform &CorsairPainter::frameToDispTransform() const
{
    return m_frame_to_disp_tr;
}

const QSize &CorsairPainter::frameSize() const
{
    return m_frame_size;
}

void CorsairPainter::video_record_switch()
{
    _video_record_mutex.lock();
    _video_record = !_video_record;
    if(!_video_record) {
        videoRegistrator->release();
    }
    _video_record_mutex.unlock();
}

void CorsairPainter::video_record_off()
{
    _video_record_mutex.lock();
    _video_record = false;
    videoRegistrator->release();
    _video_record_mutex.unlock();
}

void CorsairPainter::setTarget(bool en, int size, int id, bool fast)
{
    target_en   = en;
    target_size = size;
    target_id   = id;
    target_fast = fast;
}

void CorsairPainter::setGsp(const Controller::t_gsp &newGsp, double z_shift, bool GSPControlEnable)
{
    _gsp = newGsp;
    z_shift_angle = z_shift;
    m_GSPControlEnable = GSPControlEnable;
}

void CorsairPainter::setCurrentChannelID(int32_t newCurrentChannelID, int32_t currIdChannelW, int32_t currIdChannelH)
{
    m_currentChannelID = newCurrentChannelID;
    m_currIdChannelW = currIdChannelW;
    m_currIdChannelH = currIdChannelH;
}

void CorsairPainter::setShipPointer()
{
    drawp::draw_setShipPointer();
}
void CorsairPainter::setHelicopterPointer()
{
    drawp::draw_setHelicopterPointer();
}

void CorsairPainter::writeVideoFrame(cv::Mat &frame)
{
    (*videoRegistrator) << frame;
}

void CorsairPainter::writeImage(cv::Mat &frame)
{
    videoRegistrator->save_image(frame);
    image_save_off();
}

void CorsairPainter::drawTracker(const AbstractChannel *channel, QPainter &p)
{
    drawp::draw_target(p, *m_disp_q_img, target_id, target_fast, target_en, target_size);
    // if(host_etld_en)
    // {
    //     for(int i = 0; i < 4; ++i)
    //     {
    //         drawp::draw_object(*disp_img,
    //                            i,
    //                            objects[i].valid, objects[i].trusted,
    //                            objects[i].x, objects[i].y, objects[i].w, objects[i].h,
    //                            channel->const_zoom() * channel->digital_zoom(),
    //                            channel->w(), channel->h(), channel->x_inv(), channel->y_inv());
    //     }
    // }
    // else if(tracker_channel != 0xFF)

    if(tracker_channel != 0xFF)
    {
        if((tracker_channel == channel->id()) || (channel->id() == CHANNEL_ID_TVRGB && tracker_channel == CHANNEL_ID_TVGRAY))
        {
            for(int i = 0; i < 4; ++i)
            {
                // drawp::draw_object(p, *m_disp_q_img,
                //                    i,
                //                    objects[i].valid, objects[i].trusted,
                //                    objects[i].x, objects[i].y, objects[i].w, objects[i].h,
                //                    channel->const_zoom() * channel->digital_zoom(),
                //                    channel->w(), channel->h(), channel->x_inv(), channel->y_inv());

                drawp::draw_object_v2(p, i,
                                   objects[i].valid, objects[i].trusted,
                                   objects[i].x, objects[i].y, objects[i].w, objects[i].h,
                                   m_frame_to_disp_tr);
            }
        }
    }
}

QTransform CorsairPainter::buildFrameToDispTransform(const cv::Mat &original, double digital_zoom, const QSize &dst_size, bool x_inv, bool y_inv)
{
    const int W0 = original.cols;
    const int H0 = original.rows;
    const int Wd = dst_size.width();
    const int Hd = dst_size.height();

    // ---- digital_zoom: ровно те же выражения, что в imgp::digital_zoom ----
    const int w  = int(std::round(W0 / digital_zoom));
    const int h  = int(std::round(H0 / digital_zoom));
    const int xz = W0 / 2 - w / 2;
    const int yz = H0 / 2 - h / 2;

    // ---- fit: ровно те же выражения, что в imgp::fit ----
    int fx, fy, fw, fh;
    if (W0 == Wd && H0 == Hd) {
        fx = 0; fy = 0; fw = Wd; fh = Hd;
    } else {
        const int hh = Wd * H0 / W0;
        const int ww = Hd * W0 / H0;
        if (hh <= Hd) { fx = 0;             fy = (Hd - hh) / 2; fw = Wd; fh = hh; }
        else          { fx = (Wd - ww) / 2; fy = 0;             fw = ww; fh = Hd; }
    }

    // 1) Инверсия внутри исходного кадра:
    //    (X, Y) -> (x_inv ? W0 - X : X,  y_inv ? H0 - Y : Y)
    QTransform inv(x_inv ? -1.0 : 1.0, 0.0,
                   0.0, y_inv ? -1.0 : 1.0,
                   x_inv ? double(W0) : 0.0,
                   y_inv ? double(H0) : 0.0);

    // 2) digital_zoom: сдвиг на (-xz, -yz), затем масштаб (W0/w, H0/h)
    const double zsx = double(W0) / w;
    const double zsy = double(H0) / h;
    QTransform zoom(zsx, 0.0,
                    0.0, zsy,
                    -xz * zsx, -yz * zsy);

    // 3) fit: масштаб (fw/W0, fh/H0), затем сдвиг (fx, fy)
    const double fsx = double(fw) / W0;
    const double fsy = double(fh) / H0;
    QTransform fit(fsx, 0.0,
                   0.0, fsy,
                   double(fx), double(fy));

    // Порядок применения к точке: inv, затем zoom, затем fit.
    // В Qt p * (A * B) == (p * A) * B, значит пишем именно так.
    return inv * zoom * fit;
}

void CorsairPainter::sl_paint()
{
    if(m_enable)
    {
        _video_record_mutex.lock();
        cv::Mat image = m_image;
        cv::Mat inverted_image, zoomed_image, fitted_image;

        imgp::direction_inversion(image, inverted_image, channel->x_inv(), channel->y_inv());
        imgp::digital_zoom(inverted_image, zoomed_image, channel->digital_zoom());
        imgp::fit(zoomed_image, fitted_image, cv::Size(m_disp_q_img->width(), m_disp_q_img->height()));

        *m_disp_q_img = imgp::cv2qimage(fitted_image);

        m_frame_to_disp_tr = buildFrameToDispTransform(
            m_image,
            channel->digital_zoom(),
            m_disp_q_img->size(),
            channel->x_inv(), channel->y_inv());

        m_frame_size = {m_image.cols, m_image.rows};

        /** ########################################################
         *      Начало блока нанесения графики на изображение
         *  ######################################################## */

        QPainter p;
        p.begin(m_disp_q_img);

        bool isDrawOnScreen{_infographics_on_screen};
        bool isDrawOnVideo{_infographics_on_record};
        bool isDrawOnImage{_infographics_on_picture};

        // // костыль костылич для тепляка
        // if (m_mwirDumpEn == false) {
        //     if (m_currentChannelID == CHANNEL_ID_MWIR) {
        //         if (m_mwirSensorStatus || (m_currIdChannelW != 640) || (m_currIdChannelH != 512)) {
        //             drawp::draw_mw_loading_image(p, *m_disp_q_img, true);
        //         }
        //     }
        //     // if(channel->id() == CHANNEL_ID_MWIR || m_mwirWaitingEn) {
        //     //     if( m_mwirSensorStatus ) {
        //     //         drawp::draw_mw_loading_image(p, *disp_img, true);
        //     //     }
        //     // }
        // }

        if (isDrawOnScreen || isDrawOnVideo || isDrawOnImage) {
            drawp::draw_common_field(p, *m_disp_q_img);
            drawp::draw_pic_in_pic(p, *m_disp_q_img, channel->pic_in_pic_on());
            drawp::draw_channel(p, *m_disp_q_img, channel->show_name());

            if (_alignmentCrossEn)
                drawp::draw_alignment_cross(p, *m_disp_q_img);
            else
                drawp::draw_aim(p, *m_disp_q_img, target_size, _rangefinder.flags.rangefinder_power_on);

            double gspX = channel->y_inv() ? -_gsp.x_angle : _gsp.x_angle;
            gspX = _infographics_angleInversion_X ? -gspX : gspX;
            double gspZ = channel->x_inv() ? -(_gsp.z_angle + z_shift_angle)
                                           : (_gsp.z_angle + z_shift_angle);
            gspZ = _infographics_angleInversion_Z ? -gspZ : gspZ;

            drawp::draw_gsp(p, *m_disp_q_img, gspX, gspZ, m_zAngleRange180);
            emit sg_sendGSPAngles(gspX, gspZ);

            //            drawp::draw_zoom(*disp_img, channel->focus_mm(), channel->min_focus_mm(), channel->max_focus_mm(), channel->digital_zoom());

            drawp::draw_highlighter(p,
                                    *m_disp_q_img,
                                    _rangefinder.flags.rangefinder_power_on,
                                    _rangefinder.flags.rangefinder_rdy,
                                    _rangefinder.flags.rangefinder_pause,
                                    _rangefinder.flags.rangefinder_cont_emit,
                                    (_rangefinder.flags.rangefinder_mode
                                     == Controller::en_lrf_rc_single_measure)
                                        ? 1
                                        : 0,
                                    _rangefinder.rangefinder_range_mono);

            drawTracker(channel, p);

            drawp::draw_bc(p, *m_disp_q_img, channel->brightness(), channel->contrast());
            drawp::draw_agc(p, *m_disp_q_img, channel->agc_on());
            drawp::draw_inversion(p, *m_disp_q_img, channel->inversion_on());
            drawp::draw_autofocus(p, *m_disp_q_img, channel->autofocus());
            drawp::draw_field_sync_reg(p, *m_disp_q_img, channel->reg_field_sync());
            //            drawp::draw_gsp_state(*disp_img,
            //                                  _gsp.flags.off,
            //                                  _gsp.flags.mech_arretir,
            //                                  _gsp.flags.el_arretir,
            //                                  _gsp.flags.stabilization);

            drawp::draw_field_angle(p, *m_disp_q_img, channel->field_angle());
            drawp::draw_video_record(p, *m_disp_q_img, _video_record);

            /* Сохранение кадра в видео или изображение */
            bool isSaveFrame{video_record() || image_save()};
            bool isSeveImageWithDraw{isDrawOnImage && image_save()};
            bool isSeveVideoWithDraw{isDrawOnVideo && video_record()};
            bool isSeveImageWithoutDraw{!isDrawOnImage && image_save()};
            bool isSeveVideoWithoutDraw{!isDrawOnVideo && video_record()};
            if (isSaveFrame) {
                if (isSeveImageWithDraw || isSeveVideoWithDraw) {
                    cv::Mat frame_with_infographics = imgp::QImage2Mat(*m_disp_q_img);
                    if (video_record()) {
                        writeVideoFrame(frame_with_infographics);
                    }
                    if (image_save()) {
                        writeImage(frame_with_infographics);
                    }
                }

                if (isSeveImageWithoutDraw || isSeveVideoWithoutDraw) {
                    QImage disp_img_1 = imgp::cv2qimage(zoomed_image);
                    QPainter painter;
                    painter.begin(&disp_img_1);
                    drawp::draw_mw_loading_image(painter, disp_img_1, true);
                    painter.end();
                    cv::Mat buff = imgp::QImage2Mat(disp_img_1);
                    if (isSeveImageWithoutDraw) {
                        writeImage(buff);
                    }
                    if (isSeveVideoWithoutDraw) {
                        writeVideoFrame(buff);
                    }
                }
            }
        }
        p.end();
        /** ########################################################
         *     Завершение блока нанесения графики на изображение
         *  ######################################################## */

        if (!isDrawOnScreen) {
            *m_disp_q_img = imgp::cv2qimage(fitted_image);
            // костыль костылич для тепляка
            // if(channel->id() == CHANNEL_ID_MWIR) {
            //     if( m_mwirSensorStatus ) {
            //         drawp::draw_mw_loading_image(p, *disp_img, true);
            //     }
            // }
            if (m_currentChannelID == CHANNEL_ID_MWIR) {
                if (m_mwirSensorStatus || (m_currIdChannelW != 640) || (m_currIdChannelH != 512)) {
                    drawp::draw_mw_loading_image(p, *m_disp_q_img, true);
                }
            }
        }

        // костыль костылич для тепляка
        // MWIRKostyl();

        _video_record_mutex.unlock();

        emit sg_ready();
    }
}












