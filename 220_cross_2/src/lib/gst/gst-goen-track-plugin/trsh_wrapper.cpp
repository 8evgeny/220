#include "trsh_wrapper.h"
#include "tracshats/trac_struct.hpp"
#include "tracshats/trac_api.hpp"

#include <iostream>

class TrShWrapperPrivate {

public:

    void createTracker(const cv::Mat &f)
    {
        if (m_t != nullptr)
            return;

        m_t_ctl.trac_id = 255;
        m_t_ctl.fr_w0 = f.cols;
        m_t_ctl.fr_h0 = f.rows;
        m_t_ctl.img_orig_type = f.type();
        m_t_ctl.new_frame = 1;

        bool ok = false;
        m_t = std::shared_ptr<trac_tmml>(create_track(m_iniFileName.c_str(), ok, m_t_ctl));
        if(!ok) {
            std::cerr << "Could not create tmml trac" << std::endl;
            return;
        }
    }

    void setReinitFrame(int offset = 1)
    {
        m_reinit_frm = m_f_cnt + offset;
    }

    void initTracker()
    {
        float cw = 1.0f / m_t_ctl.fr_w0;
        float ch = 1.0f / m_t_ctl.fr_h0;

        m_t_ctl.obj_xy_x = (m_targetObj.cx) * cw;
        m_t_ctl.obj_xy_y = (m_targetObj.cy) * ch;
        m_t_ctl.obj_wh_2_w = (m_targetObj.w / 2.0f) * cw;
        m_t_ctl.obj_wh_2_h = (m_targetObj.h / 2.0f) * ch;
        m_t_ctl.key = 0;
        m_t_ctl.zahvat = 1;
        m_t_ctl.rect_ok = 2;
    }

    bool track(cv::Mat &f)
    {
        m_t_ctl.work_number = m_f_cnt;
        m_t_ctl.img_orig_data = f.data;

        m_t_ctl.roi_l = std::clamp(
            1,
            (int)std::round((m_t_ctl.obj_xy_x - m_t_ctl.obj_wh_2_w * m_ext_wh) *
                             f.cols),
            f.cols - 4);

        m_t_ctl.roi_t = std::clamp(
            1,
            (int)std::round((m_t_ctl.obj_xy_y - m_t_ctl.obj_wh_2_h * m_ext_wh) *
                             f.rows),
            f.rows - 4);

        m_t_ctl.roi_w = std::clamp(
            2, (int)std::round(m_t_ctl.obj_wh_2_w * 2.0f * m_ext_wh * f.cols),
            f.cols - m_t_ctl.roi_l - 1);

        m_t_ctl.roi_h = std::clamp(
            2, (int)std::round(m_t_ctl.obj_wh_2_h * 2.0f * m_ext_wh * f.rows),
            f.rows - m_t_ctl.roi_t - 1);

        try {

            if (get_trac(m_t) >= 0 && m_t_ctl.ok_match) {

                m_objF.cx = m_t_ctl.obj_xy_x;
                m_objF.cy = m_t_ctl.obj_xy_y;
                m_objF.w = m_t_ctl.obj_wh_2_w * 2.0f;
                m_objF.h = m_t_ctl.obj_wh_2_h * 2.0f;

                m_obj.cx = (int)std::round(m_objF.cx * f.cols);
                m_obj.cy = (int)std::round(m_objF.cy * f.rows);
                m_obj.w = (int)std::round(m_objF.w * f.cols);
                m_obj.h = (int)std::round(m_objF.h * f.rows);

                m_roi.w = m_t_ctl.roi_w;
                m_roi.h = m_t_ctl.roi_h;
                m_roi.cx = m_t_ctl.roi_l + m_roi.w / 2;
                m_roi.cy = m_t_ctl.roi_t + m_roi.h / 2;

                return true;
            }

        } catch (const cv::Exception &e) {

            std::cerr << "OpenCV exception: " << e.what() << std::endl;
        }

        return false;
    }

    bool update(cv::Mat &frame)
    {
        if (m_f_cnt == 0)
            createTracker(frame);

        if (m_reinit_frm == m_f_cnt)
            initTracker();

        bool res = false;

        if (m_t_ctl.zahvat)
            res = track(frame);


        m_f_cnt++;
        return res;
    }


public:
    std::string m_iniFileName;

    trac_struct m_t_ctl;
    float m_ext_wh = 8; //2.f * m_ext_wh_2;
    std::shared_ptr<trac_tmml> m_t = nullptr;

    long long m_f_cnt = 0;
    long long m_reinit_frm = -1;

    TrShWrapper::Rect m_targetObj;

    TrShWrapper::Rect m_obj;
    TrShWrapper::RectF m_objF;
    TrShWrapper::Rect m_roi;


};

//--------------------------------------------------

TrShWrapper::TrShWrapper()
    : m_d {new TrShWrapperPrivate()}
{
}

TrShWrapper::~TrShWrapper()
{

    delete m_d;
}

void TrShWrapper::setIniFileNamePath(const std::string &fileNamePath)
{
    m_d->m_iniFileName = fileNamePath;
}

float TrShWrapper::roiExt() const
{
    return m_d->m_ext_wh;
}

void TrShWrapper::setRoiExt(float ext)
{
    m_d->m_ext_wh = ext;
}

void TrShWrapper::setTargetObject(const Rect &objectRect)
{
    stopTracking();

    if (!objectRect.valid())
        return;
    if (objectRect.cx < 0 ||
        objectRect.cx > (m_d->m_t_ctl.fr_w0 - objectRect.w / 2))
        return;
    if (objectRect.cy < 0 ||
        objectRect.cy > (m_d->m_t_ctl.fr_h0 - objectRect.h / 2))
        return;

    m_d->m_targetObj = objectRect;

    m_d->setReinitFrame();
}

void TrShWrapper::stopTracking()
{
    m_d->m_t_ctl.key = 27;
}

bool TrShWrapper::trackFrame(cv::Mat &frame)
{
    return m_d->update(frame);
}

bool TrShWrapper::active() const
{
    return m_d->m_t != nullptr && m_d->m_t_ctl.zahvat;
}

bool TrShWrapper::lockedOn() const
{
    return m_d->m_t != nullptr && m_d->m_t_ctl.ok_match;
}

const TrShWrapper::Rect &TrShWrapper::roiRect() const
{
    return m_d->m_roi;
}

const TrShWrapper::Rect &TrShWrapper::objectRect() const
{
    return m_d->m_obj;
}

const TrShWrapper::RectF &TrShWrapper::objectRectF() const
{
    return m_d->m_objF;
}

long long TrShWrapper::frameCounter() const
{
    return m_d->m_f_cnt;
}

void *TrShWrapper::tctl()
{
    return &m_d->m_t_ctl;
}
