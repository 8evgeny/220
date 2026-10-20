#ifndef SWIRCHANNEL_H
#define SWIRCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class SWIRChannel : public AbstractChannel
{
    Q_OBJECT
public:
    explicit SWIRChannel(QObject * parent = nullptr, const QString & _name = CHANNEL_NAME_SWIR,
                         const QString & _show_name = CHANNEL_SHOW_NAME_SWIR, const uint32_t & _id = CHANNEL_ID_SWIR,
                         const int & _w = 640, const int & _h = 512);

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
};

#endif // SWIRCHANNEL_H
