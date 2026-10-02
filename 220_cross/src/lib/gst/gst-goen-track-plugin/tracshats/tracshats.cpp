#include "tracshats.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

TracShats::TracShats(const string& config_path, Mat& frame_process_0, bool& ok)
{
    cout << "Constructor TrackShats!\n" << endl;
    ok = get_ini_params(config_path);
    if(!ok){cout << "NOT TracShats::get_ini_params"; return;}
    setOriginalFrameSize(frame_process_0);
    cout <<"BEGIN create_trac" << endl;
    trac = shared_ptr<class_name>(create_name(config_path.c_str(), ok, trac_str));
    if(!ok){cout << "NOT TracShats::shared_ptr::trac!\n"; return;}
    cout << "work_in_round=" << (int)trac_str.work_in_round << "; img_orig_type=" << trac_str.img_orig_type << endl;
} // END TracShats

void TracShats::setOriginalFrameSize(Mat& frame_process_0)
{
    fr_w0 = frame_process_0.cols;
    fr_h0 = frame_process_0.rows;
    fr_w0_1 = 1.f/fr_w0;
    fr_h0_1 = 1.f/fr_h0;
    trac_str.fr_w0 = fr_w0;
    trac_str.fr_h0 = fr_h0;
 #if defined(TKDNN)
    trac_str.roi_w = cfg_w;
    trac_str.roi_h = cfg_h;
 #endif // END #if defined(TKDNN)
    trac_str.img_orig_type = frame_process_0.type();
} // -- END setOriginalFrameSize

TracShats::~TracShats(){cout << "Destructor TrackShats" << endl;}


bool TracShats::update(Mat& img, Rect2f& aimRectShats)
{
    if(first_frame)
    {
        fr_h_show = round(fr_w0_1 * fr_h0 * fr_w_show);
        sz_show = Size(fr_w_show, fr_h_show);
        show_orig = fr_w0_1 * fr_w_show;
        trac_str.zahvat = 1;
        trac_str.rect_ok = 2;
        trac_str.key = 0;
        wh_local_orig = Point2f(round(ext_wh * trac_str.obj_wh_2_w * fr_w0), round(ext_wh * trac_str.obj_wh_2_h * fr_h0));
        first_frame = false;        
    } // -- END if(first_frame)
    img_orig = img;
    bool ok = process();
    if(ok)
    {
        if(trac_str.zahvat && trac_str.ok_match)
        {
 #if !defined(TKDNN)
            aimRectShats = Rect2f(Point2f(trac_str.obj_xy_x - trac_str.obj_wh_2_w, trac_str.obj_xy_y - trac_str.obj_wh_2_h),
                     Point2f(trac_str.obj_xy_x + trac_str.obj_wh_2_w, trac_str.obj_xy_y + trac_str.obj_wh_2_h));
 #endif // END #if !defined(TKDNN)
            return 1;
        } // -- END if(trac_str.zahvat and trac_str.ok_match)
    } // -- END if(ok)
    return 0;
} // -- END update

void TracShats::getSearchRect(Rect2f& searchRect)
{
    searchRect = Rect2f(fr_w0_1 * trac_str.roi_l, fr_h0_1 * trac_str.roi_t,
               fr_w0_1 * trac_str.roi_w, fr_h0_1 * trac_str.roi_h);
} // -- END getSearchRect

Point2f TracShats::getTargetCenter(){return Point2f(trac_str.obj_xy_x, trac_str.obj_xy_y);}

bool TracShats::FileIsExist(const string& filePath)
{
    bool isExist = false;
    ifstream fin(filePath.c_str());
    if(fin.is_open()){isExist = true;}
    fin.close();
    return isExist;
} // -- END FileIsExist

bool TracShats::get_ini_params(const string& config)
{
    cout << "BEGIN get_ini_params TracShats" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    if(!FileIsExist(config))
    {
        cout << "File '" << config << "' not exist!" << endl;
        return 0;
    } // -- END if(!FileIsExist(config))
    INIReader reader(config);
    if(reader.ParseError()<0)
    {
        cout << "Can't load '" << config << "'\n";
        return 0;
    } // -- END if(reader.ParseError()<0)

    scaling = reader.GetInteger("tracking", "scaling", -1);
    if(scaling == -1){cout << "scaling not declared!\n"; return 0;}
    else{cout << "scaling = " << scaling << ";\n";}

    string yolo_block0 = reader.Get("switcher", "yolo_block0", "oops");
    if(yolo_block0 == "oops"){cout << "yolo_block0 not declared!\n"; return 0;}

    cfg_w = reader.GetInteger(yolo_block0, "cfg_w", -1);
    if(cfg_w == -1){cout << "cfg_w not declared!\n"; return 0;}

    cfg_h = reader.GetInteger(yolo_block0, "cfg_h", -1);
    if(cfg_h == -1){cout << "cfg_h not declared!\n"; return 0;}
    cout <<"END get_ini_params TracShats" << endl;
    return 1;
} // -- END get_ini_params

