#ifndef CMD_VERTELKA_STRUCT_HPP
#define CMD_VERTELKA_STRUCT_HPP

#include <iostream>

#pragma pack(push,1)

struct CmdVertelka_close
{
    uint8_t buf_zeros[64];
};

struct CmdVertelka_run_10
{
    uint8_t buf_zeros[64];
};


struct CmdVertelka_run_8
{
    uint8_t buf_zeros[64];
};

#pragma pack(pop)

#endif //CMD_VERTELKA_STRUCT_HPP
