#include "BC95NBExt.h"

BC95NBExt::BC95NBExt(Modem *_modem)
    : m_modem(_modem),
      m_state(MqttState::OPEN_HOST)
{
}

BC95NBExt::~BC95NBExt()
{
}

void BC95NBExt::configDnsServerAddr(const char *primaryAddr, const char *secondaryAddr)
{
    char cmd[BC95_MQTT_MAX_CMD_LENGTH];
    snprintf(cmd, BC95_MQTT_MAX_CMD_LENGTH, "AT+QIDNSCFG=%s,%s\r\n", primaryAddr, secondaryAddr);
    m_modem->send(cmd);
    m_modem->waitForResponse(1000);
}

MqttState BC95NBExt::connect(const char *host, int port, const char *clientId, const char *userName, const char *password)
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
        int resIndex = response.indexOf("+QMTOPEN:");
        if (resIndex != -1)
        {
            resIndex += strlen("+QMTOPEN:");
            resIndex = response.indexOf(",", resIndex) + 1;
            int res = response.charAt(resIndex) - '0';
            if (res == 0)
                m_state = MqttState::CONNECT_TO_SERVER;
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
        ResponseCode retCode = m_modem->waitForResponse(1000, &response);
        int index = response.indexOf("+QMTCONN:");
        if (index != -1)
        {
            index += strlen("+QMTCONN:");
            int resultIndex = response.indexOf(",", index) + 1;
            int retCodeIndex = response.indexOf(",", resultIndex) + 1;
            int result = response.charAt(resultIndex) - '0';
            int retCode = response.charAt(retCodeIndex) - '0';
            if ((result == 0) || (retCode == 0))
                m_state = MqttState::READY;
        }
        break;
    }
    default:
        break;
    }
    return m_state;
}