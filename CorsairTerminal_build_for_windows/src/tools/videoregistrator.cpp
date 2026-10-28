#include "videoregistrator.h"


#include <ctime>
#include <ostream>
#include <cstdio>
#include <QDir>
#include <opencv2/core/utils/filesystem.hpp>
#include <algorithm>

using namespace cv;
using namespace std;

static string get_datetime()
{
    time_t t = time(nullptr);
    struct tm * now = localtime(&t);
    ostringstream os;
    os << now->tm_year + 1900 << "_" << setw(2) << setfill('0') << now->tm_mon << "_" << setw(2) << setfill('0')  << now->tm_mday << "_" << setw(2) << setfill('0') << now->tm_hour << "_" << setw(2) << setfill('0') << now->tm_min << "_" << setw(2) << setfill('0') << now->tm_sec;
    return os.str();
}
static string get_extention(const string & file_name)
{
    stringstream test(file_name);
    string segment;
    vector<string> seglist;
    while(getline(test, segment, '.'))
    {
       seglist.push_back(segment);
    }
    return seglist.back();
}
static vector<string> get_existing_video_files(const string & path)
{
    vector<string> files;
    QDir dir(QString(path.c_str()));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);   ///устанавливаем фильтр выводимых файлов/папок (см ниже)
    dir.setSorting(QDir::Name | QDir::Reversed);   //устанавливаем сортировку "от меньшего к большему"
    QFileInfoList list = dir.entryInfoList(QStringList("*.mp4"));     //получаем список файлов директории
    for (auto & p : list) {
        files.push_back(p.filePath().toStdString());
    }
    return files;
}

VideoRegistrator::VideoRegistrator(const string & path, const string & file_name_prefix,
                                   const Size & size, const int & fourcc, const int & fps,
                                   const int & len_sec, const int & max_cnt):
    _path(path),
    _file_name_prefix(file_name_prefix),
    _size(size),
    _fourcc(fourcc),
    _fps(fps),
    _len_sec(len_sec),
    _len(len_sec * fps),
    _max_cnt(max_cnt),
    cur_len(len_sec * fps)
{
    cv::utils::fs::createDirectory(path);
    vector<string> existing_records = get_existing_video_files(_path);
    sort(existing_records.begin(), existing_records.end());
    for(size_t i = 0; i < existing_records.size(); ++i)
    {
        records.push(existing_records[i]);
        /*if(records.size() > size_t(_max_cnt))
        {
            string file_name = records.front();
            remove(file_name.c_str());
            records.pop();
        }*/
    }
}
VideoWriter& VideoRegistrator::operator << (const Mat & image)
{
    if( ((cur_len == _len) && isOpened()) || // если запись достигла максимального количество кадров
      ( (image.cols != _size.width) || (image.rows != _size.height) ) ) // или пришел кадр с другими размерами
    {
        release(); // закрыть файл
    }
    if(!isOpened())
    {
        cur_len = 0;
        /*if(records.size() == size_t(_max_cnt))
        {
            string file_name = records.front();
            remove(file_name.c_str());
            records.pop();
        }*/
        string file_name = get_video_filename();
        records.push(file_name);
        _size = image.size();
        open(get_video_filename(), _fourcc, _fps, _size);
    }
    ++cur_len;
//    if( (image.cols != _size.width) || (image.rows != _size.height) )
//    {
//        Mat scaled_image;
//        resize(image, scaled_image, _size);
//        return VideoWriter::operator<<(scaled_image);
//    }
    return VideoWriter::operator<<(image);
}
void VideoRegistrator::save_image(const cv::Mat & image)
{
    cv::imwrite(get_image_filename(), image);
}
string VideoRegistrator::get_video_filename()
{
    ostringstream os;
    os << _path << "/" << _file_name_prefix << "_" << get_datetime() << ".mp4";
    string file_name = os.str();
    return file_name;
}
string VideoRegistrator::get_image_filename()
{
    ostringstream os;
    os << _path << "/" << _file_name_prefix << "_" << get_datetime() << ".png";
    string file_name = os.str();
    return file_name;
}
