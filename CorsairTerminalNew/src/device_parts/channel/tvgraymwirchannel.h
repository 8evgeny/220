#ifndef TVGRAYMWIRCHANNEL_H
#define TVGRAYMWIRCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class FusionAlgorithm;

class TVGrayMWIRChannel : public AbstractChannel
{
    Q_OBJECT
public:
    explicit TVGrayMWIRChannel(QObject * parent = nullptr, const QString & _name = CHANNEL_NAME_TVGRAYMWIR, const QString & _show_name = CHANNEL_SHOW_NAME_TVGRAYMWIR, const uint32_t & _id = CHANNEL_ID_TVGRAYMWIR, const int & _w = 640 * 2, const int & _h = 480);
    ~TVGrayMWIRChannel() override;

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
    void enable_changed(bool);
    void method_changed(int);
    void offset_x_changed(int);
    void offset_y_changed(int);
    void method_agc_en_changed(bool);
    void method_c_changed(double);
private:
    FusionAlgorithm * fusionAlgorithm;

    bool enable;
    int method;
};

#endif // TVGRAYMWIRCHANNEL_H
