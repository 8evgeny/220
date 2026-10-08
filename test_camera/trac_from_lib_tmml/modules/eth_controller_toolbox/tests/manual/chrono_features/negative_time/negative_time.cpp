#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
	chrono::system_clock::time_point tp1, tp2;
	tp1 = chrono::system_clock::now();
	this_thread::sleep_for(chrono::milliseconds(500));
	tp2 = chrono::system_clock::now();

	auto exec_one_duration = chrono::duration_cast<std::chrono::microseconds>(tp1 - tp2);
	std::cout << exec_one_duration.count() << std::endl;

	std::cout << "END main" << std::endl;
	return 0;
} // -- END main
