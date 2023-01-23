#include "BC95.h"
#include <cstring>

BC95::BC95(Stream *stream, const int resetPin)
    : _stream(stream), _resetPin(resetPin)
{
}

BC95::~BC95()
{
}

void BC95::begin(void)
{
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, LOW);
}

void BC95::sendCMD(const char *cmd)
{
    _stream->print(cmd);
    _stream->print("\r");
}

int BC95::waitForResponse(unsigned long timeout_ms, String &buffer)
{
    while (_stream->available() > 0)
    {
        char byte = _stream->read();
        buffer += byte;
    }
}