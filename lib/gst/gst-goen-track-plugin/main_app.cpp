#include "main_app.h"
#include <opencv2/imgcodecs.hpp>
#include <thread>

#define USE_LOGGER 1

Main_app::Main_app(bool &ok, const std::string &pathToConfig) {
    this->pathToConfig=pathToConfig;

}



void Main_app::create_tracker(const cv::Mat &f)
{
    if (m_t != nullptr)
        return;

    float cw = 1.0f / f.cols;
    float ch = 1.0f / f.rows;
    //Test rect
    m_t_ctl.obj_xy_x = (300) * cw;
    m_t_ctl.obj_xy_y = (300) * ch;
    m_t_ctl.obj_wh_2_w = (200 / 2) * cw;
    m_t_ctl.obj_wh_2_h = (200 / 2) * ch;
    m_t_ctl.key = 0;
    m_t_ctl.zahvat = 1;
    m_t_ctl.rect_ok = 2;
    //Test rect end


    m_t_ctl.trac_id = 255;
    m_t_ctl.fr_w0 = f.cols;
    m_t_ctl.fr_h0 = f.rows;
    m_t_ctl.img_orig_type = f.type();
    m_t_ctl.new_frame = 1;

    bool ok = false;
    m_t = std::shared_ptr<trac_tmml>(create_track("tracking.ini", ok, m_t_ctl));
    if(!ok) {
        std::cout << "Could not create tmml trac" << std::endl;
        return;
    }
}


#define _OUT_INT(x) " - " #x << " " << (int)(x) << std::endl
#define _OUT(x) " - " #x << " " << (x) << std::endl


static void dumpTracStr(const trac_struct & t)
{
    std::cout << " ***** " << std::endl
              // << _OUT_INT(t.key)
              // << _OUT_INT(t.tot_show_img_w)
              // << _OUT_INT(t.tot_show_img_h)
              << _OUT_INT(t.zahvat)
              << _OUT_INT(t.rect_ok)
              << _OUT_INT(t.work_number)
              << _OUT_INT(t.ok_match)
              << _OUT_INT(t.validate)
              << _OUT_INT(t.validate_min)
              << std::endl
              << _OUT(t.obj_xy_x)
              << _OUT(t.obj_xy_y)
              << _OUT(t.obj_wh_2_w)
              << _OUT(t.obj_wh_2_h)
              << std::endl
              << _OUT_INT(t.roi)
              << _OUT_INT(t.roi_l)
              << _OUT_INT(t.roi_t)
              << _OUT_INT(t.roi_w)
              << _OUT_INT(t.roi_h)
              // << std::endl
              // << _OUT_INT(t.img_orig_type)
              // << _OUT_INT(t.fr_w0)
              // << _OUT_INT(t.fr_h0)
              << _OUT_INT(t.id)
              // << _OUT_INT(t.new_frame)
              // << _OUT(t.scan_exec_time)
              // << _OUT(t.yolo_exec_time)
              // << _OUT_INT(t.work_in_round)
              // << _OUT_INT(t.vtrac_data.size())

              << std::endl;
}

     void Main_app::exec(cv::Mat frame_recieve )
    {
         auto f= frame_recieve;
        if (f.empty())
            return;

        if (m_fcnt == 0)
            create_tracker(f);

        m_fcnt++;

        dumpTracStr(m_t_ctl);


        if (m_t_ctl.zahvat) {
            m_t_ctl.work_number = m_fcnt;
            m_t_ctl.img_orig_data = f.data;

            m_t_ctl.roi_l = std::clamp(
                1,    (int)round((m_t_ctl.obj_xy_x - m_t_ctl.obj_wh_2_w * m_ext_wh) *
                       f.cols),
                f.cols - 4);

            m_t_ctl.roi_t = std::clamp(
                1,
               (int) round((m_t_ctl.obj_xy_y - m_t_ctl.obj_wh_2_h * m_ext_wh) *
                       f.rows),
                f.rows - 4);

            m_t_ctl.roi_w =
                std::clamp(2,
                      (int)round(m_t_ctl.obj_wh_2_w * 2.0f * m_ext_wh * f.cols),
                       f.cols - m_t_ctl.roi_l - 1);

            m_t_ctl.roi_h =
                std::clamp(2, (int)round(m_t_ctl.obj_wh_2_h * 2.0f * m_ext_wh * f.rows),
                       f.rows - m_t_ctl.roi_t - 1);

            auto find = get_trac(m_t);
           // if (find < 0)
               // qWarning() << ".." << find;
        };


    } // -- END exec

