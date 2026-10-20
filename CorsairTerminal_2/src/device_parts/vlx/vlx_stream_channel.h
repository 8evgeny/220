#pragma once

#include <QObject>
#include <QThread>
#include <QElapsedTimer>
#include <QSettings>

#include <opencv2/opencv.hpp>
#include "device_parts/channel/abstractchannel.h"
#include "vlx_media/vlx_av.h"

class VlxStreamChannel : public AbstractChannel
{
    Q_OBJECT

    struct Internal;

public:

    explicit VlxStreamChannel(quint16 networkPort,
                              const QString &_name = CHANNEL_NAME_VLX_STREAM_1,
                              const QString &_show_name = CHANNEL_SHOW_NAME_VLX_STREAM_1,
                              const uint32_t &_id = CHANNEL_ID_VLX_STREAM_1,
                              const int &w = 1920,
                              const int &h = 1080,
                              QObject *parent = nullptr);

    virtual ~VlxStreamChannel();

    uint8_t* fake_rcv(int & w, int & h, int & id, int & num);

signals:

    // AbstractChannel interface
public:
    void parse_raw_img(cv::Mat_<uint8_t> raw_img);

    static Vlx::Av::InputDemuxerPtr demuxer();

private slots:

    void initialization();

    void onAvErrorOccured();

    void onAvStarted();

    void onAvStopped();

    void onInputDemuxerStateChanged();

    void onSettingsSaved();

    void startAv();

private:

    Internal *m_i;

    cv::Mat m_frame;

    /**
     * @brief триггер перезапуска. Обрабатывается по событию avStopped,
     * если true вызвает av->start и устанавливается в false
     */
    bool m_autoRestart = false;

    qint64 m_lastFrameMs = 0;
    quint64 m_framesCnt = 0;
    quint64 m_fc = 0;
    qreal m_fps = 0.0f;

    QElapsedTimer m_fcElapsedCntr;

    // std::list<cv::Mat> m_frames;

    // QMutex m_mtx;

    // AbstractChannel interface
public:
    void selected();
    void deselected();

};
