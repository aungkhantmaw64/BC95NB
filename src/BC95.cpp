#include "BC95.h"
#include <cstring>
#include <errno.h>

#define DEFAULT_REBOOT_TIME_MS 3000

BC95::BC95(Stream *stream, const int resetPin)
    : _stream(stream),
      _resetPin(resetPin)
{
    _lastCmd.reserve(100);
    _lastResp.reserve(100);
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

static int checkResponseStatus(String *buffer)
{
    if (buffer->length() == 0)
        return MODEM_STATUS_TIMEOUT_ERROR;
    else if (buffer->lastIndexOf("+CME ERROR:") != -1)
        return MODEM_STATUS_UE_ERROR;
    else if (buffer->lastIndexOf("ERROR") != -1)
        return MODEM_STATUS_INVALID_PARAMETERS;
    else if (buffer->lastIndexOf("OK") != -1)
        return MODEM_STATUS_VALID_RESPONSE;
    else if (buffer->endsWith("\r\n"))
        return MODEM_STATUS_URC_EVENT;
    else
        return MODEM_STATUS_UNKNOWN;
}

int BC95::waitForResponse(unsigned long timeout_ms, String *buffer)
{
    int status = MODEM_STATUS_UNKNOWN;
    _lastResp = "";
    unsigned long startTime = millis();
    while ((millis() - startTime) < timeout_ms)
    {
        while (_stream->available() > 0)
        {
            char byte = _stream->read();
            _lastResp += byte;
        }
    }
    status = checkResponseStatus(&_lastResp);
    if (_lastResp.startsWith(_lastCmd, _lastCmd.length()))
    {
        _lastResp.replace(_lastCmd, "");
        _lastResp.trim();
    }
    if (buffer)
    {
        *buffer = _lastResp;
    }
    return status;
}

bool BC95::isReady(void)
{
    send("AT");
    return (waitForResponse(300) == MODEM_STATUS_VALID_RESPONSE);
}

String BC95::extractCode(const char *prefix, int codeSize)
{
    String code;
    code.reserve(codeSize);
    int status = waitForResponse(300, &code);
    if (status == MODEM_STATUS_VALID_RESPONSE)
    {
        int pos = code.indexOf(prefix) + 1;
        code = code.substring(pos, pos + codeSize);
    }
    return code;
}

String BC95::getIMEI(void)
{
    send("AT+CGSN=1");
    return extractCode("+CGSN:", 15);
}

String BC95::getICCID(void)
{
    send("AT+NCCID");
    return extractCode("+NCCID:", 20);
}

String BC95::getManufacturerRevision()
{
    return "";
}

String BC95::getIMSI()
{
    return "";
}

String BC95::getRSSI()
{
    return "";
}

int BC95::reset(void)
{
    unsigned long startTime = millis();
    send("AT+NRB");
    String buffer;
    waitForResponse(1000, &buffer);
    if (buffer.indexOf("REBOOT") != -1)
        return 0;
    else
        return -EAGAIN;
}