#ifndef IMGP_H
#define IMGP_H

#include <QImage>
#include <opencv2/opencv.hpp>

namespace imgp
{
    void digital_zoom(cv::Mat & src, cv::Mat & dst, const double & digital_zoom);
    void inversion(cv::Mat & src, const bool & inversion_on);
    void bc(cv::Mat & src, const int & brightness, const double & contrast);
    void direction_inversion(cv::Mat & src, cv::Mat & dst, const bool & x_inv, const bool & y_inv);
    double fit(cv::Mat & src, cv::Mat & dst, const cv::Size & dst_size);

    QImage cv2qimage(const cv::Mat & frame);
    QImage GrayMat2QImage(cv::Mat const & src);
    QImage Mat2QImage(cv::Mat const & src);
    cv::Mat QImage2Mat(QImage const & src);
}
#endif // IMGP_H
