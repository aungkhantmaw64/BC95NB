#include "MockBC95.h"
#include <unity.h>
#include <ArduinoFake.h>

using namespace fakeit;

void test_MockBC95_responseExpect(void)
{
    MockBC95 mock;
    String response;
    mock.responseExpect("ERROR", MODEM_STATUS_INVALID_PARAMETERS);
    mock.responseExpect("OK", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("", MODEM_STATUS_TIMEOUT_ERROR);
    mock.responseExpect("rewfwkefw@#$@", MODEM_STATUS_UNKNOWN);
    mock.responseExpect("\r\n+QMTSTAT:0,1\r\n", MODEM_STATUS_URC_EVENT);
    mock.responseExpect("\r\nAT\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);

    TEST_ASSERT_EQUAL(MODEM_STATUS_INVALID_PARAMETERS, mock.waitForResponse(300, &response));
    TEST_ASSERT_EQUAL_STRING("ERROR", response.c_str());

    TEST_ASSERT_EQUAL(MODEM_STATUS_VALID_RESPONSE, mock.waitForResponse(300, &response));
    TEST_ASSERT_EQUAL_STRING("OK", response.c_str());

    TEST_ASSERT_EQUAL(MODEM_STATUS_TIMEOUT_ERROR, mock.waitForResponse(300, &response));
    TEST_ASSERT_EQUAL_STRING("", response.c_str());

    TEST_ASSERT_EQUAL(MODEM_STATUS_UNKNOWN, mock.waitForResponse(300, &response));
    TEST_ASSERT_EQUAL_STRING("rewfwkefw@#$@", response.c_str());

    TEST_ASSERT_EQUAL(MODEM_STATUS_URC_EVENT, mock.waitForResponse(300, &response));
    TEST_ASSERT_EQUAL_STRING("\r\n+QMTSTAT:0,1\r\n", response.c_str());

    TEST_ASSERT_EQUAL(MODEM_STATUS_VALID_RESPONSE, mock.waitForResponse(300, &response));
    TEST_ASSERT_EQUAL_STRING("\r\nAT\r\n\r\nOK\r\n", response.c_str());
}

void setUp()
{
}

void tearDown()
{
    ArduinoFakeReset();
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_MockBC95_responseExpect);
    UNITY_END();
    return 0;
}