#include "application.hpp"



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
        cout << "Call quit_handler" << endl;
        signal(sig, SIG_DFL);
        if(app != nullptr)
        {
            app->quit();
            app.release();
        }
    }; // -- quit_handler

    string config_path = "../tracking.ini"; // -- Путь к файлу конфигурации.
    // cout << argc << "\t" << argv[1] << endl;
    if(argc == 2)
    {
        config_path = argv[1];
    } // -- END if(argc != 2)
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

    app.reset();
    cout << "End main" << endl;
    return 0;
} // -- END main
