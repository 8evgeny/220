#include "network.h"
#include <unistd.h>
#include "telemetry.h"
#include "cmd_parser.h"
#include "settingsmanager.h"
#include <QScopedValueRollback>

Network::Network(SettingsManager *sm, GST_Wrapper *gst_wrapper , QObject *parent)
    : QObject(parent)  
    , sm {sm}
    ,m_gst_wrapper{gst_wrapper}
{

    bool b_rows = true;
    bool b_cols = false;
    int video_width = sm->settings_store.tv_cam_width;
    int video_height = sm->settings_store.tv_cam_height;
    qDebug() << Q_FUNC_INFO<< "Network thread "<< video_width* video_height;
    thread = new QThread;
    initSocket();


}

Network::~Network()
{
    qDebug() << Q_FUNC_INFO;
    //socket->close();
    //udpSocketSend->close();
    SettingsManager *sm= nullptr;
    GST_Wrapper *gst_wrapper = nullptr;
    QObject *parent  = nullptr;

    thread->quit();
    auto res = thread->wait();
    delete thread;

    qDebug() << "~~~~~~~~~~~~~~" << res << QThread::currentThreadId() << Q_FUNC_INFO;
}


void Network::initSocket()
{
    moveToThread(thread);

    connect(thread, &QThread::started, this, [this]{


        qDebug() << " ************* Network thread id: " << QThread::currentThreadId() << Q_FUNC_INFO;

        socket = new QUdpSocket();
        bool result =  socket->bind(QHostAddress::AnyIPv4, sm->settings_store.udp_port_dev);
        socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, QVariant(1024 * 8000));


        udpSocketSend = new QUdpSocket();
      //  dst_host =QHostAddress("192.168.1.11");
        dst_host = QHostAddress(sm->settings_store.ip_host);// QHostAddress("192.168.1.125");
        dst_port = sm->settings_store.udp_port_host;// 5000;QHostAddress::Broadcast;//
      //  emit sig_FrameRequestTV(sFrame);



        if(result)
        {
            qDebug() << "Socket PASS";
        }
        else
        {
            qDebug() << "Socket FAIL";
        }

        processPendingDatagrams();
        connect(socket, &QUdpSocket::readyRead, this, &Network::processPendingDatagrams);

        connect(udpSocketSend, &QUdpSocket::bytesWritten, this, [this] (qint64 b){
            dequeuePacket();
        }, Qt::QueuedConnection);

        telemetry_timer = new QTimer(this);

        connect(telemetry_timer, &QTimer::timeout, this, &Network::telemetry_timer_send);
        telemetry_timer->start(40);
        connect(this, &Network::sig_checkCmd, &cmd_parser, &CMD_Parser::proc_checkCmd, Qt::QueuedConnection);


    }, Qt::DirectConnection); //cant have parameter sorry, when using connect

    // int video_width = 1920;
    // int video_height = 1080;
    // memset(frame,0,video_height*video_width);
    // for(int it_h = 1; it_h < video_height; it_h=it_h+2)
    // {
    //     for(int it_w = 0; it_w < video_width; it_w=it_w+2)
    //     {
    //         frame[it_w+it_h*video_width]=255;
    //     }
    // }


    QObject::connect(thread, SIGNAL(finished()), this, SLOT(proc_thread_finished()));



    thread->start();
}


void Network::telemetry_timer_send()
{
   // qDebug() << "Tel timer";

    if(m_gst_wrapper == nullptr)
        return;
    m_gst_wrapper->proc_getOutObject();
    m_gst_wrapper->proc_getActiveLockTracking();

    auto ba_sdata = m_telemetry.setServiceData();
    m_udpFifo.push_back(ba_sdata);
    dequeuePacket();
}
void Network::frameSend()
{
    static int fc = 0;

    qDebug() << Q_FUNC_INFO <<  " *** " << fc++;

    QScopedValueRollback _t {isBusy};

    isBusy = true;

    int subframe_cnt = size / udp_payload_len;
     qDebug() <<Q_FUNC_INFO<<"frame size: " <<  size << " subframe_cnt: "<< subframe_cnt;

    // for(int sf = 0; sf < subframe_cnt; sf++)
    // {

    //     auto ba_data = vframe.setData(sf, (char*)pFrame + sf * udp_payload_len );
    //     bytesToBeWritten += ba_data.size();


    //     if (auto res = udpSocketSend->writeDatagram(ba_data, dst_host,dst_port); res < 0)
    //     {
    //         qDebug() << "writeDatagramm failure: " << udpSocketSend->errorString()
    //                  << " bytesToBeWritten " << bytesToBeWritten  << " bytesWritten " << bytesWritten
    //                  << "sf:" << sf;
    //         bytesToBeWritten -= ba_data.size();
    //         break;
    //     }
    //     QThread::msleep(10);

    // }
   //  v4l2_ptr->frameFreed();



    qDebug() << Q_FUNC_INFO << " *** " << " bytesToBeWritten " << bytesToBeWritten  << " bytesWritten " << bytesWritten;
}

