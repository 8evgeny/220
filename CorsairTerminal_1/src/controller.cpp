#include <QDateTime>
#include <QThread>
#include <QTimer>
#include "controller.h"

#include "device_parts/cmd/cmdmanager.h"
#include "device_parts/gsp/gspcontroller.h"
#include "eth/ethcontroller.h"
#include "etld/etld.h"

#include "device_parts/channel/tvrgbchannel.h"
#include "device_parts/channel/swirchannel.h"
#include "device_parts/channel/mwirchannel.h"
#include "device_parts/channel/tvgraymwirchannel.h"
#include "device_parts/channel/tvgraychannel.h"
#include "device_parts/channel/fusionchannel.h"
#include "device_parts/channel/testchannel.h"
#include "device_parts/channel/cpuchannel.h"
#include "device_parts/vlx/vlx_stream_channel.h"
#include "device_parts/vlx/vlx_telemetry_channel.h"

#include "tools/CorsairPainter.hpp"
#include "tools/imgp.h"
#include "tools/drawp.h"

#include "device_parts/cmd/vdb_registers.h"

#include "tools/logger.h"

// #define TEST_FRAME
#ifdef TEST_FRAME
    #include "tools/framegenerator.h"
#endif

Q_LOGGING_CATEGORY(logController, "Controller")

#define MAX_ZOOM_SPEED  (0xFF)
#define MAX_FOCUS_SPEED (0x4F)
#define MAX_IRIS_SPEED  (0xFF)

Controller::Controller(QObject *parent, DEV_TYPES dev_type)
    : QObject(parent)
    , _dev_type(dev_type)
    , _channel(nullptr)
    , _cmdManager(new CmdManager(this))
    , _gspController(new GSPController(this))
    , _ethController(&EthController::inst())
    , _host_etld_en(false)
    , _etld(new ETLD())
    , _target_en(false)
    , _target_size(30)
    , _target_id(0)
    , _target_fast(false)
    , _tracker_state(0x00u)
    , _tracker_channel(0xFFu)
    , m_rdy_img{ new QImage(CHANNEL_W, CHANNEL_H, QImage::Format_RGB888)}
    , _img_ready(false)
    , _painter{new CorsairPainter}
    // , m_painterThread{new QThread(this)}
{
    // init gsp data
    memset(&_gsp, 0x00, sizeof(t_gsp));
    _gsp.x_angle = 0.;
    _gsp.z_angle = -90.;
    memset(&_settings, 0x00, sizeof(t_settings));
    memset(&_rangefinder, 0x00, sizeof(t_rangefinder));

    for(int i = 0; i < 4; ++i)
    {
        _objects[i].valid = false;
        _objects[i].trusted = false;
        _objects[i].x = 0;
        _objects[i].y = 0;
        _objects[i].w = 0;
        _objects[i].h = 0;
    }

    _channels[CHANNEL_ID_TVRGB]      = new TVRGBChannel();
    _channels[CHANNEL_ID_SWIR]       = new SWIRChannel();
    _channels[CHANNEL_ID_MWIR]       = new MWIRChannel();
    _channels[CHANNEL_ID_TVGRAYMWIR] = new TVGrayMWIRChannel();
    _channels[CHANNEL_ID_TVGRAY]     = new TVGrayChannel();
    _channels[CHANNEL_ID_FUSION]     = new FusionChannel();
    _channels[CHANNEL_ID_TEST]       = new TestChannel();
    _channels[CHANNEL_ID_VLX_STREAM_1] = new VlxStreamChannel(5000, CHANNEL_NAME_VLX_STREAM_1, CHANNEL_SHOW_NAME_VLX_STREAM_1, CHANNEL_ID_VLX_STREAM_1);
    _channels[CHANNEL_ID_VLX_STREAM_2] = new VlxStreamChannel(5008, CHANNEL_NAME_VLX_STREAM_2, CHANNEL_SHOW_NAME_VLX_STREAM_2, CHANNEL_ID_VLX_STREAM_2);
    _channels[CHANNEL_ID_CPU]        = new CPUChannel();

    // триггерит создание канала телеметрии на основном потоке
    auto &tmChan = VlxTelemetryChannel::i();
    Q_UNUSED(tmChan);

    _etld->load_settings("etld_config.ini");

    drawp::init_drawp();

    // m_painter->moveToThread(m_painterThread);
    // connect(this, &Controller::sg_paint, m_painter, &CorsairPainter::sl_paint, /*Q*/t::ConnectionType(/*Qt::QueuedConnection |*/ Qt::UniqueConnection));
    connect(_painter, &CorsairPainter::sg_ready, this, &Controller::sl_updateFrame, Qt::ConnectionType(/*Qt::QueuedConnection |*/ Qt::UniqueConnection));
    // m_painterThread->start();
}
Controller::~Controller()
{
    _painter->setEnable(false);
    _painter->disconnect();

    stop();
    // host_etld_mutex.lock();
    // host_etld_mutex.unlock();
    delete _etld;
    // img_mutex.lock();
    // img_mutex.unlock();

    for(auto it = _channels.begin(); it != _channels.end(); ++it)
    {
        delete it->second;
    }
    _channels.clear();
    // m_painterThread->quit();
    // m_painterThread->wait();
    _painter->deleteLater();
    // m_painterThread->deleteLater();
}

QImage *Controller::image(QSize &frm_size, QTransform &disp_to_frm_tr)
{
    if(_img_ready) {
        std::lock_guard lock(_img_mutex);
        _img_ready = false;
        disp_to_frm_tr = m_disp_to_frm_tr;
        frm_size = m_frm_size;
        return m_rdy_img;
    }
    return nullptr;
}

AbstractChannel *Controller::channel()
{
    return _channel;
}

const AbstractChannel *Controller::channel() const
{
    return _channel;
}

AbstractChannel *Controller::channel(uint32_t id)
{
    if ((id == CHANNEL_ID_NONE) || (_channel != nullptr && _channel->id() == id))
        return _channel;
    if (auto i = _channels.find(id); i != _channels.end())
        return i->second;
    return nullptr;
}

const AbstractChannel *Controller::channel(uint32_t id) const
{
    if ((id == CHANNEL_ID_NONE) || (_channel != nullptr && _channel->id() == id))
        return _channel;
    if (auto i = _channels.find(id); i != _channels.end())
        return i->second;
    return nullptr;
}

void Controller::openMirror(const QString src, const QString dst)
{

}

double Controller::gsp_get_z_delta_shift()
{
    return  _gspController->get_z_shift_angle();
}

int8_t Controller::gsp_get_z_inv()
{
    if(_channel) {
        return _channel->x_inv();
    } else {
        return 1;
    }
}

CmdManager *Controller::cmdManger()
{
    return _cmdManager;
}

