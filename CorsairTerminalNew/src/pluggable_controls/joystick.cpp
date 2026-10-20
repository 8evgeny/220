#include "joystick.h"
//#include <cmath>
#include "math.h"
#include <iostream>

#include "tools/resource.h"
#include "vpc_wb/vpcwb.h"
#include "vlx_common/vlx_settings.h"

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logJoystick, "Joystick")


#define RESOURCE_SECTION    ("joystick")
#define RESOURCE_FILENAME   ("rsc/joystick.xml")

// ---------------------------Default Joystick---------------------------------- //
#define MAX_JOYSTICK_BUTTON         (12)

// #define BUTTON_PREV_CH          (0x001) // (1 << 0)
// #define BUTTON_NEXT_CH          (0x002) // (1 << 1)
#define BUTTON_VIDEO_RECORD     (0x080) // (1 << 2)
#define BUTTON_FIX_ANGLE        (0x008) // (1 << 3)
#define BUTTON_STAB             (0x001) // (1 << 6)
#define BUTTON_ZERO             (0x002) // (1 << 4)
#define BUTTON_INVERSION_MW     (0x004) // (1 << 9)
// #define BUTTON_ZOOM_FIX         (0x020) // (1 << 5)
// #define BUTTON_ZOOM_FIX_WIDTH   (0x100) // (1 << 8)
#define BUTTON_AUTOFOCUS        (0x200) // (1 << 11)
#define BUTTON_TARGET_CAPTURE   (0x010) // (1 << 10)
#define BUTTON_LRF_START      (0x020) // (1 << 10)
#define BUTTON_SWITCH_TRACKER (0x0040)
// ---------------------------Default Joystick---------------------------------- //
#define MAX_HIK_JOYSTICK_BUTTON       (12)

#define BUTTON_HIK_PREV_CH          (0x001) // (1 << 0)
#define BUTTON_HIK_NEXT_CH          (0x002) // (1 << 1)
#define BUTTON_HIK_VIDEO_RECORD     (0x004) // (1 << 2)
#define BUTTON_HIK_ZOOM_IN          (0x008) // (1 << 3)
#define BUTTON_HIK_ZOOM_OUT         (0x040) // (1 << 6)
#define BUTTON_HIK_FOCUS_IN         (0x010) // (1 << 4)
#define BUTTON_HIK_FOCUS_OUT        (0x080) // (1 << 7)
#define BUTTON_HIK_ZOOM_FIX         (0x020) // (1 << 5)
#define BUTTON_HIK_ZOOM_FIX_WIDTH   (0x100) // (1 << 8)
#define BUTTON_HIK_INVERSION_MW     (0x200) // (1 << 9)
#define BUTTON_HIK_AUTOFOCUS        (0x800) // (1 << 11)
#define BUTTON_HIK_TARGET_CAPTURE   (0x400) // (1 << 10)
#define BUTTON_HIK_TARGET_FREE      (0x400) // (1 << 10)
// #define BUTTON_HIK_NULL_POSITION      (0x200) // (1 << 4)
//#define BUTTON_HIK_FIELD_NARROW       (0x080) // (1 << 5)
//#define BUTTON_HIK_FIELD_SYNC_CONT    (0x100) // (1 << 6)
// #define BUTTON_HIK_RANGEFINDER_START  (0x100) // (1 << 7)
// -----------------------------PS Joystick------------------------------------- //
#define MAX_PS_JOYSTICK_BUTTON  (15)

#define BUTTON_0            (1 << 0)    //(0x001)
#define BUTTON_PS_LWIR      (0x0002)    //(1 << 1)
#define BUTTON_PS_TV        (0x0004)    //(1 << 2)
#define BUTTON_PS_TV_MONO   (0x0008)    //(1 << 3)
#define BUTTON_4            (1 << 4)    //(0x010)
#define BUTTON_5            (1 << 5)    //(0x020)
#define BUTTON_6            (1 << 6)    //(0x040)
#define BUTTON_7            (1 << 7)    //(0x080)
#define BUTTON_8            (1 << 8)
#define BUTTON_9            (1 << 9)
#define BUTTON_10           (1 << 10)
#define BUTTON_PS_ZOOM_IN   (0x0800)    //(1 << 11)
#define BUTTON_PS_ZOOM_OUT  (0x1000)    //(1 << 12)
#define BUTTON_PS_FOCUS_IN  (0x2000)    //(1 << 13)
#define BUTTON_PS_FOCUS_OUT (0x4000)    //(1 << 14)

// -------------------------Thrustmaster Joystick------------------------------- //
#define MAX_TM_JOYSTICK_BUTTON  (21)

#define BUTTON_TM_FREE_TARGET    (0x00002)
#define BUTTON_TM_CAPTURE_TARGET (0x00001)
#define BUTTON_TM_FOCUS_IN       (0x00010)
#define BUTTON_TM_ZOOM_FIX_WIDTH (0x00020)
#define BUTTON_TM_ZOOM_FIX       (0x00100)
#define BUTTON_TM_FOCUS_OUT      (0x00200)
#define BUTTON_TM_AUTOFOCUS      (0x00004)
#define BUTTON_TM_LRF_START      (0x00008)
#define BUTTON_TM_SWITCH_TRACKER (0x02000)
#define BUTTON_TM_MWIR_INVERSION (0x04000)
#define BUTTON_TM_VIDEORECORD    (0x08000)
#define BUTTON_TM_ZERO           (0x00800)
#define BUTTON_TM_STAB           (0x01000)
#define BUTTON_TM_FIXANGLE       (0x00400)

//-------------------------Logitech Extreme 3D Pro---------------------------------//
#define MAX_LGCH_JOYSTICK_BUTTON  (21)

#define BUTTON_LGCH_CAPTURE_TARGET (0x00001)
#define BUTTON_LGCH_FREE_TARGET    (0x00002)
#define BUTTON_LGCH_ZOOM_FIX_WIDTH (0x00004)
#define BUTTON_LGCH_ZOOM_FIX       (0x00008)
#define BUTTON_LGCH_PREV_CHANNEL   (0x00010)
#define BUTTON_LGCH_NEXT_CHANNEL   (0x00020)

#define BUTTON_LGCH_MWIR_INVERSION (0x00040)
#define BUTTON_LGCH_VIDEORECORD    (0x00080)
#define BUTTON_LGCH_AUTOFOCUS      (0x00100)
#define BUTTON_LGCH_FIXANGLE       (0x00200)
#define BUTTON_LGCH_STAB           (0x00400)
#define BUTTON_LGCH_ZERO           (0x00800)


static SDL_Joystick * s_joystick = nullptr;

Joystick::Joystick(QObject *parent) :
    QObject(parent),
    joystick(nullptr),
    joystick_name(""),
    joystick_tmr(nullptr),
    _enable{true},
    prev_buttons(0),
    prev_hats(0),
    x_coord(0),
    z_coord(0),
    y_coord(0),
    zoom_coord(0),
    focus_coord(0),
    sizeUp(0),
    sizeDown(0),
    x_adjust_coord(0),
    z_adjust_coord(0),
    _opened(false),
    _x_inverted_koef(-1.),
    _z_inverted_koef(-1.)
{
    load();

    open(0);

    // Start joystick timer
    joystick_tmr = new QTimer(this);
    connect(joystick_tmr, SIGNAL(timeout()), this, SLOT(joystick_tmr_timedout()));
    joystick_tmr->start(joystick_poll_interval_ms);
}

Joystick::~Joystick()
{
    if(joystick_tmr) {
        joystick_tmr->stop();
        joystick_tmr->deleteLater();
        joystick_tmr = nullptr;
    }
    save();
    close();
}

SDL_Joystick *Joystick::currentJoystick()
{
    return s_joystick;
}

void Joystick::setPollingIntervalMs(int ms)
{
    joystick_tmr->setInterval(ms);
}

QString Joystick::joystickName() const
{
    return joystick_name;
}

void Joystick::joystick_tmr_timedout()
{
    if (_enable) {
        if (joystick == nullptr && joystick_name == "VPC") {
            check_vpc_events();
        } else if (joystick && (SDL_JoystickGetAttached(joystick) == SDL_TRUE)) {
            SDL_JoystickUpdate();
            if (joystick_name == "PS4 Controller") {
                check_ps_events();
            } else if ((joystick_name == "T.16000M") || (joystick_name == "T.A320 Pilot")
                       || (joystick_name == "T.A320 Copilot")
                       || (joystick_name == "Virtual Flight Stick")
                       ) {
                check_thrustmaster_events();
            } else if (joystick_name == "DS_1005K") {
                check_hikvision_events();
            } else if (joystick_name == "XInput Controller #1") {
                check_machenike();
            } else if (joystick_name == "Logitech Extreme 3D") {
                check_logitech_events();
            } else if (joystick_name == "Xbox 360 Controller") {
                check_xbox360controller_events();
            } else {
                check_events();
            }
        } else {
            static int cnt = 0;
            if (++cnt > 20) {
                cnt = 0;
                open(0);
            }
        }
    }
}
void Joystick::set_enable(bool flag)
{
    _enable = flag;
    if(_enable) {
        open(0);
    } else {
        close();
    }
}

