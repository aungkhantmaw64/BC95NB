#include <ArduinoFake.h>
#include <unity.h>
#include "mocks/MockSerial.h"
#include "mocks/MockClock.h"
#include "BC95.h"
#include <errno.h>

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
    void _enableMocks(void)
    {
        mockSerial->begin();
        mockClock->begin();
    }
    void _expectResponse(const char *expected)
    {
        mockSerial->setRxContent(expected);
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
        _enableMocks();

        driverUnderTest->send("OK");

        TEST_ASSERT_EQUAL_STRING("OK\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_SendsStringTypeATCommand(void)
    {
        _enableMocks();
        String theATcmd = "AT";

        driverUnderTest->send(theATcmd);

        TEST_ASSERT_EQUAL_STRING("AT\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_SendsCmdOnlyAfterWaitingForTwentyMilliseconds()
    {
        _enableMocks();

        driverUnderTest->send("AT");

        Verify(Method(ArduinoFake(), millis)).AtLeast(2);
    }

    void test_BC95_FlushChannelBeforeSendingANewCommmand(void)
    {
        _enableMocks();

        driverUnderTest->send("AT");

        Verify(Method(ArduinoFake(Stream), flush)).Once();
    }

    void test_BC95_ReceivesValidResponse(void)
    {
        _enableMocks();
        _expectResponse("\r\nAT\r\n\r\nOK\r\n");

        String buffer;
        driverUnderTest->send("AT");
        int status = driverUnderTest->waitForResponse(timeout_ms, &buffer);

        TEST_ASSERT_EQUAL(MODEM_STATUS_VALID_RESPONSE, status);
    }

    void test_BC95_ReceivesUeError()
    {
        _enableMocks();
        _expectResponse("\r\n+CME ERROR:23\r\n");

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, &buffer);

        TEST_ASSERT_EQUAL(MODEM_STATUS_UE_ERROR, status);
    }

    void test_BC95_ReceivesTimeOutError()
    {
        _enableMocks();
        _expectResponse("");

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, &buffer);

        TEST_ASSERT_EQUAL(MODEM_STATUS_TIMEOUT_ERROR, status);
    }

    void test_BC95_ReceivesInvalidCmdError()
    {
        _enableMocks();
        _expectResponse("\r\nAT+INVALID\n\nERROR\r\n");

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, &buffer);

        TEST_ASSERT_EQUAL(MODEM_STATUS_INVALID_PARAMETERS, status);
    }

    void test_BC95_ReceivesUnknownError()
    {
        _enableMocks();
        _expectResponse("234resfdsdfsfew");

        String buffer;
        int status = driverUnderTest->waitForResponse(timeout_ms, &buffer);

        TEST_ASSERT_EQUAL(MODEM_STATUS_UNKNOWN, status);
    }

    void test_BC95_StripsAndRemovesCommandEchoFromValidResponse(void)
    {
        _enableMocks();
        _expectResponse("\r\nAT\n\nOK\r\n");

        String buffer;
        buffer.reserve(200);
        driverUnderTest->send("AT");
        int status = driverUnderTest->waitForResponse(timeout_ms, &buffer);

        TEST_ASSERT_EQUAL(MODEM_STATUS_VALID_RESPONSE, status);
        TEST_ASSERT_EQUAL_STRING("OK", buffer.c_str());
    }

    void test_BC95_IsReadyAndNoHardwareIssue(void)
    {
        _enableMocks();
        _expectResponse("\r\nAT\n\nOK\r\n");

        bool ready = driverUnderTest->isReady();

        TEST_ASSERT_TRUE(ready);
        Verify(Method(ArduinoFake(Stream), flush));
        TEST_ASSERT_EQUAL_STRING("AT\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_IsNotReadyWhenReceiveNoResponse(void)
    {
        _enableMocks();
        _expectResponse("");

        bool ready = driverUnderTest->isReady();

        TEST_ASSERT_TRUE(!ready);
        Verify(Method(ArduinoFake(Stream), flush));
        TEST_ASSERT_EQUAL_STRING("AT\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_RebootDueToSoftwareReset(void)
    {
        _enableMocks();
        _expectResponse("\r\nREBOOTING\r\n");

        int errCode = driverUnderTest->reset();

        TEST_ASSERT_EQUAL(0, errCode);
        Verify(Method(ArduinoFake(), millis)).AtLeastOnce();
        TEST_ASSERT_EQUAL_STRING("AT+NRB\r", mockSerial->getTxBuffer().c_str());
    }

    void test_BC95_RebootFailsDuringSoftwareReset(void)
    {
        _enableMocks();
        _expectResponse("");

        int errCode = driverUnderTest->reset();

        TEST_ASSERT_EQUAL(-1, errCode);
        Verify(Method(ArduinoFake(), millis)).AtLeastOnce();
        TEST_ASSERT_EQUAL_STRING("AT+NRB\r", mockSerial->getTxBuffer().c_str());
    }

        void run_tests(void)
    {
        RUN_TEST(test_BC95_SetsResetPinToOutputAndLowAfterBegin);
        RUN_TEST(test_BC95_AppendsCarriageReturnOnATCommand);
        RUN_TEST(test_BC95_SendsStringTypeATCommand);
        RUN_TEST(test_BC95_SendsCmdOnlyAfterWaitingForTwentyMilliseconds);
        RUN_TEST(test_BC95_FlushChannelBeforeSendingANewCommmand);
        RUN_TEST(test_BC95_ReceivesValidResponse);
        RUN_TEST(test_BC95_ReceivesUeError);
        RUN_TEST(test_BC95_ReceivesTimeOutError);
        RUN_TEST(test_BC95_ReceivesInvalidCmdError);
        RUN_TEST(test_BC95_ReceivesUnknownError);
        RUN_TEST(test_BC95_StripsAndRemovesCommandEchoFromValidResponse);
        RUN_TEST(test_BC95_IsReadyAndNoHardwareIssue);
        RUN_TEST(test_BC95_IsNotReadyWhenReceiveNoResponse);
        RUN_TEST(test_BC95_RebootDueToSoftwareReset);
        RUN_TEST(test_BC95_RebootFailsDuringSoftwareReset);
    }
}