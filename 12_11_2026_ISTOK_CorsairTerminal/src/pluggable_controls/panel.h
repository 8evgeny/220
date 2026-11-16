#ifndef PANEL_H
#define PANEL_H

#include <QObject>

class QTimer;
class QSerialPort;

class Panel : public QObject
{
    Q_OBJECT
public:
    explicit Panel(QObject * parent = nullptr);
    ~Panel();

    bool is_opened();
    bool open(const QString & name);
    void close();

    void set_port_name(const QString & port_name) {_port_name = port_name;}
    inline QString port_name() {return _port_name;}
    void set_auto_connect(const bool & auto_connect) {_auto_connect = auto_connect;}
    inline bool auto_connect() {return _auto_connect;}
    inline bool panel_connected() {return _panel_connected;}
    inline bool enabled() {return _enabled;}

signals:
    void select_channel(const uint32_t & channel_id);
    void digital_zoom_switch();
    void inversion();
    void brightness_up();
    void brightness_down();
    void contrast_up();
    void contrast_down();
    void bc_reset();
    void target_switch();
    void target_window_set(const int &);
    void lrf_start();
    void autofocus_en();
    void autofocus_dis();
    void gsp_speed_set(const double &, const double &);
    void zoom_in(const double & speed = 1.0);
    void zoom_out(const double & speed = 1.0);
    void zoom_stop();
    void focus_near(const double & speed = 1.0);
    void focus_far(const double & speed = 1.0);
    void focus_stop();

public slots:
    void set_enable(const bool & enabled);

private slots:
    void com_tmr_task();
    void com_port_data_ready();

private:
    void handle_msg();
    void handle_cmd(const uint8_t & code, const int & data_ind);
    char crc(const int & start, const int & len);

    void gspSpeedSet(const int & x_speed, const int & z_speed);
    void zoomSpeedSet(const int & zoomSpeed);
    void focusSpeedSet(const int & focusSpeed);
    void strobeSizeSet(const short & strobeSize);

private:
    QTimer * tmr;
    QSerialPort * com_port;
    QByteArray reports;
    QString _port_name;
    bool _auto_connect;
    bool _panel_connected;
    bool _enabled;
};

#endif // PANEL_H
