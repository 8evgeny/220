#include "pcapsocket.hpp"

#include <iostream>
#include <cstring>
#include <sstream>

static char errbuf[PCAP_ERRBUF_SIZE];
static uint8_t buffer[8192];

#define ETH_TYPE_IP         (0x0800)
#define IP_VER_IHL          (0x45)
#define IP_FLAGS_FO         (0x4000)

bool PcapSocket::open()
{
    // Open device
    if( (_adhandle = pcap_open_live(_address.dev().c_str(), // name of the device
                                     65536,                 // portion of the packet to capture
                                                            // 65536 guarantees that the whole packet will be captured on all the link layers
                                     1,                     // promiscuous mode
                                     -1,                    // read timeout
                                     errbuf                 // error buffer
                                     )
          ) == nullptr)
    {
        std::cout << "PcapSocket::open: Error: " << errbuf << std::endl;
        return false;
    }
    // Compile the filter
    uint32_t netmask = 0xFFFF0000u;
    std::stringstream ss;
    ss << "ip && udp && (dst net " << _socketAddress.str_ip() << " || dst net 255.255.255.255) && dst port " << _socketAddress.port();
    std::string filter = ss.str();
    struct bpf_program fcode;
    if(pcap_compile(_adhandle, &fcode, filter.c_str(), 1, netmask) < 0)
    {
        std::cout << "PcapSocket::open: Error: Unable to compile the packet filter. Check the syntax " << filter << std::endl;
        return false;
    }
    // Set the filter
    if(pcap_setfilter(_adhandle, &fcode) < 0)
    {
        std::cout << "PcapSocket::open: Error: Can not set the filter " << filter << std::endl;
        return false;
    }
    fill_headers();
    return true;
}
void PcapSocket::cls()
{
    if(!is_open()) return;
    pcap_close(_adhandle);
    _adhandle = nullptr;
}
bool PcapSocket::setNonBlocking(const bool & non_blocking)
{
    if(!is_open()) return false;
    int _non_blocking = static_cast<int>(non_blocking);
    return (pcap_setnonblock(_adhandle, _non_blocking, errbuf) != PCAP_ERROR);
}
bool PcapSocket::set_timeout(const int & sec, const int & usec)
{
    if(!is_open()) return false;
    int msec = sec * 1000 + usec / 1000;
    return (pcap_set_timeout(_adhandle, msec) == 0);
}
bool PcapSocket::set_buffer_size(const int & size)
{
    if(!is_open()) return false;
    return (pcap_set_buffer_size(_adhandle, size) == 0);
}
bool PcapSocket::sndto(const SocketAddress & dst, const void * data, const int & size)
{
    if(!is_open()) return false;
    static uint16_t identification = 0;
    udp_ip_packet * packet = reinterpret_cast<udp_ip_packet *>(buffer);
    // MAC Header
    packet->eth_hdr.to_addr[0] = dst.mac()[5];
    packet->eth_hdr.to_addr[1] = dst.mac()[4];
    packet->eth_hdr.to_addr[2] = dst.mac()[3];
    packet->eth_hdr.to_addr[3] = dst.mac()[2];
    packet->eth_hdr.to_addr[4] = dst.mac()[1];
    packet->eth_hdr.to_addr[5] = dst.mac()[0];
    // IP Header
    packet->ip_hdr.tlen = htons(uint16_t(size_t(size) + sizeof(udp_header) + sizeof(ip_header)));
    packet->ip_hdr.identification = htons(identification++);
    packet->ip_hdr.daddr[0] = dst.pip()[3];
    packet->ip_hdr.daddr[1] = dst.pip()[2];
    packet->ip_hdr.daddr[2] = dst.pip()[1];
    packet->ip_hdr.daddr[3] = dst.pip()[0];
    packet->ip_hdr.crc = 0;
    packet->ip_hdr.crc = checksum(reinterpret_cast<uint16_t *>(&packet->ip_hdr), sizeof(ip_header));
    // UDP Header
    packet->udp_hdr.dport = dst.nport();
    packet->udp_hdr.len = htons(uint16_t(size_t(size) + sizeof(udp_header)));
    // UDP Datagram
    memcpy(packet->data, data, size_t(size));
    int buffer_length = size + int(sizeof(ethernet_header) + sizeof(ip_header) + sizeof(udp_header));
    return (pcap_sendpacket(_adhandle, buffer, buffer_length) == 0);
}
const uint8_t * PcapSocket::rcvfrom(SocketAddress & src, int & size)
{
    if(!is_open()) {return nullptr;}
    struct pcap_pkthdr pkthdr;
    const udp_ip_packet * packet;
    const uint8_t * data = pcap_next(_adhandle, &pkthdr);
    if(data == nullptr) {return nullptr;}
    packet = reinterpret_cast<const udp_ip_packet *>(data);
    src.set_ip(packet->ip_hdr.saddr);
    src.set_nport(packet->udp_hdr.sport);
    size = int(size_t(pkthdr.len) - sizeof(ethernet_header) - sizeof(ip_header) - sizeof(udp_header));
    return packet->data;
}
void PcapSocket::fill_headers()
{
    udp_ip_packet * packet = reinterpret_cast<udp_ip_packet *>(buffer);
    // MAC Header
    packet->eth_hdr.type = htons(ETH_TYPE_IP);
    packet->eth_hdr.from_addr[0] = _socketAddress.mac()[5];
    packet->eth_hdr.from_addr[1] = _socketAddress.mac()[4];
    packet->eth_hdr.from_addr[2] = _socketAddress.mac()[3];
    packet->eth_hdr.from_addr[3] = _socketAddress.mac()[2];
    packet->eth_hdr.from_addr[4] = _socketAddress.mac()[1];
    packet->eth_hdr.from_addr[5] = _socketAddress.mac()[0];
    // IP Header
    packet->ip_hdr.ver_ihl = IP_VER_IHL;
    packet->ip_hdr.tos = 0x00;
    packet->ip_hdr.flags_fo = htons(IP_FLAGS_FO);
    packet->ip_hdr.ttl = 40;
    packet->ip_hdr.proto = IPPROTO_UDP;
    packet->ip_hdr.crc = 0;
    packet->ip_hdr.saddr[0] = _socketAddress.pip()[3];
    packet->ip_hdr.saddr[1] = _socketAddress.pip()[2];
    packet->ip_hdr.saddr[2] = _socketAddress.pip()[1];
    packet->ip_hdr.saddr[3] = _socketAddress.pip()[0];
    // UDP Header
    packet->udp_hdr.sport = _socketAddress.nport();
    packet->udp_hdr.crc = 0;
}
std::vector<std::string> PcapSocket::devices()
{
    std::vector<std::string> devs;

    pcap_if_t * alldevs;
    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
#ifdef DEBUG
        std::cout << "PcapSocket::devices: Can not get devices: " << errbuf << std::endl;
#endif
        return devs;
    }
    for(pcap_if_t * d = alldevs; d != nullptr; d = d->next)
    {
        devs.push_back(d->name);
    }
    pcap_freealldevs(alldevs);

    return devs;
}
uint16_t PcapSocket::checksum(uint16_t * data, const int & data_len)
{
    int sum = 0;
    uint16_t answer = 0;
    uint16_t * w = data;
    int nleft = data_len;
    while(nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }
    if(nleft == 1)
    {
        *(u_char *)(&answer) = *(u_char *)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = uint16_t(~sum);
    return(answer);
}
