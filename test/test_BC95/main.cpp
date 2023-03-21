#include <ArduinoFake.h>
#include <unity.h>
#include "MockSerial.h"
#include "MockClock.h"
#include "BC95.h"
#include <errno.h>

#define RESPONSE_BUFFER_SIZE 100

using namespace fakeit;

MockSerial *mockSerial = NULL;
MockClock *mockClock = NULL;
BC95 *driverUnderTest = NULL;
Stream *stream = NULL;

const int reset_pin = 23;
const unsigned long timeout_ms = 300;
const unsigned long initTime = 0;
const unsigned long timeStep = 1;

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

void test_BC95_RemovesCommandEchoFromValidResponse(void)
{
    _enableMocks();
    _expectResponse("\r\nAT\r\n\r\nOK\r\n");

    String buffer;
    buffer.reserve(200);
    driverUnderTest->send("AT");
    int status = driverUnderTest->waitForResponse(timeout_ms, &buffer);

    TEST_ASSERT_EQUAL(MODEM_STATUS_VALID_RESPONSE, status);
    TEST_ASSERT_EQUAL_STRING("\r\n\r\n\r\nOK\r\n", buffer.c_str());
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

void setUp()
{
    stream = ArduinoFakeMock(Stream);
    driverUnderTest = new BC95(stream, reset_pin);
    mockSerial = new MockSerial();
    mockClock = new MockClock(initTime, timeStep);
}

void tearDown()
{
    ArduinoFakeReset();
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_BC95_SetsResetPinToOutputAndLowAfterBegin);
    RUN_TEST(test_BC95_AppendsCarriageReturnOnATCommand);
    RUN_TEST(test_BC95_SendsStringTypeATCommand);
    RUN_TEST(test_BC95_SendsCmdOnlyAfterWaitingForTwentyMilliseconds);
    RUN_TEST(test_BC95_FlushChannelBeforeSendingANewCommmand);
    RUN_TEST(test_BC95_ReceivesValidResponse);
    RUN_TEST(test_BC95_ReceivesTimeOutError);
    RUN_TEST(test_BC95_ReceivesInvalidCmdError);
    RUN_TEST(test_BC95_ReceivesUnknownError);
    RUN_TEST(test_BC95_RemovesCommandEchoFromValidResponse);
    RUN_TEST(test_BC95_IsReadyAndNoHardwareIssue);
    RUN_TEST(test_BC95_IsNotReadyWhenReceiveNoResponse);
    UNITY_END();
    return 0;
}