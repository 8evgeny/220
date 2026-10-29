
#ifndef VDB_PROTOCOL_H
#define VDB_PROTOCOL_H


#ifdef __cplusplus
    extern "C"
    {
#endif


    #include <stdint.h>

    #define VDB__CMD__WRITE_REQUEST     (0x00)
    #define VDB__CMD__WRITE_RESPONSE    (0x80)
    #define VDB__CMD__READ_REQUEST      (0x01)
    #define VDB__CMD__READ_RESPONSE     (0x81)

    #define VDB__REQUEST_TYPE__REQUEST_WITHOUT_RESPONSE     (0x00)
    #define VDB__REQUEST_TYPE__REQUEST_WITH_RESPONSE        (0x80)


    typedef struct
    {
        uint8_t     function;
        uint8_t     device;
    }
    __attribute__((packed)) VdbAddress;


    typedef struct
    {
        uint8_t     cmd;
        uint8_t     status;

        VdbAddress  src;
        VdbAddress  dst;

        uint16_t    byteCount;
    }
    __attribute__((packed)) VdbHeader;



#ifdef __cplusplus
    }
#endif


#endif