void Joystick::set_inverse_x(bool flag)
{
    _x_inverted_koef = flag ? -1. : 1.;
}

void Joystick::set_inverse_z(bool flag)
{
    _z_inverted_koef = flag ? -1. : 1.;
}

void Joystick::load()
{
    // Resource resource(RESOURCE_SECTION);
    // if(resource.load(RESOURCE_FILENAME))
    // {
    //     bool ok;
    //     QVariant tmp;
    //     tmp                               = resource.get("MIN_TRACKBALL_COORD", ok);
    //     if(ok) MIN_TRACKBALL_COORD        = tmp.toInt();
    //     tmp                               = resource.get("MAX_TRACKBALL_COORD", ok);
    //     if(ok) MAX_TRACKBALL_COORD        = tmp.toInt();
    //     tmp                               = resource.get("MIN_SENCE_TRACKBALL_COORD", ok);
    //     if(ok) MIN_SENCE_TRACKBALL_COORD  = tmp.toInt();
    //     tmp                               = resource.get("TRACKBALL_COORD_EXP_FILTER", ok);
    //     if(ok) TRACKBALL_COORD_EXP_FILTER = tmp.toDouble();
    // }
    // else
    // {
    //     save();
    // }
}
void Joystick::save()
{
//     Resource resource(RESOURCE_SECTION);
//     resource.set("MIN_TRACKBALL_COORD",        MIN_TRACKBALL_COORD);
//     resource.set("MAX_TRACKBALL_COORD",        MAX_TRACKBALL_COORD);
//     resource.set("MIN_SENCE_TRACKBALL_COORD",  MIN_SENCE_TRACKBALL_COORD);
//     resource.set("TRACKBALL_COORD_EXP_FILTER", TRACKBALL_COORD_EXP_FILTER);
//     resource.save(RESOURCE_FILENAME);
}

void Joystick::open(const int & device_index)
{
    close();

    // Reset previous buttons mask
    prev_buttons = 0;
    prev_hats = 0;
    // Reset previous coordinates
    x_coord = 0;
    z_coord = 0;
    y_coord = 0;
    zoom_coord = 0;
    focus_coord = 0;
    sizeUp = 0;
    sizeDown = 0;

    // VPC WarBRD JOYSTICK
    /*
    auto j = new VpcWB::Joystick(this);
    connect(j, &VpcWB::Joystick::errorOccured, this, [this](const QString &err) {
        qCCritical(logJoystick) <<  "VpcWB::Joystick ERROR:" << err;
    });

    if (!j->init()) {
        _opened = false;

    } else {

        joystick = nullptr;

        joystick_name = "VPC";

        qInfo(logJoystick(), "open: VPC WB opened");

        _opened = true;
        return;
    }
    */

    // Joystick Initialization
    SDL_Init(SDL_INIT_JOYSTICK);
    // Enable Joystick event polling
    // SDL_JoystickEventState(SDL_ENABLE);
    //Disable
    SDL_JoystickEventState(SDL_DISABLE);

    // create virtual joystick
    auto di = device_index;

    if (VlxSettings::i().useVirtualJoystick())
        di = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_FLIGHT_STICK, 2, 16, 1);

    // Open Joystick
    joystick = SDL_JoystickOpen(di);

    s_joystick = joystick;

    if(joystick == nullptr)
    {
        _opened = false;
    }
    else
    {
        joystick_name = SDL_JoystickName(joystick);
//        std::cout << "Axes num    : " << SDL_JoystickNumAxes(joystick)    << std::endl;
//        std::cout << "Balls num   : " << SDL_JoystickNumBalls(joystick)   << std::endl;
//        std::cout << "Hats num    : " << SDL_JoystickNumHats(joystick)    << std::endl;
//        std::cout << "Buttons num : " << SDL_JoystickNumButtons(joystick) << std::endl;
        _opened = true;
        qInfo(logJoystick(), "open: opened");
    }
}

