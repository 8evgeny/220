#include "colibritvserver.h"

#define COLIBRITV_CMD_BUF_LEN           (1024)
#define COLIBRITV_FRAME_BUF_W           (1920)
#define COLIBRITV_FRAME_BUF_H           (1080)
#define COLIBRITV_FRAME_BUF_H_2         (540)
#define COLIBRITV_FRAME_BUF_LEN         (COLIBRITV_FRAME_BUF_W  * COLIBRITV_FRAME_BUF_H )
#define COLIBRITV_FULL_VID_POCKET_LEN   (3846)
#define COLIBRITV_VID_POCKET_LEN        (3840)

ColibriTVServer::ColibriTVServer()
{
    cmd_buf = new uint8_t[COLIBRITV_CMD_BUF_LEN];
    cmd_rdy = new uint8_t[COLIBRITV_CMD_BUF_LEN];
    cmd_length = 0;
    cmd_ready = false;
    frame = new uint8_t[COLIBRITV_FRAME_BUF_LEN];
    frame_buf = new uint8_t[COLIBRITV_FRAME_BUF_LEN];
    frame_rdy = new uint8_t[COLIBRITV_FRAME_BUF_LEN];
    frame_w = COLIBRITV_FRAME_BUF_W;
    frame_h = COLIBRITV_FRAME_BUF_H;
    frame_id = 0;
    frame_num = 0;
    frame_ready.store(false, std::memory_order_release);

    std::thread handle_frame_thd(&ColibriTVServer::run_handle_frame, this);
    handle_frame_thd.detach();
}

ColibriTVServer::~ColibriTVServer()
{
    cls();
    cmd_mutex.lock();
    cmd_mutex.unlock();
    delete [] cmd_buf;
    delete [] cmd_rdy;
    frame_mutex.lock();
    frame_mutex.unlock();
    delete [] frame;
    delete [] frame_buf;
    delete [] frame_rdy;
}

void ColibriTVServer::quit()
{
    handle_frame_execute.store(false, std::memory_order_acquire);
    frame_cv.notify_one();
}

void ColibriTVServer::handle(const SocketAddress &address, const void *data,
                             const int &data_length)
{
    const uint8_t* data_ = reinterpret_cast<const uint8_t*>(data);
    if((data_[3] != 2))  // Video packet
    {
//        // проверка на конец кадра: каждый пакет содержит 2 кадровых строки
//        if( packet_counter != COLIBRITV_FRAME_BUF_H_2)
//            printf("Packet loss\n");
//        else
//        {
            const std::lock_guard guard(frame_mutex);
            frame_num++;
            frame_w = COLIBRITV_FRAME_BUF_W;
            frame_h = COLIBRITV_FRAME_BUF_H;
            // замена хранящего фреймбуфера на считывающий
            uint8_t * tmp = frame_buf;
            frame_buf = frame;
            frame = tmp;

            frame_ready.store(true, std::memory_order_release);
            frame_cv.notify_one();
//        }
        packet_counter = 0;
    }
    else
    {
        // расчет смещения в считывающем фреймбуфере для текущих данных
        const int offset = 2*(data_[5] + 256*data_[4] - 1);
        // запись данных в считывающий фреймбуфер
        for(size_t i = 0; i < COLIBRITV_VID_POCKET_LEN; i++)
        {
            frame[offset * COLIBRITV_FRAME_BUF_W + i] = data_[6 + i];
        }
        packet_counter++;
    }
}

void ColibriTVServer::run_handle_frame()
{
    handle_frame_execute.store(true, std::memory_order_acquire);
    while(handle_frame_execute.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lk(frame_mutex);
        frame_cv.wait(lk);
        if(!handle_frame_execute.load(std::memory_order_acquire))
        {
            break;
        }
        for(auto handler : frame_handlers)
        {
            frame_ready.store(false, std::memory_order_release);
            handler->handle(frame_buf, frame_w, frame_h, frame_num, frame_id);
        }
    }
}
