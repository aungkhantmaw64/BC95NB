#include "BC95MQTT.h"

BC95MQTT::BC95MQTT(Modem *modem)
    : modem_(modem)
{
    buffer_.reserve(100);
}

BC95MQTT::~BC95MQTT()
{
}

int BC95MQTT::begin(const char *host, int port)
{
    modem_->sendf("AT+QMTOPEN=0,\"%s\",%d", host, port);
    if (MODEM_STATUS_VALID_RESPONSE == modem_->waitForResponse(1000, &buffer_))
    {
        if (buffer_.indexOf("+QMTOPEN:0,0") != -1)
            return MQTT_NETWORK_OPENED;
        if (buffer_.indexOf("+QMTOPEN:0,1") != -1)
            return MQTT_NETWORK_WRONG_PARAMETER;
        if (buffer_.indexOf("+QMTOPEN:0,2") != -1)
            return MQTT_NETWORK_IDENTIFIER_OCCUPIED;
        if (buffer_.indexOf("+QMTOPEN:0,3") != -1)
            return MQTT_NETWORK_PDP_ACTIVATION_FAILED;
        if (buffer_.indexOf("+QMTOPEN:0,4") != -1)
            return MQTT_NETWORK_DNS_PARSE_FAILED;
        if (buffer_.indexOf("+QMTOPEN:0,5") != -1)
            return MQTT_NETWORK_DISCONNECTED;
    }
    return MQTT_NETWORK_OPEN_FAILED;
}