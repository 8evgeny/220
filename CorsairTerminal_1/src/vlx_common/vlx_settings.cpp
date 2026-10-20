#include "vlx_settings.h"

VlxSettings::VlxSettings()
    : QSettings("VLX", "CorsairTerminal_")
{}


quint16 VlxSettings::networkPort() const
{
    return (quint16)value(K::network_port, D::rtp_mux_port).toUInt();
}

void VlxSettings::setNetworkPort(quint16 port)
{
    if (port != networkPort())
        setValue(K::network_port, port);
}

int VlxSettings::hwIndex() const
{
    return value(K::hw_idx, D::hw_idx).toInt();
}

void VlxSettings::setHwIndex(int index)
{
    if (index != hwIndex())
        setValue(K::hw_idx, index);
}

int VlxSettings::devId() const
{
    return value(K::dev_id, D::dev_id).toInt();
}

void VlxSettings::setDevId(int id)
{
    if (id != devId())
        setValue(K::dev_id, id);
}

int VlxSettings::vstream1Pid() const
{
    return value(K::vstream_1_pid, D::vstream_1_pid).toInt();
}

void VlxSettings::setVStream1Pid(int pid)
{
    if (pid != vstream1Pid())
        setValue(K::vstream_1_pid, pid);
}

int VlxSettings::vstream2Pid() const
{
    return value(K::vstream_2_pid, D::vstream_2_pid).toInt();
}

void VlxSettings::setVStream2Pid(int pid)
{
    if (pid != vstream2Pid())
        setValue(K::vstream_2_pid, pid);
}

int VlxSettings::dstreamPid() const
{
    return value(K::dstream_pid, D::dstream_pid).toInt();
}

void VlxSettings::setDStreamPid(int pid)
{
    if (pid != dstreamPid())
        setValue(K::dstream_pid, pid);
}

int VlxSettings::avLogLevel()
{
    return value(K::av_log_level, D::av_log_level).toInt();
}

void VlxSettings::setAvLogLevel(int lv)
{
    if (lv != avLogLevel())
        setValue(K::av_log_level, lv);
}

bool VlxSettings::dispAlignmentCross() const
{
    return value(K::disp_alignment_cross, D::disp_alignment_cross).toBool();
}

void VlxSettings::setDispAlignmentCross(bool en)
{
    if (en != dispAlignmentCross())
        setValue(K::disp_alignment_cross, en);
}

int VlxSettings::joystickPollIntervalMs() const
{
    return value(K::joystick_poll_interval_ms, D::joystick_poll_interval_ms).toInt();
}

void VlxSettings::setJoystickPollInterval(int ms)
{
    if (joystickPollIntervalMs() != ms)
        setValue(K::joystick_poll_interval_ms, ms);
}

bool VlxSettings::useVirtualJoystick() const
{
    return value(K::use_virtual_joystick, D::use_virtual_joystick).toBool();
}

void VlxSettings::setVirtualJoystickUsage(bool set)
{
    if (useVirtualJoystick() != set)
        setValue(K::use_virtual_joystick, set);
}


VlxSettings &VlxSettings::i()
{
    static VlxSettings s;
    return s;
}
