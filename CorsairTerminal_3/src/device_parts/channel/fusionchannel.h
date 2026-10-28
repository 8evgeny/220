#ifndef FUSIONCHANNEL_H
#define FUSIONCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class FusionChannel : public AbstractChannel
{
    Q_OBJECT
public:
    explicit FusionChannel(QObject * parent = nullptr, const QString & _name = CHANNEL_NAME_FUSION, const QString & _show_name = CHANNEL_SHOW_NAME_FUSION, const uint32_t & _id = CHANNEL_ID_FUSION, const int & _w = 640, const int & _h = 480);

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
};

#endif // FUSIONCHANNEL_H
