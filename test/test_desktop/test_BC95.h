#include <ArduinoFake.h>
#include <unity.h>
#include "mocks/MockSerial.h"
#include "mocks/MockClock.h"
#include "BC95.h"

using namespace fakeit;

#define RESPONSE_BUFFER_SIZE 100

extern BC95 *driverUnderTest;
extern MockSerial *mockSerial;
extern MockClock *mockClock;
extern const int reset_pin;
extern const unsigned long timeout_ms;
extern const unsigned long initTime;
extern const unsigned long timeStep;
namespace BC95Test
{
    void test_BC95_SetsResetPinToOutputAndLowAfterBegin(void)
    {
        When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
        When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
        Stream *stream = ArduinoFakeMock(Stream);

        driverUnderTest->begin();

        Verify(Method(ArduinoFake(), pinMode).Using(reset_pin, OUTPUT)).Once();
        Verify(Method(ArduinoFake(), digitalWrite).Using(reset_pin, LOW)).Once();
    }

    void test_BC95_AppendsCarriageReturnOnATCommand(void)
    {
        mockSerial->begin();

        driverUnderTest->sendCMD("OK");

        TEST_ASSERT_EQUAL_STRING("OK\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_WaitForModemResponseAndReceiveImmediately()
    {
        const char expected[] = "OK";
        mockSerial->begin();
        mockClock->begin();
        mockSerial->setRxBuffer(expected);

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL_STRING(expected, buffer.c_str());
    }

    void test_BC95_WaitForValidResponseThatContainsOk(void)
    {
        const char expected[] = "\r\nAT\r\n\r\nOK\r\n";
        mockSerial->begin();
        mockClock->begin();
        mockSerial->setRxBuffer(expected);

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL_STRING(expected, buffer.c_str());
        TEST_ASSERT_EQUAL(CommandSucess, status);
    }

    void test_BC95_WaitForModemResponseButTimeOutOccurs()
    {
        const char expected[] = "";
        mockSerial->begin();
        mockClock->begin();
        mockSerial->setRxBuffer(expected);

        String buffer;

        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL_STRING(expected, buffer.c_str());
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