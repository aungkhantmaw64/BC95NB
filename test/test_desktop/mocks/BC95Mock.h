#include <BC95.h>

class BC95Mock : public Modem
{
public:
    BC95Mock();
    ~BC95Mock();
    void begin() override;
    void send(const char *cmd) override;
    void send(const String &cmd) override;
    int waitForResponse(unsigned long timeout_ms, String *buffer = NULL) override;
    //==================Get Modem's Information=====================//
    bool isReady() override;
    String getManufacturerRevision() override;
    String getIMEI() override;
    String getICCID() override;
    String getIMSI() override;
    String getRSSI() override;

    String getLastCmd(void);
    void setResponseStatus(int status);

private:
    String _lastCmd;
    int _respStatus;
};

BC95Mock::BC95Mock()
    : _lastCmd(""),
      _respStatus(MODEM_STATUS_UNKNOWN)
{
}

BC95Mock::~BC95Mock()
{
}

void BC95Mock::begin()
{
}

void BC95Mock::send(const char *cmd)
{
    _lastCmd = String(cmd) + "\r";
}
void BC95Mock::send(const String &cmd)
{
}
int BC95Mock::waitForResponse(unsigned long timeout_ms, String *buffer)
{
    return _respStatus;
}
bool BC95Mock::isReady()
{
    return false;
}
String BC95Mock::getManufacturerRevision()
{
    return "";
}
String BC95Mock::getIMEI()
{
    return "";
}
String BC95Mock::getICCID()
{
    return "";
};
String BC95Mock::getIMSI()
{
    return "";
};
String BC95Mock::getRSSI()
{
    return "";
}
String BC95Mock::getLastCmd(void)
{
    return _lastCmd;
}
void BC95Mock::setResponseStatus(int status)
{
    _respStatus = status;
}