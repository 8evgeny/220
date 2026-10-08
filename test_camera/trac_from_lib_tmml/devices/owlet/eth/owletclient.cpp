#include "owletclient.h"

#include <cstdint>
#include <cstring>
#include "owlet_global.h"

#define OWLET_UDP_BUFFER_LEN  (4096)

OwletClient::OwletClient():
    buffer(new uint8_t[OWLET_UDP_BUFFER_LEN])
{
    memset(buffer, 0, OWLET_UDP_BUFFER_LEN);
}
OwletClient::~OwletClient()
{
    delete [] buffer;
}

bool OwletClient::send_cmd(const uint8_t * data, const int & data_length)
{
    cmd_packet * packet = reinterpret_cast<cmd_packet *>(buffer);
    packet->preambule = 0;
    packet->flag = OWLET_UDP_CMD_FLAG;
    memcpy(packet->data, data, size_t(data_length));
    return send(buffer, OWLET_UDP_CMD_LEN);
}
