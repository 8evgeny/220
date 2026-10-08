#ifndef PCAPSOCKET_H
#define PCAPSOCKET_H

#include "eth/base/Socket.hpp"
#include <pcap.h>
#include <vector>

typedef struct _ethernet_header {
    uint8_t to_addr[6];         // mac address of source
    uint8_t from_addr[6];       // mac address of destination
    uint16_t type;              // type of protocol/length of data
} ethernet_header; // 14B
typedef struct _ip_header {
    uint8_t ver_ihl;            // Version (4 bits) + Internet header length (4 bits)
    uint8_t tos;                // Type of service
    uint16_t tlen;              // Total length
    uint16_t identification;    // Identification
    uint16_t flags_fo;          // Flags (3 bits) + Fragment offset (13 bits)
    uint8_t ttl;                // Time to live
    uint8_t proto;              // Protocol
    uint16_t crc;               // Header checksum
    uint8_t saddr[4];           // Source address
    uint8_t daddr[4];           // Destination address
} ip_header; // 20B
typedef struct _udp_header {
    uint16_t sport;             // Source port
    uint16_t dport;             // Destination port
    uint16_t len;               // Datagram length
    uint16_t crc;               // Checksum
} udp_header; // 8B
typedef struct _ethernet_packet {
    ethernet_header hdr;
    uint8_t data[];
} ethernet_packet;
typedef struct _ip_packet {
    ip_header hdr;
    uint8_t data[];
} ip_packet;
typedef struct _udp_packet {
    udp_header hdr;
    uint8_t data[];
} udp_packet;
typedef struct _udp_ip_packet {
    ethernet_header eth_hdr;
    ip_header ip_hdr;
    udp_header udp_hdr;
    uint8_t data[];
} udp_ip_packet;

class PcapSocket: public Socket
{
public:
    PcapSocket():_adhandle(nullptr){}
    bool open(const SocketAddress & address) override {_socketAddress = address;return opn();}
    void close_() override;

    bool isOpen() const override {return (_adhandle != nullptr);}

    bool setBroadcast(const bool &) override {return true;}
    bool setReusable(const bool &) override {return true;}
    bool setNonBlocking(const bool & non_blocking) override;
    bool set_timeout(const int & sec, const int & usec) override;
    bool set_buffer_size(const int & size) override;

    bool sndto(const SocketAddress & dst, const void * data, const int & size);
    const uint8_t * rcvfrom(SocketAddress & src, int & size);

    static std::vector<std::string> devices();

    static uint16_t checksum(uint16_t * data, const int & data_len);
private:
    bool open() override;
    void fill_headers();
private:
    pcap_t * _adhandle;
};

#endif // PCAPSOCKET_H
