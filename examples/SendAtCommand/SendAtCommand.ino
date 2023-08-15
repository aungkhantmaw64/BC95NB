#include <Arduino.h>
#include "BC95NB.h"

const uint8_t resetPin = 4;
const bool activeHigh = true;
Modem modem(Serial2, resetPin, activeHigh);
String response;

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600);
    Serial.println("Triggered hard reset.");
    modem.reset();
    modem.waitForResponse(5000, &response);
    Serial.println(response);
    modem.send("AT\r\n");
    modem.waitForResponse(300, &response);
    Serial.println(response);
    modem.send("AT+CFUN?\r\n");
    modem.waitForResponse(300, &response);
    Serial.println(response);
    modem.send("AT+CGMR\r\n");
    modem.waitForResponse(1000, &response);
    Serial.println(response);
    Serial.println("Done");
    while (true)
        ;
}

void loop()
{
    // put your main code here, to run repeatedly:
}
