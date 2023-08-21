#include "setup.h"
#include <gtest/gtest.h>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

const char *host = "test.mosquitto.org";
const int port = 1883;
const char *userName = "userName";
const char *password = "password";
const char *clientId = "clientId";

class BC95NBExtTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ArduinoFakeReset();
        modemBuilder = new ModemBuilder();
        testSupport.setupMocks();
    }
    void TearDown() override
    {
        delete modemBuilder;
        testSupport.reset();
    }
};

static void BC95NB_EXT_CMD_TEST(MqttState givenState, MqttState expectedState, const char *expectedCmd)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NBExt mqttClient(modem);
    mqttClient.setState(givenState);
    EXPECT_EQ(givenState, mqttClient.getState());
    MqttState state = mqttClient.connect(host, port, clientId, userName, password);
    EXPECT_EQ(expectedState, state);
    EXPECT_STREQ(expectedCmd, testSupport.getTxBuffer().c_str());
}

static void BC95NB_EXT_RESPONSE_TEST(MqttState givenState, const char *givenResponse, MqttState expectedState)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NBExt mqttClient(modem);
    testSupport.putRxBuffer(givenResponse);
    mqttClient.setState(givenState);
    MqttState state = mqttClient.connect(host, port, clientId, userName, password);
    EXPECT_EQ(expectedState, state);
}

TEST_F(BC95NBExtTest, OpenHost)
{
    char expectedCmd[100];
    memset(expectedCmd, 0, 100);
    sprintf(expectedCmd, "AT+QMTOPEN=0,\"%s\",%d\r\n", host, port);

    BC95NB_EXT_CMD_TEST(MqttState::OPEN_HOST, MqttState::WAIT_OPEN_HOST_RESPONSE, expectedCmd);
}

TEST_F(BC95NBExtTest, ProceedToConnectClientToServer_WhenPortIsOpened)
{
    BC95NB_EXT_RESPONSE_TEST(MqttState::WAIT_OPEN_HOST_RESPONSE,
                             "\r\nOK\r\n\r\n+QMTOPEN: 0,0\r\n",
                             MqttState::CONNECT_TO_SERVER);
}

TEST_F(BC95NBExtTest, ConnectToServer)
{
    char expectedCmd[100];
    memset(expectedCmd, 0, 100);
    sprintf(expectedCmd, "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n", clientId, userName, password);

    BC95NB_EXT_CMD_TEST(MqttState::CONNECT_TO_SERVER, MqttState::WAIT_CONNECT_TO_SERVER_RESPONSE, expectedCmd);
}

TEST_F(BC95NBExtTest, ProceedToReadyState_WhenServerIsConnected)
{
    BC95NB_EXT_RESPONSE_TEST(MqttState::WAIT_CONNECT_TO_SERVER_RESPONSE,
                             "\r\nOK\r\n\r\n+QMTCONN: 0,0,0\r\n",
                             MqttState::READY);
}

TEST_F(BC95NBExtTest, SubscribeToMqttTopic)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NBExt mqttClient(modem);
    const char *topic = "topic/subtopic";
    uint8_t QoS = 0;
    char expectedCmd[100];
    sprintf(expectedCmd, "AT+QMTSUB=0,0,\"%s\",%d\r\n", topic, QoS);
    testSupport.putRxBuffer("\r\nOK\r\n\r\n+QMTSUB: 0,0,1,0\r\n");

    int retCode = mqttClient.subscribe(topic, QoS);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), expectedCmd);
    EXPECT_EQ(retCode, 1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
        ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}