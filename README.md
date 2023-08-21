# BC95 Library For Arduino

[![Arduino CI](https://github.com/aungkhantmaw64/BC95NB/actions/workflows/workflow.yml/badge.svg)](https://github.com/aungkhantmaw64/BC95NB/actions)
[![License: MIT](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/aungkhantmaw64/BC95NB/blob/main/LICENSE)

## About the BC95x Modem

BC95 is a high-performance NB-IoT module with extremely low power consumption. The BC95NB driver allows the MCU to communicate with the modem through the UART Serial Interface and lets you send the raw AT commands or use standard 3GPP and modem (BC95) specific commands for MQTT communication.

> **Note**
> BC95 Series' built-in firmware do not support TLS/SSL connections. But, it is still possible to use the modem with secure MQTT by utilizing its TCP functionality.

## Example Usage

Name: SimplePubSub

Description: This example shows how to connect the modem to the MQTT host/broker, subscribe to a topic and send a string message,"Hi".

### Headers

In order to use the built-in MQTT functionality of the Modem, you must include the following header. _BC95NB.h_ contains standard 3GPP comands for querying, and changing network parameters while _BC95NBExt.h_ contains. the modem specific functionality.

```CPP
#include "BC95NB.h"
#include "BC95NBExt.h"
```

You can set your MQTT host and related user information by using the following macro definitions.

```CPP
#define MQTT_HOST "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "client"
#define MQTT_USER_ID "user"
#define MQTT_PASSWORD "password"
```

The reset circuit of the modem could be different from modules to modules. So, when choosing the reset pin, you should set the activeHigh value (true/false) based on the reset circuit of your module.

```cpp
const uint8_t resetPin = 4;
const bool activeHigh = true;
```

Modem provides different contructors. Use the following to see the AT response as debug messages on the serial terminal.

```cpp
Modem modem(Serial2, resetPin, activeHigh, &Serial);
```

Or you can use the following if you want to disable the debug messages

```
Modem modem(Serial2, resetPin, activeHigh);
```

```CPP
BC95NB nb(&modem);
BC95NBExt mqttClient(&modem);

char ipAddress[STD_MODEM_MAX_IP_ADDR_LENGTH];
char imsi[STD_MODEM_MAX_IMSI_LENGTH];

class MyMqttSubscriberCallback : public MqttSubscriberCallback
{
    void onMessage(const char *topic, const char *message)
    {
        // Everytime the subscribed topics receive a message,
        // this function will be called.
        Serial.printf("Topic: %s, Message: %s\n", topic, message);
    }
};

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600);
    mqttClient.setMqttSubscriberCallback(new MyMqttSubscriberCallback);
    Serial.println("MODEM>> Booting up LTE-M Modem");
    modem.reset();
    Serial.println("MODEM>> Establishing a connection with the network...");
    while (nb.begin() != BC95NBState::NET_READY)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nMODEM>> Connection established successfully");
    mqttClient.configDnsServerAddr("8.8.8.8", "8.8.4.4");
    Serial.println("MODEM>> Connecting to the Mqtt Host.");
    while (mqttClient.connect(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID) != MqttState::READY)
    {
        Serial.print(".");
        delay(10);
    }
    Serial.printf("\nMODEM>> Connected to the Mqtt Host: %s, %d\n", MQTT_HOST, MQTT_PORT);
    Serial.println("MODEM>> Subscribing to a topic. Topic: topic/subtopic ");
    mqttClient.subscribe("topic/subtopic", 0);
    Serial.println("MODEM>> Subscribed to a topic. Topic: topic/subtopic ");
    mqttClient.publish("topic/subtopic", "Hello\n", 0, false);
}

void loop()
{
    mqttClient.publish("topic/subtopic", "Hi\n", 0, false);
}
```

## LICENSE

MIT
