#include "recorder.hpp"

#include <sys/types.h>
#include <sys/stat.h>

Recorder::Recorder(const std::string path, const std::string keyWord, bool &ok) :
    _keyWord(keyWord)
{
    ok = false;
    if(getSettings(path))
        ok = true;
}

bool Recorder::init(int id)
{
    bool isDirExist = true;
    if(id == 0)
    {
        while(isDirExist){
            id = rand() % 999999 + 100000;
            fullPathImages = pathImages + "/rec_" + _keyWord + "_" + std::to_string(id);
            isDirExist = checkDirectoryExist(fullPathImages);
        }
    }
    else
    {
        fullPathImages = pathImages + "/rec_" + _keyWord + "_" + std::to_string(id);
    }
    _id = id;
    fullPathDescriptor = pathDescriptor + "/way2init_rect" +
            std::to_string(id) + ".txt";
    std::string cmd_mkdir = "mkdir " + fullPathImages;
    system(cmd_mkdir.c_str());
    frameCnt = 0;
    _isInited = true;

    get_imwrite_params();

    return true;
}

void Recorder::deinit()
{
    if(_isInited)
        writeDescription();
    fullPathImages = "";
    fullPathDescriptor = "";
    _isInited = false;
}

bool Recorder::isInited()
{
    return _isInited;
}

bool Recorder::writeDescription(const cv::Rect2f rect)
{
    std::ofstream file(fullPathDescriptor);
    if(!file.is_open()){
        std::cout << "Can't open file: " << fullPathDescriptor << std::endl;
        return false;
    }
    file.close();
    descriptionRect = rect;
    return true;
}

bool Recorder::writeImage(const cv::Mat &image)
{
    std::string currPath = fullPathImages + "/" +
            std::to_string(startImageIndex + frameCnt) + ".jpg";
    if(cv::imwrite(currPath, image, params)){
        std::cout << "Write file " << currPath << std::endl;
        frameCnt++;
        return true;
    }
    return false;
}

void Recorder::setFrameCounter(const int cnt)
{
    if(startFrameNum == 0)
        startFrameNum = cnt;
    frameCnt = cnt;
}

int Recorder::getID()
{
    return _id;
}

void Recorder::deleteFiles()
{
    std::ofstream file(fullPathDescriptor);
    if(file.is_open())
    {
        file.close();
        std::string cmd_del_descriptor = "rm " + fullPathDescriptor;
        system(cmd_del_descriptor.c_str());
    }
    std::string cmd_del_images = "rm -r " + fullPathImages;
    system(cmd_del_images.c_str());
    _isInited = false;
}

bool Recorder::getSettings(const std::string path_to_ini)
{
    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0){
        std::cout << "ini reader: Parse error";
        return false;
    }

    pathImages = reader.Get("recorder", "path_for_images", "oops");
    if(pathImages == "oops"){
        std::cout << "ini reader: Parse path_for_images error\n";
        return false;
    }

    pathDescriptor = reader.Get("recorder", "path_for_descriptor", "oops");
    if(pathDescriptor == "oops"){
        std::cout << "ini reader: Parse path_for_descriptor error\n";
        return false;
    }


    return true;
}

bool Recorder::checkDirectoryExist(const std::string path)
{
    bool isExist;
    struct stat info;
    if( stat( path.c_str(), &info ) != 0 )
        isExist = false;
    else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows
        isExist = true;
    return isExist;
}

bool Recorder::writeDescription()
{
    std::ofstream file(fullPathDescriptor);
    if(file.is_open()){
        float halfWidth = descriptionRect.width / 2;
        float halfHeight = descriptionRect.height / 2;
        file << descriptionRect.x + halfWidth << " " <<
                descriptionRect.y + halfHeight << " " <<
                halfWidth << " " << halfHeight << " ";
        file << startFrameNum << " " << frameCnt << std::endl;
    } // -- END if(!f_in.is_open())
    else{
        std::cout << "Can't open file: " << fullPathDescriptor << std::endl;
        return false;
    }
    file.close();
    return true;
}
