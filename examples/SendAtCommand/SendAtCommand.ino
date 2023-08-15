#include <Arduino.h>
#include "BC95NB.h"

const uint8_t resetPin = 4;
const bool activeHigh = true;
Modem modem(Serial2, resetPin, activeHigh, &Serial);
BC95NB nb(&modem);

String response;

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600);
    Serial.println("Triggered hard reset.");
    modem.reset();
    modem.waitForResponse(5000);
    modem.send("AT\r\n");
    modem.waitForResponse(300);
    modem.send("AT+CFUN?\r\n");
    modem.waitForResponse(300);
    modem.send("AT+CGMR\r\n");
    modem.waitForResponse(1000);
    modem.send("AT+NCONFIG=AUTOCONNECT,TRUE\r\n");
    modem.waitForResponse(2000);
    modem.send("AT+NRB");
    modem.waitForResponse(5000);
    while (true)
        ;
}

void loop()
{
    // put your main code here, to run repeatedly:
}
