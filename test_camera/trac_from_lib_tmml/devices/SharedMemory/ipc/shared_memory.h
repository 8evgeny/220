#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <string>
#include <semaphore.h>

class shared_memory
{
public:
    shared_memory();
    ~shared_memory();

    bool is_opn() const {return (shm_fd != -1);}

    bool opn(const std::string & name, const size_t & size, const bool & create);
    void cls();

    bool snd(const void * data, const size_t & size);
    size_t rcv(void * data, const size_t & max_size);
private:
    std::string name;
    size_t max_size;
    bool create;
    sem_t * sem_id;
    int shm_fd;
    uint8_t * shm_p;
};

#endif // SHARED_MEMORY_H
