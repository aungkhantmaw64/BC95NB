#ifndef D_MOCK_SERIAL_H
#define D_MOCK_SERIAL_H

#include <ArduinoFake.h>
#include <cstring>
#include <map>

using namespace fakeit;

class MockSerial
{
public:
    MockSerial();
    ~MockSerial();
    void begin(void);
    String getTxBuffer(void);
    String getRxContent(void);
    void cleanTxBuffer(void);
    void setRxContent(const char *str);

private:
    String _txBuffer;
    String _rxBuffer;
    std::map<String, String> _cmdDict;

    char _mockRead(void);
};

MockSerial ::MockSerial()
{
    _txBuffer.reserve(100);
    _rxBuffer.reserve(100);
    _txBuffer = "";
}

MockSerial ::~MockSerial()
{
    _txBuffer = "";
    _rxBuffer = "";
}

void MockSerial::begin()
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

String MockSerial::getTxBuffer(void)
{
    return _txBuffer;
}
void MockSerial::cleanTxBuffer(void)
{
    _txBuffer = "";
}

String MockSerial::getRxContent(void)
{
    return _rxBuffer;
}

void MockSerial::setRxContent(const char *str)
{
    _rxBuffer = String(str);
}

char MockSerial::_mockRead()
{
    if (_rxBuffer.length() > 0)
    {
        char byte = _rxBuffer.charAt(0);
        _rxBuffer.remove(0, 1);
        return byte;
    }
    return -1;
}
#endif