void Joystick::close()
{
    s_joystick = nullptr;
    // Reset previous buttons mask
    prev_buttons = 0;
    prev_hats = 0;
    // Reset previous coordinates
    x_coord = 0;
    z_coord = 0;
    y_coord = 0;
    zoom_coord = 0;
    focus_coord = 0;
    sizeUp = 0;
    sizeDown = 0;
    _opened = false;

    if (joystick == nullptr && joystick_name == "VPC") {
        Q_ASSERT(VpcWB::Joystick::inst() != nullptr);
        VpcWB::Joystick::inst()->deinit();
        delete VpcWB::Joystick::inst();
        return;
    }


    if(joystick && (SDL_JoystickGetAttached(joystick) == SDL_TRUE))
    {
        SDL_JoystickClose(joystick);
    }
    joystick = nullptr;
    if(SDL_WasInit(SDL_INIT_JOYSTICK))
    {
        // Disable Joystick event polling
        SDL_JoystickEventState(SDL_IGNORE);
        // Joystick Deinitialization
        SDL_Quit();
    }

//    qInfo(logJoystick(), "close: closed");
}
int Joystick::exp_filter(const int & prev, const int & next, const double & a)
{
    double cur = double(prev) * (1 - a) + double(next) * a;
    return int(round(cur));
}
double Joystick::exp_filter(const double & prev, const double & next, const double & a)
{
    double cur = prev * (1 - a) + next * a;
    return cur;
}
void Joystick::check_events()
{

    return;

    // Polls for currently pending events
//     SDL_Event joystick_events;
//     SDL_PollEvent(&joystick_events);
//     // Get joystick trackball coordinates
//     int next_x_coord = SDL_JoystickGetAxis(joystick, 1);
//     int next_z_coord = SDL_JoystickGetAxis(joystick, 0);
//     int next_y_coord = SDL_JoystickGetAxis(joystick, 3);
// //    qInfo(logJoystick(), QString::number(next_x_coord).toStdString().c_str());

//     double next_x_coord_d = (next_x_coord < 0) ? (double(next_x_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_x_coord) / double(MAX_TRACKBALL_COORD));
//     double next_z_coord_d = (next_z_coord < 0) ? (double(next_z_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_z_coord) / double(MAX_TRACKBALL_COORD));
//     double next_y_coord_d = (next_y_coord < 0) ? (double(next_y_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_y_coord) / double(MAX_TRACKBALL_COORD));

//     const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD) / double(MAX_TRACKBALL_COORD);
//     if(fabs(next_x_coord_d) <= min_sence_trackball_coord) next_x_coord_d = 0.0;
//     if(fabs(next_z_coord_d) <= min_sence_trackball_coord) next_z_coord_d = 0.0;
//     if(fabs(next_y_coord_d) <= min_sence_trackball_coord) next_y_coord_d = 0.0;

//     next_x_coord_d = pow(next_x_coord_d, 5);
//     next_z_coord_d = pow(next_z_coord_d, 5);
//     next_y_coord_d = pow(next_y_coord_d, 5);

//     x_coord =  exp_filter(x_coord, next_x_coord_d, TRACKBALL_COORD_EXP_FILTER);
//     z_coord =  exp_filter(z_coord, next_z_coord_d, TRACKBALL_COORD_EXP_FILTER);
//     y_coord =  exp_filter(y_coord, next_y_coord_d, TRACKBALL_COORD_EXP_FILTER);

//     if(fabs(x_coord) <= min_sence_trackball_coord) x_coord = 0.0;
//     if(fabs(z_coord) <= min_sence_trackball_coord) z_coord = 0.0;
//     if(fabs(y_coord) <= min_sence_trackball_coord) y_coord = 0.0;

//     // Process trackball
//     QString s_trackball;
//     static bool flag = false;
//     double x_coord_out{-x_coord};
//     double z_coord_out{z_coord};
//     double y_coord_out{y_coord};
//     if((x_coord_out != 0.0) || (z_coord_out != 0.0))
//     {
//         emit gsp_speed_set(x_coord_out * _x_inverted_koef, z_coord_out * _z_inverted_koef);
//         s_trackball += QString("X_COORD = ") + QString::number(x_coord_out) + QString(" ");
//         s_trackball += QString("Z_COORD = ") + QString::number(z_coord_out) + QString(" ");
//         flag = true;
//     }
//     else if(flag)
//     {
//         emit gsp_speed_set(x_coord_out, z_coord_out);
//         s_trackball += QString("X_COORD = ") + QString::number(x_coord_out) + QString(" ");
//         s_trackball += QString("Z_COORD = ") + QString::number(z_coord_out) + QString(" ");
//         flag = false;
//     }
//     if(y_coord_out != 0.0)
//     {
//         emit target_window_set(y_coord_out);
//         s_trackball += QString("Y_COORD = ") + QString::number(y_coord_out) + QString(" ");
//     }

//     // Hats processing
//     auto hats = SDL_JoystickGetHat(joystick, 0);
//     auto hats_pressed = hats & ~prev_hats;
//     auto hats_released = ~hats & prev_hats;
//     prev_hats = hats;

//     if (hats_pressed & SDL_HAT_UP)
//         emit zoom_in();
//     else if (hats_pressed & SDL_HAT_DOWN)
//         emit zoom_out();

//     if (hats_pressed & SDL_HAT_LEFT)
//         emit focus_far();
//     else if (hats_pressed & SDL_HAT_RIGHT)
//         emit focus_near();

//     if (hats_released & (SDL_HAT_UP | SDL_HAT_DOWN))
//         emit zoom_stop();
//     if (hats_released & (SDL_HAT_LEFT | SDL_HAT_RIGHT))
//         emit focus_stop();


// //    if(s_trackball != "") qInfo(logJoystick(), s_trackball.toStdString().c_str());

//     // Get joystick buttons mask
//     int buttons = 0;
//     for(int i = 0; i < MAX_JOYSTICK_BUTTON; ++i)
//     {
//         if(SDL_JoystickGetButton(joystick, i))
//         {
//             buttons |= (1 << i);
//         }
//     }

//     int buttons_pressed  =  buttons & ~prev_buttons;
//     int buttons_released = ~buttons &  prev_buttons;
//     prev_buttons = buttons;


//     if (buttons_pressed) {
//         qDebug() << "BTN:" << buttons_pressed;

//         if (buttons_pressed & SDL_BUTTON(5)) { // Y
//             gsp_park();
//             // qDebug() << "BUTTON: Y";
//         }
//         if (buttons_pressed & SDL_BUTTON(2)) { // B
//             lrf_enable_switch();
//             // qDebug() << "BUTTON: B";
//         }
//         if (buttons_pressed & SDL_BUTTON(1)) { // A
//             autofocus();
//             // qDebug() << "BUTTON: A";
//         }
//         if (buttons_pressed & SDL_BUTTON(4)) { // X
//             gsp_stabilization_switch();
//             // qDebug() << "BUTTON: X";
//         }
//         if (buttons_pressed & SDL_BUTTON(8)) { // RB
//             qDebug() << "BUTTON: RB";
//         }
//         if (buttons_pressed & SDL_BUTTON(7)) { // LB
//             qDebug() << "BUTTON: LB";
//         }
//         if (buttons_pressed & SDL_BUTTON(10)) { // RT
//             target_set_enabled(true);
//             target_capture();
//             // qDebug() << "BUTTON: RT";
//         }
//         if (buttons_pressed & SDL_BUTTON(9)) { // LT
//             target_set_enabled(false);
//             // qDebug() << "BUTTON: LT";
//         }
//         if (buttons_pressed & SDL_BUTTON(16)) { // HOME
//             qDebug() << "BUTTON: HOME";
//         }
//     }

    QString s_buttons;
    // Process pressed buttons
    // if(buttons_pressed & BUTTON_ZOOM_OUT)
    // {
    //     emit zoom_in();
    //     s_buttons += QString("BUTTON_ZOOM_OUT pressed") + QString(" ");
    // }
    // else if(buttons_pressed & BUTTON_ZOOM_IN)
    // {
    //     emit zoom_out();
    //     s_buttons += QString("BUTTON_ZOOM_IN pressed") + QString(" ");
    // }
    // if(buttons_pressed & BUTTON_FOCUS_OUT)
    // {
    //     emit focus_far();
    //     s_buttons += QString("BUTTON_FOCUS_OUT pressed") + QString(" ");
    // }
    // else if(buttons_pressed & BUTTON_FOCUS_IN)
    // {
    //     emit focus_near();
    //     s_buttons += QString("BUTTON_FOCUS_IN pressed") + QString(" ");
    // }
//    else if(buttons_pressed & BUTTON_NULL_POSITION)
//    {
//        emit gsp_null_position();
//        s_buttons += QString("BUTTON_NULL_POSITION") + QString(" ");
//    }
//    if(buttons_pressed & BUTTON_FIELD_NARROW)
//    {
//        emit field_sync_narrow();
//        s_buttons += QString("BUTTON_FIELD_NARROW") + QString(" ");
//    }
//    if(buttons_pressed & BUTTON_FIELD_SYNC_CONT)
//    {
//        emit field_sync_reg_switch();
//        s_buttons += QString("BUTTON_FIELD_SYNC_CONT") + QString(" ");
//    }
//    else if(buttons_pressed & BUTTON_RANGEFINDER_START)
//    {
//        emit lrf_start();
//        s_buttons += QString("BUTTON_RANGEFINDER_START") + QString(" ");
//    }
    // if(buttons_pressed & BUTTON_PREV_CH)
    // {
    //     emit prev_channel();
    //     s_buttons += QString("BUTTON_PREV_CH") + QString(" ");
    // }
    // if(buttons_pressed & BUTTON_NEXT_CH)
    // {
    //     emit next_channel();
    //     s_buttons += QString("BUTTON_NEXT_CH") + QString(" ");
    // }
    // if(buttons_pressed & BUTTON_TARGET_CAPTURE)
    // {
    //     emit target_capture();
    //     s_buttons += QString("BUTTON_TARGET_CAPTURE") + QString(" ");
    // }
    // else if(buttons_pressed & BUTTON_TARGET_FREE)
    // {
    //     emit target_free();
    //     s_buttons += QString("BUTTON_TARGET_FREE") + QString(" ");
    // }
    // Process released buttons
    // if(buttons_released & BUTTON_ZOOM_OUT)
    // {
    //     emit zoom_stop();
    //     s_buttons += QString("BUTTON_ZOOM_OUT released") + QString(" ");
    // }
    // else if(buttons_released & BUTTON_ZOOM_IN)
    // {
    //     emit zoom_stop();
    //     s_buttons += QString("BUTTON_ZOOM_IN released") + QString(" ");
    // }
    // if(buttons_released & BUTTON_FOCUS_OUT)
    // {
    //     emit focus_stop();
    //     s_buttons += QString("BUTTON_FOCUS_OUT released") + QString(" ");
    // }
    // else if(buttons_released & BUTTON_FOCUS_IN)
    // {
    //     emit focus_stop();
    //     s_buttons += QString("BUTTON_FOCUS_IN released") + QString(" ");
    // }


    ///

    // if(s_buttons != "") qInfo(logJoystick(), s_buttons.toStdString().c_str());
}

