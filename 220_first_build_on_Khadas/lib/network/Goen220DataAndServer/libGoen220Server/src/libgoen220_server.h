#pragma once

#include <QHostAddress>
#include <QObject>

#include <libgoen220data.h>

namespace Goen220 {
namespace Server {

/// @brief Класс сервера GOEN220
/// @details
///  - Управляет TCP сервером
///  - Автоматически отправляет телемтрическую информацию, при наличии подключенного клиента
///  - Обеспечивает уведомление об ошибках
class Intrf : public QObject
{
    Q_OBJECT

protected:
    /// @brief Конструктор
    explicit Intrf(QObject *parent = nullptr);


public:

    /// @brief Деструктор
    virtual ~Intrf();

    /// @brief Коды ошибок сервера
    enum class Error {
        None,      ///< Нет ошибок
        TcpServer, ///< Ошибка Tcp сервера
        TcpClient, ///< Ошибка на Tcp-сокете клиента
        Udp        ///< Ошибка отправки Udp датаграммы
    };

    Q_ENUM(Error)

    /// @brief Возвращает номер tcp порта сервера
    virtual quint16 tcpPort() const = 0;

    /// @brief Возвращает true если разрешен механизм поддержания сессии TCP
    virtual bool isKeepAliveProcEnabled() const = 0;

    /// @brief Возвращает true если есть подключенный tcp клиент
    virtual bool isClientConnected() const = 0;

    /// @brief Возвращает адрес текущего (или последнего подключенного) tcp клиента
    virtual QHostAddress client() const = 0;

    /// @brief Возврашает номер udp порта на который отправляется телеметрическая информация
    virtual quint16 tlmUdpPort() const = 0;

    /// @brief Возвращает текущий интервал (мс) отправки телеметрической информации
    virtual int tlmTransmissionIntervalMs() const = 0;

    /// @brief Возвращает указатель на текущий объект телеметрической иформации
    /// @details по умолчнию: сервер имеет свой объект Tlm
    virtual Goen220::Tlm *tlm() const = 0;

    /// @brief Устанавливает адрес текущего объекта телеметрической информаци
    /// @details
    ///  - по умолчанию: сервер имеет свой объект Tlm
    ///  - при отправке телеметрии данные будут читаться из указанного источника
    ///  - для сброса в состояние по умаолчнию, выставить параметр tlm в nullptr
    /// @param tlm адрес объекта источника телеметрической информации
    virtual void setTlmSrc(Goen220::Tlm *tlm) = 0;

    /// @brief Возвращает строковое описание последней возникшей ошибки
    virtual QString lastErrorString() const = 0;

    /// @brief Возвращает последнюю возникшую ошибку
    virtual Error lastError() const = 0;

    /// @brief Возвращает доп.код последней ошибки
    /// @details Для ошибок на Tcp Udp возврашает значение QAbstractSocket::SocketError
    virtual int lastErrorCode() const = 0;

public slots:

    /// @brief Устанавливает активность tcp сервера
    virtual void setListening(bool state) = 0;

    /// @brief Разрешает/запрещает работу механизма поддержания TCP-сессии
    virtual void setKeepAliveProcEnabled(bool state) = 0;

    /// @brief Устанавливает номер порта TCP сервера
    /// @note номер порта будет применен только при активации сервера
    virtual void setTcpPort(quint16 port) = 0;

    /// @brief Устанавливает номер udp порта отправки телемтрической информации
    virtual void setTlmUdpPort(quint16 port) = 0;

    /// @brief Устанавливает интервал (мс) отправки телеметрической информации
    virtual void setTlmTransmissionIntervalMs(int ms) = 0;

    /// @brief Отправляет клиенту статус выполнения команды
    /// @param cmd команда
    /// @param ack код подтверждения
    virtual void cmdAck(Goen220::Cmd cmd, Goen220::CmdAck ack) = 0;

signals:

    /// @brief отправляет уведомление о возникшей ошибке
    /// @param error код ошибки
    void errorOccured(Error error);

    /// @brief уведомляет об изменении статуса подключения клиента
    /// @param state true если клиент подключен
    void clientConnectionStateChanged(bool state);

    /// @brief Уведомляет о получении новой команды
    /// @param cmd команда
    /// @param payload данные команды
    /// @param ackRequired команда требует подтверждения выполнения (@see Goen220Server::cmdAck)
    void cmdReceived(Goen220::Cmd cmd, QByteArray payload, bool ackRequired);
};

/// @brief Создает объект сервера
Intrf *create(QObject *parent = nullptr);

/**
 * @brief Создает объект сервер на потоке th
 * @details
 * - Если поток работает, помещает вызов Server::create в поток th и блокируется до выполнения
 * - Если поток не работает, функция вызывает дополнительно метод Qthread::start
 * - Если th == nullptr, то функция создает поток
 *   так же функци блокируется до запуска созданного потока. Указатель на новый поток может быть получен
 *   через метод объекта Server QObject::thread
 * @note Созданные объекты не имеют parent и пользоватлеь несет ответсвенность за их удаление
 * @note Блокировка выполнятся с применением локального QEventLoop
 * @param th укказатель на целевой объект потока
 * @return указатель на созданный объект сервера
 */
Intrf *createOnThread(QThread *th = nullptr);

/**
 * @brief Вызывает метод QObject::deleteLater и блокируется до его выпонения
 * @note Блокировка выполнятся с применением локального QEventLoop
 * @param server удаляемый Server
 */
void removeServer(Intrf *server);

} // namespace Server
} // namespace Goen220
