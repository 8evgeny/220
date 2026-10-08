#include "shared_memory.h"

#include <cstring>
#include <thread>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

shared_memory::shared_memory():
    name(""),
    max_size(0),
    create(false),
    sem_id(nullptr),
    shm_fd(-1),
    shm_p(nullptr)
{
}
shared_memory::~shared_memory()
{
    cls();
}
bool shared_memory::opn(const std::string & name, const size_t & max_size, const bool & create)
{
    cls();
    this->name = name;
    this->max_size = max_size;
    this->create = create;
    // creating the shared memory object
    if(create)
    {
        shm_fd = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    }
    else
    {
        shm_fd = shm_open(name.c_str(), O_RDWR, 0);
    }
    if(shm_fd < 0)
    {
        return false;
    }
    // adjusting mapped file size (make room for the whole segment to map)
    int res = ftruncate(shm_fd, long(max_size + sizeof(int) + sizeof(size_t)));
    (void)(res);
    // Semaphore open
    if(create)
    {
        sem_id = sem_open(name.c_str(), O_CREAT, S_IRUSR | S_IWUSR, 1);
    }
    else
    {
        sem_id = sem_open(name.c_str(), 0, 0, 0);
    }
    if(sem_id == nullptr)
    {
        return false;
    }
    // requesting the shared segment
    shm_p = reinterpret_cast<uint8_t *>(mmap(nullptr, max_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if(shm_p == MAP_FAILED)
    {
        return false;
    }
    if(sem_post(sem_id) == -1)
    {
        return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return true;
}
void shared_memory::cls()
{
    if(is_opn())
    {
        // Shared Memory unmup: Unmup a named Shared Memory.
        munmap(shm_p, max_size);
        // Semaphore close: Close a named Semaphore
        sem_close(sem_id);
        if(create)
        {
            // Shared Memory unlink: Remove a named Shared Memory from the system.
            shm_unlink(name.c_str());
            // Semaphore unlink: Remove a named Semaphore from the system.
            sem_unlink(name.c_str());
        }
    }

    name = "";
    max_size = 0;
    create = false;
    sem_id = nullptr;
    shm_fd = -1;
    shm_p = nullptr;
}
bool shared_memory::snd(const void * data, const size_t & size)
{
    if(!is_opn()) return false;
    if(sem_wait(sem_id) == -1)
    {
        return false;
    }
    if(size > max_size)
    {
        return false;
    }
    memcpy(shm_p, data, size);
    if(sem_post(sem_id) == -1)
    {
        return false;
    }
    return true;
}
size_t shared_memory::rcv(void * data, const size_t & size)
{
    if(!is_opn()) return 0;
    if(sem_wait(sem_id) == -1)
    {
        return 0;
    }
    if(size > max_size)
    {
        return 0;
    }
    memcpy(data, shm_p, size);
    if(sem_post(sem_id) == -1)
    {
        return 0;
    }
    return size;
}
