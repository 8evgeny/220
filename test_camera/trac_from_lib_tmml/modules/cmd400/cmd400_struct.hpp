#ifndef CMD400_STRUCT_HPP
#define CMD400_STRUCT_HPP

#include <stdint.h>

#pragma pack(push,1)
struct Cmd400_init
{
    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;
    uint8_t buf_zeros[64];
}; // END Cmd400_init


struct Cmd400_deinit
{
  uint8_t buf_zeros[64];
}; // END Cmd400_deinit

struct Cmd400_record_on
{
    uint8_t buf_zeros[64];

}; // END Cmd400_record_on



#pragma pack(pop)

#endif //CMD400_STRUCT_HPP
