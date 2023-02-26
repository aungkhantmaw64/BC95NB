#ifndef D_BC95_H
#define D_BC95_H

#include <Arduino.h>
#include "Modem.h"

class BC95 : public Modem
{
public:
    BC95(Stream *stream, const int resetPin);
    ~BC95();
    void begin() override;
    void send(const char *cmd) override;
    void send(const String &cmd) override;
    int waitForResponse(unsigned long timeout_ms, String *buffer = NULL) override;

    //==================Get Modem's Information=====================//
    bool isReady();
    int hardReset(bool inverted);
    int reset();
    String getManufacturerRevision() override;
    String getIMEI() override;
    String getICCID() override;
    String getIMSI() override;
    String getRSSI() override;

private:
    Stream *_stream;
    int _resetPin;
    String _lastCmd;
    String _lastResp;
    String extractCode(const char *prefix, int codeLength);
};

#endif