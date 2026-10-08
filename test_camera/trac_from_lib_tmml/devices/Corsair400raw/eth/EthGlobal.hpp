#ifndef ETHGLOBAL_H
#define ETHGLOBAL_H

#include <cstdint>

#if defined(_WIN32) && defined(ETH_LIBRARY)
    #ifdef ETH_LIBRARY
        // windows
        #define ETHSHARED_EXPORT __declspec(dllexport)
    #else
        // Using a Windows DLL
        #define ETHSHARED_EXPORT __declspec(dllimport)
    #endif
#else // Windows or Linux static library, or Linux so
    #define ETHSHARED_EXPORT
#endif


#ifdef DEBUG
#include <iostream>
#define ERROR_MSG(char_array) {std::cerr << "ETH: " << char_array << std::endl;}
#define LOG_MSG(char_array, separator)   {std::clog << char_array << separator;}
#else
#define ERROR_MSG(char_array)
#define LOG_MSG(char_array, separator)
#endif

/******************************************
 *  Using default UDP packet parametrs
 ******************************************/
/** Data packet */
#define UDP_CMD_FLAG        (0x0100u)
#define UDP_CMD_DATA_LEN    (20)
#define UDP_CMD_SRV_LEN     (4)
#define UDP_CMD_LEN         (18)//(516)
/** Service packet (pre frame) */
#define UDP_SRV_FLAG        (0x0200u)
#define UDP_SRV_DATA_LEN    (8)
#define UDP_SRV_SRV_LEN     (4)
#define UDP_SRV_LEN         (12)
/** Video packet */
#define UDP_VID_FLAG        (0x0300u)
#define UDP_VID_SRV_LEN     (6)
/* Host */
#define UDP_VID_DATA_LEN    (5120)
#define UDP_VID_LEN         (5126)
/* Sitara */
#define UDP_VID_DATA_LEN_SITARA (1280)
#define UDP_VID_LEN_SITARA      (1286)

/******************************************
 *  parametrs
 ******************************************/
#define CMD_BUF_LEN     (1024)
#define FRAME_BUF_W     (1920)
#define FRAME_BUF_H     (1080)
#define FRAME_BUF_LEN   (FRAME_BUF_W * FRAME_BUF_H)
/**
 *  UDP packet parametrs
 */
struct UDPPacketParametrs {
    /** packet Flag */
    uint32_t flag;
    /** UDP packet length */
    uint32_t length;
    /** payload header packet length */
    uint32_t headerLength;
    /** UDP payload length */
    uint32_t dataLength;

};
/**
 *  UDP packet parametrs
 */
struct FrameParametrs
{
    /** Service packet receive Flag */
    bool frameStart;
    /** Current frame fragment number */
    int32_t currFragment;
    /** Number of fragments in a frame */
    int32_t fragments;
    /** Frame width [pixel] */
    int32_t width;
    /** Frame heigh [pixel] */
    int32_t height;
    /** Channel id */
    int32_t id;
    /** External counter frames */
    int32_t number;
};
/******************************************
 *  Payload headers
 ******************************************/
/**
 *  Data packet header
 */
#pragma pack(push, 1)
typedef struct _cmd_packet {
    uint16_t preambule;
    uint16_t flag;
    uint8_t data[];
} cmd_packet;
#pragma pack(pop)
/**
 *  Service packet header
 */
#pragma pack(push, 1)
typedef struct _srv_packet {
    uint16_t preambule;
    uint16_t flag;
    uint16_t frameNumber;
    uint16_t width;
    uint16_t heigh;
    uint16_t id;
} srv_packet;
#pragma pack(pop)
/**
 *  Video packet header
 */
#pragma pack(push, 1)
typedef struct _vid_packet {
    uint16_t preambule;
    uint16_t flag;
    uint16_t fragment_number;
    uint8_t  data[];
} vid_packet;
#pragma pack(pop)


#endif // ETHGLOBAL_H
