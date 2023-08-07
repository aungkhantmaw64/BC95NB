#ifndef BC95NB_SRC_MODEM_H
#define BC95NB_SRC_MODEM_H

#include <Arduino.h>

#define MODEM_CMD_MAX_SIZE 100
#define MODEM_MAX_RESPONSE_HANDLERS 4

enum ResponseCode
{
    EMPTY,
    OK,
    ERROR,
    CME_ERROR
};

class ModemResponseHandler
{
public:
    virtual void onReceive(String _response) = 0;
};
class Modem
{
public:
    Modem(Stream &_stream, uint8_t _resetPin, bool _activeHigh);
    void reset();
    void send(const char *_cmd);
    ResponseCode waitForResponse(uint32_t _timeoutMs);
    int addResponseHandler(ModemResponseHandler *_handler);

private:
    Stream &m_stream;
    uint8_t m_resetPin;
    bool m_activeHigh;
    ModemResponseHandler *m_respHandlers[MODEM_MAX_RESPONSE_HANDLERS] = {nullptr};
};

#endif
