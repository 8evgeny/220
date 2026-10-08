#ifndef ICOMMANDHANDLER_H
#define ICOMMANDHANDLER_H

class ICommandHandler
{
public:
    virtual ~ICommandHandler() {}
    virtual void handle(const std::vector<uint8_t> & cmd_data) = 0;
};

#endif // ICOMMANDHANDLER_H
