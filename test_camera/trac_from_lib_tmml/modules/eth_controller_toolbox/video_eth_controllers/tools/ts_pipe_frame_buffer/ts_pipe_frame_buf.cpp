#include "ts_pipe_frame_buf.hpp"
#include <thread>

namespace ts_pipe_frame_buf
{
    TSPipeFrameBuf::TSPipeFrameBuf(const Settings &_settings) : settings(_settings)
    {
        
    } // -- END TSPipeFrameBuf

    TSPipeFrameBuf::~TSPipeFrameBuf()
    {
        std::cout << "BEGIN Destructor TSPipeFrameBuf" << std::endl;
        if(!quit())
        {
            std::cout << "\tERROR: quit return false" << std::endl;
        }
        std::cout << "END Destructor TSPipeFrameBuf" << std::endl;
    } // -- END ~TSPipeFrameBuf

    bool TSPipeFrameBuf::setup()
    {
        if(sync.is_configure.load()) // уже введен в эксплуатацию
        {
            return true;
        }
        if(sync.setup_was_call.load()) // был зафиксирован факт хотя бы однократного вызова setup
        {
            return false;
        }
        if(sync.quit._was_call.load()) // если зафиксирован факт хотя бы однократного вызова quit
        {
            return false;
        }

        sync.setup_was_call.store(true);
        items_mtx.lock();
        std::cout << "BEGIN TSPipeFrameBuf::setup()" << std::endl;
        // проверка корректности настройки. TODO: сделать фабрику и проверять в ней.
        if(settings.total_capacity < settings.minCountThresholdOverwritePrepareFrame)
        {
            std::cout << "ERROR: TSPipeFrameBuf: total_capacity < minCountThresholdOverwritePrepareFrame" << std::endl;
            items_mtx.unlock();
            std::cout << "END TSPipeFrameBuf::setup()" << std::endl;
            return false;
        }
        async_frame_bank::Settings afb_settings; // afb - async frame bank
        afb_settings.frame_size = settings.frame_size;
        afb_settings.frame_type = settings.frame_type;
        afb_settings.total_capacity = settings.total_capacity;

        frame_bank = std::make_shared<async_frame_bank::AsyncFrameBank>(afb_settings);
        if(!frame_bank->setup())
        {
            std::cout << "ERROR: AsyncFrameBank::setup() failed" << std::endl;
            items_mtx.unlock();
            std::cout << "END TSPipeFrameBuf::setup()" << std::endl;
            return false;
        }

        items_all_buffer.reserve(settings.total_capacity);

        items_procRead_vec.reserve(settings.total_capacity);
        items_procWrite_vec.reserve(settings.total_capacity);
        items_procStorageEmpty_vec.reserve(settings.total_capacity);
        items_procStorageEmpty_vec.reserve(settings.total_capacity);

        dispatch_frameID2procReadVecID          = std::vector<int>(settings.total_capacity, -1); // elem count, elem value
        dispatch_frameID2procWriteVecID         = std::vector<int>(settings.total_capacity, -1);
        dispatch_frameID2procStorageEmptyID     = std::vector<int>(settings.total_capacity, -1);
        dispatch_frameID2procStoragePreparedID  = std::vector<int>(settings.total_capacity, -1);

        for(int i = 0; i < settings.total_capacity; ++i)
        {
            MSid msid_tmp;
            msid_tmp.frame_ptr = frame_bank->get_frame();
            if(msid_tmp.frame_ptr == nullptr)
            {
                std::cout << "ERROR: frame_bank->get_frame() return false" << std::endl;
                items_mtx.unlock();
                std::cout << "END TSPipeFrameBuf::setup()" << std::endl;
                return false;
            }
            msid_tmp.id = i;
            msid_tmp.state = MSidStates::STATE_STORAGE_EMPTY;
            items_all_buffer.push_back(msid_tmp);
            items_procStorageEmpty_vec.push_back(&items_all_buffer.back());
            dispatch_framesPtr2mqsidPtr[msid_tmp.frame_ptr] = &items_all_buffer.back();
            dispatch_frameID2procStorageEmptyID[i] = i;
        }
        items_mtx.unlock();
        sync.is_configure.store(true);
        return true;   
    } // -- END setup

