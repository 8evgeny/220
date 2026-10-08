#include "folderreader.h"

using namespace std;
using namespace cv;

FolderReader::FolderReader()
{
    cout << "Constructor FolderReader\n";
} // END FolderReader

FolderReader::~FolderReader(){cout << "Destructor FolderReader" << endl;}

FolderReader::FolderReader(const string& path_to_ini, bool& ok, int & dev_fps)
{
    cout << "Begin Constructor FolderReader\n";
    ok = getSettings(path_to_ini);
    if(!ok){return;}
    dev_fps = fps;

    if(!rect_init_from_file)
    {
        if(num_fr_0 >= 0 && num_fr_0 < fileList.size())
        {
            firstFrameNum = num_fr_0;
        frame_id = firstFrameNum;
        }
        else
        {
            firstFrameNum = 0;
        frame_id = 0;
        }
        if(num_fr_1 > firstFrameNum && num_fr_1 <= fileList.size())
        {
            lastFrameNum = num_fr_1;
        }
        else
        {
            lastFrameNum = fileList.size();
        }
    } // if(!rect_init_from_file)
    else 
    {
        if(lastFrameNum == 0)
        {
            lastFrameNum = fileList.size();
        }
        frame_id = firstFrameNum - delay_value;
    }
    lastFrameNum1 = lastFrameNum - 5;
    if(lastFrameNum1 < 5){lastFrameNum1 = 5;}
    cout << "firstFrameNum=" << firstFrameNum << "; lastFrameNum=" << lastFrameNum << endl;
    cout << "frame_id=" << frame_id << "/" << fileList.size() << endl;

#ifdef USE_ROI_TMP
    string frame_et_way = "../img_et/krugi.png";
    frame_et = imread(frame_et_way, cv::ImreadModes::IMREAD_GRAYSCALE);
#endif // END USE_ROI_TMP

    tp_get_fr = chrono::high_resolution_clock::now();

    string filename = path + "/" + fileList[frame_id];
    if(!frame_w || !frame_h)
    {
        frame = imread(filename);
        frame_w = frame.cols;
        frame_h = frame.rows;
        cout << "FolderReader:: autodetextion Frame size != " << frame.size() << endl;
    } // END if(!frame_w || !frame_h)

    if(parallel)
    {
        thread execution(&FolderReader::exec, this);
        execution.detach();
    } // END if(parallel)
#ifdef GUI_OK
    imshow("first_frame", frame);
    unsigned char key = waitKey(1000);
    destroyWindow("first_frame");
    if(key == key_exit){ok = 0; return;}
#endif // END #ifdef GUI_OK
    cout << "END Constructor FolderReader\n";
} // -- END FolderReader


void FolderReader::keyHandler(unsigned char & key)
{
    if(END_FileList){key = key_exit;}
} // -- END keyHandler

//void FolderReader::start()
//{
//    if(parallel)
//    {
//        thread execution(&FolderReader::exec, this);
//        execution.detach();
//    }
//} // -- END FolderReader

void FolderReader::workflow()
{
}

uint8_t *FolderReader::receiveFrame(int &w, int &h, int &id, int &num)
{
    if(frame_ready.load())
    {
        frame_mutex.lock();
        w = frame.cols;
        h = frame.rows;
        return frame.data;
        frame_mutex.unlock();
        frame_ready.store(false);
    }
    return nullptr;
} // -- END receiveFrame


void FolderReader::getFrame(Mat& frame)
{
    tp_get_fr_cur = chrono::high_resolution_clock::now();
    delay_get_fr_ms = chrono::duration_cast<chrono::milliseconds>(tp_get_fr_cur - tp_get_fr).count();
    if(delay_get_fr_ms < sourceFrameDelay_ms)
    {
        this_thread::sleep_for(chrono::milliseconds(sourceFrameDelay_ms - delay_get_fr_ms));
    }
    tp_get_fr = chrono::high_resolution_clock::now();
    bool success = getNextFrame(frame);
} // -- END getFrame

