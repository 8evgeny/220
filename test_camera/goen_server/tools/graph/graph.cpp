#include "graph.hpp"

using namespace cv;
using namespace std;

Graph::Graph(bool &ok)
{
    init();
    if(img_rotate_vec.data) {ok = true;}
    else {ok = false;}
} // -- END Graph

void Graph::show()
{
    imshow(winname, img_rotate_vec);
} // -- END show

void Graph::init()
{
    namedWindow(winname, WINDOW_NORMAL); // DBG::
    img_rotate_vec = Mat(Size(width, width), CV_8SC3, white);
    t_sphere = 0;
    a_sphere = 0;
    k_sphere = 0;
    t_board = 0;
    a_board = 0;
    k_board = 0;

    blue = Scalar(255, 0, 0);
    green = Scalar(0, 255, 0);
    red = Scalar(0, 0, 255);
    black = Scalar(0, 0, 0);
    white = Scalar(255, 255, 255);

    width = 900;
    thick_line = 1;
    thick_text = 2;
    pt_zero = Point(0, 0);     //центр координат
    axis_len = 0;   // длина осей Z, Y
    vec_len = 0;       // длина единичного отрезка
    ax_angle = 60.f * k_deg2rad;    // угол оси Х
    ax_scale = cos(ax_angle);     //масштабирование отрезков на оси X
    z_ax = Point(0, 0); // оси координад
    z_ax_ = Point(0, 0);
    y_ax = Point(0, 0);
    y_ax_ = Point(0, 0);
    x_ax = Point(0, 0);
    x_ax_ = Point(0, 0);
} // END init

void Graph::coord_plane()
{
    img_rotate_vec = Mat(Size(width, width), CV_8SC3, white);
    float vec_scale = 0.33;
    float coord_cent = 0.5;
    float axis_scale = 0.4;
    pt_zero = Point(width * coord_cent, width * coord_cent);
    axis_len = width * axis_scale;
    vec_len = width * vec_scale;
    z_ax = Point(pt_zero.x + axis_len, pt_zero.y);
    z_ax_ = Point(pt_zero.x - axis_len, pt_zero.y);
    y_ax = Point(pt_zero.x, pt_zero.y - axis_len);
    y_ax_ = Point(pt_zero.x, pt_zero.y + axis_len);
    x_ax = Point(pt_zero.x + ax_scale * axis_len * cos(ax_angle), pt_zero.y - ax_scale * axis_len * sin(ax_angle));
    x_ax_ = Point(pt_zero.x - ax_scale * axis_len * cos(ax_angle), pt_zero.y + ax_scale * axis_len * sin(ax_angle));
} // END coord_plane

void Graph::draw_coord_plane()
{
    int arr_len = 25;                   // длина стрелки
    float arr_angle = 12 * k_deg2rad;   //угол между стрелкой и осями
    int pt_size = 3;
    float text_size = 1.5;

    // система координат
    line(img_rotate_vec, x_ax_, x_ax, black, thick_line);
    line(img_rotate_vec, y_ax_, y_ax, black, thick_line);
    line(img_rotate_vec, z_ax_, z_ax, black, thick_line);
    putText(img_rotate_vec, "Y", y_ax, FONT_HERSHEY_PLAIN, text_size, black, thick_text);
    putText(img_rotate_vec, "X", x_ax, FONT_HERSHEY_PLAIN, text_size, black , thick_text);
    putText(img_rotate_vec, "Z", z_ax, FONT_HERSHEY_PLAIN, text_size, black, thick_text);

    // стрелки
    Point arr_y1(y_ax.x - arr_len * sin(arr_angle), y_ax.y + arr_len * cos(arr_angle));
    Point arr_y2(y_ax.x + arr_len * sin(arr_angle), y_ax.y + arr_len * cos(arr_angle));
    Point arr_z1(z_ax.x - arr_len * cos(arr_angle), z_ax.y - arr_len * sin(arr_angle));
    Point arr_z2(z_ax.x - arr_len * cos(arr_angle), z_ax.y + arr_len * sin(arr_angle));
    Point arr_x1(x_ax.x - arr_len * cos(arr_angle + ax_angle), x_ax.y + arr_len * sin(arr_angle + ax_angle));
    Point arr_x2(x_ax.x - arr_len * cos(ax_angle - arr_angle), x_ax.y + arr_len * sin(ax_angle - arr_angle));

    line(img_rotate_vec, y_ax, arr_y1, black, thick_line);
    line(img_rotate_vec, y_ax, arr_y2, black, thick_line);
    line(img_rotate_vec, z_ax, arr_z1, black, thick_line);
    line(img_rotate_vec, z_ax, arr_z2, black, thick_line);
    line(img_rotate_vec, x_ax, arr_x1, black, thick_line);
    line(img_rotate_vec, x_ax, arr_x2, black, thick_line);

    // масштаб по осям
    Point y_1(pt_zero.x, pt_zero.y - vec_len);
    Point z_1(pt_zero.x + vec_len, pt_zero.y);
    Point y_1_(pt_zero.x, pt_zero.y + vec_len);
    Point z_1_(pt_zero.x - vec_len, pt_zero.y);
    Point x_1(pt_zero.x + ax_scale * vec_len * cos(ax_angle), pt_zero.y - ax_scale * vec_len * sin(ax_angle));
    Point x_1_(pt_zero.x - ax_scale * vec_len * cos(ax_angle), pt_zero.y+ ax_scale * vec_len * sin(ax_angle));

    circle(img_rotate_vec, y_1, pt_size, black, FILLED);
    circle(img_rotate_vec, z_1, pt_size, black, FILLED);
    circle(img_rotate_vec, x_1, pt_size, black, FILLED);
    circle(img_rotate_vec, y_1_, pt_size, black, FILLED);
    circle(img_rotate_vec, z_1_, pt_size, black, FILLED);
    circle(img_rotate_vec, x_1_, pt_size, black, FILLED);

    ellipse(img_rotate_vec, pt_zero, Size(vec_len, vec_len), 0, 0, 360, 2);

    for(int i = 0; i < 360; i++)
    {
        circle(img_rotate_vec, pt_zero + Point(vec_len * sin(i * k_deg2rad), ax_scale * vec_len * cos(ax_angle) * cos(i * k_deg2rad)), 2, red, 1);
    }
    // ellipse(img_rotate_vec, pt_zero, Size(vec_len, 1.2 * ax_scale * vec_len * cos(ax_angle)), 0, 0, 360, 2);
} // END draw_coord_plane()

