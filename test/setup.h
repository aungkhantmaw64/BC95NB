#ifndef BC95NB_TEST_SETUP_H
#define BC95NB_TEST_SETUP_H

#include <ArduinoFake.h>
#include "Modem.h"
#include "StdModem.h"
#include "BC95NB.h"

#include <vector>

#define DEFAULT_RESET_PIN 14;

using namespace fakeit;

class ModemBuilder
{
public:
    Modem *buildModem()
    {
        return new Modem(*m_stream, m_resetPin, m_activeHigh);
    }
    ModemBuilder *setResetPin(uint8_t _resetPin)
    {
        m_resetPin = _resetPin;
        return this;
    }
    ModemBuilder *setActiveHigh(bool _activeHigh)
    {
        m_activeHigh = _activeHigh;
        return this;
    }

private:
    Stream *m_stream = ArduinoFakeMock(Stream);
    uint8_t m_resetPin = DEFAULT_RESET_PIN;
    bool m_activeHigh = HIGH;
};

class TestSupport
{
public:
    const uint8_t RESET_PIN = DEFAULT_RESET_PIN;
    void setClock(uint32_t _startTime, uint32_t _timeStep)
    {
        m_startTime = _startTime;
        m_timeStep = _timeStep;
    }
    uint32_t getTicks(void)
    {
        unsigned long currentTime = m_startTime;
        m_startTime += m_timeStep;
        return currentTime;
    }
    void putRxBuffer(const char *_str)
    {
        m_rxBuffer = String(_str);
    }
    String getRxBuffer(void)
    {
        return m_rxBuffer;
    }
    char popRxBuffer(void)
    {
        char chr = m_rxBuffer.charAt(0);
        m_rxBuffer.remove(0, 1); // remove(0) doesn't work
        return chr;
    }
    String getTxBuffer(void)
    {
        return m_txBuffer;
    }
    void setupMocks(void)
    {
        // Digital I/O
        When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
        When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

        // Stream and serial communications
        When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *)))
            .AlwaysDo([this](const char *str) -> int
                      { m_txBuffer = String(str);
                        return strlen(str); });
        When(Method(ArduinoFake(Stream), read))
            .AlwaysDo([this]() -> char
                      { return popRxBuffer(); });
        When(Method(ArduinoFake(Stream), available))
            .AlwaysDo([this]() -> int
                      { return m_rxBuffer.length(); });

        // Time services
        When(Method(ArduinoFake(), delay)).AlwaysReturn();

        When(Method(ArduinoFake(), millis))
            .AlwaysDo([this]() -> unsigned long
                      { return getTicks(); });
    }
    void reset(void)
    {
        m_txBuffer = "";
        m_rxBuffer = "";
        m_startTime = 0;
        m_timeStep = 1;
    }

private:
    String m_txBuffer;
    String m_rxBuffer;
    uint32_t m_startTime = 0;
    uint32_t m_timeStep = 1;
};

#endif