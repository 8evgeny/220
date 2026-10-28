#ifndef VIDEOREGISTRATOR_H
#define VIDEOREGISTRATOR_H

#include <opencv2/opencv.hpp>

class VideoRegistrator: public cv::VideoWriter
{
public:
    VideoRegistrator(const std::string & path, const std::string & file_name_prefix,
                     const cv::Size & size, const int & fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'), const int & fps = 25,
                     const int & len_sec = 180, const int & max_cnt = 60);

    std::string path() {return _path;}
    std::string file_name_prefix() {return _file_name_prefix;}
    int fourcc() {return _fourcc;}
    int fps() {return _fps;}
    cv::Size size() {return _size;}
    int len_sec() {return _len_sec;}
    int len() {return _len;}
    int max_cnt() {return _max_cnt;}
    size_t cnt() {return records.size();}
    void set_fps(const int & fps) {_fps = fps; cur_len = _len;}
    void set_size(const cv::Size & size) {_size = size; cur_len = _len;}
    void set_len_sec(const int & len_sec) {_len_sec = len_sec; _len = len_sec * _fps; cur_len = _len;}
    void set_len(const int & len) {_len = len; _len_sec = len / _fps; cur_len = _len;}
    void set_max_cnt(const int & max_cnt) {_max_cnt = max_cnt;}

    cv::VideoWriter & operator << (const cv::Mat & image) override;

    void save_image(const cv::Mat & image);

private:
    std::string get_video_filename();
    std::string get_image_filename();
private:
    const std::string _path;
    const std::string _file_name_prefix;
    cv::Size _size;
    const int _fourcc;
    int _fps;
    int _len_sec;
    int _len;
    int _max_cnt;

    int cur_len;

    std::queue<std::string> records;
};


#endif // VIDEOREGISTRATOR_H