bool FolderReader::getFirstFrame(Mat &frame)
{
    frame_id = firstFrameNum;
    bool success = getNextFrame(frame);
    frame_id = firstFrameNum;
    return success;
} // -- END getFirstFrame

int FolderReader::getFrameCounter()
{
    return frameCnt;
} // -- END getFrameCounter


int FolderReader::getColorChannels()
{
#if defined(CCM_8UC1)
    return 1;
#elif defined(CCM_8UC3)
    return 3;
#else
    return 0;
#endif
} // -- END getColorChannels


bool FolderReader::isBayerColorChannel()
{
    return false;
} // -- END isBayerColorChannel


void FolderReader::getFormatedImage(uint8_t * dat, int w, int h, int id, cv::Mat & image)
{
#if defined(CCM_8UC1)
    memcpy(image.data, dat, image.total());
#elif defined(CCM_8UC3)
    memcpy(image.data, dat, 3*image.total());
#else
    throw std::runtime_error("Not supported color space for output format");
#endif
} // END getFormatedImage

void FolderReader::quit()
{
    if(parallel)
    {
        _execute.store(false, memory_order_release);
        this_thread::sleep_for(chrono::milliseconds(sourceFrameDelay_ms * 2));
    }
} // -- END quit

int FolderReader::getId()
{
   return frame_id;
} // END -- getId


void FolderReader::exec()
{
    _execute.store(true, memory_order_release);
    handle_frame_execute.store(true, memory_order_release);
    thread frameHandler(&FolderReader::runHandleFrame, this);
    WatchDog timer(1000);
    bool success = 0;
    while(_execute.load(memory_order_acquire))
    {
        chrono::system_clock::time_point timePoint = chrono::system_clock::now() + chrono::milliseconds(sourceFrameDelay_ms);
        success = getNextFrame(frame);
        if(!success)
        {
            break;
        } // END if(!success)
        this_thread::sleep_until(timePoint);
        // this_thread::sleep_for(chrono::milliseconds(sourceFrameDelay_ms));
    } // -- END while(_execute.load(memory_order_acquire))
    handle_frame_execute.store(false, memory_order_release);
    frameCV.notify_one();
    frameHandler.join();
    quit();
} // -- END exec

bool FolderReader::getSettings(const string& path_to_ini)
{
    cout << "Begin FolderReader::getSettings\n";

    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0)
    {
        cout << "ini reader parse error!\n";
        return 0;
    }

    parallel = reader.GetInteger("main_settings", "parallel", -1);
    if(parallel == -1)
    {
        cout << "parallel not declared!!" << endl;
        return 0;
    }
    else
    {
        cout << "parallel = " << parallel << ";\n";
    }

    int manual = reader.GetInteger("main_settings", "get_rect_manual", -1);
    if(manual == -1)
    {
        std::cout << "ini reader parse path error: get_rect_manual not found!\n";
        return false;
    }
    else
    {
        rect_init_from_file = !static_cast<bool>(manual);
    }

    if(rect_init_from_file)
    {
        std::string way2init_rect = reader.Get("folder", "way2init_rect", "oops");
        if(way2init_rect == "oops")
        {
            std::cout << "ini reader parse path error: way2init_rect not found!\n";
            return false;
        }
        else
        {
            loadFirstLastFrameNum(way2init_rect);
        } // -- END if(way2init_rect == "oops")
    } // -- END if(manual == 0)

    repeat = reader.GetInteger("folder", "repeat", -1);
    if(repeat == -1)
    {
        cout << "ini reader parse error: repeat not found!\n";
        return 0;
    }
    else
    {
        cout << "repeat=" << repeat << "\n";
    }

    num_fr_0 = reader.GetInteger("folder", "num_fr_0", -1);
    if (num_fr_0 == -1)
    {
        cout << "ini reader: Parse source num_fr_0 name error\n";
        return 0;
    }
    else
    {
        cout << "num_fr_0=" << num_fr_0 << endl;
    }

    num_fr_1 = reader.GetInteger("folder", "num_fr_1", -1);
    if (num_fr_1 == -1)
    {
        cout << "ini reader: Parse source num_fr_1 name error\n";
        return 0;
    } // END if (num_fr_1 == -1)
    else
    {
        cout << "num_fr_1=" << num_fr_1 << endl;
    } // END else

    frame_w = reader.GetInteger("folder", "frame_w", -1);
    if (frame_w == -1)
    {
        cout << "ini reader: Parse source frame_w name error\n";
        return 0;
    } // END if (frame_w == -1)
    else
    {
        cout << "frame_w=" << frame_w << endl;
    } // END else

    frame_h = reader.GetInteger("folder", "frame_h", -1);
    if (frame_h == -1)
    {
        cout << "ini reader: Parse source frame_h name error\n";
        return 0;
    } // END if (frame_h == -1)
    else
    {
        cout << "frame_h=" << frame_h << endl;
    } // END else

    fps = reader.GetReal("folder", "fps", -1);
    if(fps == -1)
    {
        cout << "ini reader: Parse source fps name error\n";
        return 0;
    } // END if(fps == -1)
    else
    {
        if(fps != 0)
        {
            sourceFrameDelay_ms = round(1000.f / fps);
        } // END if(fps != 0)
        else
        {
            sourceFrameDelay_ms = 0;
        } // END else
        fps_count_max = 2 * fps;
        cout << "fps=" << fps << endl;
    } // END else