void Controller::start()
{
    _execute.store(true, std::memory_order_release);
    _thd = std::thread(&Controller::run, this);
}
void Controller::stop()
{
    if(_execute.load(std::memory_order_acquire))
    {
        _execute.store(false, std::memory_order_release);
        if(_thd.joinable())
        {
            _thd.join();
        }
    }
}
void Controller::vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length)
{
    _cmdManager->vdb_read_request(dst_board, dst_reg, length);
}
void Controller::vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word)
{
    _cmdManager->vdb_write_request(dst_board, dst_reg, word);
}
void Controller::vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data)
{
    _cmdManager->vdb_write_request_array(dst_board, dst_reg, data);
}
void Controller::send_msg(Packet& req)
{
    _cmdManager->request(req);
}
void Controller::new_object(bool is_single, const int & _x, const int & _y, const int & _w, const int & _h)
{
    if(_channel)
    {
        if(_target_en)
        {
            int x = _x;
            int y = _y;
            int w = _w;
            int h = _h;
            // _channel->recount_object(x, y, w, h, _currentFrameSize.width, _currentFrameSize.height);
            qInfo() << _channel->name() << "--> tracking: new object [" << x << y << w << h << "]";
            if(x >= 0 && y >= 0) {
                if(_host_etld_en)
                {
                    _host_etld_mutex.lock();
                    _etld->init(int(x), int(y), int(w), int(h));
                    _gspController->start_host_etld_object_error();
                    _host_etld_mutex.unlock();
                }
                else
                {
                    if(is_single) {
                        _target_id = 0;
                        _cmdManager->capture_target_single(_channel->id(), x + w / 2, y + h / 2, w, h);
                    } else {
                        _cmdManager->capture_target_multi(_channel->id(), _target_id, _target_fast, x + w / 2, y + h / 2, w, h);
                    }
                }
            }
        }
    }
}
void Controller::stop_object(bool is_single)
{
    qInfo(logController(), "stop_object: Stop_Object");
    if(_host_etld_en)
    {
        _host_etld_mutex.lock();
        if(_etld->isOn())
        {
            _etld->deinit();
            int32_t x_speed, z_speed;
            _gspController->set_speed(0, 0, x_speed, z_speed);
            _cmdManager->set_speed(x_speed, z_speed);
        }
        _host_etld_mutex.unlock();
    }
    else
    {
        if(is_single) {
            _target_id = 0;
            _cmdManager->free_target_single();
        } else {
            _cmdManager->free_target_multi(_target_id);
        }
    }
}
void Controller::video_click(const int& x0, const int& y0)
{
    int x{x0};
    int y{y0};
    int w{10};
    int h{10};
    if(_channel != nullptr) {
        _channel->recount_object(x, y, w, h, _currentFrameSize.width, _currentFrameSize.height);
        qInfo(logController(), "video_click: New Point[ %u, %u]", x, y);
        if(x > 0 && y > 0) {

            uint16_t x_p = static_cast<uint16_t>((0xFFFF * x) / _channel->w());
            uint16_t y_p = static_cast<uint16_t>((0xFFFF * y) / _channel->h());

            if(_settings.flags.is_tracker_enable && _settings.flags.tracker_deviation){
                _cmdManager->tracker_deviation(x_p, y_p);
            }
            if(!_settings.flags.is_tracker_enable && _settings.flags.click_control) {
                _cmdManager->gsp_click_control(x_p, y_p);
            }
        }
    }
}
void Controller::host_etld_en_set(const bool & v)
{
    _host_etld_en = v;
    emit host_etld_state_changed(_host_etld_en);
    if(_host_etld_en)
    {
        for(int i = 0; i < 4; ++i)
        {
            _objects[i].valid = false;
            _objects[i].trusted = false;
            _objects[i].x = 0;
            _objects[i].y = 0;
            _objects[i].w = 0;
            _objects[i].h = 0;
        }
    }
}
void Controller::host_x_inv_set(const bool & v)
{
    for(auto it = _channels.begin(); it != _channels.end(); ++it)
    {
        it->second->set_x_inv(v);
    }
}
void Controller::host_y_inv_set(const bool & v)
{
    for(auto it = _channels.begin(); it != _channels.end(); ++it)
    {
        it->second->set_y_inv(v);
    }
}
void Controller::select_channel(const uint32_t & channel_id)
{
    if( _channel && (channel_id != _channel->id()) )
    {
        // NOTE: При переключении канала трекинг в железе по старому каналу продолжается
        if(_host_etld_en)
        {
            stop_object(false);
        }
    }

    if(_channels.find(channel_id) == _channels.end())
    {
        qWarning(logController(), "select_channel: Channel Unknown[ %i ]", channel_id);
    }
    else
    {
        if(m_rdy_img) {
            _img_mutex.lock();
            *m_rdy_img = QImage(CHANNEL_W, CHANNEL_H, QImage::Format_RGB888);
            m_rdy_img->fill(QColor(0, 0, 0));
            m_disp_to_frm_tr.reset();
            _img_ready = true;
            _img_mutex.unlock();
            emit image_ready();
        }

        int32_t previousIdChannel = CHANNEL_ID_NONE;
        if(_channel != nullptr) {
            previousIdChannel = _channel->id(); // костыль
        }
        auto ch = _channels[channel_id];
        if(ch)
        {
            if (_channel != nullptr) {
                _channel->deselected();
            }

            _channel = ch;

            qInfo(logController(), "select_channel: Select channel %s[ %i ]", _channel->name().toStdString().c_str(), _channel->id());
            //********** костыль ***************
            if( (_mwirSensorStatus != 0) &&
                (previousIdChannel == CHANNEL_ID_MWIR) &&
                (_channel->id() != CHANNEL_ID_MWIR) )
            {
                _painter->mwirWaitingEn(true);

                QTimer::singleShot(4000, this, SLOT(sl_mwirWaitingStatusOff()));
            }
            //**********************************
            _channel->selected();
            _cmdManager->select_channel(_channel->id());

            emit channelChanged(channelId());
        }
#ifdef TEST_FRAME
        FrameGenerator::inst().set_frame_size(channel->w(), channel->h());
#endif
    }
}
void Controller::agc_switch(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        ch->set_agc_on(!ch->agc_on());
        qInfo(logController(), "agc: Set %s AGC [ %s ]", ch->name().toStdString().c_str(), ch->agc_on() ? "On" : "Off");
        _cmdManager->agc_switch(ch->id(), ch->agc_on());
    }
}
void Controller::agc_on(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        ch->set_agc_on(true);
        qInfo(logController(), "agc: Set %s AGC [ %s ]", ch->name().toStdString().c_str(), ch->agc_on() ? "On" : "Off");
        _cmdManager->agc_switch(ch->id(), ch->agc_on());
    }
}
void Controller::agc_off(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        ch->set_agc_on(false);
        qInfo(logController(), "agc: Set %s AGC [ %s ]", ch->name().toStdString().c_str(), ch->agc_on() ? "On" : "Off");
        _cmdManager->agc_switch(ch->id(), ch->agc_on());
    }
}
void Controller::field_sync_narrow(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "field_narrow: %s Narrow Field Synchronisation", ch->name().toStdString().c_str());
        _cmdManager->field_sync_narrow(ch->id());
    }
}
void Controller::field_sync(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "field_sync: %s Field Synchronisation", ch->name().toStdString().c_str());
        _cmdManager->field_sync(ch->id());
    }
}
void Controller::field_sync_reg_switch(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        bool _field_sync_cont_en = !ch->reg_field_sync();
        qInfo(logController(), "field_sync_reg: %s Continous Field Synchronisation %s", ch->name().toStdString().c_str(), _field_sync_cont_en ? "On" : "Off");
        _cmdManager->field_sync_reg(ch->id(), _field_sync_cont_en);
    }
}
void Controller::field_sync_reg_on(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "field_sync_reg_on: %s Continous Field Synchronisation On", ch->name().toStdString().c_str());
        _cmdManager->field_sync_reg(ch->id(), true);
    }
}
void Controller::field_sync_reg_off(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "field_sync_reg_off: %s Continous Field Synchronisation Off", ch->name().toStdString().c_str());
        _cmdManager->field_sync_reg(ch->id(), false);
    }
}
void Controller::autofocus(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "autofocus: %s Autofocus", ch->name().toStdString().c_str());
        _cmdManager->autofocus(ch->id());
    }
}
void Controller::autofocus_en(const uint32_t & channel_id)
{
    if(channel_id == CHANNEL_ID_NONE)
    {
        qInfo(logController(), "autofocus: Autofocus all channel Enable");
        _cmdManager->autofocus_en(CHANNEL_ID_TVRGB);
        _cmdManager->autofocus_en(CHANNEL_ID_SWIR);
        _cmdManager->autofocus_en(CHANNEL_ID_MWIR);
    }
    else
    {
        AbstractChannel * channel = _channels[channel_id];
        if(channel)
        {
            qInfo(logController(), "autofocus: %s Autofocus Enable", channel->name().toStdString().c_str());
            _cmdManager->autofocus_en(channel->id());
        }
    }
}
void Controller::autofocus_dis(const uint32_t & channel_id)
{
    if(channel_id == CHANNEL_ID_NONE)
    {
        qInfo(logController(), "autofocus: Autofocus all channel Disable");
        _cmdManager->autofocus_dis(CHANNEL_ID_TVRGB);
        _cmdManager->autofocus_dis(CHANNEL_ID_SWIR);
        _cmdManager->autofocus_dis(CHANNEL_ID_MWIR);
    }
    else
    {
        AbstractChannel * channel = _channels[channel_id];
        if(channel)
        {
            qInfo(logController(), "autofocus: %s Autofocus Disable", channel->name().toStdString().c_str());
            _cmdManager->autofocus_dis(channel->id());
        }
    }
}
void Controller::swir_gain_switch()
{
    if(_channel && (_channel->id() == CHANNEL_ID_SWIR))
    {
        _channel->set_swir_gain((_channel->swir_gain() == 2) ? 0 : (_channel->swir_gain() + 1));
        switch(_channel->swir_gain())
        {
        case 0:
            qInfo(logController(), "swir_mode: Set SWIR auto gain");
            _cmdManager->swir_auto_gain();
            break;
        case 1:
            qInfo(logController(), "swir_mode: Set SWIR day gain");
            _cmdManager->swir_day_gain();
            break;
        case 2:
            qInfo(logController(), "swir_mode: Set SWIR night gain");
            _cmdManager->swir_night_gain();
            break;
        }
    }
}
void Controller::swir_gain_auto()
{
    _channels[CHANNEL_ID_SWIR]->set_swir_gain(0);
    qInfo(logController(), "swir_mode: Set SWIR auto gain");
    _cmdManager->swir_auto_gain();
}
void Controller::swir_gain_day()
{
    _channels[CHANNEL_ID_SWIR]->set_swir_gain(1);
    qInfo(logController(), "swir_mode: Set SWIR day gain");
    _cmdManager->swir_day_gain();
}
void Controller::swir_gain_night()
{
    _channels[CHANNEL_ID_SWIR]->set_swir_gain(2);
    qInfo(logController(), "swir_mode: Set SWIR night gain");
    _cmdManager->swir_night_gain();
}

