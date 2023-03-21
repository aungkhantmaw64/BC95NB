#include "BC95NB.h"

static const int reset_pin = 18;
static BC95 modem(&Serial2, reset_pin);
static NBClass nb(&modem);

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
}

void loop()
{
}