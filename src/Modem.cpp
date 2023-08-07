#include "Modem.h"

Modem::Modem(Stream &_stream, uint8_t _resetPin, bool _activeHigh)
    : m_stream(_stream),
      m_resetPin(_resetPin),
      m_activeHigh(_activeHigh)
{
}

void Modem::reset()
{
    digitalWrite(m_resetPin, m_activeHigh);
    delay(200);
    digitalWrite(m_resetPin, !m_activeHigh);
    delay(200);
};

void Modem::send(const char *_cmd)
{
    m_stream.print(_cmd);
}

ResponseCode Modem::waitForResponse(uint32_t _timeoutMs)
{
    String response;
    ResponseCode code = ResponseCode::EMPTY;
    uint32_t startMillis = millis();
    while (millis() - startMillis < _timeoutMs)
    {
        if (m_stream.available())
        {
            char ch = m_stream.read();
            response += ch;
            if (ch == '\n')
            {
                if (response.indexOf("OK") != -1)
                    code = ResponseCode::OK;
                else if (response.indexOf("+CME") != -1)
                    code = ResponseCode::CME_ERROR;
                else if (response.indexOf("ERROR") != -1)
                    code = ResponseCode::ERROR;
                else
                {
                    for (auto i{0}; i < MODEM_MAX_RESPONSE_HANDLERS; i++)
                    {
                        if (m_respHandlers[i] != nullptr)
                        {
                            response.replace("\r", "");
                            response.replace("\n", "");
                            response.trim();
                            if (response.length())
                                m_respHandlers[i]->onReceive(response);
                        }
                    }
                }
                response = "";
            }
        }
    }
    return code;
}

int Modem::addResponseHandler(ModemResponseHandler *_handler)
{
    for (auto i{0}; i < MODEM_MAX_RESPONSE_HANDLERS; i++)
    {
        if (m_respHandlers[i] == nullptr)
        {
            m_respHandlers[i] = _handler;
            return 0;
        }
    }
    return -1;
}