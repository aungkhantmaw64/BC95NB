#include "BC95.h"
#include <cstring>
#include <errno.h>

#define DEFAULT_REBOOT_TIME_MS 3000

BC95::BC95(Stream *stream, const int resetPin)
    : _stream(stream),
      _resetPin(resetPin),
      state_(AT_STATE_READING)
{
    _lastCmd.reserve(100);
    response_.reserve(100);
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

int BC95::waitForResponse(unsigned long timeout_ms, String *buffer)
{
    response_ = "";
    unsigned long startTime = millis();
    while (millis() - startTime < timeout_ms)
    {
        switch (state_)
        {
        case AT_STATE_READING:
        {
            if (_stream->available() > 0)
            {
                char byte_ = (char)_stream->read();
                response_ += byte_;
                if (byte_ == '\n')
                {
                    state_ = AT_STATE_IDENTIFYING_RESPONSE;
                }
            }
            break;
        }
        case AT_STATE_IDENTIFYING_RESPONSE:
        {
            if (!response_.startsWith("\r\nAT") && response_.endsWith("\r\n"))
            {
                state_ = AT_STATE_HANDLING_URC;
            }
            else
            {
                state_ = AT_STATE_READING;
            }
            break;
        }
        case AT_STATE_HANDLING_URC:
        {
            /* code for urc handling*/
            state_ = AT_STATE_READING;
            break;
        }
        default:
            break;
        }
    }
    if (response_.length() == 0)
        return MODEM_STATUS_TIMEOUT_ERROR;
    if (buffer)
    {
        *buffer = response_;
        buffer->replace(_lastCmd, "");
    }
    if (response_.indexOf("OK") != -1)
    {
        return MODEM_STATUS_VALID_RESPONSE;
    }
    if (response_.indexOf("ERROR") != -1)
    {
        return MODEM_STATUS_INVALID_PARAMETERS;
    }
    return MODEM_STATUS_UNKNOWN;
}

bool BC95::isReady(void)
{
    send("AT");
    return (waitForResponse(300) == MODEM_STATUS_VALID_RESPONSE);
}

int BC95::reset()
{

    return -1;
}