void Network::proc_thread_finished()
{
    m_gst_wrapper = nullptr;
    telemetry_timer->stop();
    socket->close();
    udpSocketSend->close();

    QObject::disconnect(this, nullptr, nullptr, nullptr);

    qDebug() << "****" << QThread::currentThreadId() << Q_FUNC_INFO;

    delete(socket);
    delete(udpSocketSend);
   // thread->exit();

    // emit frameSendDone();
    // isBusy =false;
}

void Network::dequeuePacket()
{
    if (m_udpFifo.empty())
        return;

    int cnt = 20;

    while (cnt-- > 0 && !m_udpFifo.empty())
    {
        const auto &packet = m_udpFifo.front();

        // qDebug() << Q_FUNC_INFO << "1." << packet.mid(0,16).toHex();

        bytesToBeWritten += packet.size();

        if (auto res = udpSocketSend->writeDatagram(packet, dst_host,dst_port); res >= 0)
        {
            m_udpFifo.pop_front();
            // qDebug() << Q_FUNC_INFO << "2. OK";
        }
        else
        {
            // qDebug() << Q_FUNC_INFO << "2. FAIL";
            // qDebug() << "writeDatagramm failure: " << udpSocketSend->errorString()
           //  << "queue size:" << m_udpFifo.size();

            txActive = false;

            QMetaObject::invokeMethod(this, &Network::dequeuePacket, Qt::QueuedConnection);

            return;
        }
    }

    txActive = !m_udpFifo.empty();

    // qDebug() << Q_FUNC_INFO << "packets left:" << m_udpFifo.size() << "frames left:" << ((qreal)m_udpFifo.size() / 405.0)  << QThread::currentThreadId();
}

void Network::enqueueTvFrame()
{
    int subframe_cnt = size / udp_payload_len;
    // qDebug() <<Q_FUNC_INFO<<"frame size: " <<  size << " subframe_cnt: "<< subframe_cnt;
    static uint16_t it_frame_num;
    static const uint16_t video_width = sm->settings_store.tv_cam_width;
    static const uint16_t video_height = sm->settings_store.tv_cam_height;
     static const uint16_t frame_type = 0x00; //RAW BAYER

    if(size<video_height*video_width)
    {
        qDebug()<<"Frame too small!";
        return;
    }

   // m_telemetry.updateServiceData(sFrame);

    auto ba_sdata = m_telemetry.setServiceData();
    m_udpFifo.push_back(ba_sdata);

    // ba_sdata = vframe.setVideoServiceData(it_frame_num, video_width,video_height,frame_type);
    // m_udpFifo.push_back(ba_sdata);


    //memcpy(frame, pFrame, video_height * video_width);



    // for(int it_h = 0; it_h < video_height; it_h=it_h+1)
    // {
    //     for(int it_w = 0; it_w < video_width; it_w=it_w+2)
    //     {
    //         frame[it_w+it_h*video_width]=((char*)pFrame)[it_w+1+it_h*video_width];
    //         frame[it_w+1+it_h*video_width]=((char*)pFrame)[it_w+it_h*video_width];

    //     }
    // }


    // //Bayer map apply
    // for(int it_wh = 0; it_wh < video_height*video_width; it_wh=it_wh+1)
    // {
    //     frame[it_wh]=((char*)pFrame)[bayer_map[it_wh]];
    // }

    // for(int sf = 0; sf < subframe_cnt; sf++)
    // {
    //     auto ba_data = vframe.setData(sf, (char*)pFrame + sf * udp_payload_len );

    //     m_udpFifo.push_back(ba_data);
    // }

     it_frame_num++;
}



