#ifndef RTP_CLIENT_SETTINGS
#define RTP_CLIENT_SETTINGS

#include <string>
#include <memory>
#include <iostream>

#include "INIReader.h"
#include "eth_cnt_common_utils/eth_cnt_common_utils.hpp"
#include "toolbox_common_utils/toolbox_common_utils.hpp"

namespace rtp::client
{

    struct VideoSettings
    {
        std::string format = "BGR";
        int width = 1024;
        int height = 600;
        int framerate = 25;
    };

    struct ParserSettings
    {
        int configInterval = 1; /// Send SPS and PPS Insertion Interval in seconds
        bool updateTimecode = false;
    };

    struct DecoderSettings
    {
        bool directRendering = false; /// Enable direct rendering
        int lowres = 0; /// At which resolution to decode images
        int maxThreads = 0; /// Whether libav should output frames even if corrupted
    };

    struct DepaySettings
    {
        bool requestKeyframe = false;
        bool waitForKeyframe = false;
    };

    struct UdpSettings
    {
        std::string address = "127.0.0.1"; /// Address to receive packets for. This is equivalent to the multicast-group property for now
        int bufferSize = 0;                /// Size of the kernel receive buffer in bytes
        int mtu = 1500;                    /// Maximum expected packet size
        int port = 8080;                   /// The port to receive the packets from
        int clockRate = 90000;
        std::string encodingName = "H264";
        int payload = 26;
    };

    struct WatchdogSettings
    {
        int timeout_ms = 15'000;
    };

    struct AppsinkSettings
    {
        int max_buffers = 3;
    };

    struct Settings
    {
        VideoSettings video;
        DecoderSettings decoder;
        ParserSettings parser;
        DepaySettings depay;
        UdpSettings udp;
        WatchdogSettings wtdog;
        AppsinkSettings appsink;
    };

bool getIniSettings(const std::string &path2ini,
                    const std::string &ini_section_name,
                    Settings &settings);

bool checkValidIniSettings(Settings &settings);


    using SettingsPtr = std::shared_ptr<Settings>;
}

#endif // RTP_SETTINGS
