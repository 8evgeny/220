#include "imgp.h"
#include <cmath>

#include <QPainter>

namespace imgp
{
    void digital_zoom(cv::Mat & src, cv::Mat & dst, const double & digital_zoom)
    {
        if(digital_zoom != 1.0)
        {
            int origin_w = src.cols;
            int origin_h = src.rows;
            int w = int(round(origin_w / digital_zoom));
            int h = int(round(origin_h / digital_zoom));
            int x = origin_w / 2 - w / 2;
            int y = origin_h / 2 - h / 2;
            cv::resize(src(cv::Rect(x, y, w, h)), dst, cv::Size(origin_w, origin_h), cv::INTER_LINEAR_EXACT);
        }
        else
        {
            dst = src;
        }
    }
    void inversion(cv::Mat & src, const bool & inversion_on)
    {
        if(inversion_on)
        {
            src = cv::Scalar::all(255) - src;
        }
    }
    void bc(cv::Mat & src, const int & brightness, const double & contrast)
    {
        if(brightness != 0)
        {
            src += cv::Scalar::all(brightness);
        }
        if(contrast != 1.0)
        {
            src *= contrast;
        }
    }
    void direction_inversion(cv::Mat & src, cv::Mat & dst, const bool & x_inv, const bool & y_inv)
    {
        if(x_inv || y_inv)
        {
            cv::flip(src, dst, x_inv ? (y_inv ? -1 : 0) : 1);
        }
        else
        {
            dst = src;
        }
    }
    double fit(cv::Mat & src, cv::Mat & dst, const cv::Size & dst_size)
    {
        double multiplier = 0;
        if( (src.cols != dst_size.width) || (src.rows != dst_size.height) ) {
            dst = cv::Mat::zeros(dst_size.height, dst_size.width, src.type());
            int h = dst_size.width * src.rows / src.cols;
            int w = dst_size.height * src.cols / src.rows;
            if(h <= dst_size.height) {
                multiplier = h / dst_size.height;
                cv::resize(src, dst(cv::Rect(0, (dst_size.height - h) / 2, dst_size.width, h)), cv::Size(dst_size.width, h), cv::INTER_LINEAR_EXACT);
            } else {
                multiplier = w / dst_size.width;
                cv::resize(src, dst(cv::Rect((dst_size.width - w) / 2, 0, w, dst_size.height)), cv::Size(w, dst_size.height), cv::INTER_LINEAR_EXACT);
            }
        }  else {
            multiplier = 1.;
            dst = src;
        }
        return multiplier;
    }

    QImage cv2qimage(const cv::Mat & frame)
    {
      if(frame.channels() == 3)
      {
          return Mat2QImage(frame);
      }
      else
      {
          return GrayMat2QImage(frame);
      }
    }
    QImage GrayMat2QImage(cv::Mat const& src)
    {
      cv::Mat temp;
      src.copyTo(temp);
      QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Indexed8);
      dest.bits();
      return dest;
    }
    QImage Mat2QImage(cv::Mat const& src)
    {
      cv::Mat temp;
      cvtColor(src, temp, cv::COLOR_BGR2RGB);
      QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
      dest.bits();
      return dest;
    }
    cv::Mat QImage2Mat(QImage const& src)
    {
      cv::Mat tmp(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
      cv::Mat result;
      cvtColor(tmp, result, cv::COLOR_RGB2BGR);
      return result;
    }
}
