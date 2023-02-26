#include "BC95NB.h"

NBClass::NBClass(Modem *modem)
    : _modem(modem),
      _state(NB_STATE_DISABLING_AUTOCONECT),
      _networkStatus(NB_NETWORK_NOT_REGISTERED)
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
    case NB_STATE_DISABLING_AUTOCONECT:
    {
        _modem->send("AT+NCONFIG=AUTOCONNECT,FALSE");
        modem_status = _modem->waitForResponse(300);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
            _state = NB_STATE_AUTOCONNECT_DISABLED;
        break;
    }
    case NB_STATE_AUTOCONNECT_DISABLED:
    {
        _modem->send("AT+NRB");
        _state = NB_STATE_REBOOTING_MODEM;
        break;
    }
    case NB_STATE_REBOOTING_MODEM:
    {
        modem_status = _modem->waitForResponse(1000);
        if (modem_status == MODEM_STATUS_REBOOTED)
        {
            _state = NB_STATE_MODEM_REBOOTED;
        }
        break;
    }
    case NB_STATE_MODEM_REBOOTED:
    {
        char cmd[30];
        memset(cmd, 0, 10);
        sprintf(cmd, "AT+NBAND=%d", band);
        _modem->send(cmd);
        _state = NB_STATE_SWITCHING_BAND;
        break;
    }
    case NB_STATE_SWITCHING_BAND:
    {
        modem_status = _modem->waitForResponse(300);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
        {
            _state = NB_STATE_BAND_SWITCHED;
        }
        break;
    }
    case NB_STATE_BAND_SWITCHED:
    {
        _modem->send("AT+CFUN=1");
        _state = NB_STATE_SETTING_FULL_FUNCTIONALITY;
        break;
    }
    case NB_STATE_SETTING_FULL_FUNCTIONALITY:
    {
        modem_status = _modem->waitForResponse(300);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
        {
            _state = NB_STATE_FULL_FUNCTIONALITY_SET;
        }
        break;
    }
    case NB_STATE_FULL_FUNCTIONALITY_SET:
    {
        _modem->send("AT+CGATT=1");
        _state = NB_STATE_TRIGERRING_NETWORK_ATTACHMENT;
        break;
    }
    case NB_STATE_TRIGERRING_NETWORK_ATTACHMENT:
    {
        modem_status = _modem->waitForResponse(300);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
        {
            _state = NB_STATE_NETWORK_ATTACHMENT_TRIGGERED;
        }
        break;
    }
    case NB_STATE_NETWORK_ATTACHMENT_TRIGGERED:
    {
        _modem->send("AT+CEREG?");
        _state = NB_STATE_VERIFYING_NETWORK_REGISTRATION;
        break;
    }
    case NB_STATE_VERIFYING_NETWORK_REGISTRATION:
    {
        String resp;
        resp.reserve(20);
        modem_status = _modem->waitForResponse(300, &resp);
        if (modem_status == MODEM_STATUS_VALID_RESPONSE)
        {
            int indexOfComma = resp.indexOf(",") + 1;
            int code = resp.substring(indexOfComma, indexOfComma + 1).toInt();
            switch (code)
            {
            case 1:
            {
                _state = NB_STATE_NETWORK_VERIFIED;
                _networkStatus = NB_NETWORK_REGISTERED;
                break;
            }
            case 2:
            {
                _networkStatus = NB_NETWORK_SEARCHING_FOR_OPERATOR;
                break;
            }
            case 3:
            {
                _networkStatus = NB_NETWORK_REGSTRATION_DENIED;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    default:
        break;
    }
    return _networkStatus;
}