bool TracShats::dirContent(const string& fold, vector<string>& img_names, int typ)
{
    img_names.clear();
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(fold.c_str())) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            string fname = string(ent -> d_name);
            // -- typ = 4 (folder), typ = 8 (file).
            if(ent -> d_type == typ && fname != "." && fname != "..")
            {
                img_names.emplace_back(fname);
            }
        } // -- END while((ent = readdir (dir)) != NULL)
        closedir(dir);
        return 1;
    } // -- END if((dir = opendir(way)) != NULL)
    return 0;
} // -- END dirContent

bool TracShats::init(const Rect2f& rct_in)
{
    Point2f xy = 0.5*(rct_in.br() + rct_in.tl());
    trac_str.obj_xy_x = xy.x;
    trac_str.obj_xy_y = xy.y;
    Point2f wh_2 = 0.5*(rct_in.br() - rct_in.tl());
    trac_str.obj_wh_2_w = wh_2.x;
    trac_str.obj_wh_2_h = wh_2.y;
    trac_str.zahvat = 1;
    trac_str.key = 0;
    trac_str.rect_ok = 2;
    wh_local_orig = Point2f(round(ext_wh * trac_str.obj_wh_2_w * fr_w0), round(ext_wh * trac_str.obj_wh_2_h * fr_h0));
    return 1;
} // -- END init

void TracShats::deinit(){trac_str.key = 27;}

bool TracShats::isInited(){return trac_str.zahvat;}

void TracShats::setTrackerId(int id){trac_str.id = id;}

void TracShats::setWorkNumber(int cnt){trac_str.work_number = cnt;}

bool TracShats::img_orig_prepare()
{
    if(scaling > 1)
    {
        wh_local_orig = Point2f(round(ext_wh * trac_str.obj_wh_2_w * fr_w0), round(ext_wh * trac_str.obj_wh_2_h * fr_h0));
    } // -- END if(scaling > 1)
    Point2f center_orig = Point2f(trac_str.obj_xy_x * fr_w0, trac_str.obj_xy_y * fr_h0);
    Point2f lt = center_orig - 0.5 * wh_local_orig;
    if(lt.x < 0){lt.x = 0;}
    if(lt.y < 0){lt.y = 0;}
    Point2f br = lt + wh_local_orig;
    if(br.x >= fr_w0){br.x = fr_w0 - 1;}
    if(br.y >= fr_h0){br.y = fr_h0 - 1;}
    Point2f br_wh1 = br - wh_local_orig;
    if(br_wh1.x < lt.x){lt.x = br_wh1.x;}
    if(br_wh1.y < lt.y){lt.y = br_wh1.y;}
    if(lt.x < 0 || lt.y < 0){return 0;}
    rct_local_orig = Rect2f(lt, br);
    trac_str.roi_l = round(rct_local_orig.x);
    trac_str.roi_t = round(rct_local_orig.y);
#if !defined(TKDNN)
    trac_str.roi_w = round(rct_local_orig.width);
    trac_str.roi_h = round(rct_local_orig.height);
#endif // END #if !defined(TKDNN)
    trac_str.img_orig_data = img_orig.data;
    return 1;
} // -- END img_orig_prepare

bool TracShats::process()
{    
    trac_str.work_number++;
    if(!trac_str.zahvat){return 0;}
    bool ok = img_orig_prepare();
    if(!ok)
    {
        trac_str.zahvat = 0;
        cout << "Don't img_orig_prepare!\n";
        return 0;
    } // -- END if(!ok)
    find_ = get_trac(trac);
    if(find_ == -1)
    {
        cout << "Escape" << endl;
        return 0;
    } // -- END if(find_ == -1)
    return 1;
} // -- END process

int TracShats::work()
{   // ======================== Эта функция только для нейросетевой библиотеки.
    return get_trac(trac);
} // -- END work
