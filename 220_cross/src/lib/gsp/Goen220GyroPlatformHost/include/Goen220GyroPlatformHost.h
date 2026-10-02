#ifndef GOEN_220_GYRO_PLATFORM_HOST_H
#define GOEN_220_GYRO_PLATFORM_HOST_H


#include "MemsBoardStatus.h"
#include "MotorStatus.h"

#include <QObject>


class Goen220GyroPlatformHostPrivate;


/// \brief  Goen220GyroPlatformHost Модуль управляет удаленным устройством - Гироплатформой через последовательный
///                                 порт.


class Goen220GyroPlatformHost : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Goen220GyroPlatformHost)

    /// \brief  Свойства, предназначенниые для подключения GUI:
    Q_PROPERTY( QStringList serialPorts READ getSerialPorts NOTIFY serialPortsChanged)
    Q_PROPERTY( QString serialPortName READ getSerialPortName WRITE setSerialPortName NOTIFY serialPortNameChanged)
    Q_PROPERTY( QString serialPortErrorString READ getSerialPortErrorString NOTIFY serialPortErrorStringChanged)

    Q_PROPERTY( bool printTxRawDataSerialPort READ getPrintTxRawDataSerialPort WRITE setPrintTxRawDataSerialPort
                NOTIFY printTxRawDataSerialPortChanged)

    Q_PROPERTY( bool printRxRawDataSerialPort READ getPrintRxRawDataSerialPort WRITE setPrintRxRawDataSerialPort
                NOTIFY printRxRawDataSerialPortChanged)



public:
    explicit Goen220GyroPlatformHost( QObject *parent = nullptr);
    ~Goen220GyroPlatformHost();



    //  Интерфейс управления Последовательным портом:

    /// \brief getSerialPorts   Получить список последовательных портов, представленных в системе.
    QStringList getSerialPorts() const;

    /// \brief  connect     Подключиться к последовательному порту и начать обмен данными с устройством на другом
    ///                     конце.
    Q_INVOKABLE void connect( const bool con);

    /// \brief  getSerialPortName   Получить имя установленного последовательного порта.
    QString getSerialPortName() const;

    /// \brief  setSerialPortName   Установить имя последовательного порта для дальнейшего подключения к нему через
    ///                             метод connect.
    Q_INVOKABLE void setSerialPortName( const QString &portName);


    /// \brief  getSerialPortErrorString    Получить текст ошибки при открытии последовательного порта. Имеет смысл
    ///                                     после получения сигнала serialPortErrorStringChanged.
    QString getSerialPortErrorString() const;


    /// \brief  getPrintTxRawDataSerialPort     Получить статус текстового вывода "сырых" передаваемых данных через
    ///                                         сигнал txRawDataSerialPortText.
    bool getPrintTxRawDataSerialPort() const;

    /// \brief  setPrintTxRawDataSerialPort     Разрешение текстового вывода "сырых" передаваемых данных через сигнал
    ///                                         txRawDataSerialPortText.
    Q_INVOKABLE void setPrintTxRawDataSerialPort( const bool enable);


    /// \brief  getPrintRxRawDataSerialPort     Получить статус текстового вывода "сырых" принимаемых данных через
    ///                                         сигнал rxRawDataSerialPortText.
    bool getPrintRxRawDataSerialPort() const;

    /// \brief  setPrintRxRawDataSerialPort     Разрешение текстового вывода "сырых" принимаемых данных через сигнал
    ///                                         rxRawDataSerialPortText.
    Q_INVOKABLE void setPrintRxRawDataSerialPort( const bool enable);



    //  Интерфейс настройки VDB протокола:

    /// \brief  setHostId       Идентификатор текущего устройства.
    Q_INVOKABLE void setHostId( const uint8_t hostId);

    /// \brief  setDeviceId     Идентификатор удаленного устройства.
    Q_INVOKABLE void setDeviceId( const uint8_t deviceId);

    /// \brief setTimeoutWaitingResponse    Установка времени ожидания ответа от удаленного устройства.
    /// \param msec                         Время в мсек. Не рекомендуется устанавливать ниже 40 мсек. ввиду задержек
    ///                                     в ОС.
    Q_INVOKABLE void setTimeoutWaitingResponse( const uint32_t msec);

    /// \brief setTimeBetweenStatusRequests Установка временного периода между запросами статуса устройства в случае
    ///                                     отсутствия пользовательских команд.
    /// \param msec                         Время в мсек. Не рекомендуется устанавливать ниже 40 мсек. ввиду задержек
    ///                                     в ОС.
    Q_INVOKABLE void setTimeBetweenStatusRequests( const uint32_t msec);



    //  Интерфейс управления Гироплатформой:

    ///	\brief	Mode	Допустимые режимы работы Гироплатформы.
    enum Mode
    {
         Dummy
        ,Stabilization
        ,RotaryPlatform
        ,Parkin
        ,Off
    };

    Q_ENUM(Mode)

    /// \brief  setMode         Установака режима работы удаленного устройства.
    Q_INVOKABLE void setMode( const Mode mode);

    /// \brief  getModeNames    Получить текстовый список допустимых режимов работы удаленного устройства.
    Q_INVOKABLE QStringList getModeNames() const;


    /// \brief  setControlPosition  Управление по положению.
    /// \param  yaw                 Угловое положение по оси курс, град. (-180.0...+180.0);
    /// \param  pitch               Угловое положение по оси тангаж, град. (-180.0...+180.0);
    Q_INVOKABLE void setControlPosition( const float yaw, const float pitch);

    /// \brief  setControlSpeed     Управление по скорости.
    /// \param  yaw                 Курс, град./с (-180.0...+180.0);
    /// \param  pitch               Тангаж, град./с (-180.0...+180.0);
    Q_INVOKABLE void setControlSpeed( const float yaw, const float pitch);

    /// \brief  setControlTracking  Управление относительными углами.
    /// \param  yawMisalignment     Рассогласование по оси курса, град. (-45.0...+45.0);
    /// \param  pitchMisalignment   Рассогласование по оси тангажа, град. (-45.0...+45.0);
    /// \param  yawAccelLimit       Ограничение ускорения по оси курса, град./(с^2) (0.0...180.0);
    /// \param  pitchAccelLimit     Ограничение ускорения по оси тангажа, град./(с^2) (0.0...180.0);
    Q_INVOKABLE void setControlTracking(    const float yawMisalignment, const float pitchMisalignment,
                                            const float yawAccelLimit, const float pitchAccelLimit);

