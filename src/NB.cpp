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
        if (modem_->waitForResponse(300, &buffer_))
        {
            if (buffer_.indexOf("+CFUN:1") != -1)
            {
                connectionState_ = NB_MODEM_AWAKEN;
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