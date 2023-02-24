#ifndef D_MOCK_SERIAL_H
#define D_MOCK_SERIAL_H

#include <ArduinoFake.h>
#include <cstring>
#include <map>

using namespace fakeit;

class MockSerial
{
public:
    MockSerial() : _singleMode(true)
    {

        _txBuffer.reserve(100);
        _rxBuffer.reserve(100);
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
                      { return _mockRead(); });
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
    String getRxContent(void)
    {
        return _rxBuffer;
    }
    void setRxContent(const char *str)
    {
        _rxBuffer = String(str);
    }

    char _mockRead()
    {
        if (_rxBuffer.length() > 0)
        {
            char byte = _rxBuffer.charAt(0);
            _rxBuffer.remove(0, 1);
            return byte;
        }
        return -1;
    }

private:
    String _txBuffer;
    String _rxBuffer;
    std::map<String, String> _cmdDict;
    bool _singleMode;
};

#endif