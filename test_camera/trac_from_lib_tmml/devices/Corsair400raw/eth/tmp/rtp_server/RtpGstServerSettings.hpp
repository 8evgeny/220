#ifndef RTP_SERVER_SETTINGS
#define RTP_SERVER_SETTINGS

#include <string>
#include <memory>

namespace rtp::server
{
    struct VideoSettings
    {
        bool color = true;
        std::string media_type = "video/x-bayer"; // video/x-bayer | video/x-raw
        std::string format = "grbg"; // grbg, bggr (to bayer2rgb) | GRAY8(to videoconvert)
        int width = 1920;
        int height = 1080;
        int fps = 25;
    }; 

    struct EncoderSettings
    {
        int bitrate = 8000000;
        int peek_bitrate = 8388608;
        int control_rate = 1; // 0 Disable
                              // 1 Variable bit rate
                              // 2 Constant bit rate
                              // 3 Variable bit rate with frame skip. The encoder skips
                              // frames as necessary to meet the target bit rate.
                              // 4 Constant bit rate with frame skip
        bool insert_sps_pps = true;// set video parametrs in stream
        bool insert_vui = true; // Enable or disable inserting video usability information in SPS.
        int preset = 2; // 0 - DisablePreset
                        // 1 - UltraFastPreset
                        // 2 - FastPreset
                        // Only Integer Pixel (integer-pel) block motion is
                        // estimated. For I/P macroblock mode decisions, only
                        // Intra 16×16 cost is compared with intermode costs.
                        // Supports intra 16×16 and intra 4×4 modes.
                        // 3 - MediumPreset
                        // Supports up to Half Pixel (half-pel) block motion
                        // estimation. For I/P macroblock mode decisions, only
                        // Intra 16×16 cost is compared with intermode costs.
                        // Supports intra 16×16 and intra 4×4 modes.
                        // 4 - SlowPreset
                        // Supports up to Quarter Pixel (Qpel) block motion
                        // estimation. For I/P macroblock mode decisions, intra
                        // 4×4 as well as intra 16×16 cost is compared with
                        // intermode costs. Supports intra 16×16 and intra 4×4
                        // modes.
    };

    struct PaySettings
    {
        int configInterval = 1;
        int pt = 96;
        int mtu = 7000;
    };

    struct UdpSettings
    {
        std::string host = "127.0.0.1";
        int port = 5004;
    };

    struct Settings
    {
        VideoSettings video;
        EncoderSettings encoder;
        PaySettings pay;
        UdpSettings udp;
    };

    using SettingsPtr = std::shared_ptr<Settings>;
}

#endif // RTP_SETTINGS
