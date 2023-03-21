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

class BC95MQTT
{
public:
    BC95MQTT(Modem *modem);
    ~BC95MQTT();
    int begin(const char *host, int port);

private:
    Modem *modem_;
    String buffer_;
};

#endif