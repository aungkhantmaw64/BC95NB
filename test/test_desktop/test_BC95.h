#include <ArduinoFake.h>
#include <unity.h>
#include "mocks/MockSerial.h"
#include "BC95.h"

using namespace fakeit;

namespace BC95Test
{
    void test_BC95_SetsResetPinToOutputAndLowAfterBegin(void)
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

    void test_BC95_AppendsCarriageReturnOnATCommand(void)
    {
        Stream *stream = ArduinoFakeMock(Stream);
        const int reset_pin = 23;
        BC95 modem(stream, reset_pin);

        MockSerial mock_serial;
        mock_serial.begin();

        modem.sendCMD("OK");

        TEST_ASSERT_EQUAL_STRING("OK\r", mock_serial.getTxBuffer().c_str());
    }
    void run_tests(void)
    {
        RUN_TEST(test_BC95_SetsResetPinToOutputAndLowAfterBegin);
        RUN_TEST(test_BC95_AppendsCarriageReturnOnATCommand);
    }
}