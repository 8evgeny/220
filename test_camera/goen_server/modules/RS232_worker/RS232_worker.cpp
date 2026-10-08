#include <RS232_worker.hpp>
using namespace std;
using namespace LibSerial;

void RS232_worker::setBautrade(int speed)
{
    if (speed == 9600) serial_ptr->SetBaudRate(BaudRate::BAUD_9600);
    else if (speed == 19200) serial_ptr->SetBaudRate(BaudRate::BAUD_19200);
    else if (speed == 38400) serial_ptr->SetBaudRate(BaudRate::BAUD_38400);
    else if (speed == 57600) serial_ptr->SetBaudRate(BaudRate::BAUD_57600);
    else if (speed == 115200) serial_ptr->SetBaudRate(BaudRate::BAUD_115200);
    else if (speed == 230400) serial_ptr->SetBaudRate(BaudRate::BAUD_230400);
    else
    {
        cout<<"incorrect bautrade RS232!!! set 115200\n";
        serial_ptr->SetBaudRate(BaudRate::BAUD_115200);
    }//END else
} // -- END setBautrade(int speed)

RS232_worker::RS232_worker(int speed, const string & port)
{
    cout << "RS232_worker Ctor" << endl;
    common_data_ptr = make_shared<common_data>();
#ifdef USE_SERIAL_PTR
    try
    {
        vector<string> ports =  serial_ptr->GetAvailableSerialPorts();
        cout<<"--- Available for 232 SerialPorts ---"<<endl;
        for(auto &i:ports) {cout<<i<<endl;}
        cout<<endl;
        serial_ptr = make_unique<SerialPort>(port);
        port_open_OK = true;
    } // END try
    catch (OpenFailed)
    {
        cout<<"Serial Port 232 error opening !!!" <<endl;
    } // END catch
    if(port_open_OK)
    {
        setBautrade(speed);
        data_from_rs232.reserve(_telemetryLenToBoard);
        data_from_rs232.clear();
    } // END if(port_open_OK)
    else
    {
        cout<<"\n=====================  port RS232 not OPEN !!!  ======================\n" <<endl;
    } // END else
#else // USE_SERIAL_PTR
    port_open_OK = true;
#endif // !USE_SERIAL_PTR

} // -- END RS232_worker(int speed, const string & port)

RS232_worker::~RS232_worker()
{
    cout << "Destructor RS232_worker" << endl;
} // -- END ~RS232_worker()

uint8_t RS232_worker::getCMD() const
{
    return _CMD_FROM_RS232;
} // -- END getCMD

void RS232_worker::setCMD(uint8_t newCMD)
{
    _CMD_FROM_RS232 = newCMD;
} // -- END getCmdLen()

void RS232_worker::printDataRS232(TYPE_DATA type)
{
    if(type == TYPE_DATA::CMD)
    {
        printf( "\nreceived from rs232: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "
                "\n",
                cmdBuf[0], cmdBuf[1], cmdBuf[2], cmdBuf[3], cmdBuf[4], cmdBuf[5], cmdBuf[6], cmdBuf[7],
                cmdBuf[8], cmdBuf[9], cmdBuf[10], cmdBuf[11], cmdBuf[12], cmdBuf[13], cmdBuf[14], cmdBuf[15]
                );
    } // END if(type == TYPE_DATA::CMD)
    if(type == TYPE_DATA::TELEMETRY)
    {
        printf( "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "
                "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "
                "\n",
                telemetryBuf[0], telemetryBuf[1], telemetryBuf[2], telemetryBuf[3], telemetryBuf[4], telemetryBuf[5], telemetryBuf[6], telemetryBuf[7],
                telemetryBuf[8], telemetryBuf[9], telemetryBuf[10], telemetryBuf[11], telemetryBuf[12], telemetryBuf[13], telemetryBuf[14], telemetryBuf[15],
                telemetryBuf[16], telemetryBuf[17], telemetryBuf[18], telemetryBuf[19], telemetryBuf[20], telemetryBuf[21], telemetryBuf[22], telemetryBuf[23],
                telemetryBuf[24], telemetryBuf[25], telemetryBuf[26], telemetryBuf[27], telemetryBuf[28], telemetryBuf[29], telemetryBuf[30], telemetryBuf[31]
                );
    } // END if(type == TYPE_DATA::TELEMETRY)
} // -- END printDataRS232()

uint8_t RS232_worker::check_sum(const uint8_t *buf, const size_t buf_size)
{
    int buff_sum = 0x00;
    uint8_t res = 0;
    for(int i = 0; i < buf_size; i++)
    {
        // if(i == 15) {continue;}
        buff_sum += buf[i];
    }  // END for(int i = start_bit; i < end_bit; i++)
    res = buff_sum & 0xFF;
    return res;
} // -- END check_sum()

bool RS232_worker::check_check_sum()
{
    if(cmdBuf[15] == check_sum(cmdBuf, 15)) return true;
    return false;
} // -- END check_check_sum