void Joystick::check_xbox360controller_events()
{
    // Polls for currently pending events
    SDL_Event joystick_events;
    SDL_PollEvent(&joystick_events);
    // Get joystick trackball coordinates
    int next_x_coord = SDL_JoystickGetAxis(joystick, 1);
    int next_z_coord = SDL_JoystickGetAxis(joystick, 0);
    int next_y_coord = SDL_JoystickGetAxis(joystick, 3);
    //    qInfo(logJoystick(), QString::number(next_x_coord).toStdString().c_str());

    double next_x_coord_d = (next_x_coord < 0) ? (double(next_x_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_x_coord) / double(MAX_TRACKBALL_COORD));
    double next_z_coord_d = (next_z_coord < 0) ? (double(next_z_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_z_coord) / double(MAX_TRACKBALL_COORD));
    double next_y_coord_d = (next_y_coord < 0) ? (double(next_y_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_y_coord) / double(MAX_TRACKBALL_COORD));

    const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD) / double(MAX_TRACKBALL_COORD);
    if(fabs(next_x_coord_d) <= min_sence_trackball_coord) next_x_coord_d = 0.0;
    if(fabs(next_z_coord_d) <= min_sence_trackball_coord) next_z_coord_d = 0.0;
    if(fabs(next_y_coord_d) <= min_sence_trackball_coord) next_y_coord_d = 0.0;

    next_x_coord_d = pow(next_x_coord_d, 5);
    next_z_coord_d = pow(next_z_coord_d, 5);
    next_y_coord_d = pow(next_y_coord_d, 5);

    x_coord =  exp_filter(x_coord, next_x_coord_d, TRACKBALL_COORD_EXP_FILTER);
    z_coord =  exp_filter(z_coord, next_z_coord_d, TRACKBALL_COORD_EXP_FILTER);
    y_coord =  exp_filter(y_coord, next_y_coord_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(x_coord) <= min_sence_trackball_coord) x_coord = 0.0;
    if(fabs(z_coord) <= min_sence_trackball_coord) z_coord = 0.0;
    if(fabs(y_coord) <= min_sence_trackball_coord) y_coord = 0.0;

    static bool flag = false;
    double x_coord_out{-x_coord};
    double z_coord_out{z_coord};
    double y_coord_out{y_coord};
    if((x_coord_out != 0.0) || (z_coord_out != 0.0))
    {
        emit gsp_speed_set(x_coord_out * _x_inverted_koef, z_coord_out * _z_inverted_koef);
        flag = true;
    }
    else if(flag)
    {
        emit gsp_speed_set(x_coord_out, z_coord_out);
        flag = false;
    }
    if(y_coord_out != 0.0)
    {
        emit target_window_set(y_coord_out);
    }

    // Hats processing
    auto hats = SDL_JoystickGetHat(joystick, 0);
    auto hats_pressed = hats & ~prev_hats;
    auto hats_released = ~hats & prev_hats;
    prev_hats = hats;

    if (hats_pressed & SDL_HAT_UP)
        emit zoom_out();
    else if (hats_pressed & SDL_HAT_DOWN)
        emit zoom_in();

    if (hats_pressed & SDL_HAT_LEFT)
        emit focus_far();
    else if (hats_pressed & SDL_HAT_RIGHT)
        emit focus_near();

    if (hats_released & (SDL_HAT_UP | SDL_HAT_DOWN))
        emit zoom_stop();
    if (hats_released & (SDL_HAT_LEFT | SDL_HAT_RIGHT))
        emit focus_stop();


    //    if(s_trackball != "") qInfo(logJoystick(), s_trackball.toStdString().c_str());

    // Get joystick buttons mask
    int buttons = 0;
    for(int i = 0; i < MAX_JOYSTICK_BUTTON; ++i)
    {
        if(SDL_JoystickGetButton(joystick, i))
        {
            buttons |= (1 << i);
        }
    }

    int buttons_pressed  =  buttons & ~prev_buttons;
    int buttons_released = ~buttons &  prev_buttons;
    prev_buttons = buttons;


    if (buttons_pressed) {
        // qDebug() << "BTN:" << buttons_pressed;

        if (buttons_pressed & SDL_BUTTON(7)) { // back
            gsp_park();
        }
        if (buttons_pressed & SDL_BUTTON(2)) { // B
            lrf_enable_switch();
        }
        if (buttons_pressed & SDL_BUTTON(1)) { // A
            autofocus();
        }
        if (buttons_pressed & SDL_BUTTON(3)) { // X
            gsp_stabilization_switch();
            // qDebug() << "BUTTON: X";
        }
        if (buttons_pressed & SDL_BUTTON(6)) { // RB
            emit lrf_single();
        }
        if (buttons_pressed & SDL_BUTTON(5)) { // LB
            emit lrf_toggle_continuous();
        }
        if (buttons_pressed & SDL_BUTTON(10)) { // RightStick press
            target_set_enabled(true);
            target_capture();
        }
        if (buttons_pressed & SDL_BUTTON(4)) { // Y
            target_set_enabled(false);
        }
        if (buttons_pressed & SDL_BUTTON(8)) { // start
            emit switch_channel();
        }
    }
}

void Joystick::check_hikvision_events()
{
    // // Polls for currently pending events
    // SDL_Event joystick_events;
    // SDL_PollEvent(&joystick_events);
    // Get joystick trackball coordinates
    int next_x_coord = SDL_JoystickGetAxis(joystick, 1);
    int next_z_coord = SDL_JoystickGetAxis(joystick, 0);
    int next_y_coord = SDL_JoystickGetAxis(joystick, 2);
//    qInfo(logJoystick(), QString::number(next_x_coord).toStdString().c_str());

    double next_x_coord_d = (next_x_coord < 0) ? (double(next_x_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_x_coord) / double(MAX_TRACKBALL_COORD));
    double next_z_coord_d = (next_z_coord < 0) ? (double(next_z_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_z_coord) / double(MAX_TRACKBALL_COORD));
    double next_y_coord_d = (next_y_coord < 0) ? (double(next_y_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_y_coord) / double(MAX_TRACKBALL_COORD));

    const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD) / double(MAX_TRACKBALL_COORD);
    if(fabs(next_x_coord_d) <= min_sence_trackball_coord) next_x_coord_d = 0.0;
    if(fabs(next_z_coord_d) <= min_sence_trackball_coord) next_z_coord_d = 0.0;
    if(fabs(next_y_coord_d) <= min_sence_trackball_coord) next_y_coord_d = 0.0;

    next_x_coord_d = pow(next_x_coord_d, 5);
    next_z_coord_d = pow(next_z_coord_d, 5);
    next_y_coord_d = pow(next_y_coord_d, 5);
    next_x_coord_d *= 1.05;
    next_z_coord_d *= 1.05;
    if(next_x_coord_d > 1.) next_x_coord_d = 1.;
    if(next_z_coord_d > 1.) next_z_coord_d = 1.;

    x_coord =  exp_filter(x_coord, next_x_coord_d, TRACKBALL_COORD_EXP_FILTER);
    z_coord =  exp_filter(z_coord, next_z_coord_d, TRACKBALL_COORD_EXP_FILTER);
    y_coord =  exp_filter(y_coord, next_y_coord_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(x_coord) <= min_sence_trackball_coord) x_coord = 0.0;
    if(fabs(z_coord) <= min_sence_trackball_coord) z_coord = 0.0;
    if(fabs(y_coord) <= min_sence_trackball_coord) y_coord = 0.0;

    // Process trackball
    QString s_trackball;
    static bool flag = false;
    double x_coord_out{-x_coord};
    double z_coord_out{z_coord};
    double y_coord_out{y_coord};
    if((x_coord_out != 0.0) || (z_coord_out != 0.0))
    {
        // emit gsp_speed_set(x_coord_out, z_coord_out);
        emit gsp_speed_set(_x_inverted_koef * x_coord_out, _z_inverted_koef * z_coord_out);
        s_trackball += QString("X_COORD = ") + QString::number(x_coord_out) + QString(" ");
        s_trackball += QString("Z_COORD = ") + QString::number(z_coord_out) + QString(" ");
        flag = true;
    }
    else if(flag)
    {
        emit gsp_speed_set(x_coord_out, z_coord_out);
        s_trackball += QString("X_COORD = ") + QString::number(x_coord_out) + QString(" ");
        s_trackball += QString("Z_COORD = ") + QString::number(z_coord_out) + QString(" ");
        flag = false;
    }
    if(y_coord_out != 0.0)
    {
        emit target_window_set(y_coord_out);
        s_trackball += QString("Y_COORD = ") + QString::number(y_coord_out) + QString(" ");
    }
