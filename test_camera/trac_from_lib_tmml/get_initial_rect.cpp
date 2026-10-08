#include <vector>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <string>
#include <condition_variable>
#include <mutex>
#include <map>
#include <list>
#include <thread>
#include <future>
#include <dirent.h>

#include <opencv2/core/utility.hpp>

#include "opencv2/imgproc.hpp"
#include <opencv2/core/core.hpp>

#include <opencv2/highgui/highgui.hpp>
#include "opencv2/highgui.hpp"

#include "INIReader.h"

using namespace std;
using namespace cv;
using namespace chrono;

int wh0_2 = 24; // Величина полуширины квадратной рамки.

int pref = 10000000;
static bool click_down = 0;
static bool click_up = 0;
static Point p_down = Point(0, 0);

class get_initial_rect
{
 public:
   // конструктор:
   get_initial_rect(const std::string& config_path, bool& ok)
   {
      ok = get_ini_params(config_path);
      if(!ok){return;}
   } // -- END get_initial_rect (конструктор)
   ~get_initial_rect() = default;

   static void CallBackFunc(int event, int x, int y, int flags, void* param)
   {
        Point *p = (Point*)param;
        p->x = x;
        p->y = y;
        switch(event)
        {
           case EVENT_LBUTTONDOWN:
           {
               click_down = 1;
               p_down = *p;
               break;
           }
           case EVENT_LBUTTONUP:
           {
               if(click_down){click_up = 1;}
               break;
           }
        } // -- END switch(event)
   } // -- END CallBackFunc

   string way2img, way2save_file;

   bool FileIsExist(const string& filePath)
   {
       bool isExist = false;
       ifstream fin(filePath.c_str());
       if(fin.is_open()){isExist = true;}
       fin.close();
       return isExist;
   } // -- END FileIsExist

   void save_txt(const string& file, const string& txt)
   {
       setlocale(LC_NUMERIC, "en_US.UTF-8");
       ofstream f_out;
       f_out.open(file, ios::app);
       f_out << txt;
       f_out.close();
   } // -- END save_txt

   bool get_ini_params(const string& config)
   {
      cout << "BEGIN get_ini_params" << endl;
      if(!FileIsExist(config)){cout << "File '" << config << "' not exist!" << endl; return 0;}
      INIReader reader(config);
      if(reader.ParseError()<0){cout << "Can't load '" << config << "'\n"; return 0;}
      setlocale(LC_NUMERIC, "en_US.UTF-8");

      way2img = reader.Get("get_initial_rect", "way2img", "oops");
      if(way2img == "oops"){cout << "way2img not declared!\n"; return 0;}
      else{cout << "way2img = " << way2img << ";\n";}

      way2save_file = reader.Get("get_initial_rect", "way2save_file", "oops");
      if(way2save_file == "oops"){cout << "way2save_file not declared!\n"; return 0;}
      else{cout << "way2save_file = " << way2save_file << ";\n";}

      return 1;
   } // -- END get_ini_params

   int win_x = 10;
   int win_y = 10;
   int wait_ms_work_mouse = 10;
   const string win_name = "Kvadrat";
   char key = 0;
   Point p_mouse = Point(0, 0);
   Rect2f object_rect;

   bool work_mouse(const Mat& img1, const string& win_name, char& key)
   {
      while(true)
      {
         Mat img2 = img1.clone();
         if(click_down)
         {
             Point dp = p_mouse - p_down;
             Point dp_abs(abs(dp.x), abs(dp.y));
             Rect rct(p_down - dp_abs, p_down + dp_abs);
             rectangle(img2, rct, red, 1);
             circle(img2, p_down, 2, green, -1);
         } //--END if(click_down)
         imshow(win_name, img2);
         moveWindow(win_name, win_x, win_y);
         key = waitKey(wait_ms_work_mouse);
         if(key == '`'){return 0;}
         setMouseCallback(win_name, CallBackFunc, &p_mouse);
         if(click_up)
         {
             click_up = 0;
             Point dp = p_mouse - p_down;
             Point dp_abs(abs(dp.x), abs(dp.y));
             Rect rct(p_down - dp_abs, p_down + dp_abs);
             rectangle(img2, rct, red, 1);
             circle(img2, p_down, 2, green, -1);
             if(rct.area()>0)
             {
                 click_down = 0;
                 object_rect = rct;
                 return 1;
             } // -- END if(rct.area()>0)
         } //--END if(click_after)
      } // -- END while
      return 0;
   } // -- END work_mouse

   Scalar red =  Scalar(0, 0, 255);
   Scalar blue = Scalar(255, 0, 0);
   Scalar green = Scalar(0, 255, 0);
   Scalar white = Scalar(255, 255, 255);
   Scalar black = Scalar(0, 0, 0);
   Scalar viol = Scalar(255, 0, 255);
   Scalar cyan = Scalar(255, 255, 0);
   Scalar yell = Scalar(0, 255, 255);
}; // -- END class get_initial_rect


int main(int argc, char* argv[])
{
    string config_path = "../tracking.ini"; // -- Путь к файлу конфигурации.
    if(argc != 2)
    {
        argc = 2;
        argv[1] = (char*)config_path.c_str();
    } // -- END if(argc != 2)

    bool ok = 0;
    get_initial_rect gr(argv[1], ok);
    if(!ok){cout << "NOT get_initial_rect!" << endl; return 1;}

    if(!gr.FileIsExist(gr.way2img))
    {
        cout << "Not img for way2img=" << gr.way2img << " !" << endl;
        return -1;
    }

    Mat img_orig = imread(gr.way2img);
    if(!img_orig.data)
    {
        cout << "Not data for img_orig!" << endl;
        return -1;
    }
    ok = gr.work_mouse(img_orig, gr.win_name, gr.key);
    if(!ok){cout << "ESC from get_initial_rect" << endl; return 1;}
    Point2f xy = 0.5*(gr.object_rect.tl() + gr.object_rect.br());
    Point2f wh_2 = 0.5*(gr.object_rect.br() - gr.object_rect.tl());

    if(wh0_2){wh_2 = Point2f(wh0_2, wh0_2);}

    float x = xy.x/img_orig.cols;
    float y = xy.y/img_orig.rows;
    float w_2 = wh_2.x/img_orig.cols;
    float h_2 = wh_2.y/img_orig.rows;    

    string txt = to_string(x) + " " + to_string(y) + " " + to_string(w_2) + " " + to_string(h_2) + " ";
    int i_ = gr.way2img.length();
    for(int i = gr.way2img.length(); i>0; i--)
    {
        if(gr.way2img[i] == '/'){i_ = i + 1; break;}
    } // -- END for(int i = ge.way2img.length(); i>0; i--)

    string txt2 = gr.way2img.substr(i_, gr.way2img.length() - i_ - 4);
    int num = stoi(txt2) - pref;
    cout << "txt2=" << txt2 << "; num=" << num << endl;
    txt += to_string(num) + " " + txt2;
    gr.save_txt(gr.way2save_file, txt);

    cout << endl << "END main" << endl;
    return 0;
} // -- END main
