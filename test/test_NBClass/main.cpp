#include <ArduinoFake.h>
#include <unity.h>
#include "MockBC95.h"
#include "NB.h"

MockBC95 modemMock;
NBClass NB(&modemMock);

using namespace fakeit;

void test_NB_begin_auto()
{
    modemMock.responseExpect("\r\n+CFUN:0\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);
    modemMock.responseExpect("\r\n+CFUN:1\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);
    modemMock.responseExpect("\r\n+CEREG:0,0\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);
    modemMock.responseExpect("\r\n+CEREG:0,1\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);
    modemMock.responseExpect("\r\n+CGATT:0\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);
    modemMock.responseExpect("\r\n+CGATT:1\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);
    modemMock.responseExpect("\r\n460111174590523\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);
    modemMock.responseExpect("\r\n+CGPADDR:0,10.169.241.248\r\n\r\nOK\r\n", MODEM_STATUS_VALID_RESPONSE);

    TEST_ASSERT_EQUAL(NB_IDLE, NB.begin());
    TEST_ASSERT_EQUAL(NB_MODEM_AWAKEN, NB.begin());
    TEST_ASSERT_EQUAL(NB_MODEM_AWAKEN, NB.begin());
    TEST_ASSERT_EQUAL(NB_NETWORK_REGISTERED, NB.begin());
    TEST_ASSERT_EQUAL(NB_NETWORK_REGISTERED, NB.begin());
    TEST_ASSERT_EQUAL(NB_NETWORK_ATTACHED, NB.begin());
    TEST_ASSERT_EQUAL_STRING("460111174590523", NB.getIMSI().c_str());
    TEST_ASSERT_EQUAL_STRING("0,10.169.241.248", NB.getIPAddress().c_str());
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
    RUN_TEST(test_NB_begin_auto);
    UNITY_END();
    return 0;
}