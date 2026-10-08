#include "headers/application.hpp"

using namespace std;
using namespace cv;

function<void(int)> quit_handler;
void systemSignalsHandler(int signal) { quit_handler(signal);} // оболочка над лямбда-выражением

int main(int argc, char *argv[])
{
    cout << "BEGIN main" << endl;

    /*
    Важно сначала назначить nullptr,
    т.к. если сигнал пройдет раньше,
    чем загрузится app, нужно обеспечить
    корректное поведение. В обработчике quit_handler стоит проверка на nullptr.
    */
    unique_ptr<Application> app = nullptr;
    quit_handler = [&app](int sig)
    {
        if(sig == SIGINT)
        {
            cout << "pressed Ctrl-C" << endl;
            if(app != nullptr)
            {
                app->signal_flag = true;
            } // END if(app != nullptr)
        }// END if(sig == SIGINT)
    }; // -- quit_handler
    string config_path = "../config.ini";
    cout << "config_path = "  << config_path << endl;
    time_keeper::TimeKeeper::inst();

    // Install a signal handler
    signal(SIGINT,  systemSignalsHandler);
    signal(SIGABRT, systemSignalsHandler);
    signal(SIGTERM, systemSignalsHandler);
    signal(SIGKILL, systemSignalsHandler);
    signal(SIGQUIT, systemSignalsHandler);
    signal(SIGSTOP, systemSignalsHandler);
    
    bool ok = 0;
    cout << "Create app" << endl;
    app = make_unique<Application>(config_path, ok);
    if(ok)
    {
        app->start();
    } // -- END if(ok)
    else
    {
        cout << "Application create failed..." << endl;
    }
    cout << "quit app->start()" << endl;
    this_thread::sleep_for(chrono::milliseconds(50));
    if (app->powerOFF_flag)
    {
        // app->rs485_worker_ptr->set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
        app.reset();
        cout << "End main" << endl;
        cout <<" ------------------- POWER_OFF ---------------------\n";
        string cmd = "systemctl poweroff";
        int res = system(cmd.c_str());
        return res;
    }// END if (app->powerOFF_flag)
    else
    {
        app.reset();
        cout << "End main" << endl;
        return 0;
    }// END if (!app->powerOFF_flag)
} // -- END main
