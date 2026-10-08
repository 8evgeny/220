#ifndef BASETHREAD_HPP
#define BASETHREAD_HPP

#include <atomic>
#include <thread>
#include <cstdint>

class BaseThread
{
public:
    /**
     *  List error code
     */
    enum ThdErrorCode {
        thdNoError = 0,
        thdInitializationError = 1
    };
public:
    /**
     *  Constructor
     */
    explicit BaseThread(const int32_t& executionPeriodMs);
    /**
     *  Desstructor
     */
    virtual ~BaseThread();
    /**
     *  Run a thread
     */
    void startThd();
    /**
     *  Stop a thread
     */
    void stopThd();
    /**
     *  Is thread running
     */
    bool isRunningThd() const;
    /**
     *  Get thread last error
     */
    ThdErrorCode getErrorThd() const {return _errorID;}
    /**
     *  Get thread sleep time between executions
     */
    int32_t executionPeriodMsThd() const {return _thdExecutionPeriod;}
    /**
     *  Set thread sleep time between executions
     */
    void setThdExecutionPeriodMs(const int32_t& periodMs);

protected:
    /**
     * Initialization before thread loop. In thread.
     * Include in "BaseThread::run()"
     * It's empty. To override
     */
    virtual bool initThd() {return false;}
    /**
     * Thread loop.
     * Include in "BaseThread::run()"
     * It's empty. To override
     */
    virtual void runThd() {;}
    /**
     *  After leaving the thread loop. In thread.
     *  Include in "BaseThread::run()"
     *  It's empty. To override
     */
    virtual void closeThd() {;}
    /**
     *  Set thread execute
     */
    void setExecuteFlagThd(bool flag);


private:
    /**
     *  Thread function
     */
    void run();

private:
    /** Execute Flag */
    std::atomic<bool> _execute;
    /** This thread */
    std::thread _thd;
    /** Error if: _errorFlag == true */
    bool _errorFlag;
    /** Error if: _errorID != 0 */
    ThdErrorCode _errorID;
    /** Sleep time between calls [ms] */
    int32_t _thdExecutionPeriod;
};

#endif // BASETHREAD_HPP
