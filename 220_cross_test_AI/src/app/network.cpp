#include "network.h"
#include "telemetry.h"
#include "cmd_parser.h"
#include "settingsmanager.h"
#include <QScopedValueRollback>

// ═══════════════════════════════════════════════════════════════════
//  CONSTRUCTOR / DESTRUCTOR
// ═══════════════════════════════════════════════════════════════════
Network::Network(SettingsManager *sm, GST_Wrapper *gst_wrapper, QObject *parent)
    : QObject(parent), sm{sm}, m_gst_wrapper{gst_wrapper}
{
    qDebug() << Q_FUNC_INFO << "Network";
    initSocket();
} /* Network::Network */

Network::~Network()
{
    qDebug() << Q_FUNC_INFO;

    // Close TCP server (accepts no more connections)
    if (m_tcpServer)
    {
        m_tcpServer->close();
        // Sockets will be cleaned up via disconnected → deleteLater
    } /* if */

    // Stop thread event loop and wait
    if (thread.isRunning())
    {
        thread.quit();
        thread.wait(3000);
    } /* if */
} /* Network::~Network */

// ═══════════════════════════════════════════════════════════════════
//  initSocket — creates UDP + TCP listeners in the Network thread
// ═══════════════════════════════════════════════════════════════════
void Network::initSocket()
{
    moveToThread(&thread);

    connect(
        &thread, &QThread::started, this,
        [this]
        {
            qDebug() << "************* Network thread id:" << QThread::currentThreadId()
                     << Q_FUNC_INFO;

            // ─── UDP Receive ─────────────────────────────────────────
            socket = new QUdpSocket(this);
            bool udpOk = socket->bind(QHostAddress::AnyIPv4, sm->settings_store.udp_port_dev);
            socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,
                                    QVariant(1024 * 8000));

            if (udpOk)
                qDebug() << "[UDP] Listening on port" << sm->settings_store.udp_port_dev;
            else
                qWarning() << "[UDP] BIND FAILED:" << socket->errorString();

            connect(socket, &QUdpSocket::readyRead, this, &Network::processPendingDatagrams);

            // ─── UDP Send ────────────────────────────────────────────
            udpSocketSend = new QUdpSocket(this);
            dst_host = QHostAddress(sm->settings_store.ip_host);
            dst_port = sm->settings_store.udp_port_host;

            connect(
                udpSocketSend, &QUdpSocket::bytesWritten, this,
                [this](qint64)
                {
                    dequeuePacket();
                } /* lambda */,
                Qt::QueuedConnection);

            // ─── TCP Server (same port as UDP — they coexist) ────────
            m_tcpServer = new QTcpServer(this);

            if (m_tcpServer->listen(QHostAddress::AnyIPv4, sm->settings_store.udp_port_dev))
            {
                qDebug() << "[TCP] Listening on port" << sm->settings_store.udp_port_dev;

                connect(m_tcpServer, &QTcpServer::newConnection, this,
                        &Network::onTcpNewConnection);

            } /* if */
            else
            {
                qWarning() << "[TCP] LISTEN FAILED:" << m_tcpServer->errorString();
            } /* else */

            // ─── Telemetry timer (40 ms = 25 Hz) ─────────────────────
            telemetry_timer = new QTimer(this);

            connect(telemetry_timer, &QTimer::timeout, this, &Network::telemetry_timer_send);

            telemetry_timer->start(40);

            // ─── Command parser (runs in its own thread via moveToThread) ──

            connect(this, &Network::sig_checkCmd, &cmd_parser, &CMD_Parser::proc_checkCmd,
                    Qt::QueuedConnection);

            // ─── Telemetry preamble for MPEG-TS KLV ──────────────────
            m_TelemPreamb = QByteArray(5, '\0');
            for (int i = 0; i < 5; i++)
                m_TelemPreamb[i] = static_cast<char>(9 - i);
        } /* lambda */,
        Qt::DirectConnection);

    QObject::connect(&thread, &QThread::finished, this, &Network::proc_thread_finished);

    thread.start();
} /* Network::initSocket */

// ═══════════════════════════════════════════════════════════════════
//  UDP RECEIVE
// ═══════════════════════════════════════════════════════════════════
void Network::processPendingDatagrams()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(static_cast<int>(socket->pendingDatagramSize()));

        QHostAddress senderAddr;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &senderAddr, &senderPort);

        const QString source =
            QStringLiteral("UDP %1:%2").arg(senderAddr.toString()).arg(senderPort);

        handleIncomingCommand(datagram, source);
    } /* while */
} /* Network::processPendingDatagrams */

