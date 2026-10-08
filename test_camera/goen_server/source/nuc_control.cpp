#include "nuc_control.hpp"
using namespace std;
using namespace LibSerial;

NucControl::NucControl(std::string & port_, bool & ok)
{
    cout << "NucControl CONSTRUCTOR" << endl;
    port = port_;
    ok = false;
    system("sudo ir_cam_setup");
    std::this_thread::sleep_for(500ms);serial_ptr = std::make_unique<SerialPort>();
    if(serial_ptr->IsOpen()) {cout << "Serial port was open early" << endl;}
    else {cout << "Try open " << port << endl;}
    try
    {
        serial_ptr->Open(port);
        serial_ptr->SetBaudRate(BaudRate::BAUD_115200);
        if(!serial_ptr->IsOpen())
        {
            cout << "ERROR OPEN " << port << endl;
            ok = false;
        } // END if(!serial_ptr->IsOpen())
        else
        {
            cout << "Open " << port  << "SUCCESS" << endl;
            ok = true;
            // this->set_nuc_interval(1);
            this_thread::sleep_for(40ms);
            this->set_mod_num(9);
            this_thread::sleep_for(40ms);
            this->shutter_correction();
        } // END if(serial_ptr->IsOpen())
    } // END try
    catch(const exception & e)
    {
        cout << "NucControl constructor ERROR: " << e.what() << endl;
        ok = false;
        return;
    } // END catch(const exception & e)
} // -- END NucControl

NucControl::~NucControl()
{
    std::cout << "Destructor NucControll. Close serial: " << port << std::endl;
    close();
} // -- END ~NucControl

void NucControl::shutter_correction()
{
    if(is_open())
    {
        cout << "\nSHUTTER CORRECTION\n\n";
        vector<uint8_t> data_c = {0xAA, 0x05, 0x01, 0x11, 0x02, 0x01, 0x00, 0xEB, 0xAA};
        checksum(data_c, 6);
        send_data(data_c, data_c.size());
    } // END  if(is_open())
} // -- END shutter_correction

void NucControl::autonuc_on()
{
    if(is_open())
    {
        cout << "\nAUTONUC ON\n\n";
        autonuc_enable = true;
        vector<uint8_t> data_c = {0xAA, 0x05, 0x01, 0x01, 0x01, 0x01, 0xB3, 0xEB, 0xAA};
        send_data(data_c, data_c.size());
    } // END  if(is_open())
} // -- END autonuc_on

void NucControl::autonuc_off()
{
    if(is_open())
    {
        cout << "\nAUTONUC OFF\n\n";
        autonuc_enable = false;
        vector<uint8_t> data_c = {0xAA, 0x05, 0x01, 0x01, 0x01, 0x00, 0xB2, 0xEB, 0xAA};
        send_data(data_c, data_c.size());
    } // END  if(is_open())
} // -- END autonuc_off

void NucControl::set_nuc_interval(int step)
{
    if(is_open())
    {
        cout << "\nSET NUC INTERVAL " << step << "[min]\n\n";
        vector<uint8_t> data_c = {0xAA, 0x05, 0x01, 0x03, 0x01, (uint8_t)step, 0x00, 0xEB, 0xAA}; // set autonuc interval
        checksum(data_c, 6);
        send_data(data_c, data_c.size());
    } // END  if(is_open())
} // -- END set_nuc_interval

void NucControl::set_mod_num(int num)
{
    if(is_open())
    {
        cout << "\nSET MOD NUM " << num << "[min]\n\n";
        vector<uint8_t> data_c = {0xAA, 0x05, 0x01, 0x19, 0x01, (uint8_t)num, 0x00, 0xEB, 0xAA}; // set autonuc interval
        checksum(data_c, 6);
        send_data(data_c, data_c.size());
    } // END  if(is_open())
} // -- END set_mod_num

bool NucControl::close()
{
    try
    {
        if(serial_ptr->IsOpen())
        {
            cout << "Try close " << port << endl;
            serial_ptr->Close();
            if(serial_ptr->IsOpen())
            {
                cout << "ERROR close " << port << endl;
                return false;
            } // END if(serial_ptr->IsOpen())
            else
            {
                cout << "Close " << port << " SUCCESS" << endl;
            } // END if(!serial_ptr->IsOpen())
        } // END if(serial_ptr->IsOpen())
        else
        {
            cout << port << "was closed early!" << endl;
            return true;
        } // END if(!serial_ptr->IsOpen())
    } // END try
    catch(const exception & e)
    {
        cout << "NucControl: Close " << port << " ERROR with exception: " << e.what() << endl;
        return false;
    } // END catch(const exception & e)
} // -- END close

bool NucControl::is_open() { bool ok = serial_ptr->IsOpen(); return ok;}

void NucControl::checksum(vector<uint8_t> &data, int num)
{
    int sum = 0;
    for(int i = 0; i < num; i++)
    {
        sum += (int)data[i];
    } // END for(int i = 0; i < num; i++)
    uint8_t checksum = (uint8_t)sum;
    data[num] = checksum;
    cout << "checksum = " << hex << "0x" << uppercase << (int)checksum << dec << endl;
} // -- END checksum

void NucControl::send_data(vector<uint8_t> &data, int data_len)
{
    DataBuffer buf ;
    cout << "Send msg: " << hex;
    for(int i = 0; i < data_len; i++)
    {
        buf.emplace_back(data[i]);
        cout << " 0x" << (int)data[i] ;
    } // END for(int i = 0; i < data_len; i++)
    cout << endl;
    serial_ptr->Write(buf);
    serial_ptr->DrainWriteBuffer();
} // -- END send_data