//    if(s_trackball != "") qInfo(logJoystick(), s_trackball.toStdString().c_str());

    // Get joystick buttons mask
    int buttons = 0;
    for(int i = 0; i < MAX_JOYSTICK_BUTTON; ++i)
    {
        if(SDL_JoystickGetButton(joystick, i))
        {
            buttons |= (1 << i);
        }
    }
    int buttons_pressed  =  buttons & ~prev_buttons;
    int buttons_released = ~buttons &  prev_buttons;
    prev_buttons = buttons;

    /** Костыль для трекера */
    static bool trackerEn = false;

    QString s_buttons;
    // Process pressed buttons

    if(buttons_pressed & BUTTON_HIK_NEXT_CH)
    {
        emit next_channel();
    }
    else if(buttons_pressed & BUTTON_HIK_PREV_CH)
    {
        emit prev_channel();
    }
    if(buttons_pressed & BUTTON_HIK_VIDEO_RECORD)
    {
        emit videoRecordSwitch();
        s_buttons += QString("BUTTON_VIDEO_RECORD_HIK") + QString(" ");
    }
    if(buttons_pressed & BUTTON_HIK_ZOOM_OUT)
    {
        emit zoom_out();
        s_buttons += QString("BUTTON_ZOOM_OUT_HIK pressed") + QString(" ");
    }
    else if(buttons_pressed & BUTTON_HIK_ZOOM_IN)
    {
        emit zoom_in();
        s_buttons += QString("BUTTON_ZOOM_IN_HIK pressed") + QString(" ");
    }
    if(buttons_pressed & BUTTON_HIK_FOCUS_OUT)
    {
        emit focus_far();
        s_buttons += QString("BUTTON_FOCUS_OUT_HIK pressed") + QString(" ");
    }
    else if(buttons_pressed & BUTTON_HIK_FOCUS_IN)
    {
        emit focus_near();
        s_buttons += QString("BUTTON_FOCUS_IN_HIK pressed") + QString(" ");
    }
    if(buttons_pressed & BUTTON_HIK_ZOOM_FIX_WIDTH)
    {
        emit zoom_pos_fix_width();
    }
    else if(buttons_pressed & BUTTON_HIK_ZOOM_FIX)
    {
        emit zoom_pos_fix();
    }
    if(buttons_pressed & BUTTON_HIK_INVERSION_MW)
    {
        emit inversion();
        s_buttons += QString("BUTTON_INVERSION_MW_HIK") + QString(" ");
    }
    if(buttons_pressed & BUTTON_HIK_AUTOFOCUS)
    {
        emit autofocus();
        s_buttons += QString("BUTTON_AUTOFOCUS_HIK") + QString(" ");
    }
    if(buttons_pressed & BUTTON_HIK_TARGET_CAPTURE)
    {
        if( !trackerEn) {
            emit target_capture();
            s_buttons += QString("BUTTON_TARGET_CAPTURE_HIK") + QString(" ");
        } else {
            emit target_free();
            s_buttons += QString("BUTTON_TARGET_FREE_HIK") + QString(" ");
        }
        trackerEn = !trackerEn;
    }
    /*
    if(buttons_pressed & BUTTON_HIK_TARGET_CAPTURE)
    {
        emit target_capture();
        s_buttons += QString("BUTTON_TARGET_CAPTURE_HIK") + QString(" ");
    }
    else if(buttons_pressed & BUTTON_HIK_TARGET_FREE)
    {
        emit target_free();
        s_buttons += QString("BUTTON_TARGET_FREE_HIK") + QString(" ");
    }*/

    // Process released buttons
    if(buttons_released & BUTTON_HIK_ZOOM_OUT)
    {
        emit zoom_stop();
        s_buttons += QString("BUTTON_ZOOM_OUT_HIK released") + QString(" ");
    }
    else if(buttons_released & BUTTON_HIK_ZOOM_IN)
    {
        emit zoom_stop();
        s_buttons += QString("BUTTON_ZOOM_IN_HIK released") + QString(" ");
    }
    if(buttons_released & BUTTON_HIK_FOCUS_OUT)
    {
        emit focus_stop();
        s_buttons += QString("BUTTON_FOCUS_OUT_HIK released") + QString(" ");
    }
    else if(buttons_released & BUTTON_HIK_FOCUS_IN)
    {
        emit focus_stop();
        s_buttons += QString("BUTTON_FOCUS_IN_HIK released") + QString(" ");
    }

    // if(s_buttons != "") qInfo(logJoystick(), s_buttons.toStdString().c_str());
}

void Joystick::check_ps_events()
{
    // Polls for currently pending events
    SDL_Event joystick_events;
    SDL_PollEvent(&joystick_events);
    // Get joystick trackball coordinates
    int next_z_coord = SDL_JoystickGetAxis(joystick, 0);
    int next_x_coord = SDL_JoystickGetAxis(joystick, 1);

    double next_x_coord_d = (next_x_coord < 0) ? (double(next_x_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_x_coord) / double(MAX_TRACKBALL_COORD));
    double next_z_coord_d = (next_z_coord < 0) ? (double(next_z_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_z_coord) / double(MAX_TRACKBALL_COORD));

    const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD_PS) / double(MAX_TRACKBALL_COORD);
    if(fabs(next_x_coord_d) <= min_sence_trackball_coord) next_x_coord_d = 0.0;
    if(fabs(next_z_coord_d) <= min_sence_trackball_coord) next_z_coord_d = 0.0;

    next_x_coord_d = pow(next_x_coord_d, 5);
    next_z_coord_d = pow(next_z_coord_d, 5);

    x_coord = exp_filter(x_coord, next_x_coord_d, TRACKBALL_COORD_EXP_FILTER);
    z_coord = exp_filter(z_coord, next_z_coord_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(x_coord) <= min_sence_trackball_coord) x_coord = 0.0;
    if(fabs(z_coord) <= min_sence_trackball_coord) z_coord = 0.0;

    static bool flag = false;
    if((x_coord != 0.0) || (z_coord != 0.0))
    {
        emit gsp_speed_set(_x_inverted_koef * x_coord, _z_inverted_koef * z_coord);
        flag = true;
    }
    else if(flag)
    {
        emit gsp_speed_set(x_coord, z_coord);
        flag = false;
    }

    // Точное наведение
    int next_z_adjust_coord = SDL_JoystickGetAxis(joystick, 2);
    int next_x_adjust_coord = SDL_JoystickGetAxis(joystick, 3);

    double next_x_adjust_coord_d = (next_x_adjust_coord < 0) ? (double(next_x_adjust_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_x_adjust_coord) / double(MAX_TRACKBALL_COORD));
    double next_z_adjust_coord_d = (next_z_adjust_coord < 0) ? (double(next_z_adjust_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_z_adjust_coord) / double(MAX_TRACKBALL_COORD));

    if(fabs(next_x_adjust_coord_d) <= min_sence_trackball_coord) next_x_adjust_coord_d = 0.0;
    if(fabs(next_z_adjust_coord_d) <= min_sence_trackball_coord) next_z_adjust_coord_d = 0.0;

    next_x_adjust_coord_d = pow(next_x_adjust_coord_d, 5);
    next_z_adjust_coord_d = pow(next_z_adjust_coord_d, 5);

    x_adjust_coord = exp_filter(x_adjust_coord, next_x_adjust_coord_d, TRACKBALL_COORD_EXP_FILTER);
    z_adjust_coord = exp_filter(z_adjust_coord, next_z_adjust_coord_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(x_adjust_coord) <= min_sence_trackball_coord) x_adjust_coord = 0.0;
    if(fabs(z_adjust_coord) <= min_sence_trackball_coord) z_adjust_coord = 0.0;

    static bool _adjust_flag = false;
    if((x_adjust_coord != 0.0) || (z_adjust_coord != 0.0))
    {
        emit gsp_slow_speed_set(x_adjust_coord, z_adjust_coord);
        flag = true;
    }
    else if(_adjust_flag)
    {
        emit gsp_slow_speed_set(x_adjust_coord, z_adjust_coord);
        flag = false;
    }

    // Get joystick buttons mask
    int buttons = 0;
    for(int i = 0; i < MAX_PS_JOYSTICK_BUTTON; ++i)
    {
        if(SDL_JoystickGetButton(joystick, i))
        {
            buttons |= (1 << i);
        }
    }

    int buttons_pressed  =  buttons & ~prev_buttons;
    int buttons_released = ~buttons &  prev_buttons;
    prev_buttons = buttons;

    // Process pressed buttons
    // Каналы
    if(buttons_pressed & BUTTON_PS_LWIR)
    {
        emit change_channel(0x03u);
    }
    if(buttons_pressed & BUTTON_PS_TV)
    {
        emit change_channel(0x00u);
    }
    if(buttons_pressed & BUTTON_PS_TV_MONO)
    {
        emit change_channel(0x04u);
    }

    // Угловое поле
    if(buttons_pressed & BUTTON_PS_ZOOM_IN)
    {
        emit zoom_in();
    }
    else if(buttons_pressed & BUTTON_PS_ZOOM_OUT)
    {
        emit zoom_out();
    }
    // Фокусировка
    if(buttons_pressed & BUTTON_PS_FOCUS_IN)
    {
        emit focus_near();
    }
    else if(buttons_pressed & BUTTON_PS_FOCUS_OUT)
    {
        emit focus_far();
    }

    // Угловое поле
    if(buttons_released & BUTTON_PS_ZOOM_IN)
    {
        emit zoom_stop();
    }
    else if(buttons_released & BUTTON_PS_ZOOM_OUT)
    {
        emit zoom_stop();
    }
    // Фокусировка
    if(buttons_released & BUTTON_PS_FOCUS_IN)
    {
        emit focus_stop();
    }
    else if(buttons_released & BUTTON_PS_FOCUS_OUT)
    {
        emit focus_stop();
    }
}

void Joystick::check_vpc_events()
{
    Q_ASSERT(VpcWB::Joystick::inst() != nullptr);
    auto &ed = VpcWB::Joystick::inst()->poll();
    if (!ed.valid())
        return;

    qCDebug(logJoystick) << " --- VPC: " << ed.toString();


}

