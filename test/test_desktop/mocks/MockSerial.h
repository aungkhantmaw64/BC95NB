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
        _rxBuffer = "";
    }
    ~MockSerial()
    {
        _txBuffer = "";
        _rxBuffer = "";
    }
    void begin(void)
    {
        When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *)))
            .AlwaysDo([this](const char *str) -> int
                      { 
                      _txBuffer += String(str) ;
                      return strlen(str); });
        When(Method(ArduinoFake(Stream), read))
            .AlwaysDo([this]() -> char
                      {if(_rxBuffer.length()>0){
                            char byte = _rxBuffer.charAt(0);
                            _rxBuffer.remove(0, 1);
                            return byte;
                            }
                        return -1; });
        When(Method(ArduinoFake(Stream), available))
            .AlwaysDo([this]() -> int
                      { return _rxBuffer.length(); });
        When(Method(ArduinoFake(Stream), flush))
            .AlwaysReturn();
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