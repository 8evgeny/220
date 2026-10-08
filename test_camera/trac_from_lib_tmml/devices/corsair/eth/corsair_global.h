#ifndef CORSAIR_GLOBAL_H
#define CORSAIR_GLOBAL_H

#include <cstdint>

#define CORSAIR_UDP_CMD_DATA_LEN    (512)
#define CORSAIR_UDP_CMD_SRV_LEN     (4)
#define CORSAIR_UDP_CMD_LEN         (516)
#define CORSAIR_UDP_CMD_FLAG        (0x0100u)

#define CORSAIR_UDP_SRV_DATA_LEN    (8)
#define CORSAIR_UDP_SRV_SRV_LEN     (4)
#define CORSAIR_UDP_SRV_LEN         (12)
#define CORSAIR_UDP_SRV_FLAG        (0x0200u)

#define CORSAIR_UDP_VID_DATA_LEN    (5120)
//#define CORSAIR_UDP_VID_DATA_LEN         (1280) // Sitara
#define CORSAIR_UDP_VID_SRV_LEN     (6)
#define CORSAIR_UDP_VID_LEN         (5126) //
//#define CORSAIR_UDP_VID_LEN         (1286) // Sitara
#define CORSAIR_UDP_VID_FLAG        (0x0300u)

typedef struct _cmd_packet {
    uint16_t preambule;
    uint16_t flag;
    uint8_t data[];
} cmd_packet;
typedef struct _srv_packet {
    uint16_t preambule;
    uint16_t flag;
    uint16_t frame_number;
    uint16_t w;
    uint16_t h;
    uint16_t id;
} srv_packet;
typedef struct _vid_packet {
    uint16_t preambule;
    uint16_t flag;
    uint16_t fragment_number;
    uint8_t data[];
} vid_packet;

#endif // CORSAIR_GLOBAL_H
