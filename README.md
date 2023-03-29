# BC95 Library For Arduino
[![Arduino CI](https://github.com/aungkhantmaw64/BC95NB/actions/workflows/workflow.yml/badge.svg)](https://github.com/aungkhantmaw64/BC95NB/actions)
[![License: MIT](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/aungkhantmaw64/BC95NB/blob/main/LICENSE)

## Introduction
BC95 is a high-performance NB-IoT module with extremely low power consumption. The ultra-compact 23.6mm × 19.9mm ×
2.2mm profile makes it a perfect choice for size sensitive applications. Designed to be compatible with Quectel GSM/GPRS
M95 module in the compact and unified form factor, it provides a flexible and scalable platform for migrating from GSM/
GPRS to NB-IoT networks.

> **Note**
> BC95 Series' built-in firmware do not support TLS/SSL connections. For those who want to use a more secure MQTT connection, this module is not suggested. 

## Examples

*Note: As BC95 Modem uses AT commands via UART Protocol, you may have to specify which UART port (Serial, Serial2, etc.)*

*Also don't forget to initialize your serial port with the right baudrate, 9600, which is the default baud for most BC95 series.*


Name: SimpleMQTT.ino

Description: This example shows how to use the API to establish a connection between the modem and the MQTT server.
```CPP
#include "BC95NB.h"
#include "BC95MQTT.h"

static const char host[] = "test.mosquitto.org";
static const int port = 1883;
static const int reset_pin = 18;
static BC95 modem(&Serial2, reset_pin);
static NBClass nb(&modem);
static BC95MQTT mqttclient(&modem);

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600);
    Serial.print("\n\nMODEM>> Establishing a connection with the NB-IoT network");
    while (nb.begin() != NB_NETWORK_ATTACHED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println();
    Serial.println("MODEM>> Connection Established.");
    Serial.println("MODEM>> IMSI: " + nb.getIMSI());
    Serial.println("MODEM>> IP Address: " + nb.getIPAddress());
    if (mqttclient.configDNSAddress("8.8.8.8", "8.8.4.4"))
        Serial.println("MODEM>> DNS Addressed Configured.");
    if (mqttclient.hasInstance())
    {
        Serial.println("MODEM>> Client has an opened port.");
        if (mqttclient.end() == MQTT_NETWORK_CLOSED)
            Serial.println("MODEM>> Port closed.");
    }
    if (mqttclient.begin(host, port) != MQTT_NETWORK_OPENED)
    {
        Serial.print("MODEM>> Failed to open mqtt network:");
        Serial.print(host);
        Serial.print(",");
        Serial.println(port);
        while (1)
        {
            /* code */
        }
    }
    Serial.println("MODEM>> MQTT network opened succesfully");
}

void loop()
{
}
```

### Tested Boards
- ESP32-WROOM-32

### Supported Protocol
- MQTT
  
### To be added
- CoAP
- UDP/TCP
 
## LICENSE
MIT
