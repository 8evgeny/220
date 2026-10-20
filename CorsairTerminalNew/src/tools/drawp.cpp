#include "drawp.h"
#include <cmath>

#include <QPainter>
#include <QPainterPath>
#include <QDateTime>
#include <QDebug>
#include <QRgb>

#define PADDING_TOP     (5)
#define PADDING_BOT     (5)
#define PADDING_LEFT    (5)
#define PADDING_RIGHT   (5)
#define TEXT_HEIGHT     (18)
#define TEXT_WIDTH      (150)
#define LINE_WIDTH      (2)
#define CROSS_SIZE      (10)
#define CROSS_LINE      (3)
#define GREEN_COLOR     (QColor(50, 205, 50, 255))
#define WHITE_COLOR     (QColor(245, 245, 245, 255))
#define YELLOW_COLOR     (QColor(255, 215, 0, 255))
#define RED_COLOR     (QColor(255, 69, 0, 255))
//#define GREEN_COLOR     (QColor(145, 220, 90, 255))
#define TEXT_FONT(height) (QFont("Lucida Console", height, QFont::Bold))
#define TEXT_BRUSH      (QBrush(WHITE_COLOR, Qt::SolidPattern))
#define TEXT_BRUSH_YELLOW (QBrush(YELLOW_COLOR, Qt::SolidPattern))
#define TEXT_BRUSH_GREEN (QBrush(GREEN_COLOR, Qt::SolidPattern))
#define TEXT_BRUSH_RED (QBrush(RED_COLOR, Qt::SolidPattern))
//#define TEXT_FONT(height) (QFont("times", height, QFont::Bold))

namespace drawp
{

// For rangefinder distance
#define METRIC_DIST
#ifdef METRIC_DIST // метры
    static const char str_radiation[]("ЛДЦ: %.3f м (ИЗЛУЧЕНИЕ)");
    static const char str_single[](   "ЛДЦ: %.3f м (один.)");
    static const char str_point[](    "ЛДЦ: %.3f м (непрер.)");
    static const char str_power_on[]( "ЛДЦ: Вых. на реж.");
    static const char str_cooling[](  "ЛДЦ: %.3f м (охлажд.)");
    static const double rf_dist_divisor = 1.;
#else // Кабельты
    static const char str_radiation[]("ЛДЦ: %.3f кбт (ИЗЛУЧЕНИЕ)");
    static const char str_single[](   "ЛДЦ: %.3f кбт (один.)");
    static const char str_point[](    "ЛДЦ: %.3f кбт (непрер.)");
    static const char str_power_on[]( "ЛДЦ: Вых. на реж.");
    static const char str_cooling[](  "ЛДЦ: %.3f кбт (охлажд.)");
    static const double rf_dist_divisor = 185.2; ; // 1 кб = 185,2 м = 1/10 мили
#endif

    static const char str_radiation_faill[]("ЛДЦ: \"НЕТ ДАННЫХ\" (ИЗЛУЧЕНИЕ)");
    static const char str_single_faill[](   "ЛДЦ: \"НЕТ ДАННЫХ\" (один.)");
    static const char str_point_faill[](    "ЛДЦ: \"НЕТ ДАННЫХ\" (непрер.)");
    static const char str_cooling_faill[](  "ЛДЦ: \"НЕТ ДАННЫХ\" (охлажд.)");
    static const char str_power_off[]("ЛДЦ: Выкл.");

    static const uint32_t dangerZoneTextHeight = TEXT_HEIGHT;
    static const QString lrf_dangerZone("ЛДЦ: ЗАПРЕЩЕННЫЙ СЕКТОР");
    static const QFont dangerZoneFont(TEXT_FONT(dangerZoneTextHeight));
//    static const QFont dangerZoneFont("times", dangerZoneTextHeight, QFont::Bold);
    static const QFontMetrics dangerZoneFontMetrics(dangerZoneFont);

    static QImage img_pointerTop;
    static QImage img_pointerProfile;
    static QImage img_helicopterTop;
    static QImage img_helicopterProfile;
    static QImage img_shipTop;
    static QImage img_shipProfile;
    static QImage agc;
    static QImage autofocus;
    static QImage _brightness;
    static QImage _contrast;
    static QImage inversion;
    static QImage img_inversion_off;
    static QImage img_inversion_on;
    static QImage field_sync_reg;
    static QImage field_sync_narrow;
    static QImage gsp_none;
    static QImage gsp_mech_arretir_x;
    static QImage gsp_mech_arretir_y;
    static QImage gsp_mech_arretir_xy;
    static QImage gsp_el_arretir_x;
    static QImage gsp_el_arretir_y;
    static QImage gsp_el_arretir_xy;
    static QImage gsp_stabilization_x;
    static QImage gsp_stabilization_y;
    static QImage gsp_stabilization_xy;
    static QImage laser_radiation;
    static QImage laser_radiation_red;
    static QImage laser_radiation_green;
    static QImage record_img;
    static QImage img_loading_mw;
    static QImage img_laser_off;
    static QImage img_laser_on;
    static QImage img_laser_cooling;
    static QImage img_laser_radiation;
    static QImage img_laser_waiting;


    static const uint32_t posY_channel = PADDING_TOP*8 + TEXT_HEIGHT;
    static const uint32_t posY_channelField = posY_channel + TEXT_HEIGHT * 2;
    static const uint32_t posY_laser_img = posY_channelField + TEXT_HEIGHT;
    static const uint32_t posY_record = posY_laser_img + 100;
    static const uint32_t posY_inversion = posY_record + TEXT_HEIGHT * 3;
    static const uint32_t posY_contrast = posY_inversion + TEXT_HEIGHT * 3;
    static const uint32_t posY_brightness = posY_contrast + TEXT_HEIGHT * 3;
    static const uint32_t posY_autofocus = posY_brightness + TEXT_HEIGHT * 3;
    static const uint32_t posY_field_sync_reg = posY_autofocus + TEXT_HEIGHT * 3;
    static const uint32_t posY_agc = posY_field_sync_reg + TEXT_HEIGHT * 3;
    static const uint32_t posY_pointer_center = 1000;
//    static const uint32_t posY_pointer_z_center = posY_pointer_x_center + 150;

    static const uint32_t posX_pointerText = 250;



