#include "main_window_constructor.hpp"

#include <iostream>

#include <functional>

using namespace std;
using namespace cv;
using namespace cvw;

int main(int argc, char *argv[])
{
    cout << "BEGIN main" << endl;
    cout << "Call route multicast CMD" << endl;
    int cmd_res = system("sudo route_multicast");
    if(cmd_res == 256) {cout << "No rule for route_multicast! Run ../desktop/add_multicast.sh!\nEND main with error!" << endl; return -1;}
    cout << "route multicast CMD SUCCESS. cmd_res = " << cmd_res << endl;
    std::string config_path = "../config.ini";
    bool ok = false;
    std::shared_ptr<MainWindowConstructor> mw_ptr = make_shared<MainWindowConstructor>(config_path, ok);
    if(!ok)
    {
        cout << "Error MainWindowConstructor() creating!" << endl;
    } // END if(!ok)
    else
    {
        mw_ptr->start();
    } // END if(ok)
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "END main" << endl;
    return 0;
} // END main
