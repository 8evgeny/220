#include "cv_main_window.hpp"

using namespace std;
using namespace cv;

namespace cvw
{

cv::Rect2f rct_pix2f(cv::Rect rct_pix, cv::Size frame_size)
{
    return cv::Rect2f((float)rct_pix.x / frame_size.width, (float)rct_pix.y / frame_size.height, (float)rct_pix.width / frame_size.width, (float)rct_pix.height / frame_size.height);
} // -- END rct_pix2dimless

cv::Rect rct_f2pix(cv::Rect2f rct_f, cv::Size frame_size)
{
    return cv::Rect(round(rct_f.x * frame_size.width), round(rct_f.y * frame_size.height), round(rct_f.width * frame_size.width), round(rct_f.height * frame_size.height));
} // -- END rct_pix2dimless

Panel::Panel(cv::Rect2f main_rct_, uint cols_, uint rows_, uint bord_, cv::Size sz_)
{
    main_rct = main_rct_;
    cols = cols_;
    rows = rows_;
    bord = bord_;
    bord_x_w = (float)bord / sz_.width;
    bord_y_h = (float)bord / sz_.height;
    vrct.reserve(cols * rows);
    for(int y = 0; y < rows; y++)
    {
        for(int x = 0; x < cols; x++)
        {
            Rect2f rct = Rect2f(main_rct.x + x * main_rct.width / cols + bord_x_w, main_rct.y + y * main_rct.height / rows + bord_y_h, main_rct.width / cols - 2 * bord_x_w, main_rct.height / rows - 2 * bord_y_h);
            vrct.emplace_back(rct);
        } // END for(int x = 0; x < cols; x++)
    } // END for(int y = 0; y < rows; y++)
} // END Panel

void Panel::show(cv::Mat &image)
{
    for(int i = 0; i < vrct.size(); i++)
    {
        Rect rct2show = Rect2f(round(vrct[i].x * image.cols),round(vrct[i].y * image.rows),round(vrct[i].width * image.cols),round(vrct[i].height * image.rows));
        rectangle(image, rct2show, show_clr, bord);
    } // END for(int i = 0; i < vrct.size(); i++)
} // END show

Rect Panel::get_pos_pix(uint x, uint y, cv::Size sz_)
{
    int pos = x + y * cols;
    bool ok = true;
    if(x > cols - 1)
    {
        cout << "Panel::INCORRECT X POSITION" << endl;
    } // END if(x > cols - 1)
    if(y > rows - 1)
    {
        cout << "Panel::INCORRECT Y POSITION" << endl;
    } // END if(y > rows - 1)
    if( (x > cols - 1) || (y > rows - 1))
    {
        cout << "Panel::ERROR POSITION IN PANEL!";
        return Rect(0,0,0,0);
    } // END if( (x > cols - 1) || (y > rows - 1))

    if(pos < vrct.size())
    {
        Rect rct2show = Rect(round(vrct[pos].x * sz_.width),round(vrct[pos].y * sz_.height),round(vrct[pos].width * sz_.width),round(vrct[pos].height * sz_.height));
        return rct2show;
    } // END if(pos < vrct.size())
} // END getPos

Rect2f Panel::get_pos_f(uint x, uint y)
{
    int pos = x + y * cols;
    bool ok = true;
    if(x > cols - 1)
    {
        cout << "Panel::INCORRECT X POSITION" << endl;
    } // END if(x > cols - 1)
    if(y > rows - 1)
    {
        cout << "Panel::INCORRECT Y POSITION" << endl;
    } // END if(y > rows - 1)
    if( (x > cols - 1) || (y > rows - 1))
    {
        cout << "Panel::ERROR POSITION IN PANEL!";
        return Rect(0,0,0,0);
    } // END if( (x > cols - 1) || (y > rows - 1))

    if(pos < vrct.size())
    {
        return vrct[pos];
    } // END if(pos < vrct.size())
} // -- END get_pos_f

}; // -- END namespace cvw
