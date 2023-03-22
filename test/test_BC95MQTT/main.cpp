#include <ArduinoFake.h>
#include <unity.h>
#include "MockBC95.h"
#include "BC95MQTT.h"

using namespace fakeit;

MockBC95 mock;
BC95MQTT mqtt(&mock);

void test_BC95MQTT_begin()
{
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,-1\r\n", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,0\r\n", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,1\r\n", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,2\r\n", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,3\r\n", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,4\r\n", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,5\r\n", MODEM_STATUS_VALID_RESPONSE);

    TEST_ASSERT_EQUAL(MQTT_NETWORK_OPEN_FAILED, mqtt.begin("test.mosquitto.org", 8883));
    TEST_ASSERT_EQUAL(MQTT_NETWORK_OPENED, mqtt.begin("test.mosquitto.org", 8883));
    TEST_ASSERT_EQUAL(MQTT_NETWORK_WRONG_PARAMETER, mqtt.begin("test.mosquitto.org", 8883));
    TEST_ASSERT_EQUAL(MQTT_NETWORK_IDENTIFIER_OCCUPIED, mqtt.begin("test.mosquitto.org", 8883));
    TEST_ASSERT_EQUAL(MQTT_NETWORK_PDP_ACTIVATION_FAILED, mqtt.begin("test.mosquitto.org", 8883));
    TEST_ASSERT_EQUAL(MQTT_NETWORK_DNS_PARSE_FAILED, mqtt.begin("test.mosquitto.org", 8883));
    TEST_ASSERT_EQUAL(MQTT_NETWORK_DISCONNECTED, mqtt.begin("test.mosquitto.org", 8883));
}

void test_BC95MQTT_hasInstance()
{
    mock.responseExpect("\r\nOK\r\n\r\n+QMTOPEN:0,\"test.mosquitto.org\",1883",
                        MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n",
                        MODEM_STATUS_VALID_RESPONSE);
    TEST_ASSERT_EQUAL(1, mqtt.hasInstance());
    TEST_ASSERT_EQUAL(0, mqtt.hasInstance());
}

void test_BC95MQTT_configDNSAddress(void)
{
    mock.responseExpect("\r\nOK\r\n\r\n", MODEM_STATUS_VALID_RESPONSE);
    TEST_ASSERT_EQUAL(1, mqtt.configDNSAddress("8.8.8.8", "8.8.4.4"));
}

void test_BC95MQTT_end()
{
    mock.responseExpect("\r\nOK\r\n\r\n+QMTCLOSE:0,-1\r\n", MODEM_STATUS_VALID_RESPONSE);
    mock.responseExpect("\r\nOK\r\n\r\n+QMTCLOSE:0,0\r\n", MODEM_STATUS_VALID_RESPONSE);
    TEST_ASSERT_EQUAL(MQTT_NETWORK_CLOSE_FAILED, mqtt.end());
    TEST_ASSERT_EQUAL(MQTT_NETWORK_CLOSED, mqtt.end());
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
    RUN_TEST(test_BC95MQTT_begin);
    RUN_TEST(test_BC95MQTT_hasInstance);
    RUN_TEST(test_BC95MQTT_configDNSAddress);
    RUN_TEST(test_BC95MQTT_end);
    UNITY_END();
    return 0;
}