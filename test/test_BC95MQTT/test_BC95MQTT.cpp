#include "setup.h"
#include <gtest/gtest.h>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

#define BC95_MQTT_MAX_CMD_LENGTH 100

enum class BC95MqttState
{
    OPEN_HOST,
    WAIT_OPEN_HOST_RESPONSE
};

class BC95Mqtt
{
public:
    BC95Mqtt(Modem *_modem)
        : m_modem(_modem),
          m_state(BC95MqttState::OPEN_HOST){

          };
    ~BC95Mqtt(){

    };
    BC95MqttState connect(const char *host, int port, const char *user, const char *password)
    {
        switch (m_state)
        {
        case BC95MqttState::OPEN_HOST:
        {
            char cmd[BC95_MQTT_MAX_CMD_LENGTH];
            snprintf(cmd, BC95_MQTT_MAX_CMD_LENGTH, "AT+QMTOPEN=0,\"%s\",%d\r\n", host, port);
            m_modem->send(cmd);
            m_state = BC95MqttState::WAIT_OPEN_HOST_RESPONSE;
            break;
        }
        default:
            break;
        }
        return m_state;
    }
    BC95MqttState getState(void)
    {
        return m_state;
    }

private:
    Modem *m_modem;
    BC95MqttState m_state;
};

class BC95MqttTest : public ::testing::Test
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

TEST_F(BC95MqttTest, OpenHost)
{
    Modem *modem = modemBuilder->buildModem();
    BC95Mqtt mqttClient(modem);
    const char *host = "test.mosquitto.org";
    const int port = 1883;
    const char *userName = "userName";
    const char *password = "password";
    char expectedCmd[100];
    memset(expectedCmd, 0, 100);
    sprintf(expectedCmd, "AT+QMTOPEN=0,\"%s\",%d\r\n", host, port);
    EXPECT_EQ(BC95MqttState::OPEN_HOST, mqttClient.getState());
    BC95MqttState state = mqttClient.connect(host, port, userName, password);
    EXPECT_EQ(BC95MqttState::WAIT_OPEN_HOST_RESPONSE, state);
    EXPECT_STREQ(expectedCmd, testSupport.getTxBuffer().c_str());
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