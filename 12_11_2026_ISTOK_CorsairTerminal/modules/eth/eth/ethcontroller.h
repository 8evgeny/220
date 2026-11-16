#ifndef ETHCONTROLLER_H
#define ETHCONTROLLER_H

#include <stdint.h>
#include <string>

class CorsairServer;
class CorsairClient;

class EthController
{
private:
    EthController();
public:
    static EthController & inst();
    ~EthController();

    bool start(const std::string & dev,
               const std::string & src_mac, const std::string & src_ip, const uint16_t & src_port,
               const std::string & dst_mac, const std::string & dst_ip, const uint16_t & dst_port);
    bool stop();

    void openMirror(const std::string src, const std::string dst);

    bool is_started();

    bool send_cmd(const uint8_t * data, const int & data_length);

    uint8_t * rcv_cmd(int & data_length);
    uint8_t * rcv_frame(int & w, int & h, int & id, int & num);
    int get_frame_num() const;

private:
    CorsairServer * server;
    CorsairClient * client;
};

#endif // ETHCONTROLLER_H
