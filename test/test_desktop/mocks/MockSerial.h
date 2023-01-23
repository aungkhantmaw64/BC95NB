#ifndef D_MOCK_SERIAL_H
#define D_MOCK_SERIAL_H

#include <ArduinoFake.h>
#include <cstring>

using namespace fakeit;

class MockSerial
{
public:
    MockSerial()
    {
        _txBuffer = "";
    }
    ~MockSerial()
    {
        _txBuffer = "";
    }
    void begin(void)
    {
        When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *)))
            .AlwaysDo([this](const char *str) -> int
                      { 
                      _txBuffer += String(str) ;
                      return strlen(str); });
    }
    String getTxBuffer(void)
    {
        return _txBuffer;
    }
    void cleanTxBuffer(void)
    {
        _txBuffer = "";
    }
    String getRxBuffer(void)
    {
        return _rxBuffer;
    }
    void setRxBuffer(const char *str)
    {
        _rxBuffer = String(str);
    }

private:
    String _txBuffer;
    String _rxBuffer;
};

#endif