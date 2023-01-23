#ifndef D_BC95_H
#define D_BC95_H

#include <Arduino.h>

class BC95
{
private:
    Stream *_stream;
    int _resetPin;

public:
    BC95(Stream *stream, const int resetPin);
    void begin();
    void sendCMD(const char *cmd);
    ~BC95();
};

#endif