void Controller::zoom_in(const double & speed, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "zoom_in: %s Zoom In %.3f", channel->name().toStdString().c_str(), speed);
        emit set_autofocus_host_pause(false);
        _cmdManager->zoom_in(ch->id(), uint8_t(round(speed * MAX_ZOOM_SPEED)));
    }
}
void Controller::zoom_out(const double & speed, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "zoom_out: %s Zoom Out %.3f", channel->name().toStdString().c_str(), speed);
        emit set_autofocus_host_pause(false);
        _cmdManager->zoom_out(ch->id(), uint8_t(round(speed * MAX_ZOOM_SPEED)));
    }
}
void Controller::zoom_stop(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
       // qInfo(logController(), "zoom_stop: %s Zoom Stop", channel->name().toStdString().c_str());
        _cmdManager->zoom_stop(ch->id());
    }
}
void Controller::zoom_pos(const uint32_t & pos, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "zoom_pos: %s Zoom %u", channel->name().toStdString().c_str(), pos);
        emit set_autofocus_host_pause(false);
        _cmdManager->zoom_pos(ch->id(), pos);
    }
}

void Controller::zoom_pos_fix()
{
    if(auto ch = channel())
    {
        uint32_t pos{0};
        if((ch->id() == CHANNEL_ID_TVRGB) || (ch->id() == CHANNEL_ID_TVGRAY)) {
            pos = 1675;
        } else if(ch->id() == CHANNEL_ID_MWIR) {
            pos = 8700;
        }
        emit set_autofocus_host_pause(false);
        _cmdManager->zoom_pos(ch->id(), pos);
    }
}
void Controller::zoom_pos_fix_width()
{
    if(auto ch = channel())
    {
        uint32_t pos{0};
        if((ch->id() == CHANNEL_ID_TVRGB) || (ch->id() == CHANNEL_ID_TVGRAY)) {
            pos = 3700;
        } else if(ch->id() == CHANNEL_ID_MWIR) {
            pos = 40;
        }
        emit set_autofocus_host_pause(false);
        _cmdManager->zoom_pos(ch->id(), pos);
    }
}
void Controller::focus_near(const double & speed, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "focus_near: %s Focus Near %.3f", channel->name().toStdString().c_str(), speed);
        emit set_autofocus_host_pause(true);
        _cmdManager->focus_near(ch->id(), uint8_t(round(speed * MAX_FOCUS_SPEED)));
    }
}
void Controller::focus_far(const double & speed, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "focus_far: %s Focus Far %.3f", channel->name().toStdString().c_str(), speed);
        emit set_autofocus_host_pause(true);
        _cmdManager->focus_far(ch->id(), uint8_t(round(speed * MAX_FOCUS_SPEED)));
    }
}
void Controller::focus_stop(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "focus_stop: %s Focus Stop", channel->name().toStdString().c_str());
        _cmdManager->focus_stop(ch->id());
    }
}
void Controller::focus_pos(const uint32_t & pos, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "focus_pos: %s Focus %u", channel->name().toStdString().c_str(), pos);
        _cmdManager->focus_pos(ch->id(), pos);
    }
}
void Controller::iris_close(const double & speed, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "iris_close: %s Iris Close %.3f", channel->name().toStdString().c_str(), speed);
        _cmdManager->iris_close(ch->id(), uint8_t(round(speed * MAX_IRIS_SPEED)));
    }
}
void Controller::iris_open(const double & speed, const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "iris_open: %s Iris Open %.3f", channel->name().toStdString().c_str(), speed);
        _cmdManager->iris_open(ch->id(), uint8_t(round(speed * MAX_IRIS_SPEED)));
    }
}
void Controller::iris_stop(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
//        qInfo(logController(), "iris_stop: %s Iris Stop", channel->name().toStdString().c_str());
        _cmdManager->iris_stop(ch->id());
    }
}

void Controller::thermostat_on(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "thermostat_on: %s Thermostat On", ch->name().toStdString().c_str());
        _cmdManager->thermostat_on(ch->id());
    }
}
void Controller::thermostat_off(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "thermostat_off: %s Thermostat Off", ch->name().toStdString().c_str());
        _cmdManager->thermostat_off(ch->id());
    }
}
void Controller::heat_on(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "heat_on: %s Heat On", ch->name().toStdString().c_str());
        _cmdManager->heat_on(ch->id());
    }
}
void Controller::heat_off(const uint32_t & channel_id)
{
    if(auto ch = channel(channel_id))
    {
        qInfo(logController(), "heat_off: %s Heat off", ch->name().toStdString().c_str());
        _cmdManager->heat_off(ch->id());
    }
}
void Controller::gsp_speed_set(const double x_speed_in, const double z_speed_in)
{
    if(_channel && _GSPControlEnable)
    {
        if(_etld->isOn()) return; // При включенном трекинге ручное управление запрещено
        //NOTE: +20 чтобы в узком поле скорость не была равна 0

        // double k = double(_channel->max_focus_mm() - _channel->focus_mm() + 50) / double(_channel->max_focus_mm() - _channel->min_focus_mm());

        double div = _channel->max_focus_mm() - _channel->min_focus_mm();
        double k = 0.05;
        if(div > 0 ) {
            k = fabs((double(_channel->max_focus_mm() - _channel->focus_mm() - _channel->min_focus_mm() + 20) / div));
        }
        static const double slower = 0.4; // NOTE дополнительный костыль-коэффициент для замедления
        // k = pow(k, 3);
        double recounted_norm_x_speed = x_speed_in * k * slower;
        double recounted_norm_z_speed = z_speed_in * k * slower;
        // if(fabs(x_speed_in) > 0.9999) {
        //     recounted_norm_x_speed = x_speed_in;
        // }
        // if(fabs(z_speed_in) > 0.9999) {
        //     recounted_norm_z_speed = z_speed_in;
        // }
        if(_channel->y_inv()) recounted_norm_x_speed *= -1.0;
        if(_channel->x_inv()) recounted_norm_z_speed *= -1.0;

        qInfo(logController(), "platform_set_speed: Set Platform Speed { X[ %.3f ]Z[ %.3f ] }", recounted_norm_x_speed, recounted_norm_z_speed);
        int32_t x_speed, z_speed;
        _gspController->set_speed(recounted_norm_x_speed, recounted_norm_z_speed, x_speed, z_speed);
        _cmdManager->set_speed(x_speed, z_speed);
    }
//    qDebug() << "CONTROLLER: SPEED = " << x_speed << "  ,  " << z_speed;
}

void Controller::gsp_speed_tracker_set(const double x_speed, const double z_speed)
{
    if(_channel && _GSPControlEnable)
    {
        if(_etld->isOn()) return; // При включенном трекинге ручное управление запрещено
        _cmdManager->set_speed(x_speed, z_speed);
    }
}