// ═══════════════════════════════════════════════════════════════════
//  TCP SERVER HANDLERS
// ═══════════════════════════════════════════════════════════════════
void Network::onTcpNewConnection()
{
    while (m_tcpServer->hasPendingConnections())
    {
        QTcpSocket *client = m_tcpServer->nextPendingConnection();

        const QString peerInfo =
            QStringLiteral("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());

        qDebug() << "[TCP] Client connected:" << peerInfo;

        m_tcpBuffers[client] = QByteArray(); // zero-init buffer for this connection

        connect(client, &QTcpSocket::readyRead, this, &Network::onTcpReadyRead);

        connect(client, &QTcpSocket::disconnected, this, &Network::onTcpDisconnected);

    } /* while */
} /* Network::onTcpNewConnection */

void Network::onTcpReadyRead()
{
    auto *client = qobject_cast<QTcpSocket *>(sender());
    if (!client)
        return;

    // Append all available data to per-connection buffer
    m_tcpBuffers[client].append(client->readAll());

    QByteArray &buf = m_tcpBuffers[client];

    // Extract complete fixed-size packets from the stream
    while (buf.size() >= CMD_PACKET_SIZE)
    {
        QByteArray packet = buf.left(CMD_PACKET_SIZE);
        buf.remove(0, CMD_PACKET_SIZE);

        const QString source = QStringLiteral("TCP %1:%2")
                                   .arg(client->peerAddress().toString())
                                   .arg(client->peerPort());

        handleIncomingCommand(packet, source);
    } /* while */

    // Overflow guard: if buffer grows without forming valid packets → flush
    if (buf.size() > TCP_MAX_BUFFER)
    {
        qWarning() << "[TCP] Buffer overflow from" << client->peerAddress().toString()
                   << "size=" << buf.size() << "— flushing";
        buf.clear();
    } /* if */
} /* Network::onTcpReadyRead */

void Network::onTcpDisconnected()
{
    auto *client = qobject_cast<QTcpSocket *>(sender());
    if (!client)
        return;

    qDebug() << "[TCP] Client disconnected:" << client->peerAddress().toString() << ":"
             << client->peerPort();

    m_tcpBuffers.remove(client);
    client->deleteLater();
} /* Network::onTcpDisconnected */

// ═══════════════════════════════════════════════════════════════════
//  COMMON COMMAND DISPATCH (UDP + TCP converge here)
// ═══════════════════════════════════════════════════════════════════
void Network::handleIncomingCommand(const QByteArray &packet, const QString &source)
{
    // Quick size check before dispatching to parser
    if (packet.size() != CMD_PACKET_SIZE)
    {
        qWarning() << "[CMD] Invalid packet size" << packet.size() << "from" << source
                   << "— dropped";
        return;
    } /* if */

    qDebug() << "[CMD]" << source << "size:" << packet.size();
    emit sig_checkCmd(packet);
} /* Network::handleIncomingCommand */

// ═══════════════════════════════════════════════════════════════════
//  TELEMETRY TIMER (25 Hz)
// ═══════════════════════════════════════════════════════════════════
void Network::telemetry_timer_send()
{
    if (m_gst_wrapper == nullptr)
        return;

    // Poll tracker state from GST thread
    QMetaObject::invokeMethod(m_gst_wrapper, &GST_Wrapper::proc_getOutObject, Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_gst_wrapper, &GST_Wrapper::proc_getActiveLockTracking,
                              Qt::QueuedConnection);

    // Build telemetry packet
    auto ba_sdata = m_telemetry.setServiceData();

    if (sm->settings_store.mpeg_ts_telemetry_en)
    {
        // Embed into MPEG-TS as KLV
        ba_sdata[0] = 0x04;
        ba_sdata.prepend(m_TelemPreamb);
        emit sig_MPEG_TS_Telemetry(ba_sdata);
    } /* if */
    else
    {
        // Standalone UDP telemetry
        m_udpFifo.push_back(ba_sdata);
    } /* else */

    dequeuePacket();
} /* Network::telemetry_timer_send */

// ═══════════════════════════════════════════════════════════════════
//  UDP TX QUEUE (non-MPEGTS mode)
// ═══════════════════════════════════════════════════════════════════
void Network::dequeuePacket()
{
    if (m_udpFifo.empty())
        return;

    int cnt = UDP_TX_BATCH;

    while (cnt-- > 0 && !m_udpFifo.empty())
    {
        const auto &packet = m_udpFifo.front();
        bytesToBeWritten += static_cast<quint64>(packet.size());

        if (auto res = udpSocketSend->writeDatagram(packet, dst_host, dst_port); res >= 0)
        {
            m_udpFifo.pop_front();
        } /* if */
        else
        {
            txActive = false;
            // Retry later when socket becomes writable again
            QMetaObject::invokeMethod(this, &Network::dequeuePacket, Qt::QueuedConnection);
            return;
        } /* else */
    } /* while */

    txActive = !m_udpFifo.empty();
} /* Network::dequeuePacket */

// ═══════════════════════════════════════════════════════════════════
//  THREAD FINISHED — cleanup
// ═══════════════════════════════════════════════════════════════════
void Network::proc_thread_finished()
{
    qDebug() << Q_FUNC_INFO;

    // Stop telemetry timer
    if (telemetry_timer)
    {
        telemetry_timer->stop();
        delete telemetry_timer;
        telemetry_timer = nullptr;
    } /* if */

    // Close TCP server — deletes all child sockets via QObject tree
    if (m_tcpServer)
    {
        m_tcpServer->close();
        // Sockets owned by QTcpServer → deleted when this object is destroyed
        m_tcpBuffers.clear();
        m_tcpServer = nullptr;
    } /* if */

    // Close UDP sockets (owned by 'this' via parent-child, auto-deleted)
    if (socket)
    {
        socket->close();
    } /* if */
    if (udpSocketSend)
    {
        udpSocketSend->close();
    } /* if */
} /* Network::proc_thread_finished */

// ═══════════════════════════════════════════════════════════════════
//  STATUS HANDLERS — called via QueuedConnection from other threads
//  All execute in Network::thread, so m_telemetry access is safe.
// ═══════════════════════════════════════════════════════════════════

void Network::procVICAPstatus(CameraStatus status)
{
    m_telemetry.procVICAPstatus(status);

    float tvFieldV = 0.0f;
    const float tvFieldH = m_telemetry.getTvFieldGrad(&tvFieldV);
    emit tvFieldChanged(tvFieldV, tvFieldH);
} /* Network::procVICAPstatus */

void Network::procLDCstatus(LDC_STATE dev_state)
{
    m_telemetry.procLDCstatus(dev_state);
} /* Network::procLDCstatus */

void Network::procJ200status(J200_DEVSTATE ds)
{
    // Записываем в телеметрию (для UDP пакета)
    m_telemetry.procJ200status(ds);

    // Считаем FOV и ЭМИТИМ сигнал для GyroPlatformHandler
    float fovW, fovH;
    Telemetry::calcIrFov(ds.zoomPosition, fovW, fovH);
    emit sig_irFieldChanged(fovW, fovH);
} /* Network::procJ200status */

void Network::proc_MINI640_State(MINI640_DEV_STATE dev_state)
{
    m_telemetry.proc_MINI640_State(dev_state);
} /* Network::proc_MINI640_State */

void Network::procGspStatus(const bool connected)
{
    m_telemetry.procGspStatus(connected);
} /* Network::procGspStatus */

void Network::procGspPositionMode(const bool zero, const bool pohod, const bool pilot,
                                  const bool park, const bool stabilization)
{
    m_telemetry.procGspPositionMode(zero, pohod, pilot, park, stabilization);
} /* Network::procGspPositionMode */

void Network::procGspMotorAngle(const float z, const float y)
{
    m_telemetry.procGspMotorAngle(z, y);
} /* Network::procGspMotorAngle */

void Network::procGspMotorSpeed(const float z, const float y)
{
    m_telemetry.procGspMotorSpeed(z, y);
} /* Network::procGspMotorSpeed */

void Network::procUpdateTrackerObject(ushort x, ushort y, ushort width, ushort height)
{
    m_telemetry.proc_UpdateTrackerObject(x, y, width, height);
} /* Network::procUpdateTrackerObject */

void Network::procUpdateActiveLockTracking(bool active, bool tracking)
{
    m_telemetry.proc_UpdateTrackerActiveLockTracking(active, tracking);
} /* Network::procUpdateActiveLockTracking */

// ═══════════════════════════════════════════════════════════════════
//  LEGACY (unused in MPEG-TS mode, kept for non-MPEGTS fallback)
// ═══════════════════════════════════════════════════════════════════
void Network::frameSend()
{
    // Q_UNUSED(isBusy);
} /* Network::frameSend */

void Network::procSetTrackerChannel(quint8 ch_id)
{
    m_telemetry.procSetTrackerChannel(ch_id);
} /* Network::procSetTrackerChannel */