void Network::enqueueIRFrame()
{
    int subframe_cnt = size / udp_payload_len;
    // qDebug() <<Q_FUNC_INFO<<"frame size: " <<  size << " subframe_cnt: "<< subframe_cnt;
    static uint16_t it_frame_num;
    static const uint16_t video_width = sm->settings_store.ir_cam_width;
    static const uint16_t video_height = sm->settings_store.ir_cam_height;
    static const uint16_t frame_type = 0x03; //RAW BAYER

    if(size<video_height*video_width)
    {
        qDebug()<<"Frame too small!";
        return;
    }

   // m_telemetry.updateServiceData(sFrame);

    auto ba_sdata = m_telemetry.setServiceData();
    m_udpFifo.push_back(ba_sdata);

    ba_sdata = m_telemetry.setVideoServiceData(it_frame_num, video_width,video_height,frame_type);
    m_udpFifo.push_back(ba_sdata);



    // for(int sf = 0; sf < subframe_cnt; sf++)
    // {
    //     auto ba_data = vframe.setData(sf, (char*)pFrame + sf * udp_payload_len );

    //     m_udpFifo.push_back(ba_data);
    // }

    it_frame_num++;
}





// void network::tvframeReady(frame_t sFrame)
// {
//     static int fc = 0;
//    // qDebug() << Q_FUNC_INFO << fc++ << m_udpFifo.size();
//     emit sig_FrameRequestTV(sFrame);

//     if (m_udpFifo.size()> 1000)
//     {

//         if (!txActive)
//             dequeuePacket();



//         return;
//     }

//     this->sFrame = sFrame;
//     this->pFrame = sFrame.baFrame.data();
//     this->size = sm->settings_store.tv_cam_width * sm->settings_store.tv_cam_height;//size;

//      enqueueTvFrame();


//     if (!txActive)
//         dequeuePacket();

// }


// void network::irframeReady(frame_t sFrame)
// {
//     static int fc = 0;
//     // qDebug() << Q_FUNC_INFO << fc++ << m_udpFifo.size();
//     emit sig_FrameRequestIR(sFrame);

//     if (m_udpFifo.size()> 1000)
//     {

//         if (!txActive)
//             dequeuePacket();



//         return;
//     }

//     this->sFrame = sFrame;
//     this->pFrame = sFrame.baFrame.data();
//     this->size = sm->settings_store.ir_cam_width * sm->settings_store.ir_cam_height;//size;

//     enqueueIRFrame();


//     if (!txActive)
//         dequeuePacket();

// }




void Network::processPendingDatagrams()
{


    qDebug() << "in !";
    QHostAddress sender;
    quint16 port;

    QByteArray datagram_send;

    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(),datagram.size(),&sender,&port);
        socket->writeDatagram(datagram,sender,port);
        qDebug() <<"Message From :: " << sender.toString();
        qDebug() <<"Port From :: "<< port;
        qDebug() <<"Message size :: " << datagram.size();

      //  qDebug() <<"Message :: " << datagram;

        emit sig_checkCmd(datagram);
     //  if(cmd_parser.checkCmd(datagram)<0)
     //       qDebug() <<"Error decode CMD";

        //udpSocketSend->close();
    }
}


void Network::procVICAPstatus(CameraStatus status)
{
    m_telemetry.procVICAPstatus(status);
    float tvFieldV = 0.0f;
    const float tvFieldH = m_telemetry.getTvFieldGrad( &tvFieldV);

    emit tvFieldChanged( tvFieldH, tvFieldV);
}

void Network::procLDCstatus(LDC_STATE dev_state)
{
    m_telemetry.procLDCstatus(dev_state);
}


void Network::procJ200status(J200_DEVSTATE ds)
{
    m_telemetry.procJ200status(ds);
}


void Network::proc_MINI640_State(MINI640_DEV_STATE dev_state)
{
    m_telemetry.proc_MINI640_State(dev_state);
}


void Network::procGspStatus( const bool connected)
{
    m_telemetry.procGspStatus( connected);
}


void Network::procGspPositionMode( const bool zero, const bool pohod, const bool pilot, const bool park, const bool stabilization)
{
    m_telemetry.procGspPositionMode( zero, pohod, pilot, park, stabilization);
}


void Network::procGspMotorAngle(const float z, const float y)
{
    m_telemetry.procGspMotorAngle( z, y);
}


void Network::procGspMotorSpeed( const float z, const float y)
{
    m_telemetry.procGspMotorSpeed( z, y);
}


void Network::procUpdateTrackerObject(ushort x,ushort y, ushort width, ushort heigth)
{
    m_telemetry.proc_UpdateTrackerObject(x,y, width,heigth);
}
void Network::procUpdateActiveLockTracking(bool active, bool tracking)
{
    m_telemetry.proc_UpdateTrackerActiveLockTracking(active, tracking);
}
