#ifndef CPUCHANNEL_H
#define CPUCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class CPUChannel : public AbstractChannel
{
        Q_OBJECT
public:
    explicit CPUChannel(QObject * parent = nullptr,
                        const QString& _name = CHANNEL_NAME_CPU,
                        const QString& _show_name = CHANNEL_SHOW_NAME_CPU,
                        const uint32_t& _id = CHANNEL_ID_CPU,
                        const int& _w = 960,
                        const int& _h = 540);

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
};

#endif // CPUCHANNEL_H
