#include <iostream>
#include "basic_toolbox/cmd_udp_server/cmd_udp_server.hpp"

#include <cstdint>
#include <chrono>
#include <thread>

#include <signal.h>
#include <functional>

std::function<void(int)> quit_handler = [](int sig){std::cout << "WARNING: empty quit_handler" << std::endl;};
void systemSignalsHandler(int signal) { quit_handler(signal);} // оболочка над лямбда-выражением

int main(int argc, char* argv[])
{
    std::atomic<bool> need_quit = {false};
    quit_handler = [&need_quit](int sig)
    {
        std::cout << "\t" << "\033[1;31mNOTE: catch system signal ---> begin close app\033[0m" << std::endl;
        signal(sig, SIG_DFL);
        need_quit.store(true);
    }; // -- quit_handler
    signal(SIGINT,  systemSignalsHandler);
    signal(SIGABRT, systemSignalsHandler);
    signal(SIGTERM, systemSignalsHandler);
    #ifdef __linux__
    signal(SIGKILL, systemSignalsHandler);
    signal(SIGQUIT, systemSignalsHandler);
    signal(SIGSTOP, systemSignalsHandler);
    #endif // __linux__

	UDPSrvSettings settings;
	settings.ip = "127.0.0.1";
	settings.port = 25025;
	bool report = false;
	CmdUDPServer srv(settings, report);
	if(!report)
	{
		return 1;
	}
	std::vector<uint8_t> cmd_tmp;
	while(!need_quit.load())
	{
		if(srv.get_cmd_count() > 0)
		{
			if(!srv.pull_one(cmd_tmp))
			{
				continue;
			}
			std::cout << "cmd = " << int(cmd_tmp[2]) << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	// srv.quit();

	std::cout << "END main" << std::endl;
	return 0;
} // -- END main