void Controller::gsp_speed_set_deg(const double & x_speed, const double & z_speed)
{
    if(_etld->isOn()) return; // При включенном трекинге ручное управление запрещено

    if(_channel && _GSPControlEnable)
    {
        _cmdManager->set_speed_deg_per_sec(x_speed, z_speed);
    }
}
void Controller::gsp_slow_speed_set(const double & x_speed, const double & z_speed)
{
    if(_channel && _GSPControlEnable)
    {
        if(_etld->isOn()) return; // При включенном трекинге ручное управление запрещено
        double recounted_norm_x_speed = x_speed;
        double recounted_norm_z_speed = z_speed;
        if(_channel->y_inv()) recounted_norm_x_speed *= -1.0;
        if(_channel->x_inv()) recounted_norm_z_speed *= -1.0;
        int32_t x_speed, z_speed;
        _gspController->set_slow_speed(recounted_norm_x_speed, recounted_norm_z_speed, x_speed, z_speed);
        _cmdManager->set_speed(x_speed, z_speed);
    }
}
void Controller::gsp_absolute_angle_set(const double & x_abs_angle, const double & z_abs_angle)
{
    if(_etld->isOn()) return; // При включенном трекинге ручное управление запрещено
    if(!_GSPControlEnable) return;
    _cmdManager->set_absolute_angle((float)x_abs_angle, (float)z_abs_angle);
    //qInfo(logController(), "platform_set_absolute_angle: Set Platform Absolute Angle { X[ %.3f ]Z[ %.3f ] }", abs_x_angle, abs_z_angle);
}
void Controller::gsp_relative_angle_set(const double & x_rel_angle, const double & z_rel_angle)
{
    if(_etld->isOn()) return; // При включенном трекинге ручное управление запрещено
    if(!_GSPControlEnable) return;
//    qInfo(logController(), "platform_set_relative_angle: Set Platform Relative Angle { X[ %.3f ]Z[ %.3f ] }", rel_x_angle, rel_z_angle);
    int16_t rel_x_angle, rel_z_speed;
    _gspController->set_relative_angle(x_rel_angle, z_rel_angle, rel_x_angle, rel_z_speed);
    _cmdManager->set_relative_angle(rel_x_angle, rel_z_speed);
}
void Controller::gsp_arretir_set()
{
    qInfo(logController(), "gsp_arretir_set: Set Mechanical Arretir");
    _cmdManager->arretir_set();
}
void Controller::gsp_arretir_reset()
{
    qInfo(logController(), "gsp_arretir_reset: Reset Mechanical Arretir, Set Electrical Arretir, Set to null position");
    _cmdManager->arretir_reset();
}
void Controller::gsp_el_arr_coarse()
{
    _cmdManager->vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_el_arr, (2 << 8) | 1);
}
void Controller::gsp_el_arr_fine()
{
    _cmdManager->vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_el_arr, (1 << 8) | 1);
}
void Controller::gsp_stabilization_switch()
{
//    if(!m_GSPControlEnable) return;
    if(_gsp.flags.stabilization)
    {
        qInfo(logController(), "gsp_stabilization_switch: Stabilisation Off");
        _cmdManager->stabilisation_off();
    }
    else
    {
        qInfo(logController(), "gsp_stabilization_switch: Stabilisation On");
        _cmdManager->stabilisation_on();
    }
}
void Controller::gsp_stabilization_on()
{
//    if(!m_GSPControlEnable) return;
    qInfo(logController(), "gsp_stabilization_on: Stabilisation On");
    _cmdManager->stabilisation_on();
}
void Controller::gsp_stabilization_off()
{
//    if(!m_GSPControlEnable) return;
    qInfo(logController(), "gsp_stabilization_off: Stabilisation Off");
    _cmdManager->stabilisation_off();
}
void Controller::gsp_park()
{
    if(!_GSPControlEnable) return;
    qInfo(logController(), "gsp_park: Park");
    _cmdManager->park();
}
void Controller::gsp_null_position()
{
//    if(!m_GSPControlEnable) return;
    qInfo(logController(), "gsp_null_position: Set to null position");
    _cmdManager->null_position();
}
void Controller::gsp_pohod_position()
{
    if(!_GSPControlEnable) return;
    qInfo(logController(), "gsp_pohod_position: Set to pohod position");
    _cmdManager->pohod_position();
}
void Controller::gsp_polyot_position()
{
    if(!_GSPControlEnable) return;
    qInfo(logController(), "gsp_polyot_position: Set to polyot position");
    _cmdManager->polyot_position();
}
void Controller::gsp_alignment()
{
    if(!_GSPControlEnable) return;
    qInfo(logController(), "gsp_alignment: Set alignment");
    _cmdManager->alignment();
}
void Controller::gsp_angle_ctrl_mode()
{
    if(!_GSPControlEnable) return;
    qInfo(logController(), "gsp_angle_ctrl_mode: Set angle control mode");
    _cmdManager->set_platform_control_by_angle();
}
void Controller::gsp_velocity_ctrl_mode()
{
    if(!_GSPControlEnable) return;
    qInfo(logController(), "gsp_velocity_ctrl_mode: Set velocity control mode");
    _cmdManager->set_platform_control_by_velocity();
}

void Controller::lrf_enable_switch()
{
//    rangefinder_en = !rangefinder_en;
//    qInfo(logController(), "rangefinder_enable: Set Rangefinder Enable [ %s ]", rangefinder_en ? "On" : "Off");
//    if(!rangefinder_en)
//    {
//        rangefinder_on = false;
//        qInfo(logController(), "rangefinder_enable: Stop Rangefinder");
////        cmdManager->rangefinder_radiation(en_lrf_rc_stop_radiation);
//    }
//    cmdManager->_rangefinder.flags.rangefinder_power_on(rangefinder_en);

    qInfo(logController(), "highlighter_power_on: Set Highlighter Power [ %s ]", _rangefinder.flags.rangefinder_power_on ? "On" : "Off");
    if (_rangefinder.flags.rangefinder_power_on)
    {
        qInfo(logController(), "highlighter_power_on: Stop Highlighter");
    }
    _cmdManager->rangefinder_power_on(!_rangefinder.flags.rangefinder_power_on);
}
void Controller::lrf_mode_switch()
{
//    if(rangefinder_en)
//    {
//        if(rangefinder_sequence && rangefinder_on)
//        {
//            rangefinder_on = false;
//            qInfo(logController(), "rangefinder_mode: Stop Rangefinder sequence");
//            cmdManager->rangefinder_radiation(en_lrf_rc_stop_radiation);
//        }
//        rangefinder_sequence = !rangefinder_sequence;
//        qInfo(logController(), "rangefinder_mode: Set Rangefinder Mode [ %s ]", rangefinder_sequence ? "5Hz" : "Mono");
//    }

    if (_rangefinder.flags.rangefinder_mode == en_lrf_rc_single_measure)
    {
        _rangefinder.flags.rangefinder_mode = en_lrf_rc_illumination;
        qInfo(logController(), "highlighter_mode: Set Highlighter Mode [ continuous ]");
    }
    else
    {
        _rangefinder.flags.rangefinder_mode = en_lrf_rc_single_measure;
        qInfo(logController(), "highlighter_mode: Set Highlighter Mode [ single measurecontinuous ]");
    }
}
void Controller::lrf_start()
{
//    if(rangefinder_en)
//    {
//        if(rangefinder_sequence)
//        {
//            rangefinder_on = !rangefinder_on;
//            qInfo(logController(), "rangefinder_start: %s Rangefinder sequence", rangefinder_on ? "Start" : "Stop");
//            if(rangefinder_on)
//            {
//                cmdManager->rangefinder_radiation(en_lrf_rc_illumination);
//            }
//            else
//            {
//                cmdManager->rangefinder_radiation(en_lrf_rc_stop_radiation);
//            }
//        }
//        else
//        {
//            qInfo(logController(), "rangefinder_start: Start Rangefinder Mono");
//            cmdManager->rangefinder_radiation(en_lrf_rc_single_measure);
//        }
//    }

    if (_rangefinder.flags.rangefinder_rdy && _rangefinder.flags.rangefinder_power_on)
    {
//        if (_rangefinder.flags.rangefinder_cont_emit)
//        {
//            qInfo(logController(), "highlighter_start: Stop Highlighter continuous illumination");
//            cmdManager->rangefinder_radiation(en_lrf_rc_stop_radiation);
//        }
//        else
//        {
//            if (_rangefinder.flags.rangefinder_mode == en_lrf_rc_single_measure)
//            {
//                qInfo(logController(), "highlighter_start: Start Highlighter single measure");
//            }
//            else if (_rangefinder.flags.rangefinder_mode == en_lrf_rc_illumination)
//            {
//                qInfo(logController(), "highlighter_start: Start Highlighter continuous illumination");
//            }

//            cmdManager->rangefinder_radiation(_rangefinder.flags.rangefinder_mode);
//        }
        if(_lrf_DangerZoneOn) {
            if(!_lrf_IsDangerZone) {
                lrf_start_mono();
            }
        } else {
            lrf_start_mono();
        }
    }
}
void Controller::lrf_start_mono()
{
//    qInfo(logController(), "lrf_start_mono: Start Rangefinder Mono");
//    cmdManager->rangefinder_radiation(en_lrf_rc_single_measure);
    _rangefinder.flags.rangefinder_mode = en_lrf_rc_single_measure;
    qInfo(logController(), "highlighter_start: Start Highlighter single measure");
    _cmdManager->rangefinder_radiation(_rangefinder.flags.rangefinder_mode);
}
void Controller::lrf_start_5Hz()
{
//    qInfo(logController(), "lrf_start_sequence: Start Rangefinder sequence");
//    cmdManager->rangefinder_radiation(en_lrf_rc_illumination);
    _rangefinder.flags.rangefinder_mode = en_lrf_rc_illumination;
    qInfo(logController(), "highlighter_start: Start Highlighter continuous illumination");
    _cmdManager->rangefinder_radiation(_rangefinder.flags.rangefinder_mode);
}
void Controller::lrf_stop()
{
//    qInfo(logController(), "lrf_stop: Stop Rangefinder");
//    cmdManager->rangefinder_radiation(en_lrf_rc_stop_radiation);
    qInfo(logController(), "highlighter_start: Stop Highlighter continuous illumination");
    _cmdManager->rangefinder_radiation(en_lrf_rc_stop_radiation);
}

