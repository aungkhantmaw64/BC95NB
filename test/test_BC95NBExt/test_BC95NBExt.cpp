#include "setup.h"
#include <gtest/gtest.h>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

const char *host = "test.mosquitto.org";
const int port = 1883;
const char *userName = "userName";
const char *password = "password";
const char *clientId = "clientId";

#define BC95_MQTT_MAX_CMD_LENGTH 100

enum class MqttState
{
    OPEN_HOST,
    WAIT_OPEN_HOST_RESPONSE,
    CONNECT_TO_SERVER,
    WAIT_CONNECT_TO_SERVER_RESPONSE,
    READY
};

class BC95NBExt
{
public:
    BC95NBExt(Modem *_modem)
        : m_modem(_modem),
          m_state(MqttState::OPEN_HOST){

          };
    ~BC95NBExt(){

    };
    MqttState connect(const char *host, int port, const char *clientId, const char *user, const char *password)
    {
        switch (m_state)
        {
        case MqttState::OPEN_HOST:
        {
            char cmd[BC95_MQTT_MAX_CMD_LENGTH];
            snprintf(cmd, BC95_MQTT_MAX_CMD_LENGTH, "AT+QMTOPEN=0,\"%s\",%d\r\n", host, port);
            m_modem->send(cmd);
            m_state = MqttState::WAIT_OPEN_HOST_RESPONSE;
            break;
        }
        case MqttState::WAIT_OPEN_HOST_RESPONSE:
        {
            String response;
            ResponseCode retcode = m_modem->waitForResponse(1000, &response);
            if (retcode == ResponseCode::OK)
            {
                int resIndex = response.indexOf("+QMTOPEN:");
                if (resIndex != -1)
                {
                    resIndex += strlen("+QMTOPEN:");
                    resIndex = response.indexOf(",", resIndex) + 1;
                    int res = response.charAt(resIndex) - '0';
                    if (res == 0)
                        m_state = MqttState::CONNECT_TO_SERVER;
                }
            }
            break;
        }
        case MqttState::CONNECT_TO_SERVER:
        {
            char cmd[BC95_MQTT_MAX_CMD_LENGTH];
            snprintf(cmd, BC95_MQTT_MAX_CMD_LENGTH, "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n", clientId, userName, password);
            m_modem->send(cmd);
            m_state = MqttState::WAIT_CONNECT_TO_SERVER_RESPONSE;
            break;
        }
        case MqttState::WAIT_CONNECT_TO_SERVER_RESPONSE:
        {
            String response;
            ResponseCode retCode = m_modem->waitForResponse(5000, &response);
            if (retCode == ResponseCode::OK)
            {
                int index = response.indexOf("+QMTCONN:");
                if (index != -1)
                {
                    index += strlen("+QMTCONN:");
                    int resultIndex = response.indexOf(",", index) + 1;
                    int retCodeIndex = response.indexOf(",", resultIndex) + 1;
                    int result = response.charAt(resultIndex) - '0';
                    int retCode = response.charAt(retCodeIndex) - '0';
                    if ((retCode == 0))
                        m_state = MqttState::READY;
                }
            }
            break;
        }
        default:
            break;
        }
        return m_state;
    }
    void setState(MqttState state) { m_state = state; }
    MqttState getState(void)
    {
        return m_state;
    }

private:
    Modem *m_modem;
    MqttState m_state;
};

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