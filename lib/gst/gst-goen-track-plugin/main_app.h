#ifndef MAIN_APP_H
#define MAIN_APP_H


#include "tracshats/trac_api.hpp"
#include "tracshats/trac_struct.hpp"
#include <mutex>
#include <atomic>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>



class Main_app
{
public:
    Main_app(bool &ok , const std::string & pathToConfig = "tracking.ini");
    void exec(cv::Mat frame_recieve );
    void create_tracker(const cv::Mat &f);

    std::string pathToConfig;
    uint64 m_fcnt = 0;

    trac_struct m_t_ctl;

    std::shared_ptr<trac_tmml> m_t = nullptr;

    bool m_selRectOk = false;

    const float m_ext_wh_2 = 239.f / 48.f; // Расширение полурамки локального захвата по умолчанию.
    const float m_ext_wh = 8; //2.f * m_ext_wh_2;

};

#endif // MAIN_APP_H
