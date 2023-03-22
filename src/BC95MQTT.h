#ifndef D_BC95MQTT_H
#define D_BC95MQTT_H

#include "Modem.h"

enum
{
    MQTT_NETWORK_OPEN_FAILED = -1,
    MQTT_NETWORK_OPENED,
    MQTT_NETWORK_WRONG_PARAMETER,
    MQTT_NETWORK_IDENTIFIER_OCCUPIED,
    MQTT_NETWORK_PDP_ACTIVATION_FAILED,
    MQTT_NETWORK_DNS_PARSE_FAILED,
    MQTT_NETWORK_DISCONNECTED
};

enum
{
    MQTT_NETWORK_CLOSE_FAILED = -1,
    MQTT_NETWORK_CLOSED
};

class BC95MQTT
{
public:
    /**
     * @brief Construct a new BC95MQTT object
     *
     * @param modem
     */
    BC95MQTT(Modem *modem);
    /**
     * @brief Destroy the BC95MQTT object
     *
     */
    ~BC95MQTT();
    /**
     * @brief
     *
     * @param host
     * @param port
     * @return int
     */
    int begin(const char *host, int port);
    /**
     * @brief
     *
     * @return int
     */
    int hasInstance(void);
    /**
     * @brief
     *
     * @return int
     */
    int end(void);

private:
    Modem *modem_;
    String buffer_;
};

#endif