void Joystick::check_thrustmaster_events()
{
    // // Polls for currently pending events
    // SDL_Event joystick_events;
    // SDL_PollEvent(&joystick_events);

    // Get joystick trackball coordinates
    int next_x_coord = SDL_JoystickGetAxis(joystick, 1);
    int next_z_coord = SDL_JoystickGetAxis(joystick, 0);
    int next_y_coord = SDL_JoystickGetAxis(joystick, 2);

    double next_x_coord_d = (next_x_coord < 0) ? (double(next_x_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_x_coord) / double(MAX_TRACKBALL_COORD));
    double next_z_coord_d = (next_z_coord < 0) ? (double(next_z_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_z_coord) / double(MAX_TRACKBALL_COORD));
    double next_y_coord_d = (next_y_coord < 0) ? (double(next_y_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_y_coord) / double(MAX_TRACKBALL_COORD));

    const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD_TM) / double(MAX_TRACKBALL_COORD);
    if(fabs(next_x_coord_d) <= min_sence_trackball_coord) next_x_coord_d = 0.0;
    if(fabs(next_z_coord_d) <= min_sence_trackball_coord) next_z_coord_d = 0.0;
    if(fabs(next_y_coord_d) <= min_sence_trackball_coord) next_y_coord_d = 0.0;

    next_x_coord_d = pow(next_x_coord_d, 5);
    next_z_coord_d = pow(next_z_coord_d, 5);
    next_y_coord_d = pow(next_y_coord_d, 5);

    x_coord = exp_filter(x_coord, next_x_coord_d, TRACKBALL_COORD_EXP_FILTER);
    z_coord = exp_filter(z_coord, next_z_coord_d, TRACKBALL_COORD_EXP_FILTER);
    y_coord = exp_filter(y_coord, next_y_coord_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(x_coord) <= min_sence_trackball_coord) x_coord = 0.0;
    if(fabs(z_coord) <= min_sence_trackball_coord) z_coord = 0.0;
    if(fabs(y_coord) <= min_sence_trackball_coord) y_coord = 0.0;

    // Process trackball
    QString s_trackball;
    double x_coord_out{-x_coord};
    double z_coord_out{z_coord};
    double y_coord_out{y_coord};
    static bool flag = false;
    if((x_coord_out != 0.0) || (z_coord != 0.0))
    {
        emit gsp_speed_set(_x_inverted_koef * x_coord_out, _z_inverted_koef * z_coord_out);
        flag = true;
    }
    else if(flag)
    {
        emit gsp_speed_set(x_coord_out, z_coord_out);
        flag = false;
    }
    if(y_coord_out != 0.0)
    {
        emit target_window_set(y_coord_out);
    }

    check_thrustmaster_hats();

    // Get joystick buttons mask
    int buttons = 0;
    for(int i = 0; i < MAX_TM_JOYSTICK_BUTTON; ++i)
    {
        if(SDL_JoystickGetButton(joystick, i))
        {
            buttons |= (1 << i);
        }
    }

    int buttons_pressed  =  buttons & ~prev_buttons;
    int buttons_released = ~buttons &  prev_buttons;
    prev_buttons = buttons;

    // Process pressed buttons

    if(buttons_pressed & BUTTON_TM_AUTOFOCUS)
    {
        emit prev_channel();
    }
    if(buttons_pressed & BUTTON_TM_LRF_START)
    {
        emit next_channel();
    }

    if(buttons_pressed & BUTTON_TM_FOCUS_IN)
    {
        emit autofocus();
    }
    // if(buttons_pressed & BUTTON_TM_FOCUS_IN)
    // {
    //     emit focus_near();
    // }
    // else if(buttons_pressed & BUTTON_TM_FOCUS_OUT)
    // {
    //     emit focus_far();
    // }

    if(buttons_pressed & BUTTON_TM_FREE_TARGET)
    {
        emit target_free();
    }
    if(buttons_pressed & BUTTON_TM_CAPTURE_TARGET)
    {
        emit target_capture();
    }

    if(buttons_pressed & BUTTON_TM_ZOOM_FIX_WIDTH)
    {
        emit zoom_pos_fix_width();
    }
    else
    if(buttons_pressed & BUTTON_TM_ZOOM_FIX)
    {
        emit zoom_pos_fix();
    }

    if(buttons_pressed & BUTTON_TM_SWITCH_TRACKER)
    {
        target_enable_switch();
    }
    if(buttons_pressed & BUTTON_TM_MWIR_INVERSION)
    {
        emit inversion();
    }
    if(buttons_pressed & BUTTON_TM_VIDEORECORD)
    {
        emit videoRecordSwitch();
    }
    if(buttons_pressed & BUTTON_TM_ZERO)
    {
        emit gsp_null_position();
    }
    if(buttons_pressed & BUTTON_TM_STAB)
    {
        emit gsp_stabilization_switch();
    }
    if(buttons_pressed & BUTTON_TM_FIXANGLE)
    {
        emit gspFixAngleSwitch();
    }

    // Process released buttons

    // if(buttons_released & BUTTON_TM_FOCUS_OUT)
    // {
    //     emit focus_stop();
    // }
    // else
    // if(buttons_released & BUTTON_TM_FOCUS_IN)
    // {
    //     emit focus_stop();
    // }
}
void Joystick::check_thrustmaster_hats()
{
    int hats = SDL_JoystickGetHat(joystick, 0);
    int hats_pressed  =  hats & ~prev_hats;
    int hats_released = ~hats &  prev_hats;
    prev_hats = hats;

    if(hats_pressed & SDL_HAT_UP)
    {
        emit zoom_in();
    } else if(hats_released & SDL_HAT_UP)
    {
        emit zoom_stop();
    }

    if(hats_pressed & SDL_HAT_DOWN)
    {
        emit zoom_out();
    } else if(hats_released & SDL_HAT_DOWN)
    {
        emit zoom_stop();
    }

    if(hats_pressed & SDL_HAT_RIGHT)
    {
        emit focus_near();
    } else if(hats_released & SDL_HAT_RIGHT)
    {
        emit focus_stop();
    }

    if(hats_pressed & SDL_HAT_LEFT)
    {
        emit focus_far();
    } else if(hats_released & SDL_HAT_LEFT)
    {
        emit focus_stop();
    }

    // qInfo(logJoystick(), QString("HAT %1").arg(next_hat));
}

void Joystick::check_machenike()
{
    // // Polls for currently pending events
    // SDL_Event joystick_events;
    // SDL_PollEvent(&joystick_events);

    check_machenike_axis();
    check_machenike_hats();
    check_machenike_buttons();
}