int Graph::draw_rotate_vec(float yaw, float pitch, float roll, float x_res, float y_res, float z_res)
{
    // rotate_matrix(mode, t_board, a_board, k_board, t_sphere, a_sphere, t_res_, a_res_, k_res_);
    coord_plane();
    draw_vector_coords(x_res, y_res, z_res);
    draw_coord_plane();
    std::stringstream angle;
    angle << "yaw:  = " << trunc(yaw * 100.) / 100;
    putText(img_rotate_vec, angle.str(), Point(20,50), 1, 2, color::red, 3);
    angle.str(""); angle <<  "pitch:   = " << trunc(pitch * 100.) / 100;
    putText(img_rotate_vec, angle.str(), Point(20,100), 1, 2, color::red, 3);
    angle.str(""); angle <<  "roll:    = " << trunc(roll * 100.) / 100;
    putText(img_rotate_vec, angle.str(), Point(20,150), 1, 2, color::red, 3);
    return 1;
} // END draw_rotate_vec

Point Graph::transform_vec(Point src0, Point src1, Point src2, Point dst0, Point dst1, Point dst2, Point vec)
{
    Point2f src[3];
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;

    Point2f transf[3];
    transf[0] = dst0;
    transf[1] = dst1;
    transf[2] = dst2;

    Mat warp_matr = getAffineTransform(src, transf);
    Point vec_trans;
    double M00 = warp_matr.at<double>(0, 0);
    double M10 = warp_matr.at<double>(1, 0);
    double M01 = warp_matr.at<double>(0, 1);
    double M11 = warp_matr.at<double>(1, 1);
    double M02 = warp_matr.at<double>(0, 2);
    double M12 = warp_matr.at<double>(1, 2);
    vec_trans.x = round(vec.x * M00 + vec.y * M01 + M02);
    vec_trans.y = round(vec.x * M10 + vec.y * M11 + M12);

    return vec_trans;
} // END transform_vec()

void Graph::draw_vector_coords(float x_res, float y_res, float z_res) //построение вектора по координатам x_res y_res z_res
{
    float z_pr = round(z_res * vec_len);
    float x_pr = round(x_res * vec_len);
    float y_pr = round(y_res * vec_len);

    Point end_proj_XZ(round(pt_zero.x + z_pr), round(pt_zero.y - x_pr));
    Point end_proj_XZ_tr = ZX_transform(end_proj_XZ);  //проекция на плоскость XZ
    if (abs(x_res) < eps && (z_res < 0)) {end_proj_XZ_tr.y += 1;}  //для корректной отрисовки при t = -90

    // строим проекции как отклонения к осям на эту длину
    Point z_proj(pt_zero.x + z_pr, pt_zero.y);
    Point x_proj(end_proj_XZ_tr.x - z_pr, end_proj_XZ_tr.y);
    line(img_rotate_vec, z_proj, pt_zero, blue, 2 * thick_line);
    line(img_rotate_vec, x_proj, pt_zero, blue, 2 * thick_line);
    line(img_rotate_vec, end_proj_XZ_tr, z_proj, blue, 2 * thick_line);
    line(img_rotate_vec, end_proj_XZ_tr, x_proj, blue, 2 * thick_line);

    // второй параллелограмм (синий)
    Point z_proj2(z_proj.x, z_proj.y - y_pr);
    Point x_proj2(x_proj.x, x_proj.y - y_pr);
    Point proj_zero2(pt_zero.x, pt_zero.y - y_pr);
    Point proj_end_vec2(end_proj_XZ_tr.x, end_proj_XZ_tr.y - y_pr);
    line(img_rotate_vec, z_proj2, proj_zero2, blue, 2 * thick_line);
    line(img_rotate_vec, x_proj2, proj_zero2, blue, 2 * thick_line);
    line(img_rotate_vec, proj_end_vec2, z_proj2, blue, 2 * thick_line);
    line(img_rotate_vec, proj_end_vec2, x_proj2, blue, 2 * thick_line);

    line(img_rotate_vec, z_proj, z_proj2, blue, 2 * thick_line);
    line(img_rotate_vec, x_proj, x_proj2, blue, 2 * thick_line);

    //проекции на Y
    line(img_rotate_vec, end_proj_XZ_tr, proj_end_vec2, green, 2 * thick_line);
    line(img_rotate_vec, proj_zero2, pt_zero, green, 2 * thick_line);
    line(img_rotate_vec, end_proj_XZ_tr, pt_zero, green, 2 * thick_line);
    line(img_rotate_vec, proj_zero2, proj_end_vec2, green, 2 * thick_line);

    line(img_rotate_vec, pt_zero, proj_end_vec2, red, 3 * thick_line);
}   // END draw_vector_coords


Point Graph::ZX_transform(Point vec)
{
    Point vec_trans = transform_vec(y_ax, y_ax_, z_ax, x_ax, x_ax_, z_ax, vec);
    return vec_trans;
} // END ZX_transform()
