#include <Arduino.h>
#include "BC95NB.h"
#include "BC95NBExt.h"

#define MQTT_HOST "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "client"
#define MQTT_USER_ID "user"
#define MQTT_PASSWORD "password"

const uint8_t resetPin = 4;
const bool activeHigh = true;
// Modem modem(Serial2, resetPin, activeHigh, &Serial);
Modem modem(Serial2, resetPin, activeHigh);
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