    void init_drawp()
    {
        img_helicopterTop = QImage(":/docs/image/Helicopter_TOP.png").scaled(112, 166);
        img_helicopterProfile = QImage(":/docs/image/Helicopter_PROFILE.png").scaled(150, 68);
        img_shipTop = QImage(":/docs/image/pointer.png").scaled(/*64, 160*/ 49, 123);
        img_shipProfile = QImage(":/docs/image/ship.png").scaled(/*160, 64*/ 123, 49);
        img_pointerTop = img_shipTop;
        img_pointerProfile = img_shipProfile;
        agc = QImage(":/docs/image/agc.png");
        autofocus = QImage(":/docs/image/auto-focus.png");
        _brightness = QImage(":/docs/image/brightness.png");
        _contrast = QImage(":/docs/image/contrast.png");
        inversion = QImage(":/docs/image/inversion.png");
        img_inversion_on = QImage(":/docs/image/inversion_on.png");
        img_inversion_off = QImage(":/docs/image/inversion_off.png");
        field_sync_reg = QImage(":/docs/image/field-sync-reg.png");
        field_sync_narrow = QImage(":/docs/image/field-sync-narrow.png");
        gsp_none = QImage(":/docs/image/gsp-none.png");
        gsp_mech_arretir_x = QImage(":/docs/image/gsp-mech-arretir-x.png");
        gsp_mech_arretir_y = QImage(":/docs/image/gsp-mech-arretir-y.png");
        gsp_mech_arretir_xy = QImage(":/docs/image/gsp-mech-arretir-xy.png");
        gsp_el_arretir_x = QImage(":/docs/image/gsp-el-arretir-x.png");
        gsp_el_arretir_y = QImage(":/docs/image/gsp-el-arretir-y.png");
        gsp_el_arretir_xy = QImage(":/docs/image/gsp-el-arretir-xy.png");
        gsp_stabilization_x = QImage(":/docs/image/gsp-stabilization-x.png");
        gsp_stabilization_y = QImage(":/docs/image/gsp-stabilization-y.png");
        gsp_stabilization_xy = QImage(":/docs/image/gsp-stabilization-xy.png");
        laser_radiation = QImage(":/docs/image/laser-radiation.png");
        laser_radiation_red = QImage(":/docs/image/laser_red_v2.png");
        laser_radiation_green = QImage(":/docs/image/laser_green_v2.png");
        record_img = QImage(":/docs/image/record.png");
        img_loading_mw = QImage(":/docs/image/loading.png");

        uint32_t laserImgScaleX = 80;
        uint32_t laserImgScaleY = 71;
        img_laser_off = QImage(":/docs/image/laser_gray_v3.png").scaled(laserImgScaleX, laserImgScaleY);
        img_laser_on = QImage(":/docs/image/laser_green_v3.png").scaled(laserImgScaleX, laserImgScaleY);
        img_laser_cooling = QImage(":/docs/image/laser_blue_v3.png").scaled(laserImgScaleX, laserImgScaleY);
        img_laser_radiation = QImage(":/docs/image/laser_red_v2.png").scaled(laserImgScaleX, laserImgScaleY);
        img_laser_waiting = QImage(":/docs/image/laser_yellow_v3.png").scaled(laserImgScaleX, laserImgScaleY);
    }

    void draw_X(QPainter & p, int x, int y)
    {
        p.drawLine(x - CROSS_SIZE, y - CROSS_SIZE, x + CROSS_SIZE, y + CROSS_SIZE );
        p.drawLine(x - CROSS_SIZE, y + CROSS_SIZE, x + CROSS_SIZE, y - CROSS_SIZE );
    }

