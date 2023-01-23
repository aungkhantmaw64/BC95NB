#include <ArduinoFake.h>
#include <unity.h>
#include "mocks/MockSerial.h"
#include "mocks/MockClock.h"
#include "BC95.h"

using namespace fakeit;

#define RESPONSE_BUFFER_SIZE 100
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

    void test_BC95_WaitForModemResponseAndReceiveImmediately()
    {
        Stream *stream = ArduinoFakeMock(Stream);
        MockSerial mock_serial;
        mock_serial.begin();
        mock_serial.setRxBuffer("OK");
        MockClock mock_clock(0, 1);
        mock_clock.begin();

        const int reset_pin = 23;
        const unsigned long timeout_ms = 300;
        String buffer;
        BC95 modem(stream, reset_pin);
        int status = modem.waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL_STRING("OK", buffer.c_str());
    }

    void test_BC95_WaitForValidResponseThatContainsOk(void)
    {
        Stream *stream = ArduinoFakeMock(Stream);
        MockSerial mock_serial;
        MockClock mock_clock(0, 1);
        mock_serial.begin();
        mock_clock.begin();
        mock_serial.setRxBuffer("\r\nAT\r\n\r\nOK\r\n");

        const int reset_pin = 23;
        const unsigned long timeout_ms = 300;
        String buffer;
        BC95 modem(stream, reset_pin);
        int status = modem.waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL_STRING("\r\nAT\r\n\r\nOK\r\n", buffer.c_str());
        TEST_ASSERT_EQUAL(CommandSucess, status);
    }

    void test_BC95_WaitForModemResponseButTimeOutOccurs()
    {
        Stream *stream = ArduinoFakeMock(Stream);
        MockSerial mock_serial;
        MockClock mock_clock(0, 1);
        mock_serial.begin();
        mock_clock.begin();
        mock_serial.setRxBuffer("");

        const int reset_pin = 23;
        const unsigned long timeout_ms = 300;
        String buffer;
        BC95 modem(stream, reset_pin);
        int status = modem.waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL_STRING("", buffer.c_str());
        TEST_ASSERT_EQUAL(TimeoutError, status);
    }

    void run_tests(void)
    {
        RUN_TEST(test_BC95_SetsResetPinToOutputAndLowAfterBegin);
        RUN_TEST(test_BC95_AppendsCarriageReturnOnATCommand);
        RUN_TEST(test_BC95_WaitForModemResponseAndReceiveImmediately);
        RUN_TEST(test_BC95_WaitForValidResponseThatContainsOk);
        RUN_TEST(test_BC95_WaitForModemResponseButTimeOutOccurs);
    }
}