#ifndef MWIRCHANNEL_H
#define MWIRCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class MWIRChannel : public AbstractChannel
{
    Q_OBJECT
public:
    explicit MWIRChannel(QObject * parent = nullptr, const QString & _name = CHANNEL_NAME_MWIR, const QString & _show_name = CHANNEL_SHOW_NAME_MWIR, const uint32_t & _id = CHANNEL_ID_MWIR, const int & _w = 640, const int & _h = 512);

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
};

#endif // MWIRCHANNEL_H