    void draw_pic_in_pic(QPainter &p, QImage & disp_img, const bool & pic_in_pic_on)
    {
        if(pic_in_pic_on)
        {
            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);
            int sub_w = disp_img.width() / 8;
            int sub_h = disp_img.height() / 8;
            int w = disp_img.width();
            int h = disp_img.height();
            QImage tmp = disp_img.copy(w / 2 - sub_w / 2, h / 2 - sub_h / 2, sub_w, sub_h).scaled(sub_w * 2, sub_h * 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.drawImage(0, 0, tmp);
//            p.drawImage(w - sub_w * 2, 0, tmp);
            p.setPen(QPen(Qt::black, LINE_WIDTH, Qt::SolidLine));
            p.drawRect(0, 0, sub_w * 2, sub_h * 2);
//            p.drawRect(w - sub_w * 2, 0, sub_w * 2, sub_h * 2);
            p.setPen(QPen(Qt::black, LINE_WIDTH, Qt::DotLine));
            p.drawRect(w / 2 - sub_w / 2, h / 2 - sub_h / 2, sub_w, sub_h);
        }
    }
    void draw_channel(QPainter &p, QImage & disp_img, const QString & _name)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);

        p.setBrush(TEXT_BRUSH);
        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPainterPath path;
        QFontMetrics fm(TEXT_FONT(TEXT_HEIGHT));
        int pixelsWide = fm.horizontalAdvance(_name);
        uint32_t posX = disp_img.width() - pixelsWide - PADDING_RIGHT;
        path.addText(posX, posY_channel, TEXT_FONT(TEXT_HEIGHT), _name);
        p.drawPath(path);
    }
    void draw_aim(QPainter &p, QImage & disp_img, const int & target_size, const bool rf_enable)
    {

        // draw
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);

        static const int line_border_width{1};
        const int xc{disp_img.width() / 2 - 1};
        const int yc{disp_img.height() / 2 - 1};

        static const int line_width{2};
        if(rf_enable) { // rf enable
            const int border_shift{1};
            const int half_size{static_cast<int>(target_size/2.)};
            const int line_size_deviation{static_cast<int>(ceil(target_size * 0.1)) * 2 + 1};
            // Line
            auto pen{QPen(GREEN_COLOR, line_width, Qt::SolidLine, Qt::SquareCap)};
            p.setPen(pen);
            // top line
            p.drawLine(xc - line_size_deviation, yc + half_size, xc + line_size_deviation, yc + half_size);
            // down line
            p.drawLine(xc - line_size_deviation, yc - half_size, xc + line_size_deviation, yc - half_size);
            // right line
            p.drawLine(xc + half_size, yc - line_size_deviation, xc + half_size, yc + line_size_deviation);
            // left line
            p.drawLine(xc - half_size, yc - line_size_deviation, xc - half_size, yc + line_size_deviation);

            // Border
            pen.setWidth(line_border_width);
            pen.setColor(Qt::black);
            p.setPen(pen);
            static const int left_size_corr{1};
            // top border
            p.drawLine(xc - line_size_deviation - left_size_corr, yc + half_size + border_shift,     xc + line_size_deviation,                  yc + half_size + border_shift);
            p.drawLine(xc - line_size_deviation - left_size_corr, yc + half_size - border_shift - 1, xc + line_size_deviation,                  yc + half_size - border_shift - 1);
            p.drawLine(xc - line_size_deviation - left_size_corr, yc + half_size + border_shift,     xc - line_size_deviation - left_size_corr, yc + half_size - border_shift - 1);
            p.drawLine(xc + line_size_deviation,                  yc + half_size + border_shift,     xc + line_size_deviation,                  yc + half_size - border_shift - 1);
            // down border
            p.drawLine(xc - line_size_deviation - left_size_corr, yc - half_size + border_shift,     xc + line_size_deviation,                  yc - half_size + border_shift);
            p.drawLine(xc - line_size_deviation - left_size_corr, yc - half_size - border_shift - 1, xc + line_size_deviation,                  yc - half_size - border_shift - 1);
            p.drawLine(xc - line_size_deviation - left_size_corr, yc - half_size + border_shift,     xc - line_size_deviation - left_size_corr, yc - half_size - border_shift - 1);
            p.drawLine(xc + line_size_deviation,                  yc - half_size + border_shift,     xc + line_size_deviation,                  yc - half_size - border_shift - 1);
            // right border
            p.drawLine(xc + half_size + border_shift,             yc - line_size_deviation - left_size_corr, xc + half_size + border_shift,      yc + line_size_deviation);
            p.drawLine(xc + half_size - border_shift - 1,         yc - line_size_deviation - left_size_corr, xc + half_size - border_shift - 1,  yc + line_size_deviation);
            p.drawLine(xc + half_size + border_shift,             yc - line_size_deviation - left_size_corr, xc + half_size - border_shift - 1,  yc - line_size_deviation);
            p.drawLine(xc + half_size + border_shift,             yc + line_size_deviation,                  xc + half_size - border_shift - 1,  yc + line_size_deviation);
            // left border
            p.drawLine(xc - half_size + border_shift,             yc - line_size_deviation - left_size_corr, xc - half_size + border_shift,      yc + line_size_deviation);
            p.drawLine(xc - half_size - border_shift - 1,         yc - line_size_deviation - left_size_corr, xc - half_size - border_shift - 1,  yc + line_size_deviation);
            p.drawLine(xc - half_size + border_shift,             yc - line_size_deviation - left_size_corr, xc - half_size - border_shift - 1,  yc - line_size_deviation);
            p.drawLine(xc - half_size + border_shift,             yc + line_size_deviation,                  xc - half_size - border_shift - 1,  yc + line_size_deviation);

        } else { // rf disable
            // limit line
            static const int limit_line_width{2};
            auto pen{QPen(GREEN_COLOR, limit_line_width, Qt::SolidLine, Qt::SquareCap)};
            p.setPen(pen);
            const int limit_line_size{static_cast<int>(ceil(target_size * 0.15)) * 2 + 1};
            p.drawLine(xc - target_size - 1, yc + limit_line_size, xc - target_size - 1, yc - limit_line_size);
            p.drawLine(xc + target_size + 2, yc + limit_line_size, xc + target_size + 2, yc - limit_line_size);
            p.drawLine(xc + limit_line_size, yc - target_size - 1, xc - limit_line_size, yc - target_size - 1);
            p.drawLine(xc + limit_line_size, yc + target_size + 2, xc - limit_line_size, yc + target_size + 2);
            // Line
            const int delta{static_cast<int>(0.6 * target_size)};
            pen.setWidth(line_width);
            p.setPen(pen);
            // hor line
            p.drawLine(xc - target_size, yc, xc - delta,     yc);
            p.drawLine(xc + delta,       yc, xc + target_size, yc);
            // vert line
            p.drawLine(xc, yc - target_size, xc, yc - delta);
            p.drawLine(xc, yc + delta,       xc, yc + target_size);
            const int border_shift{1};
            // Border
            pen.setWidth(line_border_width);
            pen.setColor(Qt::black);
            p.setPen(pen);
            p.drawLine(xc - target_size, yc + border_shift,     xc - delta, yc + border_shift);
            p.drawLine(xc - target_size, yc - border_shift - 1, xc - delta, yc - border_shift - 1);
            p.drawLine(xc - delta,       yc + border_shift,     xc - delta, yc - border_shift - 1);
            p.drawLine(xc - target_size - limit_line_width - 1, yc + limit_line_size, xc - target_size - limit_line_width - 1, yc - limit_line_size - 1);
            p.drawLine(xc - target_size, yc + limit_line_size,     xc - target_size, yc + 2);
            p.drawLine(xc - target_size, yc - limit_line_size - 1, xc - target_size, yc - 2);
            p.drawLine(xc - target_size - limit_line_width - 1, yc + limit_line_size,     xc - target_size - 1, yc + limit_line_size);
            p.drawLine(xc - target_size - limit_line_width - 1, yc - limit_line_size - 1, xc - target_size - 1, yc - limit_line_size - 1);


            p.drawLine(xc + delta,                              yc + border_shift,     xc + target_size, yc + border_shift);
            p.drawLine(xc + delta,                              yc - border_shift - 1, xc + target_size, yc - border_shift - 1);
            p.drawLine(xc + delta-1,                            yc + border_shift,     xc + delta-1,       yc - border_shift - 1);
            p.drawLine(xc + target_size + limit_line_width + 1, yc + limit_line_size,  xc + target_size + limit_line_width + 1, yc - limit_line_size - 1);
            p.drawLine(xc + target_size,                        yc + limit_line_size,     xc + target_size, yc + 2);
            p.drawLine(xc + target_size,                        yc - limit_line_size - 1, xc + target_size, yc - 2);
            p.drawLine(xc + target_size + limit_line_width + 1, yc + limit_line_size,     xc + target_size + 1, yc + limit_line_size);
            p.drawLine(xc + target_size + limit_line_width + 1, yc - limit_line_size - 1, xc + target_size + 1, yc - limit_line_size - 1);

            p.drawLine(xc + border_shift,        yc - target_size,                        xc + border_shift,        yc - delta);
            p.drawLine(xc - border_shift - 1,    yc - target_size,                        xc - border_shift - 1,    yc - delta);
            p.drawLine(xc + border_shift,        yc - delta,                              xc - border_shift - 1,    yc - delta);
            p.drawLine(xc + limit_line_size,     yc - target_size - limit_line_width - 1, xc - limit_line_size - 1, yc - target_size - limit_line_width - 1);
            p.drawLine(xc + limit_line_size,     yc - target_size,                        xc + 2,                   yc - target_size);
            p.drawLine(xc - limit_line_size - 1, yc - target_size,                        xc - 2,                   yc - target_size);
            p.drawLine(xc + limit_line_size,     yc - target_size - limit_line_width - 1, xc + limit_line_size,     yc - target_size - limit_line_width - 1 + limit_line_width);
            p.drawLine(xc - limit_line_size - 1, yc - target_size - limit_line_width - 1, xc - limit_line_size - 1, yc - target_size - limit_line_width - 1 + limit_line_width);

            p.drawLine(xc + border_shift,        yc + delta,                              xc + border_shift,        yc + target_size);
            p.drawLine(xc - border_shift - 1,    yc + delta,                              xc - border_shift - 1,    yc + target_size);
            p.drawLine(xc + border_shift,        yc + delta - 1,                          xc - border_shift - 1,    yc + delta - 1);
            p.drawLine(xc + limit_line_size,     yc + target_size + limit_line_width + 1, xc - limit_line_size - 1, yc + target_size + limit_line_width + 1);
            p.drawLine(xc + limit_line_size,     yc + target_size,                        xc + 2,                   yc + target_size);
            p.drawLine(xc - limit_line_size - 1, yc + target_size,                        xc - 2,                   yc + target_size);
            p.drawLine(xc + limit_line_size,     yc + target_size + limit_line_width + 1, xc + limit_line_size,     yc + target_size + limit_line_width + 1 - limit_line_width);
            p.drawLine(xc - limit_line_size - 1, yc + target_size + limit_line_width + 1, xc - limit_line_size - 1, yc + target_size + limit_line_width + 1 - limit_line_width);
        }
        // point
        auto pen{QPen(Qt::black, 4, Qt::SolidLine, Qt::SquareCap)};
        p.setPen(pen);
        p.drawLine(xc, yc, xc, yc);
        pen.setWidth(2);
        pen.setColor(GREEN_COLOR);
        p.setPen(pen);
        p.drawLine(xc, yc, xc, yc);
    }

    void draw_alignment_cross(QPainter &p, QImage &disp_img)
    {
        // draw
        p.setCompositionMode(QPainter::CompositionMode_Difference);
        QPen pen(QColor(128, 128, 128, 191), 2, Qt::SolidLine);
        p.setPen(pen);

        const int xc{disp_img.width() / 2 - 1};
        const int yc{disp_img.height() / 2 - 1};

        p.drawLine(0, yc, disp_img.width(), yc);
        p.drawLine(xc, 0, xc, disp_img.height());
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }


    void draw_gsp(QPainter & p, QImage & disp_img, const double & gsp_x_angle, const double & gsp_z_angle, const bool zAngleRange180En)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);

        int w_top = img_pointerTop.width();
        int h_top = img_pointerTop.height();
        int w_profile = img_pointerProfile.width();
        int h_profile = img_pointerProfile.height();
        int W = disp_img.width();
        int H = disp_img.height();

        /** Z COORDINATE */

        // recounting
        double gsp_z_angle_360 = gsp_z_angle;
        if(gsp_z_angle < 0.) {
            gsp_z_angle_360 += 360.;
            if (gsp_z_angle_360 > 360.) {
                gsp_z_angle_360 -= 360.;
            }
            //gsp_z_angle_360 = -((static_cast<int>(-gsp_z_angle_360) % 360) + (-(gsp_z_angle_360-static_cast<int>(gsp_z_angle_360))));
        } else if(gsp_z_angle_360 > 360.) {
            gsp_z_angle_360 = (static_cast<int>(gsp_z_angle_360) % 360) + (gsp_z_angle_360-static_cast<int>(gsp_z_angle_360));
        }

        if(zAngleRange180En == true) {
            if(gsp_z_angle_360 > 180.) {
                gsp_z_angle_360 = -(360. - gsp_z_angle_360);
            }
        }

        int32_t maxSize = (w_top > h_top) ? w_top : h_top;
        maxSize = (maxSize > w_profile) ? maxSize : w_profile;
        maxSize = (maxSize > h_profile) ? maxSize : h_profile;
        int32_t ellipseWidth = maxSize * 1.1;

        uint32_t ZposCenterX = disp_img.width() - ellipseWidth/2 - PADDING_RIGHT;
        uint32_t ZposCenterY = posY_pointer_center;
        p.setPen(Qt::NoPen);

        p.setBrush(QBrush(QColor(220, 220, 220, 100), Qt::SolidPattern));
        p.drawEllipse((ZposCenterX - ellipseWidth/2), (ZposCenterY - ellipseWidth/2 - 4), ellipseWidth, ellipseWidth);
        p.setBrush(Qt::NoBrush);
        p.drawImage((ZposCenterX - w_top/2), (ZposCenterY - h_top/2), img_pointerTop);
        // zero line
        p.setPen(QPen(GREEN_COLOR, LINE_WIDTH + 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(ZposCenterX, ZposCenterY,
                   ZposCenterX, ZposCenterY - ellipseWidth/2);
        // current z angle line
        p.setPen(QPen(Qt::red, 4, Qt::SolidLine));
        QLineF z_line;
        z_line.setP1(QPointF(ZposCenterX, ZposCenterY));
        z_line.setAngle(-(gsp_z_angle-90));
        z_line.setLength(maxSize/2);
        p.drawLine(z_line);

        /** X COORDINATE */
        uint32_t XposCenterX = ZposCenterX - w_profile - 50; // область с запасом на глаз
        uint32_t XposCenterY = posY_pointer_center;
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(QColor(220, 220, 220, 100), Qt::SolidPattern));
        p.drawEllipse((XposCenterX - ellipseWidth/2), (XposCenterY - ellipseWidth/2), ellipseWidth, ellipseWidth);
        p.setBrush(Qt::NoBrush);
        // ship
        p.drawImage((XposCenterX - w_profile/2), (XposCenterY - h_profile/2), img_pointerProfile);
        // zero line
        p.setPen(QPen(GREEN_COLOR, LINE_WIDTH + 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(XposCenterX, XposCenterY,
                   XposCenterX + ellipseWidth/2, XposCenterY);
        // current x angle line
        p.setPen(QPen(Qt::red, 4, Qt::SolidLine));
        QLineF x_line;
        x_line.setP1(QPointF(XposCenterX, XposCenterY));
        x_line.setAngle(gsp_x_angle);
        x_line.setLength(maxSize/2);
        p.drawLine(x_line);




        p.setBrush(TEXT_BRUSH);
        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine));
        QString x_angle, z_angle;
        x_angle.setNum(gsp_x_angle,     'f', 1);
        z_angle.setNum(gsp_z_angle_360, 'f', 1);
        QPainterPath path;
        QFontMetrics fm(TEXT_FONT(TEXT_HEIGHT));

        QString strX = QString("X=%1").arg(x_angle);
//        uint32_t X_textPosY = XposCenterY + w_ship + TEXT_HEIGHT;
        uint32_t X_textPosY = XposCenterY - h_profile - TEXT_HEIGHT*3;
        uint32_t X_textPosX = disp_img.width() - posX_pointerText;
        path.addText(X_textPosX,
                     X_textPosY,
                     TEXT_FONT(TEXT_HEIGHT), strX);
//        path.addText(XposCenterX - w_ship/2 - pixelsWide - 15,
//                     XposCenterY + TEXT_HEIGHT/2,
//                     TEXT_FONT(TEXT_HEIGHT), x_angle);

        QString strZ = QString("Z=%1").arg(z_angle);
        path.addText(X_textPosX,
                     X_textPosY + TEXT_HEIGHT*2,
                     TEXT_FONT(TEXT_HEIGHT), strZ);
//        path.addText((ZposCenterX - pixelsWide2/2),
//                     ZposCenterY + TEXT_HEIGHT*3,
//                     TEXT_FONT(TEXT_HEIGHT), z_angle);
        p.drawPath(path);

    }
    void draw_zoom(QPainter & p, QImage & disp_img, const double & focus_mm, const double & min_focus_mm, const double & max_focus_mm, const double & digital_zoom)
    {
        // p.setBrush(Qt::NoBrush);
        // p.setPen(Qt::NoPen);
//        p.setPen(QPen(GREEN_COLOR, LINE_WIDTH, Qt::SolidLine));
//        int w = disp_img.width() / 4;
//        int h = TEXT_HEIGHT / 2;
//        int x = (disp_img.width() - w) / 2;
//        int y = disp_img.height() - (h + PADDING_BOT);
//        int w_step = w / 50;
//        int W = disp_img.width();
//        int H = disp_img.height();

//        p.fillRect((W - w) / 2 - PADDING_LEFT, H - (PADDING_TOP + h + PADDING_BOT), PADDING_LEFT + w + 80 + PADDING_RIGHT, PADDING_TOP + h + PADDING_BOT, QBrush(QColor(128, 128, 128, 200)));

//        for(int wi = w_step; wi <= w; wi+=w_step)
//        {
//            p.drawRect(x, y, wi, h);
//        }
//        p.fillRect((W - w) / 2, H - (h + PADDING_BOT), int(round(w * double(focus_mm - min_focus_mm) / double(max_focus_mm - min_focus_mm))), h, GREEN_COLOR);

//        x += w;
//        for(int wi = 10; wi <= 80; wi*=2)
//        {
//            p.drawRect(x, y, wi, h);
//        }
//        p.fillRect((W - w) / 2 + w, H - (h + PADDING_BOT), int(round(digital_zoom * 10)), h, GREEN_COLOR);

    }
    void draw_rangefinder(QPainter & p, QImage & disp_img, const bool & rangefinder_en, const bool & rangefinder_5Hz, const bool & rangefinder_on, const double & rangefinder_range)
    {
        // p.setBrush(Qt::NoBrush);
        // p.setPen(Qt::NoPen);
//        p.setBrush(TEXT_BRUSH);
//        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

//        const double rangefinder_range_printed{rangefinder_range / rf_dist_divisor};

//        QString s;
//        if(rangefinder_en)
//        {
//            if(rangefinder_5Hz)
//            {
//                s.sprintf("ЛД 5Гц[%s]: %.3f m", rangefinder_on ? "Вкл" : "Выкл", rangefinder_range);
//            }
//            else
//            {
//                s.sprintf("ЛД: %.3f m", rangefinder_range);
//            }
//        }
//        else
//        {
//            s.sprintf("ЛД: БЛОК");
//        }
//        QPainterPath path;
//        path.addText(PADDING_LEFT, (PADDING_TOP + TEXT_HEIGHT + PADDING_BOT) * 1 + (TEXT_HEIGHT + PADDING_TOP), TEXT_FONT(TEXT_HEIGHT), s);
//        p.drawPath(path);

    }


    void draw_highlighter(QPainter &p, QImage & disp_img, const bool & highlighter_power_on, const bool & highlighter_rdy, const bool & highlighter_pause,
                          const bool & highlighter_cont_emit, const bool & highlighter_single,
                          const double & rangefinder_range)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);

        QBrush currentTextBrush;
        currentTextBrush = TEXT_BRUSH_GREEN;
        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        const double rangefinder_range_printed{rangefinder_range / rf_dist_divisor};
        bool is_valid_range;
        if((rangefinder_range > 50.) && (rangefinder_range < 30000.)) {
            is_valid_range = true;
        } else {
            is_valid_range = false;
        }
        QString s;
        if(highlighter_power_on)
        {
            if (highlighter_rdy)
            {
                if(highlighter_cont_emit)
                {
                    // КРАСНЫЙ
                    if(is_valid_range) {
                        s.sprintf(str_radiation, rangefinder_range_printed);
                    } else {
                        s.sprintf(str_radiation_faill);
                    }
                    currentTextBrush = TEXT_BRUSH_RED;
                    uint32_t posX = disp_img.width() - img_laser_radiation.width() - PADDING_RIGHT;
                    p.drawImage(posX, posY_laser_img, img_laser_radiation);
                }
                else
                {

                    if (highlighter_single)
                    {
                        // ЗЕЛЕНЫЙ
                        if(is_valid_range) {
                            s.sprintf(str_single, rangefinder_range_printed);
                        } else {
                            s.sprintf(str_single_faill);
                        }
                        currentTextBrush = TEXT_BRUSH_GREEN;
                        uint32_t posX = disp_img.width() - img_laser_on.width() - PADDING_RIGHT;
                        p.drawImage(posX, posY_laser_img, img_laser_on);
                    }
                    else
                    {
                        // ЗЕЛЕНЫЙ
                        if(is_valid_range) {
                            s.sprintf(str_point, rangefinder_range_printed);
                        } else {
                            s.sprintf(str_point_faill);
                        }
                        currentTextBrush = TEXT_BRUSH_GREEN;
                        uint32_t posX = disp_img.width() - img_laser_on.width() - PADDING_RIGHT;
                        p.drawImage(posX, posY_laser_img, img_laser_on);
                    }
                }
            }
            else
            {
                // ЖЕЛТЫЙ
                currentTextBrush = TEXT_BRUSH_YELLOW;
                s.sprintf(str_power_on);
                uint32_t posX = disp_img.width() - img_laser_waiting.width() - PADDING_RIGHT;
                p.drawImage(posX, posY_laser_img, img_laser_waiting);
            }
        }
        else if (highlighter_pause)
        {
            // СИНИЙ
            currentTextBrush = TEXT_BRUSH_YELLOW;
            if(is_valid_range) {
                s.sprintf(str_cooling, rangefinder_range_printed);
            } else {
                s.sprintf(str_cooling_faill);
            }
            uint32_t posX = disp_img.width() - img_laser_cooling.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_laser_img, img_laser_cooling);
        }
        else
        {
            // СЕРЫЙ
            uint32_t posX = disp_img.width() - img_laser_off.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_laser_img, img_laser_off);
            s.sprintf("");
