#include "BaseThread.hpp"

BaseThread::BaseThread(const int32_t& executionPeriodMs)
    : _execute(false)
    , _errorFlag(false)
    , _errorID(thdNoError)
    , _thdExecutionPeriod{executionPeriodMs}
{

} // END BaseThread

BaseThread::~BaseThread()
{
    stopThd();
} // END ~BaseThread

void BaseThread::startThd()
{
    _execute.store(true);
    _thd = std::thread(&BaseThread::run, this);
    _thd.detach();
}

void BaseThread::stopThd()
{
    if(_execute.load()) // if the thread is running
    {
        _execute.store(false);
        if(_thd.joinable())
        {
            _thd.join();
        } // END if(_thd.joinable())
        closeThd();
    } // END if(_execute.load())
} // END stopThd

bool BaseThread::isRunningThd() const
{
    return _execute.load();
} // END BaseThread::isRunningThd()

void BaseThread::run()
{
    bool error = !initThd();
    if(!error)
    {
        while(_execute.load())
        {
            runThd();
            std::this_thread::sleep_for(std::chrono::milliseconds(_thdExecutionPeriod));
        } // END while(_execute.load())
        closeThd();
    }  // END if(!error)
    else
    {
        _errorID = thdInitializationError;
        closeThd();
    } // END else
} // END run



void BaseThread::setThdExecutionPeriodMs(const int32_t& periodMs)
{
    _thdExecutionPeriod = periodMs;
} // END setThdExecutionPeriodMs

void BaseThread::setExecuteFlagThd(bool flag)
{
    _execute.store(flag);
} // EnD setExecuteFlagThd
