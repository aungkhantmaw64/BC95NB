#include <unity.h>
#include <ArduinoFake.h>
#include "BC95NB.h"
#include "MockBC95.h"

using namespace fakeit;

void test_NBClass_ConnectsNetworkSuccessfully(void)
{
    BC95Mock *modemMock = new BC95Mock();
    NBClass _BC95NB(modemMock);
    // Start establishing connection with band 8
    modemMock->setResponseStatus(MODEM_STATUS_UNKNOWN);
    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));
    TEST_ASSERT_EQUAL_STRING("AT+NCONFIG=AUTOCONNECT,FALSE\r", modemMock->getLastCmd().c_str());
    // Disables autoconnect mode before rebooting
    modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));

    // Reboots modem and waits for 1 second before configuration
    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));
    TEST_ASSERT_EQUAL_STRING("AT+NRB\r", modemMock->getLastCmd().c_str());

    // Switch the operating frequency band to band 8 (900 Mhz)
    modemMock->setResponseStatus(MODEM_STATUS_REBOOTED);
    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));

    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));
    TEST_ASSERT_EQUAL_STRING("AT+NBAND=8\r", modemMock->getLastCmd().c_str());

    modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));

    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));
    TEST_ASSERT_EQUAL_STRING("AT+CFUN=1\r", modemMock->getLastCmd().c_str());

    modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));

    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));
    TEST_ASSERT_EQUAL_STRING("AT+CGATT=1\r", modemMock->getLastCmd().c_str());

    modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));

    TEST_ASSERT_EQUAL(NB_NETWORK_NOT_REGISTERED, _BC95NB.begin(8));
    TEST_ASSERT_EQUAL_STRING("AT+CEREG?\r", modemMock->getLastCmd().c_str());

    modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
    modemMock->setResponse("\r\n+CEREG:0,2\r\nOK\r\n");
    TEST_ASSERT_EQUAL(NB_NETWORK_SEARCHING_FOR_OPERATOR, _BC95NB.begin(8));

    modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
    modemMock->setResponse("\r\n+CEREG:0,1\r\nOK\r\n");
    TEST_ASSERT_EQUAL(NB_NETWORK_REGISTERED, _BC95NB.begin(8));
}

void setUp()
{
    ArduinoFakeReset();
}

void tearDown()
{
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_NBClass_ConnectsNetworkSuccessfully);

    UNITY_END();
    return 0;
}