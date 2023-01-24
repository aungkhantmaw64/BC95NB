#include "BC95.h"
#include <cstring>

BC95::BC95(Stream *stream, const int resetPin)
    : _stream(stream),
      _resetPin(resetPin)
{
    _lastCmd.reserve(100);
}

BC95::~BC95()
{
    _lastCmd = "";
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
    while ((millis() - startTime) < timeout_ms)
    {
        while (_stream->available() > 0)
        {
            char byte = _stream->read();
            buffer += byte;
        }
    }
    status = checkResponseStatus(buffer);
    switch (status)
    {
    case UrcEvent:
        /* Write code for handling unsolicited result code */
        break;
    case CommandSucess:
    {
        buffer.replace(_lastCmd, "");
        buffer.trim();
        buffer.replace("\n", "");
    }
    break;
    default:
        break;
    }
    return status;
}

bool BC95::isReady(void)
{
    String buffer;
    send("AT");
    return (waitForResponse(300, buffer) == CommandSucess);
}