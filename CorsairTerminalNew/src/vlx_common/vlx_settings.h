#ifndef VLX_SETTINGS_H
#define VLX_SETTINGS_H

#include <QSettings>
#include "device_parts/vlx/vlx_media/vlx_av.h"

class VlxSettings : public QSettings
{
    VlxSettings();

    struct K {
        static constexpr auto hw_idx = "hw_idx";
        static constexpr auto dev_id = "dev_id";
        static constexpr auto network_port = "network_port";
        static constexpr auto vstream_1_pid = "vstream_1_pid";
        static constexpr auto vstream_2_pid = "vstream_2_pid";
        static constexpr auto dstream_pid = "dstream_pid";
        static constexpr auto av_log_level = "av_log_level";
        static constexpr auto disp_alignment_cross = "disp_alignment_cross";
        static constexpr auto joystick_poll_interval_ms = "joystick_poll_interval_ms";
        static constexpr auto use_virtual_joystick = "use_virtual_joystick";
    };

    struct D {
        static constexpr auto hw_idx = Vlx::Av::Params::D::hw_accel_index;
        static constexpr auto dev_id = Vlx::Av::Params::D::hw_accel_dev_id;
        static constexpr quint16 rtp_mux_port = 5016;
        static constexpr auto vstream_1_pid = 256;
        static constexpr auto vstream_2_pid = 257;
        static constexpr auto dstream_pid = 512;
        static constexpr auto av_log_level = Vlx::Av::Params::D::avLogLevel;
        static constexpr auto disp_alignment_cross = false;
        static constexpr auto joystick_poll_interval_ms = 50;
        static constexpr auto use_virtual_joystick = false;
    };

public:
    static VlxSettings &i();

    quint16 networkPort() const;
    void setNetworkPort(quint16 port);

    int hwIndex() const;
    void setHwIndex(int index);

    int devId() const;
    void setDevId(int id);

    int vstream1Pid() const;
    void setVStream1Pid(int pid);

    int vstream2Pid() const;
    void setVStream2Pid(int pid);

    int dstreamPid() const;
    void setDStreamPid(int pid);

    int avLogLevel();
    void setAvLogLevel(int lv);

    bool dispAlignmentCross() const;
    void setDispAlignmentCross(bool en);

    int joystickPollIntervalMs() const;
    void setJoystickPollInterval(int ms);

    bool useVirtualJoystick() const;
    void setVirtualJoystickUsage(bool set);
};

#endif // VLX_SETTINGS_H
