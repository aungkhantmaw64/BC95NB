#include "BC95.h"

static const int reset_pin = 18;
static BC95 modem(&Serial2, reset_pin);
static String response;

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600);
    modem.send("AT");
    if (modem.waitForResponse(300, &response) == MODEM_STATUS_VALID_RESPONSE)
    {
        Serial.println(response);
    }
    modem.send("ATI");
    if (modem.waitForResponse(300, &response) == MODEM_STATUS_VALID_RESPONSE)
    {
        Serial.println(response);
    }
}

void loop()
{
}