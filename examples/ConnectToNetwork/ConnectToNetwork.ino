#include <Arduino.h>
#include "BC95NB.h"

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600);
    Serial.print("Establishing Connection with Band 8");
    int status = BC95NB.begin(8);
    while (status != NB_NETWORK_REGISTERED)
    {
        if (status == NB_NETWORK_SEARCHING_FOR_OPERATOR)
        {
            Serial.println("DEBUG>> Searching for an operator.");
        }
        if (status == NB_NETWORK_REGSTRATION_DENIED)
        {
            Serial.println("DEBUG>> Network Registration Denied.");
        }
        if (status == NB_NETWORK_NOT_REGISTERED)
        {
            Serial.println("DEBUG>> Network Not Registered.");
        }
        status = BC95NB.begin(8);
        delay(300);
    }
    Serial.println();
    Serial.print("Connection Established.");
}

void loop()
{
    // put your main code here, to run repeatedly:
}