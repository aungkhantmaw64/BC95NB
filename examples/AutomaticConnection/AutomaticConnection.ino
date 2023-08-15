#include <Arduino.h>
#include "BC95NB.h"

const uint8_t resetPin = 4;
const bool activeHigh = true;
Modem modem(Serial2, resetPin, activeHigh);
BC95NB nb(&modem);
char ipAddress[STD_MODEM_MAX_IP_ADDR_LENGTH];
char imsi[STD_MODEM_MAX_IMSI_LENGTH];

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600);
    Serial.print("MODEM>> Establishing a connection with the network...");
    unsigned long startTime = millis();
    modem.reset();
    while (nb.begin() != BC95NBState::NET_READY)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println("MODEM>> Connection established successfully");
    nb.getIMSI(imsi);
    nb.getIpAddress(ipAddress);
    Serial.printf("MODEM>> IMSI: %s\n", imsi);
    Serial.printf("MODEM>> IP Address: %s\n", ipAddress);
    unsigned long connectionTime = millis() - startTime;
    Serial.printf("MODEM>> Connection time: %d ms\n", connectionTime);
    while (true)
        ;
}

void loop()
{
    // put your main code here, to run repeatedly:
}
