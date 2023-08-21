#ifndef BC95NB_SRC_BC95NB_EXT_H
#define BC95NB_SRC_BC95NB_EXT_H

#include "Modem.h"

#define BC95_MQTT_MAX_CMD_LENGTH 100

/**
 * @brief Used to indicate the status of MQTT communication
 *
 */
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
    /**
     * @brief
     *
     * @param topic
     * @param message
     */
    virtual void onMessage(const char *topic, const char *message) = 0;
};
class MqttUrcHandler : public ModemUrcHandler
{
public:
    /**
     * @brief
     *
     * @param response
     */
    void onReceive(String &response) override;
    /**
     * @brief Set the Callback object
     *
     * @param _callback
     */
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
    /**
     * @brief
     *
     * @param host
     * @param port
     * @param clientId
     * @param userName
     * @param password
     * @return MqttState
     */
    MqttState connect(const char *host, int port, const char *clientId, const char *userName = nullptr, const char *password = nullptr);
    /**
     * @brief
     *
     * @param primaryAddr
     * @param secondaryAddr
     */
    void configDnsServerAddr(const char *primaryAddr, const char *secondaryAddr);
    /**
     * @brief Set the State object
     *
     * @param state
     */
    void setState(MqttState state) { m_state = state; }
    /**
     * @brief Get the State object
     *
     * @return MqttState
     */
    MqttState getState(void) { return m_state; }
    /**
     * @brief
     *
     * @param topic
     * @param QoS
     * @return int
     */
    int subscribe(const char *topic, uint8_t QoS);
    /**
     * @brief
     *
     * @param _topic
     * @param _msg
     * @param _QoS
     * @param _retain
     * @return int
     */
    int publish(const char *_topic, const char *_msg, uint8_t _QoS = 0, bool _retain = false);
    /**
     * @brief Set the Mqtt Subscriber Callback object
     *
     * @param callback
     */
    void setMqttSubscriberCallback(MqttSubscriberCallback *callback);

private:
    Modem *m_modem;
    MqttState m_state;
    uint16_t m_msgID;
    MqttUrcHandler m_mqttUrcHandler;
};
#endif