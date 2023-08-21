#include "BC95NBExt.h"

void MqttUrcHandler::onReceive(String &_response)
{
    if (_response.indexOf("+QMTRECV:") != -1)
    {
        int connectID_index = _response.indexOf("+QMTRECV:") + strlen("+QMTRECV:") + 1;
        int msgID_index = _response.indexOf(",", connectID_index) + 1;
        int topic_index = _response.indexOf(",", msgID_index) + 1;
        int rxMsg_index = _response.indexOf(",", topic_index) + 1;
        char topic[20];
        char msg[150];
        strcpy(topic, _response.substring(topic_index, rxMsg_index - 1).c_str());
        strcpy(msg, _response.substring(rxMsg_index).c_str());
        if (m_callback)
        {
            m_callback->onMessage(topic, msg);
        }
    }
}

BC95NBExt::BC95NBExt(Modem *_modem)
    : m_modem(_modem),
      m_state(MqttState::OPEN_HOST),
      m_msgID(0)
{
    m_modem->addResponseHandler(&m_mqttUrcHandler);
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
        int qmstatIndex = response.indexOf("+QMTSTAT:");
        if (qmstatIndex != -1)
        {
            int connectIdIndex = qmstatIndex + strlen("+QMTSTAT:") + 1;
            int errIndex = response.indexOf(",", connectIdIndex) + 1;
            int err = response.charAt(errIndex) - '0';
            if (err == 1)
            {
                m_state = MqttState::OPEN_HOST;
            }
        }
        break;
    }
    case MqttState::CONNECT_TO_SERVER:
    {
        char cmd[BC95_MQTT_MAX_CMD_LENGTH];
        if (userName && password)
            snprintf(cmd, BC95_MQTT_MAX_CMD_LENGTH, "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n", clientId, userName, password);
        else
            snprintf(cmd, BC95_MQTT_MAX_CMD_LENGTH, "AT+QMTCONN=0,\"%s\"\r\n", clientId);
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

int BC95NBExt::subscribe(const char *topic, uint8_t QoS)
{
    char cmd[BC95_MQTT_MAX_CMD_LENGTH];
    String response;
    snprintf(cmd, BC95_MQTT_MAX_CMD_LENGTH, "AT+QMTSUB=0,%d,\"%s\",%d\r\n", random(1 - 65535), topic, QoS);
    m_modem->send(cmd);
    m_modem->waitForResponse(2000, &response);
    int index = response.indexOf("+QMTSUB:");
    for (auto i{0}; i < 2; i++)
        index = response.indexOf(",", index + 1);
    if (index != -1)
    {
        return response.charAt(index + 1) - '0';
    }
    return -1;
}

int BC95NBExt::publish(const char *_topic, const char *msg, uint8_t _QoS, bool _retain)
{
    char cmd[BC95_MQTT_MAX_CMD_LENGTH];
    String response;
    uint16_t msgID = 0;
    if (_QoS > 0)
        msgID = random(1, 65535);
    snprintf(cmd,
             BC95_MQTT_MAX_CMD_LENGTH,
             "AT+QMTPUB=0,%d,%d,%d,\"%s\"\r\n",
             msgID, _QoS, _retain, _topic);
    m_modem->send(cmd);
    m_modem->waitForResponse(200, &response);
    if (response.indexOf(">") != -1)
    {
        char endOfCmd = 0x1A;
        m_modem->send(msg);
        m_modem->send(&endOfCmd);
        m_modem->waitForResponse(500, &response);
    }
    return 0;
}

void BC95NBExt::setMqttSubscriberCallback(MqttSubscriberCallback *callback)
{
    m_mqttUrcHandler.setCallback(callback);
}