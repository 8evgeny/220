#ifndef TRIPLEBUFFER_HPP
#define TRIPLEBUFFER_HPP

#include <cstdint>
#include <mutex>

/*********************************************
 *
 * Triple data buffering
 *
 *********************************************/

class TripleBuffer
{
public:
    /**
     *  Constructor
     *
     *  @param size - buffer size [byte]
     */
    explicit TripleBuffer(const uint32_t& size);
    /**
     *  Destructor
     */
    ~TripleBuffer();
    /** copy and assignment not allowed */
    TripleBuffer(const TripleBuffer& ) = delete;
    void operator=(const TripleBuffer& ) = delete;
    /**
     *  Get ready data
     *
     *  @return ready data
     */
    uint8_t* get(int32_t& length);
    /**
     *  Write new data to the buffer. Appended into the same buffer
     *  until the TripleBuffer::setReady() function is called.
     *
     *  @param start  - start of data
     *  @param length - data length [byte]
     *  @return true if successful written
     */
    bool writeData(const uint8_t* start, const uint32_t& length);
    /**
     *  Get overflow error counter
     *
     *  @return counter overflow
     */
    int32_t overflowCnt() const {return _overflowCnt;}
    /**
     *  Data ready flag
     *
     *  @return true if data ready
     */
    bool ready() const {return _ready;}
    /**
     *  When all data frame written in buffer,
     *  set flag ready and swap buffers
     */
    void setReady();

protected:
    /** Swap buffers mnutex */
    std::mutex _mutex;
    /** Ready flag (true - data ready) */
    bool _ready;
    /** Buffer size [byte] */
    const uint32_t _size;
    /** Buffer given out */
    uint8_t* _viewBuf;
    /** Buffer with ready data */
    uint8_t* _rdyBuf;
    /** Buffer for writing new data */
    uint8_t* _writeBuf;
    /** Data size in _viewBuf */
    int32_t _viewLength;
    /** Data size in _rdyBuf */
    int32_t _rdyLength;
    /** Data size in _writeBuf */
    int32_t _writeLength;
    /** Number of overflow events */
    int32_t _overflowCnt;
};

#endif // TRIPLEBUFFER_HPP
