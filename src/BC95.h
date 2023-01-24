#ifndef D_BC95_H
#define D_BC95_H

#include <Arduino.h>

enum
{
    CommandSucess,
    TimeoutError,
    UnknownError,
    InvalidCmdError
};

class BC95
{
private:
    Stream *_stream;
    int _resetPin;

public:
    BC95(Stream *stream, const int resetPin);
    void begin();
    void send(const char *cmd);
    void send(const String &cmd);
    int waitForResponse(unsigned long timeout_ms, String &buffer);
    ~BC95();
};

#endif