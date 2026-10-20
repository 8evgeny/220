#ifndef DRAWP_H
#define DRAWP_H

#include <QImage>
#include "tools/global_types.h"

namespace drawp
{
    void init_drawp();
    void draw_X(QPainter & p, int x, int y);
    void draw_pic_in_pic(QPainter & p, QImage & disp_img, const bool & pic_in_pic_on);
    void draw_channel(QPainter & p, QImage & disp_img, const QString & _name);
    void draw_aim(QPainter & p, QImage & disp_img, const int & target_size, const bool rf_enable);
    void draw_alignment_cross(QPainter & p, QImage & disp_img);
    void draw_gsp(QPainter & p, QImage & disp_img, const double & gsp_x_angle, const double & gsp_z_angle, const bool zAngleRange180En);
    void draw_zoom(QPainter & p, QImage & disp_img, const double & focus_mm, const double & min_focus_mm, const double & max_focus_mm, const double & digital_zoom);
    void draw_rangefinder(QPainter & p, QImage & disp_img, const bool & rangefinder_en, const bool & rangefinder_5Hz, const bool & rangefinder_on, const double & rangefinder_range);
    void draw_highlighter(QPainter & p, QImage & disp_img, const bool & highlighter_power_on, const bool & highlighter_rdy, const bool & highlighter_pause,
                          const bool & highlighter_cont_emit, const bool &highlighter_single,
                          const double & rangefinder_range);
    void draw_target(QPainter & p, QImage & disp_img, const int & target_id, const int & target_fast, const bool & target_en, const int & target_size);
    void draw_target_v2(QPainter & p, QImage & disp_img, const int & target_id, const int& xPos, const int& yPos, const int& width, const int& height);
    void draw_object(QPainter & p, QImage & disp_img,
                     const int & object_id,
                     const bool & object_valid, const bool & object_trusted,
                     const int & object_x, const int & object_y, const int & object_w, const int & object_h,
                     const double & digital_zoom,
                     const int & _w, const int & _h, const bool & x_inv, const bool & y_inv);

    void draw_object_v2(QPainter & p,
                     const int & object_id,
                     const bool & object_valid, const bool & object_trusted,
                     const int & object_x, const int & object_y, const int & object_w, const int & object_h,
                     const QTransform &frm_to_disp_tr);

    void draw_agc(QPainter & p, QImage & disp_img, const bool & agc_on);
    void draw_inversion(QPainter & p, QImage & disp_img, const bool & inversion_on);
    void draw_bc(QPainter & p, QImage & disp_img, const int & brightness, const double & contrast);
    void draw_autofocus(QPainter & p, QImage & disp_img, const bool & autofocus_on);
    void draw_field_sync_reg(QPainter & p, QImage & disp_img, const bool & field_sync_reg_on);
    void draw_gsp_state(QPainter & p, QImage & disp_img,
                        const bool & off,
                        const bool & mech_arretir, const bool & el_arretir,
                        const bool & stabilization);
    void draw_field_angle(QPainter & p, QImage & disp_img, const double & field_angle);
    void draw_datetime(QPainter & p, QImage & disp_img);
    void draw_swir_gain(QPainter & p, QImage & disp_img, const int & mode);
    void draw_video_record(QPainter & p, QImage & disp_img, const bool & video_record);
    void draw_laser_radiation(QPainter & p, QImage & disp_img, const bool en);

    void draw_lrf_dangerZone(QPainter & p, QImage & disp_img);
    void draw_common_field(QPainter & p, QImage & disp_img);
    void draw_mw_loading_image(QPainter & p, QImage & disp_img, const bool &en);
    void draw_radarCircle(QPainter & painter, QImage & disp_img, const double & angleZ, const double & currField);
    void draw_background(QPainter * p, const uint32_t posX, const uint32_t posY,
                         const uint32_t width, const uint32_t height);
    void draw_setShipPointer();
    void draw_setHelicopterPointer();
}

#endif // DRAWP_H