//            p.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
//            s.sprintf(str_power_off);
        }

        /** TEXT */
        QPainterPath path;
        QFontMetrics fm(TEXT_FONT(TEXT_HEIGHT));
        int pixelsWide = fm.horizontalAdvance(s);
        uint32_t posX = disp_img.width()/2 - pixelsWide/2;
        if(!s.isEmpty()) {
            draw_background(&p, (posX - PADDING_TOP*4), TEXT_HEIGHT/2, (pixelsWide + PADDING_TOP*8), (TEXT_HEIGHT + PADDING_TOP*4));
        }
        p.setBrush(currentTextBrush);
        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        path.addText(posX, PADDING_TOP + TEXT_HEIGHT*2, TEXT_FONT(TEXT_HEIGHT), s);
        p.drawPath(path);

    }
    void draw_target(QPainter & p, QImage & disp_img, const int & target_id, const int & target_fast, const bool & target_en, const int & target_size)
    {
        if(target_en)
        {
            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);

            p.setPen(QPen(Qt::yellow, LINE_WIDTH, Qt::SolidLine));

            int x0 = disp_img.width()  / 2 - target_size / 2 - 1;
            int y0 = disp_img.height() / 2 - target_size / 2 - 1;
            p.drawRect(x0, y0, target_size, target_size);

            p.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
            p.setPen(QPen(Qt::black, LINE_WIDTH/4, Qt::SolidLine));
            QPainterPath path;
            QString s = QString::number(target_id);
            if(target_fast) s += "F";
            path.addText(x0, y0 - (TEXT_HEIGHT/4), TEXT_FONT(int(TEXT_HEIGHT/1.5)), s);
            p.drawPath(path);
        }
    }
    void draw_target_v2(QPainter & p, QImage &disp_img, const int &target_id, const int &xPos, const int &yPos, const int &width, const int &height)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);

        p.setPen(QPen(Qt::yellow, LINE_WIDTH, Qt::SolidLine));
        p.drawRect(xPos, yPos, width, height);

        p.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
        p.setPen(QPen(Qt::black, LINE_WIDTH/4, Qt::SolidLine));
        QPainterPath path;
        QString s = QString::number(target_id);
        path.addText(xPos, yPos - (TEXT_HEIGHT/4), TEXT_FONT(int(TEXT_HEIGHT/1.5)), s);
        p.drawPath(path);
    }
    void draw_object(QPainter &p, QImage & disp_img,
                     const int & object_id,
                     const bool & object_valid, const bool & object_trusted,
                     const int & object_x, const int & object_y, const int & object_w, const int & object_h,
                     const double & digital_zoom,
                     const int & _w, const int & _h, const bool & x_inv, const bool & y_inv)
    {
        if(object_valid)
        {

            // if (object_w > 0 && object_h > 0) {
            //     qDebug() << "---- object:" << object_x << object_y << object_w << object_h;
            // }

            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);

            int W = disp_img.width();
            int H = disp_img.height();
            p.setPen(QPen(Qt::GlobalColor(Qt::green+object_id), object_trusted ? LINE_WIDTH : LINE_WIDTH/2, Qt::SolidLine));
            int x = int(round(object_x * digital_zoom + (W - _w * digital_zoom) / 2));
            int y = int(round(object_y * digital_zoom + (H - _h * digital_zoom) / 2));
            int w = int(round(object_w * digital_zoom));
            int h = int(round(object_h * digital_zoom));
            if(x_inv) x = W - x - w;
            if(y_inv) y = H - y - h;
            p.drawRect(x, y, w, h);

            p.setBrush(QBrush(Qt::GlobalColor(Qt::green+object_id), Qt::SolidPattern));
            p.setPen(QPen(Qt::black, LINE_WIDTH/4, Qt::SolidLine));
            QPainterPath path;
            path.addText(x, y - (TEXT_HEIGHT/4), TEXT_FONT(int(TEXT_HEIGHT/1.5)), QString::number(object_id));
            p.drawPath(path);
        }
    }


    void draw_object_v2(QPainter &p, const int &object_id, const bool &object_valid, const bool &object_trusted, const int &object_x, const int &object_y, const int &object_w, const int &object_h, const QTransform &frm_to_disp_tr)
    {
        if (!object_valid)
            return;

        if (object_w == 0 || object_h == 0)
            return;

        QPoint tl = {object_x, object_y};
        QPoint br = {object_x + object_w, object_y + object_h};

        tl = frm_to_disp_tr.map(tl);
        br = frm_to_disp_tr.map(br);

        QRect rect {tl, br};

        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);
        p.setPen(QPen(Qt::GlobalColor(Qt::green+object_id), object_trusted ? LINE_WIDTH : LINE_WIDTH/2, Qt::SolidLine));
        p.drawRect(rect);

        p.setBrush(QBrush(Qt::GlobalColor(Qt::green+object_id), Qt::SolidPattern));
        p.setPen(QPen(Qt::black, LINE_WIDTH/4, Qt::SolidLine));
        QPainterPath path;
        path.addText(rect.x(), rect.y() - (TEXT_HEIGHT/4), TEXT_FONT(int(TEXT_HEIGHT/1.5)), QString::number(object_id));
        p.drawPath(path);


    }

    void draw_agc(QPainter &p, QImage & disp_img, const bool & agc_on)
    {
        if(agc_on)
        {
            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);
            uint32_t posX = disp_img.width() - agc.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_agc, agc);
        }
    }
    void draw_inversion(QPainter &p, QImage & disp_img, const bool & inversion_on)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);
        if(inversion_on)
        {
            uint32_t posX = disp_img.width() - img_inversion_on.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_inversion, img_inversion_on);
        } else {
            uint32_t posX = disp_img.width() - img_inversion_off.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_inversion, img_inversion_off);
        }
    }
    void draw_bc(QPainter &p, QImage & disp_img, const int & brightness, const double & contrast)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);

        int H = disp_img.height();
        p.setBrush(TEXT_BRUSH);
        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        QPainterPath path;
        QFontMetrics fm(TEXT_FONT(TEXT_HEIGHT));
        int pixelsWide;
        uint32_t posX, posY;

        /** BRIGHTNESS */
        QString s_b;
        s_b.sprintf("%i", brightness);
        uint32_t posX_brightness = disp_img.width() - _brightness.width() - PADDING_RIGHT;
        p.drawImage(posX_brightness, posY_brightness, _brightness);
        pixelsWide = fm.horizontalAdvance(s_b);
        posX = posX_brightness - pixelsWide - 10;
        posY = posY_brightness + _brightness.height()/2 + TEXT_HEIGHT/2;
        path.addText(posX, posY, TEXT_FONT(TEXT_HEIGHT), s_b);

        /** CONTRAST */
        QString s_c;
        s_c.sprintf("%.2f", contrast);
        uint32_t posX_contrast = disp_img.width() - _contrast.width() - PADDING_RIGHT;
        p.drawImage(posX_contrast, posY_contrast, _contrast);
        pixelsWide = fm.horizontalAdvance(s_c);
        posX = posX_contrast - pixelsWide - 10;
        posY = posY_contrast + _contrast.height()/2 + TEXT_HEIGHT/2;
        path.addText(posX, posY, TEXT_FONT(TEXT_HEIGHT), s_c);
        p.drawPath(path);
    }
    void draw_autofocus(QPainter &p, QImage & disp_img, const bool & autofocus_on)
    {
        if(autofocus_on)
        {
            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);
            uint32_t posX = disp_img.width() - autofocus.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_autofocus, autofocus);
        }
    }
    void draw_field_sync_reg(QPainter &p, QImage & disp_img, const bool & field_sync_reg_on)
    {
        if(field_sync_reg_on)
        {
            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);
            uint32_t posX = disp_img.width() - field_sync_reg.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_field_sync_reg, field_sync_reg);
        }
    }
    void draw_gsp_state(QPainter &p, QImage & disp_img,
                        const bool & off,
                        const bool & mech_arretir,
                        const bool & el_arretir,
                        const bool & stabilization)
    {
        // p.setBrush(Qt::NoBrush);
        // p.setPen(Qt::NoPen);
//        if(off)
//        {
//            p.drawImage(PADDING_LEFT, disp_img.height() - (PADDING_TOP + TEXT_HEIGHT + PADDING_BOT) * 7, gsp_none);
//        }
//        else if(mech_arretir)
//        {
//            p.drawImage(PADDING_LEFT, disp_img.height() - (PADDING_TOP + TEXT_HEIGHT + PADDING_BOT) * 7, gsp_mech_arretir_xy);
//        }
//        else if(el_arretir)
//        {
//            p.drawImage(PADDING_LEFT, disp_img.height() - (PADDING_TOP + TEXT_HEIGHT + PADDING_BOT) * 7, gsp_el_arretir_xy);
//        }
//        else if(stabilization)
//        {
//            p.drawImage(PADDING_LEFT, disp_img.height() - (PADDING_TOP + TEXT_HEIGHT + PADDING_BOT) * 7, gsp_stabilization_xy);
//        }
//        else
//        {
//            p.drawImage(PADDING_LEFT, disp_img.height() - (PADDING_TOP + TEXT_HEIGHT + PADDING_BOT) * 7, gsp_el_arretir_xy);
//        }
    }
    void draw_field_angle(QPainter &p, QImage & disp_img, const double & field_angle)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);
        QString s;
        s.sprintf("%.2f", field_angle);
        s = QString("%1%2").arg(s).arg(QString::fromUtf8("\xC2\xB0"));

        p.setBrush(TEXT_BRUSH);
        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPainterPath path;
        QFontMetrics fm(TEXT_FONT(TEXT_HEIGHT));
        int pixelsWide = fm.horizontalAdvance(s);
        uint32_t posX = disp_img.width() - pixelsWide - PADDING_RIGHT;
        path.addText(posX, posY_channelField, TEXT_FONT(TEXT_HEIGHT), s);
        p.drawPath(path);
    }
    void draw_datetime(QPainter &p, QImage & disp_img)
    {
        // p.setBrush(Qt::NoBrush);
        // p.setPen(Qt::NoPen);
//        int W = disp_img.width();
//        QString s = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

//        p.setBrush(TEXT_BRUSH);
//        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
//        QPainterPath path;
//        path.addText(W / 2 - 290, TEXT_HEIGHT + PADDING_TOP, TEXT_FONT(TEXT_HEIGHT), s);
//        p.drawPath(path);
    }
    void draw_swir_gain(QPainter &p, QImage & disp_img, const int & mode)
    {
        // p.setBrush(Qt::NoBrush);
        // p.setPen(Qt::NoPen);
//        int H = disp_img.height();
//        QString s;
//        switch(mode)
//        {
//        case 0:
//            s = "Авто";
//            break;
//        case 1:
//            s = "День";
//            break;
//        case 2:
//            s = "Ночь";
//            break;
//        }

//        p.setBrush(TEXT_BRUSH);
//        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
//        QPainterPath path;
//        path.addText(PADDING_LEFT + 200, H - PADDING_BOT, TEXT_FONT(TEXT_HEIGHT), s);
//        p.drawPath(path);
    }
    void draw_video_record(QPainter &p, QImage & disp_img, const bool & video_record)
    {
        if(video_record)
        {
            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);
            uint32_t posX = disp_img.width() - record_img.width() - PADDING_RIGHT;
            p.drawImage(posX, posY_record, record_img);
        }
    }

    void draw_laser_radiation(QPainter &p, QImage &disp_img, const bool en)
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(Qt::NoPen);
        static uint8_t cnt = 0;
        if (en)
        {
            if (cnt < 25)
            {
                cnt++;
                p.drawImage(PADDING_LEFT, disp_img.height() - (PADDING_TOP + TEXT_HEIGHT + PADDING_BOT) * 10, laser_radiation);
            }
            else
            {
                cnt++;
                if (cnt == 50)
                    cnt = 0;
            }
        }
    }

    void draw_lrf_dangerZone(QPainter &p, QImage &disp_img)
    {
        p.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        int dangerZoneWidth = dangerZoneFontMetrics.horizontalAdvance(lrf_dangerZone);
        int32_t pos_x = (disp_img.width() / 2) - (dangerZoneWidth / 2);
        pos_x = (pos_x >= 0) ? pos_x : 0;

        uint32_t pos_y = PADDING_TOP + dangerZoneTextHeight + TEXT_HEIGHT*2;

        QPainterPath path;
        path.addText(pos_x, pos_y, dangerZoneFont, lrf_dangerZone);
        p.drawPath(path);
    }

    void draw_common_field(QPainter &p, QImage &disp_img)
    {
        p.setBrush(QBrush(QColor(0, 0, 0, 50), Qt::SolidPattern));
        p.setPen(QPen(QColor(0, 0, 0, 50), LINE_WIDTH/4, Qt::SolidLine));
//        p.setPen(QPen(Qt::black, LINE_WIDTH/4, Qt::SolidLine));
//        p.setBrush(QBrush(QColor(128, 128, 128, 150), Qt::SolidPattern));
        uint32_t fieldWidth = 300;
        uint32_t fieldHeight = disp_img.height();
        uint32_t posX = disp_img.width() - fieldWidth;
        uint32_t posY = 0;
        p.drawRect(posX, posY, fieldWidth, fieldHeight);
    }

    void draw_mw_loading_image(QPainter &p, QImage &disp_img, const bool & en)
    {
        if(en)
        {
            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);

            p.setBrush(QBrush(Qt::black, Qt::SolidPattern));
            p.setPen(QPen(Qt::black, LINE_WIDTH / 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawRect(0, 0, disp_img.width(), disp_img.height());
            uint32_t posX = disp_img.width()/2 - img_loading_mw.width()/2;
            uint32_t posY = disp_img.height()/2 - img_loading_mw.height()/2;
            p.drawImage(posX, posY, img_loading_mw);
        }
    }

    void draw_background(QPainter *p, const uint32_t posX, const uint32_t posY, const uint32_t width, const uint32_t height)
    {
        p->setBrush(QBrush(QColor(0, 0, 0, 50), Qt::SolidPattern));
        p->setPen(QPen(QColor(0, 0, 0, 50), LINE_WIDTH/4, Qt::SolidLine));
        p->drawRect(posX, posY, width, height);
    }

    void draw_radarCircle(QPainter &painter, QImage &disp_img, const double &angleZ, const double &currField)
    {
        QImage buffImg{disp_img};
        QPainter p;
        p.begin(&buffImg);

        const uint32_t m_width { 120 };
        QColor m_sectorColor { QColor(0, 255, 0, 100) };

        /** Background */
        p.setPen(Qt::black);
        QColor      bgColor { QColor(255, 255, 255, 100) };
        QBrush      bgBrush { QBrush(bgColor, Qt::SolidPattern) };
        p.setBrush(bgBrush);
        int32_t currPosX = -static_cast<int32_t>(m_width)/2;
        int32_t currPosY = -static_cast<int32_t>(m_width)/2;
        p.drawEllipse(currPosX, currPosY, m_width, m_width);

        int32_t currWidth = m_width/3;
        currPosX = -currWidth/2;
        currPosY = -currWidth/2;
        p.drawEllipse(currPosX, currPosY, currWidth, currWidth);

        currWidth = m_width/3*2;
        currPosX = -currWidth/2;
        currPosY = -currWidth/2;
        p.drawEllipse(currPosX, currPosY, currWidth, currWidth);

        p.drawLine(0, -(static_cast<int32_t>(m_width)/2), 0, static_cast<int32_t>(m_width)/2);
        p.drawLine(-(static_cast<int32_t>(m_width)/2), 0, static_cast<int32_t>(m_width)/2, 0);

        /** Field */
        //    p.setPen(Qt::black);
        p.setBrush(QBrush(m_sectorColor, Qt::SolidPattern));
        currPosX = -static_cast<int32_t>(m_width)/2;
        currPosY = -static_cast<int32_t>(m_width)/2;
        p.rotate(-90.0);
        int startAngle = static_cast<int>(-(angleZ - currField/2.0));
        p.drawPie(currPosX, currPosY, m_width, m_width, startAngle*16, -currField*16);
        p.rotate(90.0);

        p.setPen(QColor(255, 215, 0, 255));
        p.rotate(angleZ-180);
        p.drawLine(0, 0, 0, m_width/2);
        p.rotate(-angleZ-180);


        p.end();

        painter.drawImage(0, 0, buffImg);
    }

    void draw_setShipPointer()
    {
        img_pointerTop = img_shipTop;
        img_pointerProfile = img_shipProfile;
    }
    void draw_setHelicopterPointer()
    {
        img_pointerTop = img_helicopterTop;
        img_pointerProfile = img_helicopterProfile;
    }
}
