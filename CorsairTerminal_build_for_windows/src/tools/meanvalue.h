#ifndef MEANVALUE_H
#define MEANVALUE_H

#include <stdint.h>
#include <vector>
#include <cassert>
#include <algorithm>

template <typename  T>
class MeanValue
{
public:
    explicit MeanValue(uint32_t window_size)
        : _is_inited(false)
        , _size{0}
    {
#ifdef DEBUG
        assert(window_size > 0u && window_size < 1000u); // Error in class mean_value: window_size - out of range!
#endif
        _buffer.reserve(window_size);
        reset();
        _size = window_size;
    }

    void reset() {
        _it = 0;
        _buffer.clear();
        _mean_value    = static_cast<T>(0u);
        _max_deviation = static_cast<T>(0u);
        _min_deviation = static_cast<T>(0u);
    }

    void add(T val)
    {
        if(_buffer.size() < _size) {
            _buffer.push_back(val);

        } else {
            _buffer[_it] = val;
        }


        if(_it == _size - 1) {
            _it = 0u;
        } else {
            _it++;
        }

        update();
    }



    T get_mean_value()    const {return _mean_value;}
    T get_max_deviation() const {return _max_deviation;}
    T get_min_deviation() const {return _min_deviation;}

private:

    void update()
    {
        _mean_value    = static_cast<T>(0u);
        _max_deviation = std::numeric_limits<T>::min();
        _min_deviation = std::numeric_limits<T>::max();
        std::for_each (_buffer.cbegin(), _buffer.cend(), [this](const T &n) {
            if(n > _max_deviation) _max_deviation = n;
            if(n < _min_deviation) _min_deviation = n;
            _mean_value    += n;
        });
        _mean_value    /= static_cast<T>(_buffer.size());
    }

    bool _is_inited;
    size_t _size;
    uint32_t _it;
    std::vector<T> _buffer;
    T _mean_value;
    T _max_deviation;
    T _min_deviation;
};

#endif // MEANVALUE_H