signals:
    ///	\brief	stateConnectToSerialPortChanged		Сигнал оповещения о подключении/отключении к
    /// 											последовательному порту.
    void stateConnectToSerialPortChanged( const bool con);

    ///	\brief	txRawDataSerialPortText		Сигнал с данными, представленными в виде текстовой строки,
    /// 									отправляемыми в последовательный порт.
    /// 									Предназначенно для отладки.
    void txRawDataSerialPortText( const QString &text);

    ///	\brief	rxRawDataSerialPortText		Сигнал с данными, представленными в виде текстовой строки,
    /// 									принятыми из последовательного порта.
    /// 									Предназначенно для отладки.
    void rxRawDataSerialPortText( const QString &text);

    ///	\brief	versionChanged	Сигнал оповещает систему о версиях протоколов используемых
    /// 						на удаленном устройстве.
    /// 						Версии запрашиваются у устройства автоматически при первом
    /// 						подключении.
    void versionChanged( const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                         const uint32_t versionDevice);

    ///	\brief	modeChanged		Сигнал оповещения изменения статуса удаленного устройства.
    void modeChanged( const Mode mode);

    ///	\brief	memsBoardStatusCommonChanged	Сигнал оповещения об изменении статуса устройств
    /// 										на плате MEMS (Медленные изменения).
    void memsBoardStatusCommonChanged( const MemsBoardStatusCommon &memsBoardStatus);

    ///	\brief	memsBoardStatusSpeedAndAngleChanged		Сигнал оповещения об изменении данных
    /// 												от датчиков на плате MEMS (Быстрые изменения).
    void memsBoardStatusSpeedAndAngleChanged( const MemsBoardStatusSpeedAndAngle &memsBoardStatus);

    ///	\brief	yawMotorStatusChanged	Сигнал оповещения об изменении данных от мотора курса.
    void yawMotorStatusChanged(	const MotorStatus &motorStatus);

    ///	\brief	pitchMotorStatusChanged	Сигнал оповещения об изменении данных от мотора тангажа.
    void pitchMotorStatusChanged(	const MotorStatus &motorStatus);

    ///	\brief	Сигналы оповещения о исполнении пользовательских команд на удаленном устройстве:
    void setModeCommandCompleted();
    void setControlPositionCommandCompleted();
    void setControlSpeedCommandCompleted();
    void setControlTrackingCommandCompleted();

    /// \brief  Сигнал оповещения о необходимости перезапуска удаленного устройства ввиду отсутствия ответа.
    void restart();

    /// \brief  Сигнал оповещения о наличии связи с удаленным устройством.
    void stateConnectToRemoteDeviceChanged( const bool con);

signals:
    ///	\brief	Сигналы, предназначенные для подключения GUI:
    void serialPortsChanged();
    void serialPortNameChanged();
    void serialPortErrorStringChanged();
    void printTxRawDataSerialPortChanged();
    void printRxRawDataSerialPortChanged();



private:
    Goen220GyroPlatformHostPrivate      *d_ptr;
};

#endif
