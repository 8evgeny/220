#pragma once
#include <cstdint>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <string>
#include <list>
#include <set>
#include <condition_variable>

#include <opencv2/core/utility.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

struct tr
{
    cv::Point2f xy = cv::Point2f(-1.f, -1.f); // Координаты центра объекта.
    cv::Point2f wh_2 = cv::Point2f(0, 0); // Координаты центра объекта.
    char class_num = -1; // Номер класса
    double tp; // Точка времени.
}; // END struct tr

struct trac_st
{
    char trac_ok = 1; // Признак найденного трака.
    char rait = -10; // Рейтинг трака -- для признака удаления.
    std::list<tr> ltr; // Лист структур для траков (сам трак).
    cv::Point2f predict = cv::Point2f(0,0); // Прогноз точки трака на текущем кадре.
    cv::Point2f wh_2 = cv::Point2f(0,0); // Полу-ширина-высота объекта.
    char cls = -1; // Номер класса трака.
}; // END struct trac_st

struct trac_data
{
    float x = -1; // x-координата объекта трака.
    float y = -1; // y-координата объекта трака.
    float w = 0; // ширина объекта трака.
    float h = 0; // высота объекта трака.
    char rait = -10; // Рейтинг трака.
}; // END struct trac_data

struct trac_struct
{
   unsigned char key = 0; // Код нажатой клавиши на активном окне при показе изображения.
   uint8_t * tot_show_img_data = nullptr; // Указатель на ptr-данные cv::Mat изображения для показа.
   int tot_show_img_w = 0; // Ширина изображения для показа.
   int tot_show_img_h = 0; // Высота изображения для показа.
   char zahvat = 0; // Признак успешного захвата.
   char rect_ok = 0; // Признак успешного выделения цели мышкой.
   size_t work_number = 0; // Текущий номер рабочего кадра.
   char ok_match = 0; // Признак успешного захвата на текущем кадре.
   char validate = 0; // Степень валидации захвата на текущем кадре.
   char validate_min = 15; // Минимальное значение валидации непрерывного повторения нахождения рамки, после которого поиск считается успешным.
   float obj_xy_x = 0; // Безразмерная x-координата центра цели на текущем кадре (в единицах ширины фрейма).
   float obj_xy_y = 0; // Безразмерная y-координата центра цели на текущем кадре (в единицах высоты фрейма).
   float obj_wh_2_w = 0; // Безразмерная полуширина цели на текущем кадре (в единицах ширины фрейма).
   float obj_wh_2_h = 0; // Безразмерная полувысота цели на текущем кадре (в единицах высоты фрейма).

   int roi = 0;     // Признак работы в ROI.
   int roi_l = 0;   // Координата левого края roi.
   int roi_t = 0;   // Координата верхнего края roi.
   int roi_w = 0;   // Ширина roi.
   int roi_h = 0;   // Высота roi.

   uint8_t * img_orig_data = nullptr; // Указатель на ptr-данные оригинального cv::Mat изображения.
   int img_orig_type = 0; // Тип cv::Mat оригинального изображения.
   int fr_w0 = 0;         // Ширина оригинального кадра.
   int fr_h0 = 0;         // Высота оригинального кадра.
   char id = 0;           // Номер цели выполняемого алгоритма.
   char new_frame = 1;      // Признак формирования нового кадра.
   float scan_exec_time = 0; // Частота сканирования кадра.
   float yolo_exec_time = 0; // Средняя частота yolo.
   char work_in_round = 0; // Признак работы в круге (1), или в прямоугольнике (0).
   std::list<trac_data> * ltrac_ptr = nullptr;
   std::vector<trac_data> vtrac_data; // вектор характеристик траков.
   uint8_t trac_id = 0xFF;
}; // END trac_struct
