#ifndef OWLET_GLOBAL_H
#define OWLET_GLOBAL_H

#include <cstdint>

#define OWLET_UDP_CMD_DATA_LEN    (512)
#define OWLET_UDP_CMD_SRV_LEN     (4)
#define OWLET_UDP_CMD_LEN         (516)
#define OWLET_UDP_CMD_FLAG        (0x0100u)

#define OWLET_UDP_SRV_DATA_LEN    (8)
#define OWLET_UDP_SRV_SRV_LEN     (4)
#define OWLET_UDP_SRV_LEN         (18)
#define OWLET_UDP_SRV_FLAG        (0x0200u)

#define OWLET_UDP_VID_DATA_LEN    (2560)
#define OWLET_UDP_VID_SRV_LEN     (6)
#define OWLET_UDP_VID_LEN         (2566)
#define OWLET_UDP_VID_FLAG        (0x0300u)

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

#endif // OWLET_GLOBAL_H
