#include "BC95NB.h"

NBClass::NBClass(Modem *modem)
    : _modem(modem),
      _state(NB_DISABLING_AUTOCONECT)
{
}

NBClass::~NBClass()
{
    delete _modem;
}

int NBClass::begin(int band)
{
    int modem_status = MODEM_STATUS_UNKNOWN;
    switch (_state)
    {
    case NB_DISABLING_AUTOCONECT:
    {
        _modem->send("AT+NCONFIG=AUTOCONNECT,FALSE");
        modem_status = _modem->waitForResponse(300);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
            _state = NB_AUTOCONNECT_DISABLED;
        break;
    }
    case NB_AUTOCONNECT_DISABLED:
    {
        _modem->send("AT+NRB");
        _state = NB_REBOOTING_MODEM;
        break;
    }
    case NB_REBOOTING_MODEM:
    {
        modem_status = _modem->waitForResponse(1000);
        if (modem_status == MODEM_STATUS_REBOOTED)
        {
            _state = NB_MODEM_REBOOTED;
        }
        break;
    }
    case NB_MODEM_REBOOTED:
    {
        char cmd[30];
        memset(cmd, 0, 10);
        sprintf(cmd, "AT+NBAND=%d", band);
        _modem->send(cmd);
        _state = NB_SWITCHING_BAND;
        break;
    }
    case NB_SWITCHING_BAND:
    {
        modem_status = _modem->waitForResponse(300);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
        {
            _state = NB_BAND_SWITCHED;
        }
        break;
    }
    case NB_BAND_SWITCHED:
    {
        _modem->send("AT+CFUN=1");
        _state = NB_SETTING_FULL_FUNCTIONALITY;
        break;
    }
    case NB_SETTING_FULL_FUNCTIONALITY:
    {
        modem_status = _modem->waitForResponse(300);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
        {
            _state = NB_FULL_FUNCTIONALITY_SET;
        }
        break;
    }
    case NB_FULL_FUNCTIONALITY_SET:
    {
        _modem->send("AT+CGATT=1");
        _state = NB_REGISTERING_NETWORK;
        break;
    }
    case NB_REGISTERING_NETWORK:
    {

        break;
    }
    default:
        break;
    }
    return _state;
}