#ifdef USE_ESRT
    path2esrt = reader.Get("folder", "path2esrt", "oops");
    if(path2esrt == "oops"){cout << "folder path2esrt not declared!\n"; return 0;}
    else{cout << "path2esrt = " << path2esrt << ";\n";}
#endif // USE_ESRT

#ifdef USE_TELETXT
    path2teletxt = reader.Get("folder", "path2teletxt", "oops");
    if(path2teletxt == "oops"){cout << "folder path2teletxt not declared!\n"; return 0;}
    else{cout << "path2teletxt = " << path2teletxt << ";\n";}
#endif // USE_TELETXT

    path = reader.Get("folder", "path", "oops");
    if(path == "oops"){std::cout << "folder path not declared!\n"; return 0;}
    else{cout << "folder_path = " << path << ";\n";}

    bool folder_path_ok = getFileList(path, fileList);
    if(!folder_path_ok)
    {
        cout << "WRONG folder_path OR folder is EMPTY !\n";
        return 0;
    } // -- END if(!folder_path_ok)


    // colorChannelsNum = reader.GetInteger(yolo_block, "color_channels", -1);
    // if(colorChannelsNum == -1)
    // {
    //     cout << "ini reader parse warning: color_channels not found!\n";
    //     return 0;
    // }
    // else
    // {
    //     cout << "colorChannelsNum=" << colorChannelsNum << endl;
    // }
    cout << "END FolderReader::getSettings\n";
    return 1;
} // -- END getSettings


bool FolderReader::getFileList(std::string& path, std::vector<std::string>& fileList)
{
    fileList.clear();
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            string filename = string(ent->d_name);
            // -- typ = 4 (folder), typ = 8 (file).
            if (ent->d_type == 8 && filename != "." && filename != "..")
            {
                fileList.emplace_back(filename);
            }
        } // -- END while((ent = readdir (dir)) != NULL)
        closedir(dir);
        std::sort(fileList.begin(), fileList.end());
        if(fileList.size())
        {
            return true;
        }
    } // -- END if((dir = opendir(way)) != NULL)
    cout << "Dir " << path << " can't open or empty!" << endl;
    return false;
} // -- END getFileList



bool FolderReader::FileIsExist(const string& filePath)
{
    bool isExist = false;
    std::ifstream fin(filePath.c_str());
    if(fin.is_open()){isExist = true;}
    fin.close();
    return isExist;
} // -- END FileIsExist


