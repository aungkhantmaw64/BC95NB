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
    void prvEnableMocks(void)
    {
        mockSerial->begin();
        mockClock->begin();
    }
    void prvExpectResponse(const char *expected)
    {
        prvEnableMocks();
        mockSerial->setRxBuffer(expected);
    }

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
        mockClock->begin();

        driverUnderTest->send("OK");

        TEST_ASSERT_EQUAL_STRING("OK\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_SendsStringTypeATCommand(void)
    {
        mockSerial->begin();
        mockClock->begin();
        String theATcmd = "AT";

        driverUnderTest->send(theATcmd);

        TEST_ASSERT_EQUAL_STRING("AT\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_SendsCmdOnlyAfterWaitingForTwentyMilliseconds()
    {
        mockSerial->begin();
        mockClock->begin();
        driverUnderTest->send("AT");

        Verify(Method(ArduinoFake(), millis)).AtLeast(2);
    }

    void test_BC95_FlushChannelBeforeSendingANewCommmand(void)
    {
        mockSerial->begin();
        mockClock->begin();
        driverUnderTest->send("AT");

        Verify(Method(ArduinoFake(Stream), flush)).Once();
    }

    void test_BC95_ReceivesValidResponse(void)
    {
        const char *expected = "\r\nAT\r\n\r\nOK\r\n";
        prvEnableMocks();

        String buffer;
        driverUnderTest->send("AT");

        mockSerial->setRxBuffer(expected);

        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL(CommandSucess, status);
    }

    void test_BC95_ReceivesTimeOutError()
    {
        const char expected[] = "";
        prvExpectResponse(expected);

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL(TimeoutError, status);
    }

    void test_BC95_ReceivesInvalidCmdError()
    {
        const char expected[] = "\r\nAT+INVALID\n\nERROR\r\n";
        prvExpectResponse(expected);

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL(InvalidCmdError, status);
    }

    void test_BC95_ReceivesUnknownError()
    {
        const char expected[] = "234resfdsdfsfew";
        prvExpectResponse(expected);

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL(Unknown, status);
    }

    void test_BC95_StripsAndRemovesCommandEchoFromValidResponse(void)
    {
        const char original[] = "\r\nAT\n\nOK\r\n";
        prvExpectResponse(original);

        String buffer;
        buffer.reserve(200);
        driverUnderTest->send("AT");
        int status = driverUnderTest->waitForResponse(timeout_ms, buffer);

        TEST_ASSERT_EQUAL(CommandSucess, status);
        TEST_ASSERT_EQUAL_STRING("OK", buffer.c_str());
    }

    void test_BC95_IsReadyAndNoHardwareIssue(void)
    {
        const char expected[] = "\r\nAT\n\nOK\r\n";
        prvExpectResponse(expected);

        bool ready = driverUnderTest->isReady();

        TEST_ASSERT_TRUE(ready);
        Verify(Method(ArduinoFake(Stream), flush));
        TEST_ASSERT_EQUAL_STRING("AT\r", mockSerial->getTxBuffer().c_str());
    }

    void run_tests(void)
    {
        RUN_TEST(test_BC95_SetsResetPinToOutputAndLowAfterBegin);
        RUN_TEST(test_BC95_AppendsCarriageReturnOnATCommand);
        RUN_TEST(test_BC95_SendsStringTypeATCommand);
        RUN_TEST(test_BC95_SendsCmdOnlyAfterWaitingForTwentyMilliseconds);
        RUN_TEST(test_BC95_FlushChannelBeforeSendingANewCommmand);
        RUN_TEST(test_BC95_ReceivesValidResponse);
        RUN_TEST(test_BC95_ReceivesTimeOutError);
        RUN_TEST(test_BC95_ReceivesInvalidCmdError);
        RUN_TEST(test_BC95_ReceivesUnknownError);
        RUN_TEST(test_BC95_StripsAndRemovesCommandEchoFromValidResponse);
        RUN_TEST(test_BC95_IsReadyAndNoHardwareIssue);
    }
}