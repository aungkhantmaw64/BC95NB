#ifndef D_MODEM_H
#define D_MODEM_H

#include <Arduino.h>

enum
{
    MODEM_STATUS_VALID_RESPONSE,
    MODEM_STATUS_INVALID_PARAMETERS,
    MODEM_STATUS_TIMEOUT_ERROR,
    MODEM_STATUS_REBOOTED,
    MODEM_STATUS_URC_EVENT,
    MODEM_STATUS_UE_ERROR,
    MODEM_STATUS_UNKNOWN,
};

class Modem
{
public:
    virtual void begin() = 0;
    virtual void send(const char *cmd) = 0;
    virtual void send(const String &cmd) = 0;
    virtual int waitForResponse(unsigned long timeout_ms, String *buffer) = 0;

    //==================Get Modem's Information=====================//
    virtual bool isReady() = 0;
    virtual String getManufacturerRevision() = 0;
    virtual String getIMEI() = 0;
    virtual String getICCID() = 0;
    virtual String getIMSI() = 0;
    virtual String getRSSI() = 0;
};

#endif