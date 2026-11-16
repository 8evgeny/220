#ifndef TVGRAYCHANNEL_H
#define TVGRAYCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class TVGrayChannel : public AbstractChannel
{
    Q_OBJECT
public:
    explicit TVGrayChannel(QObject * parent = nullptr, const QString & _name = CHANNEL_NAME_TVGRAY, const QString & _show_name = CHANNEL_SHOW_NAME_TVGRAY, const uint32_t & _id = CHANNEL_ID_TVGRAY, const int & _w = 1920, const int & _h = 1080);

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
};

#endif // TVGRAYCHANNEL_H
