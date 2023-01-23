#include <ArduinoFake.h>
#include <unity.h>
#include "BC95.h"

using namespace fakeit;

void setUp()
{
}

void tearDown()
{
    ArduinoFakeReset();
}

void test_SetsResetPinToOutputAndLowAfterBegin(void)
{
    Stream *stream = ArduinoFakeMock(Stream);
    const int reset_pin = 23;
    BC95 modem(stream, reset_pin);
    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

    modem.begin();

    Verify(Method(ArduinoFake(), pinMode).Using(reset_pin, OUTPUT)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(reset_pin, LOW)).Once();
}

void test_AppendsCarriageReturnOnATCommand(void)
{
    Stream *stream = ArduinoFakeMock(Stream);
    const int reset_pin = 23;
    BC95 modem(stream, reset_pin);

    modem.sendCMD("OK");
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_SetsResetPinToOutputAndLowAfterBegin);
    UNITY_END();
}