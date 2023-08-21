#ifndef BC95NB_SRC_BC95NB_EXT_H
#define BC95NB_SRC_BC95NB_EXT_H

#include "Modem.h"

#define BC95_MQTT_MAX_CMD_LENGTH 100

enum class MqttState
{
    OPEN_HOST,
    WAIT_OPEN_HOST_RESPONSE,
    CONNECT_TO_SERVER,
    WAIT_CONNECT_TO_SERVER_RESPONSE,
    READY
};

class MqttSubscriberCallback
{
public:
    virtual void onMessage(const char *topic, const char *message) = 0;
};
class MqttUrcHandler : public ModemUrcHandler
{
public:
    void onReceive(String &response) override;
    void setCallback(MqttSubscriberCallback *_callback)
    {
        m_callback = _callback;
    }

private:
    MqttSubscriberCallback *m_callback = nullptr;
};
class BC95NBExt
{
public:
    BC95NBExt(Modem *_modem);
    ~BC95NBExt();
    MqttState connect(const char *host, int port, const char *clientId, const char *userName = nullptr, const char *password = nullptr);
    void configDnsServerAddr(const char *primaryAddr, const char *secondaryAddr);
    void setState(MqttState state) { m_state = state; }
    MqttState getState(void) { return m_state; }

    int subscribe(const char *topic, uint8_t QoS);
    int publish(const char *_topic, const char *_msg, uint8_t _QoS = 0, bool _retain = false);
    void setMqttSubscriberCallback(MqttSubscriberCallback *callback);

private:
    Modem *m_modem;
    MqttState m_state;
    uint16_t m_msgID;
    MqttUrcHandler m_mqttUrcHandler;
};
#endif