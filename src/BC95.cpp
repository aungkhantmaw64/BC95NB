#include "BC95.h"
#include <cstring>

BC95::BC95(Stream *stream, const int resetPin)
    : _stream(stream),
      _resetPin(resetPin)
{
    _lastCmd.reserve(100);
    _responseBuffer.reserve(100);
}

BC95::~BC95()
{
    _lastCmd = "";
    _responseBuffer = "";
}

void BC95::begin(void)
{
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, LOW);
}

void BC95::send(const char *cmd)
{
    unsigned long startTime = millis();
    while ((millis() - startTime) < 20)
        ;
    _stream->flush();
    _stream->print(cmd);
    _stream->print("\r");
    _lastCmd = cmd;
}

void BC95::send(const String &cmd)
{
    this->send(cmd.c_str());
}

static int checkResponseStatus(String &buffer)
{
    if (buffer.length() == 0)
        return TimeoutError;
    else if (buffer.lastIndexOf("ERROR") != -1)
        return InvalidCmdError;
    else if (buffer.lastIndexOf("OK") != -1)
        return CommandSucess;
    else if (buffer.endsWith("\r\n"))
        return UrcEvent;
    else
        return Unknown;
}

int BC95::waitForResponse(unsigned long timeout_ms, String &buffer)
{
    int status = Unknown;
    unsigned long startTime = millis();
    _responseBuffer = "";
    while ((millis() - startTime) < timeout_ms)
    {
        while (_stream->available() > 0)
        {
            char byte = _stream->read();
            _responseBuffer += byte;
        }
    }
    status = checkResponseStatus(_responseBuffer);
    switch (status)
    {
    case UrcEvent:
        /* Write code for handling unsolicited result code */
        break;
    case CommandSucess:
    {
        // _responseBuffer.trim();
        // _responseBuffer.replace(_lastCmd, "");
        // _responseBuffer.replace("\n", "");
        buffer = _responseBuffer;
        _responseBuffer = "";
    }
    break;
    default:
        break;
    }
    return status;
}