void Joystick::check_machenike_axis()
{
    // Get joystick trackball coordinates
    int next_x_coord = SDL_JoystickGetAxis(joystick, 1);
    int next_z_coord = SDL_JoystickGetAxis(joystick, 0);

    int next_sizeUp = SDL_JoystickGetAxis(joystick, 2);
    int next_sizeDown = SDL_JoystickGetAxis(joystick, 5);

    // int next_x_coord = SDL_JoystickGetAxis(joystick, 6);
    // int next_z_coord = SDL_JoystickGetAxis(joystick, 4);
    // int next_3_coord = SDL_JoystickGetAxis(joystick, 5);
    //    qInfo(logJoystick(), QString::number(next_x_coord).toStdString().c_str());

    double next_x_coord_d = (next_x_coord < 0) ? (double(next_x_coord) / double(-MIN_TRACKBALL_COORD_MACHENIKE)) : (double(next_x_coord) / double(MAX_TRACKBALL_COORD_MACHENIKE));
    double next_z_coord_d = (next_z_coord < 0) ? (double(next_z_coord) / double(-MIN_TRACKBALL_COORD_MACHENIKE)) : (double(next_z_coord) / double(MAX_TRACKBALL_COORD_MACHENIKE));

    double next_sizeUp_d   = (next_sizeUp < 0) ?   (double(next_sizeUp)   / double(-MIN_TRACKBALL_COORD_MACHENIKE)) : (double(next_sizeUp)   / double(MAX_TRACKBALL_COORD_MACHENIKE));
    double next_sizeDown_d = (next_sizeDown < 0) ? (double(next_sizeDown) / double(-MIN_TRACKBALL_COORD_MACHENIKE)) : (double(next_sizeDown) / double(MAX_TRACKBALL_COORD_MACHENIKE));

    const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD) / double(MAX_TRACKBALL_COORD_MACHENIKE);
    if(fabs(next_x_coord_d) <= min_sence_trackball_coord) next_x_coord_d = 0.0;
    if(fabs(next_z_coord_d) <= min_sence_trackball_coord) next_z_coord_d = 0.0;

    const double min_sence_trackball_coord2 = double(MIN_SENCE_TRACKBALL_COORD) / double(MAX_TRACKBALL_COORD_MACHENIKE);
    if(fabs(next_sizeUp_d)   <= min_sence_trackball_coord2) next_sizeUp_d   = 0.0;
    if(fabs(next_sizeDown_d) <= min_sence_trackball_coord2) next_sizeDown_d = 0.0;

    next_x_coord_d = copysign(pow(next_x_coord_d, 2), next_x_coord);
    next_z_coord_d = copysign(pow(next_z_coord_d, 2), next_z_coord);

    next_sizeUp_d   = pow(next_sizeUp_d,   5);
    next_sizeDown_d = pow(next_sizeDown_d, 5);

    x_coord =  exp_filter(x_coord, next_x_coord_d, TRACKBALL_COORD_EXP_FILTER);
    z_coord =  exp_filter(z_coord, next_z_coord_d, TRACKBALL_COORD_EXP_FILTER);

    sizeUp =    exp_filter(sizeUp,   next_sizeUp_d,   TRACKBALL_COORD_EXP_FILTER);
    sizeDown =  exp_filter(sizeDown, next_sizeDown_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(x_coord) <= min_sence_trackball_coord) x_coord = 0.0;
    if(fabs(z_coord) <= min_sence_trackball_coord) z_coord = 0.0;

    if(fabs(sizeUp)   <= min_sence_trackball_coord2) sizeUp   = 0.0;
    if(fabs(sizeDown) <= min_sence_trackball_coord2) sizeDown = 0.0;

    // Process trackball
    QString s_trackball;
    static bool flag = false;
    double x_coord_out{-x_coord};
    double z_coord_out{z_coord};

    double y_coord_out{sizeUp};
    double y_coord_out2{sizeDown};
    if((x_coord_out != 0.0) || (z_coord_out != 0.0))
    {
        emit gsp_speed_set(x_coord_out * _x_inverted_koef, z_coord_out * _z_inverted_koef);
        s_trackball += QString("X_COORD = ") + QString::number(x_coord_out) + QString(" ");
        s_trackball += QString("Z_COORD = ") + QString::number(z_coord_out) + QString(" ");
        flag = true;
    }
    else if(flag)
    {
        emit gsp_speed_set(x_coord_out, z_coord_out);
        s_trackball += QString("X_COORD = ") + QString::number(x_coord_out) + QString(" ");
        s_trackball += QString("Z_COORD = ") + QString::number(z_coord_out) + QString(" ");
        flag = false;
    }

    bool priority = false;
    static double buff1 = 0.0;
    if(y_coord_out >= -0.8)
    {
        buff1 += 0.1;
        if(buff1 > 1.5) {
            buff1 = 1.5;
        }
        emit target_window_set(buff1);
        // emit target_window_set(abs(y_coord_out));
        s_trackball += QString("Y_COORD = ") + QString::number(y_coord_out) + QString(" ");
        priority = true;
    }
    else
    {
        buff1 = 0.0;
        priority = false;
    }

    static double buff2 = 0.0;
    if(!priority) {
        if(y_coord_out2 >= -0.8) {
            buff2 -= 0.1;
            if(buff2 < -1.0) {
                buff2 = -1.0;
            }
            emit target_window_set(buff2);
            // emit target_window_set(-abs(y_coord_out));
            s_trackball += QString("Y_COORD = ") + QString::number(y_coord_out) + QString(" ");
        } else {
            buff2 = 0.0;
        }
    }
    // if(y_coord_out != 0.0)
    // {
    //     emit target_window_set(y_coord_out);
    //     s_trackball += QString("Y_COORD = ") + QString::number(y_coord_out) + QString(" ");
    // } else if(y_coord_out2 != 0.0) {
    //     emit target_window_set(y_coord_out);
    //     s_trackball += QString("Y_COORD = ") + QString::number(y_coord_out) + QString(" ");
    // }
    //    if(s_trackball != "") qInfo(logJoystick(), s_trackball.toStdString().c_str());

    check_machenike_axisTwo();
}

void Joystick::check_machenike_hats()
{
    // int next_hat = SDL_JoystickGetHat(joystick, 0);
    int hats = SDL_JoystickGetHat(joystick, 0);
    int hats_pressed  =  hats & ~prev_hats;
    int hats_released = ~hats &  prev_hats;
    prev_hats = hats;

    // if(hats_released & SDL_HAT_CENTERED) {

    // } else {
        if(hats_released & SDL_HAT_UP) {
            emit zoom_pos_fix_width();
        } else if (hats_released & SDL_HAT_DOWN) {
            emit zoom_pos_fix();
        } else if (hats_released & SDL_HAT_RIGHT) {
            emit next_channel();
        } else if (hats_released & SDL_HAT_LEFT) {
            emit prev_channel();
        }
    // }
    // qInfo(logJoystick(), QString("HAT %1").arg(next_hat));
}

void Joystick::check_machenike_buttons()
{
    // Get joystick buttons mask
    int buttons = 0;
    for(int i = 0; i < MAX_JOYSTICK_BUTTON; ++i)
    {
        if(SDL_JoystickGetButton(joystick, i))
        {
            buttons |= (1 << i);
        }
    }
    int buttons_pressed  =  buttons & ~prev_buttons;
    int buttons_released = ~buttons &  prev_buttons;
    prev_buttons = buttons;

    /** Костыль для трекера */
    static bool trackerEn = false;

    QString s_buttons;
    // Process pressed buttons

    if(buttons_released & BUTTON_VIDEO_RECORD)
    {
        emit videoRecordSwitch();
        s_buttons += QString("BUTTON_VIDEO_RECORD") + QString(" ");
    }
    if(buttons_pressed & BUTTON_FIX_ANGLE)
    {
        emit gspFixAngleSwitch();
        s_buttons += QString("BUTTON_ZOOM_OUT pressed") + QString(" ");
    }
    if(buttons_pressed & BUTTON_STAB)
    {
        emit gsp_stabilization_switch();
        s_buttons += QString("BUTTON_STAB pressed") + QString(" ");
    }
    if(buttons_pressed & BUTTON_ZERO)
    {
        emit gsp_null_position();
        s_buttons += QString("BUTTON_ZERO pressed") + QString(" ");
    }
    if(buttons_pressed & BUTTON_INVERSION_MW)
    {
        emit inversion();
        s_buttons += QString("BUTTON_INVERSION_MW pressed") + QString(" ");
    }

    if(buttons_pressed & BUTTON_LRF_START)
    {
        emit lrf_start();
        s_buttons += QString("BUTTON_LRF_START pressed") + QString(" ");
    }
    if(buttons_pressed & BUTTON_SWITCH_TRACKER)
    {
        emit target_enable_switch();
        s_buttons += QString("BUTTON_SWITCH_TRACKER pressed") + QString(" ");
    }

    if(buttons_released & BUTTON_AUTOFOCUS)
    {
        emit autofocus();
        s_buttons += QString("BUTTON_AUTOFOCUS") + QString(" ");
    }
    if(buttons_released & BUTTON_TARGET_CAPTURE)
    {
        if( !trackerEn) {
            emit target_capture();
            s_buttons += QString("BUTTON_TARGET_CAPTURE") + QString(" ");
        } else {
            emit target_free();
            s_buttons += QString("BUTTON_TARGET_FREE") + QString(" ");
        }
        trackerEn = !trackerEn;
    }

    if(s_buttons != "") qInfo(logJoystick(), s_buttons.toStdString().c_str());
}

