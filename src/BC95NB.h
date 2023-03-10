#ifndef D_BC95NB_H
#define D_BC95NB_H

#include "BC95.h"

enum
{
    NB_STATE_DISABLING_AUTOCONECT,
    NB_STATE_AUTOCONNECT_DISABLED,
    NB_STATE_REBOOTING_MODEM,
    NB_STATE_MODEM_REBOOTED,
    NB_STATE_SWITCHING_BAND,
    NB_STATE_BAND_SWITCHED,
    NB_STATE_SETTING_FULL_FUNCTIONALITY,
    NB_STATE_FULL_FUNCTIONALITY_SET,
    NB_STATE_TRIGERRING_NETWORK_ATTACHMENT,
    NB_STATE_NETWORK_ATTACHMENT_TRIGGERED,
    NB_STATE_VERIFYING_NETWORK_REGISTRATION,
    NB_STATE_NETWORK_VERIFIED
};

enum
{
    NB_NETWORK_REGISTERED,
    NB_NETWORK_SEARCHING_FOR_OPERATOR,
    NB_NETWORK_REGSTRATION_DENIED,
    NB_NETWORK_NOT_REGISTERED
};

class NBClass
{
public:
    NBClass();
    NBClass(Modem *modem);
    ~NBClass();
    int begin(int band);

private:
    Modem *_modem;
    int _state;
    int _networkStatus;
};

extern NBClass BC95NB;
#endif