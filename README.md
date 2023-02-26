# BC95 Library For Arduino
[![Arduino CI](https://github.com/aungkhantmaw64/BC95NB/actions/workflows/workflow.yml/badge.svg)](https://github.com/aungkhantmaw64/BC95NB/actions)
[![License: MIT](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/aungkhantmaw64/BC95NB/blob/main/LICENSE)

## EXAMPLES

*Note: As BC95 Modem uses UART Protocol, you may have to specify which UART port (Serial, Serial2, etc.) you want to use in configNB.h as follows.*

```CPP
#ifndef D_CONFIGNB_H
#define D_CONFIGNB_H
#include <Arduino.h>

#define MODEM_UART Serial // Specify your uart here
#define MODEM_RESET_PIN 18 // Hardware reset pin
#endif
```
*Also don't forget to initialize your serial port with the right baudrate, 9600, which is the default baud for most BC95 series.*

Name: ConnectToNetwork.ino

Description: This example shows how to use the API to establish a connection between the modem and the base station.
```CPP
#include <Arduino.h>
#include "BC95NB.h"

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600);
    Serial.print("DEBUG >> Establishing Connection with the Network using Band 8");
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
    Serial.print("DEBUG>> Connection Established.");
}

void loop()
{
    // put your main code here, to run repeatedly:
}
```

## LICENSE
MIT
