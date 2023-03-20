#ifndef D_MOCK_CLOCK_H
#define D_MOCK_CLOCK_H

#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

class MockClock
{
public:
    MockClock(unsigned long initTime, unsigned long increment)
        : _initTime(initTime), _increment(increment)
    {
    }
    ~MockClock()
    {
        _initTime = 0;
        _increment = 0;
    }
    void begin(void)
    {
        When(Method(ArduinoFake(), millis))
            .AlwaysDo([this]() -> unsigned long
                      {
                unsigned long time = _initTime;
                _initTime += _increment;
                return time; });
    }

private:
    unsigned long _initTime;
    unsigned long _increment;
};

#endif