void Joystick::check_machenike_axisTwo()
{
    // Get joystick trackball coordinates
    int next_zoom_coord = SDL_JoystickGetAxis(joystick, 4);
    int next_focus_coord = SDL_JoystickGetAxis(joystick, 3);

    double next_zoom_coord_d = (next_zoom_coord < 0) ? (double(next_zoom_coord) / double(-MIN_TRACKBALL_COORD_MACHENIKE)) : (double(next_zoom_coord) / double(MAX_TRACKBALL_COORD_MACHENIKE));
    double next_focus_coord_d = (next_focus_coord < 0) ? (double(next_focus_coord) / double(-MIN_TRACKBALL_COORD_MACHENIKE)) : (double(next_focus_coord) / double(MAX_TRACKBALL_COORD_MACHENIKE));

    const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD) / double(MAX_TRACKBALL_COORD_MACHENIKE);
    if(fabs(next_zoom_coord_d) <= min_sence_trackball_coord) next_zoom_coord_d = 0.0;
    if(fabs(next_focus_coord_d) <= min_sence_trackball_coord) next_focus_coord_d = 0.0;

    next_zoom_coord_d = copysign(pow(next_zoom_coord_d, 2), next_zoom_coord);
    next_focus_coord_d = copysign(pow(next_focus_coord_d, 2), next_focus_coord);

    zoom_coord =  exp_filter(zoom_coord, next_zoom_coord_d, TRACKBALL_COORD_EXP_FILTER);
    focus_coord =  exp_filter(focus_coord, next_focus_coord_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(zoom_coord) <= min_sence_trackball_coord) zoom_coord = 0.0;
    if(fabs(focus_coord) <= min_sence_trackball_coord) focus_coord = 0.0;



    // Process trackball
    QString s_trackball;
    static bool flagZoom = false;
    static bool flagFocus = false;
    double zoom_coord_out{zoom_coord};
    double focus_coord_out{focus_coord};

    constexpr double squelch = 0.8; // порог
    // ZOOM
    if(zoom_coord_out >= squelch)
    {
        emit zoom_in();
        flagZoom = true;
    }
    else if(zoom_coord_out <= -squelch)
    {
        emit zoom_out();
        flagZoom = true;
    }
    else if(flagZoom)
    {
        emit zoom_stop();
        flagZoom = false;
    }

    // FOCUS
    if(focus_coord_out >= squelch)
    {
        emit focus_near();
        flagFocus = true;
    }
    else if(focus_coord_out <= -squelch)
    {
        emit focus_far();
        flagFocus = true;
    }
    else if(flagFocus)
    {
        emit focus_stop();
        flagFocus = false;
    }
}

void Joystick::check_logitech_events()
{
    // // Polls for currently pending events
    // SDL_Event joystick_events;
    // SDL_PollEvent(&joystick_events);

    check_logitech_axis();
    check_logitech_hats();
    check_logitech_buttons();
}

void Joystick::check_logitech_axis()
{
    // Get joystick trackball coordinates
    int next_x_coord = SDL_JoystickGetAxis(joystick, 1);
    int next_z_coord = SDL_JoystickGetAxis(joystick, 0);
    int next_y_coord = SDL_JoystickGetAxis(joystick, 2);

    double next_x_coord_d = (next_x_coord < 0) ? (double(next_x_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_x_coord) / double(MAX_TRACKBALL_COORD));
    double next_z_coord_d = (next_z_coord < 0) ? (double(next_z_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_z_coord) / double(MAX_TRACKBALL_COORD));
    double next_y_coord_d = (next_y_coord < 0) ? (double(next_y_coord) / double(-MIN_TRACKBALL_COORD)) : (double(next_y_coord) / double(MAX_TRACKBALL_COORD));

    const double min_sence_trackball_coord = double(MIN_SENCE_TRACKBALL_COORD_TM) / double(MAX_TRACKBALL_COORD);
    if(fabs(next_x_coord_d) <= min_sence_trackball_coord) next_x_coord_d = 0.0;
    if(fabs(next_z_coord_d) <= min_sence_trackball_coord) next_z_coord_d = 0.0;
    if(fabs(next_y_coord_d) <= min_sence_trackball_coord) next_y_coord_d = 0.0;

    next_x_coord_d = pow(next_x_coord_d, 5);
    next_z_coord_d = pow(next_z_coord_d, 5);
    next_y_coord_d = pow(next_y_coord_d, 5);

    x_coord = exp_filter(x_coord, next_x_coord_d, TRACKBALL_COORD_EXP_FILTER);
    z_coord = exp_filter(z_coord, next_z_coord_d, TRACKBALL_COORD_EXP_FILTER);
    y_coord = exp_filter(y_coord, next_y_coord_d, TRACKBALL_COORD_EXP_FILTER);

    if(fabs(x_coord) <= min_sence_trackball_coord) x_coord = 0.0;
    if(fabs(z_coord) <= min_sence_trackball_coord) z_coord = 0.0;
    if(fabs(y_coord) <= min_sence_trackball_coord) y_coord = 0.0;

    // Process trackball
    QString s_trackball;
    double x_coord_out{-x_coord};
    double z_coord_out{z_coord};
    double y_coord_out{y_coord};
    static bool flag = false;
    if((x_coord_out != 0.0) || (z_coord != 0.0))
    {
        emit gsp_speed_set(_x_inverted_koef * x_coord_out, _z_inverted_koef * z_coord_out);
        flag = true;
    }
    else if(flag)
    {
        emit gsp_speed_set(x_coord_out, z_coord_out);
        flag = false;
    }
    if(y_coord_out != 0.0)
    {
        emit target_window_set(y_coord_out);
    }
}

void Joystick::check_logitech_hats()
{
    int hats = SDL_JoystickGetHat(joystick, 0);
    int hats_pressed  =  hats & ~prev_hats;
    int hats_released = ~hats &  prev_hats;
    prev_hats = hats;

    if(hats_pressed & SDL_HAT_UP)
    {
        emit zoom_in();
    } else if(hats_released & SDL_HAT_UP)
    {
        emit zoom_stop();
    }

    if(hats_pressed & SDL_HAT_DOWN)
    {
        emit zoom_out();
    } else if(hats_released & SDL_HAT_DOWN)
    {
        emit zoom_stop();
    }

    if(hats_pressed & SDL_HAT_RIGHT)
    {
        emit focus_near();
    } else if(hats_released & SDL_HAT_RIGHT)
    {
        emit focus_stop();
    }

    if(hats_pressed & SDL_HAT_LEFT)
    {
        emit focus_far();
    } else if(hats_released & SDL_HAT_LEFT)
    {
        emit focus_stop();
    }

    // qInfo(logJoystick(), QString("HAT %1").arg(next_hat));
}

void Joystick::check_logitech_buttons()
{
    // Get joystick buttons mask
    int buttons = 0;
    for(int i = 0; i < MAX_LGCH_JOYSTICK_BUTTON; ++i)
    {
        if(SDL_JoystickGetButton(joystick, i))
        {
            buttons |= (1 << i);
        }
    }

    int buttons_pressed  =  buttons & ~prev_buttons;
    int buttons_released = ~buttons &  prev_buttons;
    prev_buttons = buttons;

    // Process pressed buttons

    if(buttons_pressed & BUTTON_LGCH_PREV_CHANNEL)
    {
        emit prev_channel();
    }
    if(buttons_pressed & BUTTON_LGCH_NEXT_CHANNEL)
    {
        emit next_channel();
    }

    // if(buttons_pressed & BUTTON_LGCH_FOCUS_IN)
    // {
    //     emit autofocus();
    // }
    // if(buttons_pressed & BUTTON_LGCH_FOCUS_IN)
    // {
    //     emit focus_near();
    // }
    // else if(buttons_pressed & BUTTON_LGCH_FOCUS_OUT)
    // {
    //     emit focus_far();
    // }

    if(buttons_pressed & BUTTON_LGCH_FREE_TARGET)
    {
        emit target_free();
    }
    if(buttons_pressed & BUTTON_LGCH_CAPTURE_TARGET)
    {
        emit target_capture();
    }

    if(buttons_pressed & BUTTON_LGCH_ZOOM_FIX_WIDTH)
    {
        emit zoom_pos_fix_width();
    }
    else
        if(buttons_pressed & BUTTON_LGCH_ZOOM_FIX)
        {
            emit zoom_pos_fix();
        }

    // if(buttons_pressed & BUTTON_LGCH_SWITCH_TRACKER)
    // {
    //     target_enable_switch();
    // }
    if(buttons_pressed & BUTTON_LGCH_MWIR_INVERSION)
    {
        emit inversion();
    }
    if(buttons_pressed & BUTTON_LGCH_AUTOFOCUS)
    {
        emit autofocus();
    }
    if(buttons_pressed & BUTTON_LGCH_VIDEORECORD)
    {
        emit videoRecordSwitch();
    }
    if(buttons_pressed & BUTTON_LGCH_ZERO)
    {
        emit gsp_null_position();
    }
    if(buttons_pressed & BUTTON_LGCH_STAB)
    {
        emit gsp_stabilization_switch();
    }
    if(buttons_pressed & BUTTON_LGCH_FIXANGLE)
    {
        emit gspFixAngleSwitch();
    }

    // Process released buttons

    // if(buttons_released & BUTTON_LGCH_FOCUS_OUT)
    // {
    //     emit focus_stop();
    // }
    // else
    // if(buttons_released & BUTTON_LGCH_FOCUS_IN)
    // {
    //     emit focus_stop();
    // }
}
