#include "vtrack_data_sender.hpp"

#include <iomanip>

using namespace std;

namespace track_pos_sender
{

vtrackDataSender::vtrackDataSender(const Settings & settings_, bool & ok)
{
    cout << "vtrackDataSender::BEGIN constructor vtrackDataSender" << endl;
    ok = true;
    settings = settings_;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const uint8_t*)&reuse, sizeof(reuse)) < 0)
    {
        std::cout << "vtrackDataSender::setsockopt(SO_REUSEADDR) failed";
        ok = false;
        return;
    } // END if (setsockopt(sockfd...
    if(sockfd == -1)
    {
        std::cout << "vtrackDataSender::Socket creation failed..." << std::endl;
        ok = false;
        return;
    } // END (sockfd == -1)
    else
    {
        std::cout << "vtrackDataSender::UDP socket successfully created..." << std::endl;
    } // END else

#ifdef __linux__
    memset(&servaddr, 0, sizeof(servaddr));
    //    bzero(&servaddr, sizeof(servaddr));
#endif // __linux__

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(settings.port);
    servaddr.sin_addr.s_addr = inet_addr(settings.ip_v4.c_str());

    sz_buf_max = 2 * sizeof(uint8_t) + max_size_vec * sizeof(trackPos);
    cmd_buffer = new uint8_t[sz_buf_max];
    cout << "vtrackDataSender::END constructor vtrackDataSender" << endl;
    return;
}; // -- END vtrackDataSender


vtrackDataSender::~vtrackDataSender()
{
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    // maybe shutdown like VK Stepik "Multithreading C++" course recomended?
    std::cout << "Деструктор vtrackDataSender" << std::endl;
} // -- END vtrackDataSender


bool vtrackDataSender::serialize(const vtrackDataStruct & data)
{
    int pos = 0;
    int vec_sz = data.v_track.size();

    sz_buf_cur = 2 * sizeof(uint8_t) + vec_sz * sizeof(trackPos);

    memcpy(cmd_buffer + pos, &settings.track_cmd_id, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(cmd_buffer + pos, &vec_sz, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    for(int i = 0; i < vec_sz; i++)
    {
        memcpy(cmd_buffer + pos, &data.v_track[i], sizeof(trackPos));
        pos += sizeof(trackPos);
    } // END for(int i = 0; i < vec_sz; i++)

    memset(cmd_buffer + pos, 0, 64);
    pos+=64;

    return 1;
} // -- END serialize


void vtrackDataSender::log_send_cmd()
{
	std::cout << "DBG [vtrackDataSender] : cmd_len = " + std::to_string(sz_buf_cur) + " --> [";
	for(int i = 0; i < sz_buf_cur; ++i)
	{
		std::cout << std::setw(3) << (int)cmd_buffer[i] << ", ";
    } // END for(int i = 0; i < sz_buf_cur; ++i)
	std::cout << "]\n\n";
} // -- END log_send_cmd

int vtrackDataSender::send(const vtrackDataStruct & data)
{
    serialize(data);
    vtrackDataStruct test_data;
#ifdef USE_DBG_SEND_VTRACK
    log_send_cmd();
#endif /* USE_DBG_SEND_VTRACK */
    sendto(sockfd, cmd_buffer, sz_buf_cur, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    return sz_buf_cur;
} // -- END send

int vtrackDataSender::send(const StateCmdStruct & data)
{
    serialize(data);
    vtrackDataStruct test_data;
#ifdef USE_DBG_SEND_VTRACK
    log_send_cmd();
#endif /* USE_DBG_SEND_VTRACK */
    sendto(sockfd, cmd_buffer, sz_buf_cur, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    return sz_buf_cur;
} // -- END send

bool vtrackDataSender::serialize(const StateCmdStruct & data)
{
    int pos = 0;

    sz_buf_cur = 2 * sizeof(StateCmdStruct);

    memcpy(cmd_buffer + pos, &settings.state_cmd_id, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(cmd_buffer + pos, &data.state, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memset(cmd_buffer + pos, 0, 64);
    pos+=64;

    return 1;
} // -- END serialize

}; // -- END namespace track_pos_sender