void Controller::lrf_toggle_continuos()
{
    if (_rangefinder.flags.rangefinder_cont_emit)
        lrf_stop();
    else
        lrf_start_5Hz();
}

void Controller::lrf_set_frequency(uint8_t frequency)
{
    QString str("highlighter_start: Set Highlighter frequency ");
    str.append(QString::number(frequency));
    qInfo(logController(),"%s", str.toStdString().c_str());
    _cmdManager->rangefinder_set_frequency(frequency);
}

void Controller::lrf_dangerZone_en(const bool en, const t_lrf_danger_zone_params params)
{
    _lrf_dangerZoneParams = params;
    _lrf_DangerZoneOn = en;
}

bool Controller::lrfCalcDangerZone()
{
    // >>>>>> Check for inversion >>>>>>
    double xPos = _channel->y_inv() ? -_gsp.x_angle : _gsp.x_angle;
    double zPos = _channel->x_inv() ? -(_gsp.z_angle + _gspController->get_z_shift_angle()) : (_gsp.z_angle + _gspController->get_z_shift_angle());
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // >>>>>> Recount angle from (-180..+180) to (0..360) >>>>>>>
    double gsp_z_angle_360 = zPos;
    if(zPos < 0.) {
        gsp_z_angle_360 += 360.;
        if (gsp_z_angle_360 > 360.) {
            gsp_z_angle_360 -= 360.;
        }
        //gsp_z_angle_360 = -((static_cast<int>(-gsp_z_angle_360) % 360) + (-(gsp_z_angle_360-static_cast<int>(gsp_z_angle_360))));
    } else if(gsp_z_angle_360 > 360.) {
        gsp_z_angle_360 = (static_cast<int>(gsp_z_angle_360) % 360) + (gsp_z_angle_360-static_cast<int>(gsp_z_angle_360));
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    double x = xPos;
    double z = gsp_z_angle_360;
    //------------------------------------------------
    if( x >= _lrf_dangerZoneParams.x_base ) {
        // Danger!
        return true;
    } else if ( x >= _lrf_dangerZoneParams.x_legs ) {
        // Limited zone

        /* OLD VARIANT
        bool zone1 = (z >= _lrf_dangerZoneParams.leg1_begin) && ( z <= _lrf_dangerZoneParams.leg1_end );
        bool zone2 = (z >= _lrf_dangerZoneParams.leg2_begin) && ( z <= _lrf_dangerZoneParams.leg2_end );
        bool zone3 = (z >= _lrf_dangerZoneParams.leg3_begin) && ( z <= _lrf_dangerZoneParams.leg3_end );
//        bool zone4 = (z >= _lrf_dangerZoneParams.leg4_begin) && ( z <= _lrf_dangerZoneParams.leg4_end );
        */

        double boardShift = _lrf_dangerZoneParams.zoneReserve + ( _lrf_dangerZoneParams.lrfDiameter / 2.0);

        /*bool zone1 = lrfCalcOneZone(z, lrf_dangerZoneParams.leg1_begin, lrf_dangerZoneParams.leg1_end, boardShift);
        bool zone2 = lrfCalcOneZone(z, lrf_dangerZoneParams.leg2_begin, lrf_dangerZoneParams.leg2_end, boardShift);
        bool zone3 = lrfCalcOneZone(z, lrf_dangerZoneParams.leg3_begin, lrf_dangerZoneParams.leg3_end, boardShift);*/

        bool zone1 = lrfCalcOneZone(z, 63, 93, boardShift);
        bool zone2 = lrfCalcOneZone(z, 150, 271, boardShift);
        bool zone3 = lrfCalcOneZone(z, 333, 5, boardShift);

        if( zone1 || zone2 || zone3 )
        {
            // Danger!
            return true;
        } else {
            // Free zone
            return false;
        }
    } else {
        // Free zone
        return false;
    }

}
bool Controller::lrfCalcOneZone(const double z, const double legBegin, const double legEnd, const double boardShift)
{
    bool zone_1;
    bool zone_2;
    bool result;
    if(legBegin < legEnd) {
        zone_1 = ( z <= legEnd );
        zone_2 = ( z >= legBegin );
        result = (zone_1 && zone_2);
    } else {
        zone_1 = ( z >= legBegin );
        zone_2 = ( z <= legEnd );
        result = (zone_1 || zone_2);
    }
    return result;
    /*bool zone_1 = ( z < (legEnd   + boardShift + lrf_dangerZoneParams.deltaTvLrf) );
    bool zone_2 = ( z > (legBegin - boardShift + lrf_dangerZoneParams.deltaTvLrf) );
//    bool result = (zone_1 && zone_2);
    bool result;
    if(legBegin < legEnd) {
        result = (zone_1 && zone_2);
    } else {
        result = (zone_1 || zone_2);
    }
    return result;*/
}

void Controller::sl_mwirWaitingStatusOff() {_painter->mwirWaitingEn(false);}

void Controller::sl_updateFrame()
{
    if(_img_mutex.try_lock()) {
        *m_rdy_img = _painter->outputQImage();
        m_disp_to_frm_tr = _painter->frameToDispTransform().inverted();
        m_frm_size = _painter->frameSize();
        _img_ready = true;
        _img_mutex.unlock();
        emit image_ready();
    }
}

void Controller::target_enable_switch_single()
{
    target_set_single_enabled(!_target_en);
}

void Controller::target_set_single_enabled(bool en)
{
    if (!_channel)
        return;
    if (_target_en == en)
        return;
    _target_en = en;
    qInfo(logController(), "target_enable: %s target capture", _target_en ? "Enable" : "Disable");
    if(!_target_en)
    {
        stop_object(true);
    }
    emit sg_trackerEn(_target_en);
}

void Controller::target_capture_single()
{
    if(_channel)
    {
        if(_target_en)
        {



            qInfo() << "CHANNEL:" << _channel->name();
            qInfo() << "ch.w" << _channel->w();
            qInfo() << "ch.h" << _channel->h();
            qInfo() << "target_size" << _target_size;
            qInfo() << "_currentFrameSize.width" << _currentFrameSize.width;
            qInfo() << "_currentFrameSize.haight" << _currentFrameSize.height;

            //NOTE: Приходится приводить координаты рамки к disp_img, чтобы потом обратно пересчитать в исходные координаты при помощи recount_object() в _new_object()
            // _channel->decount_object(x, y, w, h, _currentFrameSize.width, _currentFrameSize.height);
            // new_object(true, x, y, w, h);

        }
        else
        {
            qInfo(logController(), "target_capture: Enable target capture");
            _target_en = true;
            emit sg_trackerEn(_target_en);
        }
    }
}
void Controller::target_free_single()
{
    if(_channel)
    {
        stop_object(true);
    }
}
void Controller::target_enable_switch_multi()
{
    target_set_multi_enabled(!_target_en);
}

void Controller::target_set_multi_enabled(bool en)
{
    if (!_channel)
        return;
    if (_target_en == en)
        return;
    _target_en = en;
    qInfo(logController(), "target_enable: %s target capture", _target_en ? "Enable" : "Disable");
    if(!_target_en)
    {
        stop_object(false);
    }
    emit sg_trackerEn(_target_en);
}

void Controller::target_capture_multi()
{
    if(_channel)
    {
        if(_target_en)
        {

            QPoint tl = {_channel->w() / 2 - _target_size / 2, _channel->h() / 2 - _target_size / 2};
            QPoint br = {tl.x() + _target_size, tl.y() + _target_size};
            tl = m_disp_to_frm_tr.map(tl);
            br = m_disp_to_frm_tr.map(br);


            auto x0 = std::min(tl.x(), br.x());
            auto y0 = std::min(tl.y(), br.y());
            auto x1 = std::max(tl.x(), br.x());
            auto y1 = std::max(tl.y(), br.y());

            QRect r(x0, y0, (x1 - x0), (y1 - y0));

            qInfo() << "CHANNEL:" << _channel->name();
            qInfo() << "target_size" << _target_size;
            qInfo() << "new_object:" << r;

            new_object(false, r.x(), r.y(), r.width(), r.height());
        }
        else
        {
            qInfo(logController(), "target_capture: Enable target capture");
            _target_en = true;
            emit sg_trackerEn(_target_en);
        }
    }
}
void Controller::target_free_multi()
{
    if(_channel)
    {
        stop_object(false);
    }
}
void Controller::target_switch_multi()
{
    if(_channel)
    {
        if(_host_etld_en)
        {
            if(_etld->isOn()) target_free_multi();
            else target_capture_multi();
        }
        else
        {
            if(_tracker_state == 0x01) target_free_multi();
            else target_capture_multi();
        }
    }
}
void Controller::target_window_set(const double & size)
{
//    if(channel)
    {
        int delta = int(round(size * 20));
        _target_size += delta;
        if(_target_size < 10) _target_size = 10;
        else if(_target_size > _currentFrameSize.width / 2) _target_size = _currentFrameSize.height / 2;
        if (_target_size > 133) {
            _target_size = 133;
        }
//        qInfo(logController(), "target_window: Set Target Window [ %i x %i ]", target_size, target_size);
    }
}
void Controller::target_window_set(const int & size)
{
    if(_channel)
    {
        _target_size = size;
        if(_target_size < 10) _target_size = 10;
        else if(_target_size > _currentFrameSize.width / 2) _target_size = _currentFrameSize.height / 2;
//        qInfo(logController(), "target_window: Set Target Window [ %i x %i ]", target_size, target_size);
    }
}
void Controller::target_id_switch()
{
    if(_channel)
    {
        _target_id = (_target_id == 3) ? 0 : _target_id + 1;
        qInfo(logController(), "target_id_switch: Set target id [ %i ]", _target_id);
    }
}
void Controller::target_id_set(const int & id)
{
    if(_channel)
    {
        _target_id = (id < 0) ? 0 : (id > 3) ? 3 : id;
        qInfo(logController(), "target_id_set: Set target id [ %i ]", _target_id);
    }
}
void Controller::target_choose()
{
    if(_channel)
    {
        _cmdManager->choose_target_multi(_target_id);
        qInfo(logController(), "target_choose: Choose target id [ %i ]", _target_id);
    }
}
void Controller::target_fast_switch()
{
    if(_channel)
    {
        _target_fast = !_target_fast;
        qInfo(logController(), "target_fast_switch: Set target fast [ %s ]", _target_fast ? "true" : "false");
    }
}
void Controller::target_fast_set(const bool & v)
{
    if(_channel)
    {
        _target_fast = v;
        qInfo(logController(), "target_fast_set: Set target fast [ %s ]", _target_fast ? "true" : "false");
    }
}

void Controller::digital_zoom_in()
{
    if(_channel)
    {
        _channel->set_digital_zoom(_channel->digital_zoom() * 2);
        qInfo(logController(), "digital_zoom_in: Set digital zoom [ X%.2f ]", _channel->digital_zoom());
    }
}
void Controller::digital_zoom_out()
{
    if(_channel)
    {
        _channel->set_digital_zoom(_channel->digital_zoom() / 2);
        qInfo(logController(), "digital_zoom_out: Set digital zoom [ X%.2f ]", _channel->digital_zoom());
    }
}
void Controller::digital_zoom_switch()
{
    if(_channel)
    {
        if(_channel->digital_zoom() >= 8.0)
        {
            _channel->set_digital_zoom(1.0);
        }
        else
        {
            _channel->set_digital_zoom(_channel->digital_zoom() * 2);
        }
        qInfo(logController(), "digital_zoom_switch: Set digital zoom [ X%.2f ]", _channel->digital_zoom());
    }
}
void Controller::pic_in_pic()
{
    if(_channel)
    {
        _channel->set_pic_in_pic_on(!_channel->pic_in_pic_on());
        qInfo(logController(), "pic_in_pic: Set Picture in picture [ %s ]", _channel->pic_in_pic_on() ? "On" : "Off");
    }
}
void Controller::inversion_mw()
{
    if(_channel)
    {
        if(_channel->id() == CHANNEL_ID_MWIR) {
            _channel->set_inversion_on(!_channel->inversion_on());
            _cmdManager->mw_inversion(_channel->inversion_on());
            qInfo(logController(), "inversion: Set Inversion [ %s ]", _channel->inversion_on() ? "On" : "Off");
        }
    }
}
void Controller::brightness_up()
{
    if(_channel)
    {
        _channel->set_brightness(_channel->brightness() + 5);
        _cmdManager->brightness(_channel->id(), _channel->brightness());
        qInfo(logController(), "brightness_up: Set Brightness [ %i ]", _channel->brightness());
    }
}
void Controller::brightness_down()
{
    if(_channel)
    {
        _channel->set_brightness(_channel->brightness() - 5);
        _cmdManager->brightness(_channel->id(), _channel->brightness());
        qInfo(logController(), "brightness_down: Set Brightness [ %i ]", _channel->brightness());
    }
}
void Controller::contrast_up()
{
    if(_channel)
    {
        _channel->set_contrast(_channel->contrast() + 0.05);
        _cmdManager->contrast(_channel->id(), _channel->contrast());
        qInfo(logController(), "contrast_up: Set Contrast [ %.2f ]", _channel->contrast());
    }
}
void Controller::contrast_down()
{
    if(_channel)
    {
        _channel->set_contrast(_channel->contrast() - 0.05);
        _cmdManager->contrast(_channel->id(), _channel->contrast());
        qInfo(logController(), "contrast_down: Set Contrast [ %.2f ]", _channel->contrast());
    }
}
void Controller::bc_reset()
{
    if(_channel)
    {
        _channel->set_brightness(0.0);
        _channel->set_contrast(1.0);
        _cmdManager->brightness(_channel->id(), _channel->brightness());
        _cmdManager->contrast(_channel->id(), _channel->contrast());
        qInfo(logController(), "bc_reset: Set Brightness [ 0.0 ] Set Contrast [ 1.0 ]");
    }
}

void Controller::tvgraymwir_enable_changed(bool v)
{
    TVGrayMWIRChannel * ch = reinterpret_cast<TVGrayMWIRChannel *>(_channels[CHANNEL_ID_TVGRAYMWIR]);
    ch->enable_changed(v);
}
void Controller::tvgraymwir_method_changed(int v)
{
    TVGrayMWIRChannel * ch = reinterpret_cast<TVGrayMWIRChannel *>(_channels[CHANNEL_ID_TVGRAYMWIR]);
    ch->method_changed(v);
}
void Controller::tvgraymwir_offset_x_changed(int v)
{
    TVGrayMWIRChannel * ch = reinterpret_cast<TVGrayMWIRChannel *>(_channels[CHANNEL_ID_TVGRAYMWIR]);
    ch->offset_x_changed(v);
}
void Controller::tvgraymwir_offset_y_changed(int v)
{
    TVGrayMWIRChannel * ch = reinterpret_cast<TVGrayMWIRChannel *>(_channels[CHANNEL_ID_TVGRAYMWIR]);
    ch->offset_y_changed(v);
}
void Controller::tvgraymwir_method_agc_en_changed(bool v)
{
    TVGrayMWIRChannel * ch = reinterpret_cast<TVGrayMWIRChannel *>(_channels[CHANNEL_ID_TVGRAYMWIR]);
    ch->method_agc_en_changed(v);
}
void Controller::tvgraymwir_method_c_changed(double v)
{
    TVGrayMWIRChannel * ch = reinterpret_cast<TVGrayMWIRChannel *>(_channels[CHANNEL_ID_TVGRAYMWIR]);
    ch->method_c_changed(v);
}
void Controller::gsp_drift(const int32_t & x_drift, const int32_t & z_drift)
{
    if(!_GSPControlEnable) return;
    _gspController->set_drift(x_drift, z_drift);
    int32_t x_speed, z_speed;
    _gspController->set_speed(0, 0, x_speed, z_speed);
    _cmdManager->set_speed(x_speed, z_speed);
}

void Controller::gsp_set_z_delta_shift(const double& delta)
{
    if(!_GSPControlEnable) return;
    if(delta > -360. && delta < 360.) {
        double z_delta_shift = (360-_gsp.z_angle) - delta;
        _gspController->set_z_shift_angle(z_delta_shift);
    }
}

void Controller::self_test_on()
{
    qInfo(logController(), "self_test_on: SelfTest On");
    _cmdManager->self_test(true);
}
void Controller::self_test_off()
{
    qInfo(logController(), "self_test_off: SelfTest Off");
    _cmdManager->self_test(false);
}
void Controller::video_record_switch()  {_painter->video_record_switch();}
void Controller::video_record_on()      {_painter->video_record_on();}
void Controller::video_record_off()     {_painter->video_record_off();}
void Controller::image_save_switch()    {_painter->image_save_switch();}
void Controller::image_save_on()        {_painter->image_save_on();}
void Controller::image_save_off()       {_painter->image_save_off();}
void Controller::infographics_on_screen(bool flag) {_painter->infographics_on_screen(flag);}
void Controller::infographics_on_record(bool flag) {_painter->infographics_on_record(flag);}
void Controller::infographics_on_picture(bool flag) {_painter->infographics_on_picture(flag);}
void Controller::infographics_angleInversion_X(bool flag)
{
    _painter->infographics_angleInversion_X_changed(flag);
}
void Controller::infographics_angleInversion_Z(bool flag)
{
    _painter->infographics_angleInversion_Z_changed(flag);
}

void Controller::sl_setZAngleRange180(bool val) {_painter->setGspAngleRange(val);}

void Controller::sl_mwirDumpEnable(const bool en) {_painter->mwirDumpEn(en);}

void Controller::sl_infographic_setShipPointer()
{
    _painter->setShipPointer();
}
void Controller::sl_infographic_setHelicopterPointer()
{
    _painter->setHelicopterPointer();
}

void Controller::run()
{

    static int32_t cntT = 73;
    static int32_t cnt = 0;
    static int32_t channel = CHANNEL_ID_TVRGB;
    while(_execute.load(std::memory_order_acquire))
    {
//        static int cnt = 0;
//        if(++cnt == 1000)
//        {
//            cnt = 0;
//            cmdManager->ping();
//        }
        rcv_frame();
        rcv_cmd();
        std::this_thread::sleep_for(std::chrono::milliseconds(3));

        // if(cnt >= cntT) {
        //     cnt = 0;
        //     cntT = 73 + (double(rand()) / RAND_MAX * 2000);
        //     select_channel(channel);
        //     if(channel == CHANNEL_ID_TVRGB) {
        //         channel = CHANNEL_ID_MWIR;
        //     } else if(channel == CHANNEL_ID_MWIR) {
        //         channel = CHANNEL_ID_TVGRAY;
        //     } else if(channel == CHANNEL_ID_TVGRAY) {
        //         channel = CHANNEL_ID_TVRGB;
        //     }
        // }
        // cnt += 3;
    }
}
void Controller::rcv_frame()
{
    uint8_t * img_data;
    int img_w, img_h, img_id, img_num;
#ifdef TEST_FRAME
    auto period = std::chrono::duration<int, std::ratio<1, 30>>(1);
    auto currentTime = std::chrono::steady_clock::now();
    static auto end_time = currentTime + period;
    if(currentTime >= end_time)
    {
        end_time = currentTime + period;
        FrameGenerator::inst().tmr_timedout();
    }
    img_data = FrameGenerator::inst().get_frame(img_w, img_h);
#else

    if (_channel && (_channel->id() == CHANNEL_ID_VLX_STREAM_1  || _channel->id() == CHANNEL_ID_VLX_STREAM_2)) {
        img_data = reinterpret_cast<VlxStreamChannel*>(_channel)->fake_rcv(img_w, img_h, img_id, img_num);
    } else {
        img_data = _ethController->rcv_frame(img_w, img_h, img_id, img_num);
    }



#endif
    if(img_data)
    {
        if(_channel)
        {
        /*    if (channel->id() != img_id)
            {
                if(channels.find(img_id) == channels.end())
                {
                    qWarning(logController(), "select_channel: Channel Unknown[ %i ]", img_id);
                }
                else
                {
                    channel = channels[img_id];
                    if(channel)
                    {
                        qInfo(logController(), "select_channel: Sellect channel %s[ %i ]", channel->name().toStdString().c_str(), channel->id());
//                        cmdManager->select_channel(channel->id());
                    }
//            #ifdef TEST_FRAME
//                    FrameGenerator::inst().set_frame_size(channel->w(), channel->h());
//            #endif
                }
            } */

            _currentFrameSize.height = img_h;
            _currentFrameSize.width  = img_w;
            _channel->parse_raw_img(img_data, img_w, img_h);
            host_etld_procedure();

            _painter->setCurrentChannelID(img_id, img_w, img_h);
            _painter->setCurrentChannel(_channel);
            _painter->mwirSensorStatus(_mwirSensorStatus);
            _painter->mwirSensorStatus(_mwirSensorStatus);
            _painter->setTarget(_target_en, _target_size, _target_id, _target_fast);
            _painter->setRangefinder(_rangefinder);
            _painter->setGsp(_gsp, _gspController->get_z_shift_angle(), _GSPControlEnable);
            _painter->setTracker(_tracker_state, _tracker_channel);
            _painter->setTrackerObject(_objects);
            _painter->setFrameImage(_channel->origin_img());

            _painter->sl_paint();
        }
    }
}

void Controller::rcv_cmd()
{
    // static uint32_t cnt = 0;

    { // getting telemetry from input rtp stream
        Telemetry tm;
        if (VlxTelemetryChannel::i().getNext(tm))
            telemetry_handler(tm);
    }

    Packet packet;
//    Cmd res;
    if(_cmdManager->rcv_cmd(packet)) {
        const vdb_registers::t_host_registers dst_addr = static_cast<vdb_registers::t_host_registers>(packet.dst_addr());
        switch(dst_addr)
        {
        case vdb_registers::en_vdb_host_telemetry: {
            Telemetry tel;
            tel.from_packet(packet);
            telemetry_handler(tel);
        } break;
        case vdb_registers::en_vdb_host_autofocus_view: {
//            detector_packet_handler(packet);
        } break;
        default: break;
        }
    }
    else
    {
//        qWarning(logController(), logCmdManager(), "response[ %u ]: { ERROR }", cnt);
    }

    // ++cnt;
}
void Controller::telemetry_handler(Telemetry& cmd)
{
    if(cmd.data().size() >= sizeof(Telemetry::t_telemetry_struct)) {
//      qInfo(logController(), logCmdManager(), "response[ %u ]: { SUCCESS ID[ 0x%x ]DATA_LEN[ %u ] }", cnt, tel.get_id(), tel.get_data_len());
        Telemetry::t_telemetry_struct* tel = reinterpret_cast<Telemetry::t_telemetry_struct*>(cmd.get());
        _rangefinder.rangefinder_range_mono = double(tel->rf.last_distance);
        _gsp.flags.mech_arretir  = bool(tel->gsp.mode.mechanical_locking);
        _gsp.flags.el_arretir    = bool(tel->gsp.mode.electronic_locking);
        _gsp.flags.off           = bool(tel->gsp.mode.free_locking);
        _gsp.flags.stabilization = bool(tel->gsp.mode.gyrostabilization);
        _gsp.x_angle             = double(tel->gsp.x_angle);
        _gsp.z_angle             = double(tel->gsp.z_angle);

        Telemetry::t_union_rf_state rf_state;
        rf_state.word = tel->rf.state;
        _rangefinder.flags.rangefinder_power_on  = bool(rf_state.flags.power_on);
        _rangefinder.flags.rangefinder_rdy       = bool(rf_state.flags.ready);
        _rangefinder.flags.rangefinder_pause     = bool(rf_state.flags.cooling);
        _rangefinder.flags.rangefinder_cont_emit = bool(rf_state.flags.radiation);

        _channels[CHANNEL_ID_TVRGB]->set_focus_mm(double(tel->tv.focus_mm));
        _channels[CHANNEL_ID_TVRGB]->set_agc_on(bool(tel->agc.tv_colour_en));
        _channels[CHANNEL_ID_TVRGB]->set_field_angle(double(tel->tv.field_grad));
        _channels[CHANNEL_ID_TVRGB]->set_autofocus     (tel->controller.autofocus.state  && (static_cast<Telemetry::camera_codes>(tel->controller.autofocus.channel)  == Telemetry::tv_camera));
        _channels[CHANNEL_ID_TVRGB]->set_reg_field_sync(tel->controller.field_sync.state && (static_cast<Telemetry::camera_codes>(tel->controller.field_sync.channel) == Telemetry::tv_camera));

        _channels[CHANNEL_ID_TVGRAY]->set_focus_mm(double(tel->tv.focus_mm));
        _channels[CHANNEL_ID_TVGRAY]->set_agc_on(bool(tel->agc.tv_grey_en));
        _channels[CHANNEL_ID_TVGRAY]->set_field_angle(double(tel->tv.field_grad));
        _channels[CHANNEL_ID_TVGRAY]->set_autofocus     (tel->controller.autofocus.state  && (static_cast<Telemetry::camera_codes>(tel->controller.autofocus.channel)  == Telemetry::tv_camera));
        _channels[CHANNEL_ID_TVGRAY]->set_reg_field_sync(tel->controller.field_sync.state && (static_cast<Telemetry::camera_codes>(tel->controller.field_sync.channel) == Telemetry::tv_camera));

        _channels[CHANNEL_ID_SWIR]->set_focus_mm(double(tel->sw.focus_mm));
        _channels[CHANNEL_ID_SWIR]->set_agc_on(bool(tel->agc.swir_en));
        _channels[CHANNEL_ID_SWIR]->set_field_angle(double(tel->sw.field_grad));
        _channels[CHANNEL_ID_SWIR]->set_autofocus     (tel->controller.autofocus.state  && (static_cast<Telemetry::camera_codes>(tel->controller.autofocus.channel)  == Telemetry::sw_camera));
        _channels[CHANNEL_ID_SWIR]->set_reg_field_sync(tel->controller.field_sync.state && (static_cast<Telemetry::camera_codes>(tel->controller.field_sync.channel) == Telemetry::sw_camera));

        _channels[CHANNEL_ID_MWIR]->set_focus_mm(double(tel->mw.focus_mm));
        _channels[CHANNEL_ID_MWIR]->set_agc_on(bool(tel->agc.mwir_en));
        _channels[CHANNEL_ID_MWIR]->set_field_angle((double(tel->mw.field_grad)));
        _channels[CHANNEL_ID_MWIR]->set_autofocus     (tel->controller.autofocus.state  && (static_cast<Telemetry::camera_codes>(tel->controller.autofocus.channel)  == Telemetry::mw_camera));
        _channels[CHANNEL_ID_MWIR]->set_reg_field_sync(tel->controller.field_sync.state && (static_cast<Telemetry::camera_codes>(tel->controller.field_sync.channel) == Telemetry::mw_camera));

        _channels[CHANNEL_ID_FUSION]->set_focus_mm(double(tel->mw.focus_mm));
        _channels[CHANNEL_ID_FUSION]->set_agc_on(bool(tel->agc.tv_grey_en && tel->agc.mwir_en));
        _channels[CHANNEL_ID_FUSION]->set_field_angle(double(tel->mw.field_grad));

        _channels[CHANNEL_ID_TVGRAYMWIR]->set_focus_mm(double(tel->mw.focus_mm));
        _channels[CHANNEL_ID_TVGRAYMWIR]->set_agc_on(bool(tel->agc.tv_grey_en && tel->agc.mwir_en));
        _channels[CHANNEL_ID_TVGRAYMWIR]->set_field_angle(double(tel->mw.field_grad));

        if(!_host_etld_en)
        {
            _tracker_state = tel->controller.tracker_state;
            _tracker_channel = tel->tracker.tracker_channel;
            for(int i = 0; i < 4; ++i)
            {
                _objects[i].valid = bool(tel->tracker.targets[i].target == 0);
                _objects[i].trusted = bool(tel->tracker.targets[i].target == 0);
                _objects[i].w = tel->tracker.targets[i].x_size;
                _objects[i].h = tel->tracker.targets[i].y_size;
                _objects[i].x = tel->tracker.targets[i].x_center - _objects[i].w / 2;
                _objects[i].y = tel->tracker.targets[i].y_center - _objects[i].h / 2;
            }
        }

        _settings.flags.is_tracker_enable = tel->controller.tracker_state != 0xFF;

        _mwirSensorStatus = tel->mw.sensor_status;

        emit response(cmd);
    }
}

void Controller::host_etld_procedure()
{
    if(_channel)
    {
        if(_host_etld_en)
        {
            _host_etld_mutex.lock();
            static bool flag = false;
            if(_etld->isOn())
            {
                cv::Mat origin_img = _channel->origin_img();
                cv::Mat gray_img;
                cv::cvtColor(origin_img, gray_img, cv::COLOR_BGR2GRAY);
                static int X = 0, Y = 0, W = 320, H = 240;
                cv::Rect_<int> roi(X, Y, W, H);
                _etld->new_frame(gray_img.data, uint32_t(gray_img.cols), uint32_t(gray_img.rows), roi);
                X = roi.x;
                Y = roi.y;
                W = roi.width;
                H = roi.height;
                etld::etld_object object;
                _etld->get_object(&object);
                _objects[0].valid = object.valid;
                _objects[0].trusted = object.update;
                _objects[0].x = object.window.x;
                _objects[0].y = object.window.y;
                _objects[0].w = object.window.width;
                _objects[0].h = object.window.height;
                if(_objects[0].valid)
                {
                    int object_xc = _objects[0].x + _objects[0].w / 2;
                    int object_yc = _objects[0].y + _objects[0].h / 2;
                    double pix_field = atan(_channel->pixel_size_mm() / _channel->focus_mm()) * 180.0 / 3.1415926;
                    double rel_z_angle = pix_field * (int(_channel->w() / 2) - object_xc);
                    double rel_x_angle = pix_field * (int(_channel->h() / 2) - object_yc);
                    int32_t x_speed, z_speed;
                    _gspController->set_host_etld_object_error(rel_x_angle, rel_z_angle, x_speed, z_speed);//NOTE: инверсия по горизонтальной оси
                    _cmdManager->set_speed(x_speed, z_speed);
                    flag = true;
                }
                else
                {
                    if(flag) //NOTE: Процедура остановки платформы после потери цели
                    {
                        static int cnt = 0;
                        if(++cnt > 10)
                        {
                            flag = false;
                            cnt = 0;
                            int32_t x_speed, z_speed;
                            _gspController->set_speed(0, 0, x_speed, z_speed);
                            _cmdManager->set_speed(x_speed, z_speed);
                        }
                    }
                    else //NOTE: Процедура остановки трекинга после потери цели
                    {
                        static int cnt = 0;
                        if(++cnt > 50) // остановка через 2 секунды
                        {
                            flag = true;
                            cnt = 0;
                            _etld->deinit();
                        }
                    }
                }
            }
            else
            {
                if(flag) //NOTE: Процедура остановки платформы после остановки трекинга
                {
                    static int cnt = 0;
                    if(++cnt > 10)
                    {
                        flag = false;
                        cnt = 0;
                        int32_t x_speed, z_speed;
                        _gspController->set_speed(0, 0, x_speed, z_speed);
                        _cmdManager->set_speed(x_speed, z_speed);
                    }
                    _objects[0].valid = false;
                    _objects[0].trusted = false;
                    _objects[0].x = 0;
                    _objects[0].y = 0;
                    _objects[0].w = 0;
                    _objects[0].h = 0;
                }
            }
            _host_etld_mutex.unlock();
        }
    }
}

