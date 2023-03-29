#include "BC95NB.h"
#include "BC95MQTT.h"

static const char host[] = "";
static const int port = 0;
static const char clientid[] = "";
static const char username[] = "";
static const char password[] = "";
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
    if (!mqttclient.connect(host, username, password))
    {
        Serial.println("MODEM>> Connection rejected by the broker.");
    }
    Serial.println("MODEM>> Connection accepted by the broker.");
}

void loop()
{
}