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
    void sendf(const char *fmt, ...) override;
    int waitForResponse(unsigned long timeout_ms, String *buffer = NULL) override;

    bool isReady();
    int hardReset(bool inverted);
    int reset();

private:
    Stream *_stream;
    int _resetPin;
    int state_;
    String _lastCmd;
    String response_;
};

#endif