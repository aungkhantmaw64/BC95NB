#include "NB.h"

NBClass::NBClass(Modem *modem)
    : modem_(modem),
      connectionState_(NB_IDLE)
{
    buffer_.reserve(100);
}

NBClass::~NBClass()
{
}

int NBClass::begin()
{
    switch (connectionState_)
    {
    case NB_IDLE:
    {
        modem_->send("AT+CFUN?");
        if (MODEM_STATUS_VALID_RESPONSE == modem_->waitForResponse(300, &buffer_))
        {
            if (buffer_.indexOf("+CFUN:1") != -1)
            {
                connectionState_ = NB_MODEM_AWAKEN;
            }
        }
        break;
    }
    case NB_MODEM_AWAKEN:
    {
        modem_->send("AT+CEREG?");
        if (MODEM_STATUS_VALID_RESPONSE == modem_->waitForResponse(300, &buffer_))
        {
            if (buffer_.indexOf("+CEREG:0,1") != -1)
            {
                connectionState_ = NB_NETWORK_REGISTERED;
            }
        }
        break;
    }
    case NB_NETWORK_REGISTERED:
    {
        modem_->send("AT+CGATT?");
        if (MODEM_STATUS_VALID_RESPONSE == modem_->waitForResponse(300, &buffer_))
        {
            if (buffer_.indexOf("+CGATT:1") != -1)
            {
                connectionState_ = NB_NETWORK_ATTACHED;
            }
        }
        break;
    }
    default:
        break;
    }
    return connectionState_;
}

int NBClass::begin(int band)
{
}