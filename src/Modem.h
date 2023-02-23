#ifndef D_MODEM_H
#define D_MODEM_H

#include <Arduino.h>

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