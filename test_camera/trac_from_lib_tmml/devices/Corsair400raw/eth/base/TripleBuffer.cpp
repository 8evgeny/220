#include "TripleBuffer.hpp"
#include "eth/EthGlobal.hpp"

TripleBuffer::TripleBuffer(const uint32_t& size)
    : _ready{false}
    , _size{size}
    , _viewBuf{new uint8_t[_size]}
    , _rdyBuf{new uint8_t[_size]}
    , _writeBuf{new uint8_t[_size]}
    , _viewLength{0}
    , _rdyLength{0}
    , _writeLength{0}
    , _overflowCnt{0}
{
#ifdef DEBUG
    if((_size <= 4)) {
        ERROR_MSG("TripleBuffer: Zero size!");
    }
    if(!_viewBuf) {
        ERROR_MSG("TripleBuffer: _buf not created!");
    }
    if(!_rdyBuf) {
        ERROR_MSG("TripleBuffer: _rdy not created!");
    }
    if(!_writeBuf) {
        ERROR_MSG("TripleBuffer: _rdy not created!");
    }
#endif
} // END TripleBuffer

TripleBuffer::~TripleBuffer()
{
//    std::lock_guard<std::mutex> lock(_mutex);
    delete [] _viewBuf;
    delete [] _rdyBuf;
    delete [] _writeBuf;
} // END TripleBuffer

uint8_t* TripleBuffer::get(int32_t& length)
{
    uint8_t* data{nullptr};
    if(_ready)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _ready = false;
        // swap buffer
        uint8_t* tmp{_rdyBuf};
        _rdyBuf  = _viewBuf;
        _viewBuf = tmp;
        // swap buffer size
        _viewLength = _rdyLength;
        // out data
        data = _viewBuf;
        length = _viewLength;
    } // END if(_ready)
    return data;
} // END get

bool TripleBuffer::writeData(const uint8_t* start, const uint32_t& length)
{
    bool result = false;
    if(_writeLength + length <= _size)
    { // not exceed the size
        // write new data in buffer with offset
        const uint8_t* end{start + length};
        std::copy(start, end, _writeBuf + _writeLength);
        // save the size of the data written
        _writeLength += length;
        result = true;
    } // END if(_writeLength + length <= _size)
    else
    {  // overflow
        _overflowCnt++;
        ERROR_MSG("TripleBuffer: Overflow!");
    } // END else
    return result;
} // END writeData

void TripleBuffer::setReady()
{
    std::lock_guard<std::mutex> lock(_mutex);
    // swap buffer
    uint8_t* tmp{_rdyBuf};
    _rdyBuf  = _writeBuf;
    _writeBuf = tmp;
    // save buffer size
    _rdyLength = _writeLength;
    // reset write iterator
    _writeLength = 0;
    // flag data ready
    _ready = true;
} // END setReady
