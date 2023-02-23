#ifndef D_BC95_H
#define D_BC95_H

#include <Arduino.h>

enum
{
    CommandSucess,
    InvalidParameters,
    TimeoutError,
    UrcEvent,
    UeError,
    Unknown,
};

class BC95
{
public:
    BC95(Stream *stream, const int resetPin);
    ~BC95();
    void begin();
    void send(const char *cmd);
    void send(const String &cmd);
    int waitForResponse(unsigned long timeout_ms, String &buffer);

    //==================Get Modem's Information=====================//
    bool isReady();
    int hardReset(bool inverted);
    int reset();
    String getManufacturerRevision();
    String getIMEI();
    String getICCID();
    String getIMSI();
    String getRSSI();

private:
    Stream *_stream;
    int _resetPin;
    String _lastCmd;
    String _responseStorage;

    String extractCode(const char *prefix, int codeLength);
};

#endif