#include "abstractchannel.h"

#include <QImage>
#include <cmath>
#include <omp.h>

#include "eth/ethcontroller.h"
#include "tools/framegenerator.h"

#include "device_parts/cmd/cmdmanager.h"
#include "device_parts/gsp/gspcontroller.h"

#include <etld/etld.h>
#include <QVariant>

#include <thread>
using namespace std;

#include "tools/imgp.h"
#include "tools/drawp.h"

#include "tools/logger.h"

#include "tools/resource.h"

#define ENABLE_DELAY    (10)

#include <QMap>
const QLoggingCategory & logChannel(const QString & channel_name, const uint32_t & channel_id)
{
    static QMap<uint32_t, QLoggingCategory *> channels;
    if(!channels.contains(channel_id))
    {
        char * name = new char[100];
        sprintf(name, "%s Channel[%u]", channel_name.toStdString().c_str(), channel_id);
        channels[channel_id] = new QLoggingCategory(name);
    }
    return *(channels[channel_id]);
}

AbstractChannel::AbstractChannel(QObject *parent,
                                 const QString &name,
                                 const QString &show_name,
                                 const uint32_t &id,
                                 const int &w,
                                 const int &h)
    : QObject(parent)
    , _name(name)
    , _show_name(show_name)
    , _id(id)
    , _w(w)
    , _h(h)
    , _n(w * h)
    , _const_zoom(double(CHANNEL_H) / double(h))
    , _digital_zoom(1)
    , _pic_in_pic_on(false)
    , _swir_gain(0)
    , _agc_on(false)
    , _inversion_on(false)
    , _brightness(0)
    , _contrast(1)
    , _autofocus(false)
    , _reg_field_sync(false)
    , _focus_mm(50)
    , _min_focus_mm(1)
    , _max_focus_mm(512)
    , _pixel_size_mm(CHANNEL_MWIR_PIX_SIZE)
    , _field_angle(10.0)
    , _x_inv(false)
    , _y_inv(false)
{
}
void AbstractChannel::load()
{
    // QString RESOURCE_SECTION = _name.toLower();
    // QString RESOURCE_FILENAME = "rsc/" + _name.toLower() + ".xml";
    // Resource resource(RESOURCE_SECTION);
    // if(resource.load(RESOURCE_FILENAME))
    // {
    //     bool ok;
    //     QVariant tmp;
    //     tmp                  = resource.get("min_focus_mm",  ok);
    //     if(ok) _min_focus_mm  = tmp.toDouble();
    //     tmp                  = resource.get("max_focus_mm",  ok);
    //     if(ok) _max_focus_mm  = tmp.toDouble();
    //     tmp                  = resource.get("pixel_size_mm", ok);
    //     if(ok) _pixel_size_mm = tmp.toDouble();
    // }
    // else
    // {
    //     save();
    // }
}
void AbstractChannel::save()
{
    QString RESOURCE_SECTION = _name.toLower();
    QString RESOURCE_FILENAME = "rsc/" + _name.toLower() + ".xml";
    Resource resource(RESOURCE_SECTION);
    resource.set("min_focus_mm", _min_focus_mm);
    resource.set("max_focus_mm", _max_focus_mm);
    resource.set("pixel_size_mm", _pixel_size_mm);
    resource.save(RESOURCE_FILENAME);
}
void AbstractChannel::decount_object(int & x, int & y, int & w, int & h, const int & W, const int & H)
{
    x = int(round((x + (W - _w) / 2)));
    y = int(round((y + (H - _h) / 2)));
    w = int(round(w));
    h = int(round(h));
}
void AbstractChannel::recount_object(int & x, int & y, int & w, int & h, const int & W, const int & H)
{
    x = int(round((x - (W - _w * _const_zoom * _digital_zoom) / 2) / _const_zoom / _digital_zoom));
    y = int(round((y - (H - _h * _const_zoom * _digital_zoom) / 2) / _const_zoom / _digital_zoom));
    w = int(round(w / _const_zoom / _digital_zoom));
    h = int(round(h / _const_zoom / _digital_zoom));
    if(_x_inv) x = _w - x - w;
    if(_y_inv) y = _h - y - h;
}
AbstractChannel::~AbstractChannel()
{
    _origin_img.release();
    _display_img.release();
}