    bool TSPipeFrameBuf::quit()
    {
        if(sync.quit.current_called.load())
        {
            std::cout << "WARNING: TSPipeFrameBuf::quit() currently called." << std::endl;
            return false;
        }
        if(sync.quit.success.load())
        {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        if(!items_mtx.try_lock()) // такая попытка не считается фактом вызова
        {
            std::cout << "ERROR: failed to lock the mutex in TSPipeFrameBuf::quit()" << std::endl;
            return false;
        }
        sync.quit.current_called.store(true);
        sync.quit._was_call.store(true);
        sync.is_configure.store(false);

        std::cout << "BEGIN TSPipeFrameBuf::quit()" << std::endl;
        std::cout << "\t items_procWrite_vec.size() = "             << items_procWrite_vec.size()               << std::endl;
        std::cout << "\t items_procRead_vec.size() = "              << items_procRead_vec.size()                << std::endl;
        std::cout << "\t items_procStorageEmpty_vec.size() = "      << items_procStorageEmpty_vec.size()        << std::endl;
        std::cout << "\t items_procStoragePrepared_queue.size() = " << items_procStoragePrepared_queue.size()   << std::endl;
        std::cout << "\t total_items_size = "                       << items_all_buffer.size()                  << std::endl;
        
        int watchdog_total_time_ms = watchdog.release_wr_frames_pts.wait_total_time;
        int check_interval_ms = watchdog.release_wr_frames_pts.check_delay_time;
        int total_attempts = watchdog_total_time_ms / check_interval_ms;
        std::chrono::milliseconds interval_chrono(check_interval_ms);
        bool success_checking = false;
        for(int i = 0; i < total_attempts; ++i)
        {
            if(items_procRead_vec.size() == 0 && items_procWrite_vec.size() == 0)
            {
                success_checking = true;
                break;
            }
            std::this_thread::sleep_for(interval_chrono);
        }
        sync.quit.success.store(success_checking);
        
        if(!sync.quit.success.load())
        {
            sync.quit.current_called.store(false);
            items_mtx.unlock();
            std::cout << "END TSPipeFrameBuf::quit() -- Failed" << std::endl;
            return false;
        }

        items_procStorageEmpty_vec.clear();
        items_procStoragePrepared_queue.clear();

        for(int i = 0; i < items_all_buffer.size(); ++i)
        {
            frame_bank->release_frame(items_all_buffer[i].frame_ptr);
        }
        std::cout << "END TSPipeFrameBuf::quit() -- Success" << std::endl;
        items_mtx.unlock();
        return true;
    } // -- END quit

    cv::Mat* TSPipeFrameBuf::get_ptr_write_frame()
    {
        if(!sync.is_configure.load())
        {
            return nullptr;
        }
        items_mtx.lock();
        // write_frame_mtx.lock(); // будет разблокирован когда будет возвращен арендный фрейм (multithread write option future)
        cv::Mat* return_ptr = nullptr;
        if(items_procStorageEmpty_vec.size() > 0) // если есть доступный для записи "ожидюащий" фрейм
        {
            items_procWrite_vec.push_back(items_procStorageEmpty_vec.back()); //миграция шаг 1
            items_procStorageEmpty_vec.pop_back(); // миграция шаг 2

            dispatch_frameID2procWriteVecID[items_procWrite_vec.back()->id] = items_procWrite_vec.size() - 1;
            dispatch_frameID2procStorageEmptyID[items_procWrite_vec.back()->id] = -1;

            items_procWrite_vec.back()->state = MSidStates::STATE_PROCESS_WRITE;
            return_ptr = items_procWrite_vec.back()->frame_ptr;
            
        }
        else if(items_procStoragePrepared_queue.size() >= settings.minCountThresholdOverwritePrepareFrame && 
            items_procStoragePrepared_queue.size() > 0) 
        {
            items_procWrite_vec.push_back(items_procStoragePrepared_queue.front());
            items_procStoragePrepared_queue.pop_front(); 

            dispatch_frameID2procWriteVecID[items_procWrite_vec.back()->id] = items_procWrite_vec.size() - 1;
            dispatch_frameID2procStoragePreparedID[items_procWrite_vec.back()->id] = -1;

            items_procWrite_vec.back()->state = MSidStates::STATE_PROCESS_WRITE;
            return_ptr = items_procWrite_vec.back()->frame_ptr;
        }
        else
        {
            // write_frame_mtx.unlock(); // не удалось вернуть валидный фрейм для записи, поэтому мьютекс не блокируется
        }
        // write_frame_mtx.unlock();
        items_mtx.unlock();
        return return_ptr;
    } // -- END get_ptr_write_frame

    cv::Mat const * TSPipeFrameBuf::get_ptr_read_frame()
    {
        if(!sync.is_configure.load())
        {
            return nullptr;
        }
        items_mtx.lock();  
        //maybe lock read_frame_mtx
        cv::Mat * return_ptr = nullptr;
        if(items_procStoragePrepared_queue.size() > 0) // если вообще есть подготовленные фреймы
        {
            items_procRead_vec.push_back(items_procStoragePrepared_queue.front());
            items_procStoragePrepared_queue.pop_front();

            dispatch_frameID2procReadVecID[items_procRead_vec.back()->id] = items_procRead_vec.size() - 1;
            dispatch_frameID2procStoragePreparedID[items_procRead_vec.back()->id] = -1;

            items_procRead_vec.back()->state = MSidStates::STATE_PROCESS_READ;
            return_ptr = items_procRead_vec.back()->frame_ptr;
        }

        items_mtx.unlock();
        return return_ptr;
    } // -- END get_ptr_read_frame

    void TSPipeFrameBuf::frame_read_complete(cv::Mat const *frame_ptr)
    {
        if(!sync.is_configure.load())
        {
            return;
        }
        items_mtx.lock();  

        CheckOwnershipReturnedFrameReports check_report = 
            checkValidReturnedFramePtr(frame_ptr, STATE_PROCESS_READ);
        if(check_report != REPORT_FRAME_VALID)
        {
            items_mtx.unlock(); 
        }
        if(check_report == REPORT_FRAME_WAS_CRUSHED)
        {
            std::cout << "ERROR: detect that frame was changed/crushed. Quit TSPipeFrameBuf" << std::endl;
            if(!quit())
            {
                std::cout << "ERROR: call quit(...) return false" << std::endl;
            }
        }
        if(check_report != REPORT_FRAME_VALID)
        {
            return;
        }

        MSid* msid_reloc_ptr = dispatch_framesPtr2mqsidPtr[frame_ptr];
        vec_remove(items_procRead_vec, dispatch_frameID2procReadVecID[msid_reloc_ptr->id]);
        msid_reloc_ptr->state = MSidStates::STATE_STORAGE_EMPTY;
        items_procStorageEmpty_vec.push_back(msid_reloc_ptr);

        dispatch_frameID2procReadVecID[msid_reloc_ptr->id] = -1;
        dispatch_frameID2procStorageEmptyID[msid_reloc_ptr->id] = items_procStorageEmpty_vec.size() - 1;
        // std::cout << "dbg:: pipebuf_prepared_size = " << items_procStoragePrepared_queue.size() << " / " << settings.total_capacity << std::endl;
        // std::cout << "dbg:: pipebuf_empty_size = " << items_procStorageEmpty_vec.size() << std::endl;
        // std::cout << "dbg:: pipebuf_write_size = " << items_procWrite_vec.size() << std::endl;
        // std::cout << "dbg:: pipebuf_read_size = " << items_procRead_vec.size() << std::endl;         
        items_mtx.unlock();
    } // -- END frame_read_complete

    void TSPipeFrameBuf::frame_write_complete_success(cv::Mat const *frame_ptr)
    {
        if(!sync.is_configure.load())
        {
            return;
        }
        items_mtx.lock(); 

        CheckOwnershipReturnedFrameReports check_report = 
            checkValidReturnedFramePtr(frame_ptr, STATE_PROCESS_WRITE);
        if(check_report != REPORT_FRAME_VALID)
        {
            items_mtx.unlock(); 
        }
        if(check_report == REPORT_FRAME_WAS_CRUSHED)
        {
            std::cout << "ERROR: detect that frame was changed/crushed. Quit TSPipeFrameBuf" << std::endl;
            if(!quit())
            {
                std::cout << "ERROR: call quit(...) return false" << std::endl;
            }
        }
        if(check_report != REPORT_FRAME_VALID)
        {
            return;
        }

        MSid* msid_reloc_ptr = dispatch_framesPtr2mqsidPtr[frame_ptr];
        vec_remove(items_procWrite_vec, dispatch_frameID2procWriteVecID[msid_reloc_ptr->id]);
        msid_reloc_ptr->state = MSidStates::STATE_STORAGE_PREPARED;
        items_procStoragePrepared_queue.push_back(msid_reloc_ptr);

        dispatch_frameID2procWriteVecID[msid_reloc_ptr->id] = -1;
        dispatch_frameID2procStoragePreparedID[msid_reloc_ptr->id] = items_procStoragePrepared_queue.size() - 1;


        // write_frame_mtx.unlock();
        // std::cout << "dbg:: pipebuf_prepared_size = " << items_procStoragePrepared_queue.size() << " / " << settings.total_capacity << std::endl;
        // std::cout << "dbg:: pipebuf_empty_size = " << items_procStorageEmpty_vec.size() << std::endl;
        // std::cout << "dbg:: pipebuf_write_size = " << items_procWrite_vec.size() << std::endl;
        // std::cout << "dbg:: pipebuf_read_size = " << items_procRead_vec.size() << std::endl; 
        items_mtx.unlock();
    } // -- END frame_write_complete_success

    void TSPipeFrameBuf::frame_write_complete_failure(cv::Mat const *frame_ptr)
    {
        if(!sync.is_configure.load())
        {
            return;
        }
        items_mtx.lock(); 

        CheckOwnershipReturnedFrameReports check_report = 
            checkValidReturnedFramePtr(frame_ptr, STATE_PROCESS_WRITE);
        if(check_report != REPORT_FRAME_VALID)
        {
            items_mtx.unlock(); 
        }
        if(check_report == REPORT_FRAME_WAS_CRUSHED)
        {
            std::cout << "ERROR: detect that frame was changed/crushed. Quit TSPipeFrameBuf" << std::endl;
            if(!quit())
            {
                std::cout << "ERROR: call quit(...) return false" << std::endl;
            }
        }
        if(check_report != REPORT_FRAME_VALID)
        {
            return;
        }

        MSid* msid_reloc_ptr = dispatch_framesPtr2mqsidPtr[frame_ptr];
        vec_remove(items_procWrite_vec, dispatch_frameID2procWriteVecID[msid_reloc_ptr->id]);
        msid_reloc_ptr->state = MSidStates::STATE_STORAGE_EMPTY;
        items_procStorageEmpty_vec.push_back(msid_reloc_ptr);
        

        dispatch_frameID2procWriteVecID[msid_reloc_ptr->id] = -1;
        dispatch_frameID2procStorageEmptyID[msid_reloc_ptr->id] = items_procStorageEmpty_vec.size() - 1;

        // write_frame_mtx.unlock();
        items_mtx.unlock();
    } // -- END frame_write_complete_failure

    int TSPipeFrameBuf::get_count_prepared()
    {
        if(!sync.is_configure.load())
        {
            return -1;
        }
        items_mtx.lock();
        int count_prepared = items_procStoragePrepared_queue.size();
        items_mtx.unlock();
        return count_prepared;
    } // -- END get_count_prepared

    int TSPipeFrameBuf::get_total_capacity()
    {
        return settings.total_capacity;
    } // -- END get_total_capacity

    TSPipeFrameBuf::CheckOwnershipReturnedFrameReports 
        TSPipeFrameBuf::checkValidReturnedFramePtr(
            cv::Mat const *     frame_ptr,
            MSidStates          required_state)
    {
        if(frame_ptr == nullptr)
        {
            return REPORT_FRAME_NOT_OWNING;
        }

        if(dispatch_framesPtr2mqsidPtr.find(frame_ptr) == dispatch_framesPtr2mqsidPtr.end())
        {
            return REPORT_FRAME_NOT_OWNING; // фрейм не имеет отношение к реестру собственных ресурсов TSPipeFrameBuf
        }

        MSid* msid_reloc_ptr = dispatch_framesPtr2mqsidPtr[frame_ptr];

        if(msid_reloc_ptr->state != required_state)
        {
            return REPORT_FRAME_WRONG_STATE; // несоответствие состояния в настоящий момент
        }

        // изменение объекта cv::Mat могло произойти только когда был указатель выдан на запись
        if(
            frame_ptr->data     != msid_reloc_ptr->frame_ptr->data  ||
            frame_ptr->size()   != settings.frame_size              ||
            frame_ptr->type()   != settings.frame_type              
            )
        {
            return REPORT_FRAME_WAS_CRUSHED;
        } // -- END checkValidReturnedFramePtr

        return REPORT_FRAME_VALID;
    } // -- END checkValidReturnedFramePtr
} // -- ts_pipe_frame_buf