bool FolderReader::getNextFrame(cv::Mat& frame)
{
    if(frame_id < lastFrameNum)
    {
        string file_name = fileList[frame_id];
        string filename = path + "/" + file_name;
        //cout << "filename=" << filename << endl;
        if(!FileIsExist(filename))
        {
            cout << "filename=" << filename << " DON't Exist!" << endl;
            return 0;
        } // -- END if(!FileIsExist(filename))
        fps_count++;
        if(fps_count > fps_count_max)
        {
           fps_count = 0;
           cout << "frame_id=" << frame_id << "/" << lastFrameNum << "; file_name=" << file_name << endl;
        } // ENF if(fps_count > fps_count_max)
        frame_id++;
        //            std::scoped_lock guard(frame_mutex);
#if defined(CCM_8UC1)
        frame = imread(filename, cv::ImreadModes::IMREAD_GRAYSCALE);
#elif defined(CCM_8UC3)
        frame = imread(filename, cv::ImreadModes::IMREAD_COLOR);
#else
        throw std::runtime_error("Not supported color space for output format");
#endif
        if(!frame.data){cout << "NOT data for filename=" << filename << endl;}

#ifdef USE_ROI_TMP
        frame_et.copyTo(frame(Rect(frame_et_xy.x, frame_et_xy.y, frame_et.cols, frame_et.rows)));
#endif // END USE_ROI_TMP

        frame_ready.store(true, memory_order_release);
        frameCV.notify_one();

        if(repeat == 0 && frame_id > lastFrameNum1)
        {
            END_FileList = 1;
            cout << "END FileList!" << endl;
        } // END if(repeat == 0 && frame_id > lastFrameNum1)

        return 1;
        // -------------- END if(frame_id < lastFrameNum)
    } // END if(frame_id < lastFrameNum)
    else
    {
        if(firstFrameNum >= 0 && firstFrameNum < fileList.size())
        {
            frame_id = firstFrameNum - delay_value;
        }
        else
        {
            frame_id = 0;
        }
        return 1;
    } // -- END if(frame_id >= lastFrameNum)
    return 0;
} // -- END getNextFrame

bool FolderReader::getState()
{
    return 1;
} // END getState

void FolderReader::runHandleFrame()
{
    while(handle_frame_execute.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lk(frame_mutex);
        frameCV.wait(lk);
        if(!handle_frame_execute.load(std::memory_order_acquire))
        {
            break;
        }
        for(auto handler : frame_handlers)
        {
            handler->handle(frame.data, frame.cols, frame.rows, 0, 0);
        } // -- END for(auto handler : frame_handlers)
        lk.unlock();
        frame_ready.store(false, std::memory_order_release);
    } // -- END while
} // -- END runHandleFrame

bool FolderReader::loadFirstLastFrameNum(std::string path)
{
    std::ifstream f_in(path);
    if(!f_in.is_open())
    {
        cout << "File " << path << " NOT open!" << endl;
        return false;
    } // -- END if(!f_in.is_open())
    std::string item0, item1, item2, item3, item4, item5, item6;

    f_in >> item0; // Безразмерная x-координата центра объекта в единицах ширины фрейма,
    f_in >> item1; // Безразмерная y-координата центра объекта в единицах высоты фрейма,
    f_in >> item2; // Безразмерная полуширина прямоугольника цели в единицах ширины фрейма,
    f_in >> item3; // Безразмерная полувысота прямоугольника цели в единицах высоты фрейма,
    f_in >> item4; // Номер первого фрейма,
    f_in >> item5; // Номер последнего фрейма.
    f_in >> item6; // Величина задержки.
    f_in.close();
    try {firstFrameNum = std::stoi(item4);}
    catch (...) {firstFrameNum = 0;}
    lastFrameNum = std::stoi(item5);
    try {lastFrameNum = std::stoi(item5);}
    catch (...) {lastFrameNum = 0;}
    try {delay_value = std::stoi(item6);}
    catch (...) {delay_value = 0;}
    if(firstFrameNum - delay_value < 0){delay_value = firstFrameNum;}
    frameCnt = firstFrameNum;
    return true;
} // -- END loadFirstLastFrameNum
