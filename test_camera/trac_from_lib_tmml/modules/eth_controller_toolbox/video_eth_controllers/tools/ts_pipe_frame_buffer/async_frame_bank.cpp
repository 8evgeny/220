#include "async_frame_bank.hpp"

namespace async_frame_bank
{
    AsyncFrameBank::AsyncFrameBank(const Settings &_settings) : settings(_settings)
    {
        is_configure.store(false);
    }

    AsyncFrameBank::~AsyncFrameBank()
    {
        std::cout << "BEGIN Destructor AsyncFrameBank" << std::endl;
        if(free_items_id_vec.size() != settings.total_capacity)
        {
            std::cout << "\t Error: free_items_id_vec.size() != settings.total_capacity" << std::endl;
        }
        std::cout << "END Destructor AsyncFrameBank" << std::endl;
    }

    bool AsyncFrameBank::setup()
    {
        if(is_configure.load())
        {
            std::cout << "Warning: AsyncFrameBank was setup() previously" << std::endl;
            return false;
        }
        items_mtx.lock();
        items_vec.reserve(settings.total_capacity);
        free_items_id_vec.reserve(settings.total_capacity);

        
        for(int i = 0; i < settings.total_capacity; ++i)
        {
            cv::Mat frame_tmp; 
            frame_tmp = cv::Mat(settings.frame_size, settings.frame_type, cv::Scalar::all(0));
            Item item_tmp;
            item_tmp.frame = frame_tmp;
            item_tmp.state.is_lock = false;
            items_vec.push_back(item_tmp);
            map_framePtr_BankItemsId[item_tmp.frame.data] = i;
            free_items_id_vec.push_back(i);
        }
        is_configure.store(true);
        items_mtx.unlock();
        return true;
    }

    int AsyncFrameBank::get_count_free_items()
    {
        if(!is_configure.load())
        {
            return -1;
        }
        items_mtx.lock();
        int count_free_items = free_items_id_vec.size();
        items_mtx.unlock();

        return count_free_items;
    }

    int AsyncFrameBank::get_count_unfree_items()
    {
        if(!is_configure.load())
        {
            return -1;
        }
        items_mtx.lock();
        int count_unfree_items = settings.total_capacity - free_items_id_vec.size();
        items_mtx.unlock();

        return count_unfree_items;
    }

    cv::Mat* AsyncFrameBank::get_frame()
    {
        if(!is_configure.load())
        {
            return nullptr;
        }
        
        items_mtx.lock();
        if(free_items_id_vec.size() == 0)
        {
            return nullptr;
        }
        int free_id = free_items_id_vec.back();
        free_items_id_vec.pop_back();
        items_vec[free_id].state.is_lock = true;
        cv::Mat *mat_ptr = &items_vec[free_id].frame;
        items_mtx.unlock();

        return mat_ptr;
    }

    void AsyncFrameBank::release_frame(cv::Mat* frame)
    {
        if(!is_configure.load())
        {
            return;
        }
        uint8_t* data_ptr = frame->data;
        if(data_ptr == nullptr)
        {
            return;
        }

        items_mtx.lock();

        if(map_framePtr_BankItemsId.find(data_ptr) == map_framePtr_BankItemsId.end())
        {
            return;
        }

        int release_frame_id = map_framePtr_BankItemsId[data_ptr];
        if(!items_vec[release_frame_id].state.is_lock)
        {
            return; //Попытка сделать release уже разблокированного ранее фрейма
        }
        
        items_vec[release_frame_id].state.is_lock = false;
        free_items_id_vec.push_back(release_frame_id);

        items_mtx.unlock();
    }
} 