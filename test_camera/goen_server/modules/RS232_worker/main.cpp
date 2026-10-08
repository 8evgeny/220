#include <RS232_worker.hpp>

using namespace std;

int main(int argc, char *argv[])
{
    unique_ptr<RS232_worker> p = make_unique<RS232_worker>();

    p->serial.setPortName(p->getPortName());
    p->serial.setBaudRate(p->getSpeed());

    if (p->getPortName() != "")
    {
        p->serial.open(QIODevice::ReadOnly);
        cout << "Port name: " << p->serial.portName().toStdString() << endl;
        p->parsing();
    }
    else
    {
        cout << "\nNo port found !!!\n\n";
    }
return 0;
}// END while(1)



