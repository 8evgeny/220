#ifndef NUC_CONTROL_HPP
#define NUC_CONTROL_HPP

#include <iostream>
#include <memory>
#include <cstring>
#include <libserial/SerialPort.h>
#include <vector>
#include <cstdint>
#include <thread>
#include <chrono>

/// add usb device like COM port (ttyUSB)
/// sudo modprobe usbserial vendor=0x0424 product=0x6000

class NucControl
{
public:
  NucControl(){};
  NucControl(std::string & port_, bool & ok);
  ~NucControl();
  void shutter_correction();
  void autonuc_on();
  void autonuc_off();
  void set_nuc_interval(int step);
  bool is_open();
  void set_mod_num(int num);
  bool close();
  bool autonuc_status() {return autonuc_enable;}
private:
  std::string port = "/dev/ttyUSB2";
  void checksum(std::vector<uint8_t> & data, int num);
  void send_data(std::vector<uint8_t> & data, int data_len);
  std::unique_ptr<LibSerial::SerialPort> serial_ptr = nullptr;
  bool autonuc_enable = true;
}; // END class NucControl

#endif // NUC_CONTROL_HPP
