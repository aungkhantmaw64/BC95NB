#include <ArduinoFake.h>
#include <unity.h>
#include "BC95NB.h"
#include "mocks/BC95Mock.h"

using namespace fakeit;

namespace NBClassTest
{
    void test_NBClass_ConnectsNetworkSuccessfully(void)
    {
        BC95Mock *modemMock = new BC95Mock();
        NBClass BC95NB(modemMock);
        // Start establishing connection with band 8
        modemMock->setResponseStatus(MODEM_STATUS_UNKNOWN);
        TEST_ASSERT_EQUAL(NB_DISABLING_AUTOCONECT, BC95NB.begin(8));
        TEST_ASSERT_EQUAL_STRING("AT+NCONFIG=AUTOCONNECT,FALSE\r", modemMock->getLastCmd().c_str());
        // Disables autoconnect mode before rebooting
        modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
        TEST_ASSERT_EQUAL(NB_AUTOCONNECT_DISABLED, BC95NB.begin(8));

        // Reboots modem and waits for 1 second before configuration
        TEST_ASSERT_EQUAL(NB_REBOOTING_MODEM, BC95NB.begin(8));
        TEST_ASSERT_EQUAL_STRING("AT+NRB\r", modemMock->getLastCmd().c_str());

        // Switch the operating frequency band to band 8 (900 Mhz)
        modemMock->setResponseStatus(MODEM_STATUS_REBOOTED);
        TEST_ASSERT_EQUAL(NB_MODEM_REBOOTED, BC95NB.begin(8));

        TEST_ASSERT_EQUAL(NB_SWITCHING_BAND, BC95NB.begin(8));
        TEST_ASSERT_EQUAL_STRING("AT+NBAND=8\r", modemMock->getLastCmd().c_str());

        modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
        TEST_ASSERT_EQUAL(NB_BAND_SWITCHED, BC95NB.begin(8));

        TEST_ASSERT_EQUAL(NB_SETTING_FULL_FUNCTIONALITY, BC95NB.begin(8));
        TEST_ASSERT_EQUAL_STRING("AT+CFUN=1\r", modemMock->getLastCmd().c_str());

        modemMock->setResponseStatus(MODEM_STATUS_VALID_RESPONSE);
        TEST_ASSERT_EQUAL(NB_FULL_FUNCTIONALITY_SET, BC95NB.begin(8));

        TEST_ASSERT_EQUAL(NB_REGISTERING_NETWORK, BC95NB.begin(8));
        TEST_ASSERT_EQUAL_STRING("AT+CGATT=1\r", modemMock->getLastCmd().c_str());
    }

    void run_tests(void)
    {
        RUN_TEST(test_NBClass_ConnectsNetworkSuccessfully);
    }
}