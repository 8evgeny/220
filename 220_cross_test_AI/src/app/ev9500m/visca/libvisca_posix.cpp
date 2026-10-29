/*
 * VISCA(tm) Camera Control Library — POSIX platform (FIXED)
 */

#include "libvisca.h"
#include <qserialport.h>
#include <unistd.h>
#include <QDebug>

extern "C"
{

    /* implemented in libvisca.c */
    void _VISCA_append_byte(VISCAPacket_t *packet, unsigned char byte);
    void _VISCA_init_packet(VISCAPacket_t *packet);
    unsigned int _VISCA_get_reply(VISCAInterface_t *iface, VISCACamera_t *camera);
    unsigned int _VISCA_send_packet_with_reply(VISCAInterface_t *iface, VISCACamera_t *camera,
                                               VISCAPacket_t *packet);

    QSerialPort m_serialPort;
    QByteArray m_buff;

    /* ========================================================================
 * FIX #1: Инициализация err + waitForBytesWritten + проверка результата write
 * ======================================================================== */
    uint32_t _VISCA_write_packet_data(VISCAInterface_t *iface, VISCACamera_t *camera,
                                      VISCAPacket_t *packet)
    {
        if (!m_serialPort.isOpen())
            return VISCA_FAILURE;

        qint64 written = m_serialPort.write((char *)packet->bytes, packet->length);
        if (written < 0 || written != (qint64)packet->length)
        {
            qWarning() << "VISCA write failed:" << m_serialPort.errorString();
            return VISCA_FAILURE;
        } /* if */

        // FIX: Дожидаемся физической отправки байтов в UART
        if (!m_serialPort.waitForBytesWritten(500))
        {
            qWarning() << "VISCA write timeout (bytes not flushed)";
            return VISCA_TIMEOUT;
        } /* if */

        return VISCA_SUCCESS;
    } /* _VISCA_write_packet_data */

    uint32_t _VISCA_send_packet(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet)
    {
        if ((iface->address > 7) || (camera->address > 7) || (iface->broadcast > 1))
        {
            return VISCA_FAILURE;
        } /* if */

        // build header:
        packet->bytes[0] = 0x80;
        packet->bytes[0] |= (iface->address << 4);
        if (iface->broadcast > 0)
        {
            packet->bytes[0] |= (iface->broadcast << 3);
            packet->bytes[0] &= 0xF8;
        } /* if */
        else
        {
            packet->bytes[0] |= camera->address;
        } /* else */

        // append footer
        _VISCA_append_byte(packet, VISCA_TERMINATOR);

        return _VISCA_write_packet_data(iface, camera, packet);
    } /* _VISCA_send_packet */

    /* ========================================================================
 * FIX #3: Корректный парсинг пакета с терминатором
 * ======================================================================== */
    uint32_t _VISCA_get_packet(VISCAInterface_t *iface)
    {
        // Читаем данные пока не найдём терминатор или не истечёт таймаут
        bool found_terminator = false;

        // Сначала проверяем уже накопленный буфер
        for (int i = 0; i < m_buff.size(); i++)
        {
            if ((uint8_t)m_buff[i] == VISCA_TERMINATOR)
            {
                found_terminator = true;
                break;
            } /* if */
        } /* for */

        // Если терминатор не найден — читаем новые данные
        while (!found_terminator)
        {
            if (!m_serialPort.waitForReadyRead(1000))
            {
                // Таймаут: если в буфере есть мусор без терминатора — чистим
                if (m_buff.size() > 0)
                {
                    qWarning() << "VISCA read timeout, discarding" << m_buff.size() << "bytes";
                    m_buff.clear();
                } /* if */
                return VISCA_TIMEOUT;
            } /* if */

            QByteArray arr = m_serialPort.readAll();
            m_buff.append(arr);

            // Проверяем есть ли терминатор в новых данных
            for (int i = 0; i < arr.size(); i++)
            {
                if ((uint8_t)arr[i] == VISCA_TERMINATOR)
                {
                    found_terminator = true;
                    break;
                } /* if */
            } /* for */

            // Защита от переполнения буфера
            if (m_buff.size() >= 256)
            {
                qWarning() << "VISCA buffer overflow, clearing";
                m_buff.clear();
                return VISCA_FAILURE;
            } /* if */
        } /* while */

        // Находим позицию терминатора и извлекаем пакет
        int terminator_pos = -1;
        for (int i = 0; i < m_buff.size(); i++)
        {
            if ((uint8_t)m_buff[i] == VISCA_TERMINATOR)
            {
                terminator_pos = i;
                break;
            } /* if */
        } /* for */

        // Копируем пакет в ibuf
        int packet_len = terminator_pos + 1;
        memcpy(iface->ibuf, m_buff.data(), packet_len);
        iface->bytes = packet_len;

        // Удаляем обработанные байты из буфера (включая терминатор)
        m_buff.remove(0, packet_len);

        return VISCA_SUCCESS;
    } /* _VISCA_get_packet */

    /***********************************/
    /*       SYSTEM  FUNCTIONS         */
    /***********************************/

    /* ========================================================================
 * FIX #2: Использование параметра baudrate вместо жёстко зашитого 115200
 * ======================================================================== */
    uint32_t VISCA_open_serial(VISCAInterface_t *iface, const char *device_name, uint32_t baudrate)
    {
        m_serialPort.setPortName(device_name);

        // FIX: проверяем что baudrate — реальное число (9600..115200), а не termios константа
        if (baudrate < 100 || baudrate > 4000000)
        {
            qWarning() << "VISCA: invalid baudrate value" << baudrate << ", using 115200";
            baudrate = 115200;
        } /* if */

        if (!m_serialPort.setBaudRate((int)baudrate))
        {
            qWarning() << "VISCA: cannot set baudrate" << baudrate << ":" << m_serialPort.errorString();
            return VISCA_FAILURE;
        } /* if */

        m_serialPort.setParity(QSerialPort::NoParity);
        m_serialPort.setDataBits(QSerialPort::Data8);
        m_serialPort.setStopBits(QSerialPort::OneStop);
        m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

        if (!m_serialPort.open(QIODevice::ReadWrite))
        {
            qWarning() << "VISCA: cannot open port" << device_name << ":" << m_serialPort.errorString();
            return VISCA_FAILURE;
        } /* if */

        m_buff.clear();
        m_serialPort.clear();

        iface->address = 0;
        return VISCA_SUCCESS;
    } /* VISCA_open_serial */

    uint32_t VISCA_close_serial(VISCAInterface_t *iface)
    {
        if (m_serialPort.isOpen())
        {
            m_serialPort.close();
        } /* if */
        // FIX #4: Очищаем буфер при закрытии
        m_buff.clear();
        return VISCA_SUCCESS;
    } /* VISCA_close_serial */

    uint32_t VISCA_unread_bytes(VISCAInterface_t *iface, unsigned char *buffer, uint32_t *buffer_size)
    {
        if (!m_serialPort.isOpen())
        {
            *buffer_size = 0;
            return VISCA_FAILURE;
        } /* if */

        qint64 available = m_serialPort.bytesAvailable();
        if (available > 0)
        {
            uint32_t to_read = (uint32_t)std::min((qint64)*buffer_size, available);
            qint64 read_bytes = m_serialPort.read((char *)buffer, to_read);
            *buffer_size = (read_bytes > 0) ? (uint32_t)read_bytes : 0;
            return VISCA_FAILURE;
        } /* if */

        *buffer_size = 0;
        return VISCA_SUCCESS;
    } /* VISCA_unread_bytes */

    uint32_t VISCA_usleep(uint32_t useconds)
    {
        return (uint32_t)usleep(useconds);
    } /* VISCA_usleep */

} // extern "C"
