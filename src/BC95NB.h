#ifndef D_BC95NB_H
#define D_BC95NB_H

#include "BC95.h"

enum
{
    NB_DISABLING_AUTOCONECT,
    NB_AUTOCONNECT_DISABLED,
    NB_REBOOTING_MODEM,
    NB_MODEM_REBOOTED,
    NB_SWITCHING_BAND,
    NB_BAND_SWITCHED,
    NB_SETTING_FULL_FUNCTIONALITY,
    NB_FULL_FUNCTIONALITY_SET,
    NB_REGISTERING_NETWORK,
    NB_NETWORK_REGISTERED
};

class NBClass
{
public:
    NBClass(Modem *modem);
    ~NBClass();
    int begin(int band);

private:
    Modem *_modem;
    int _state;
};

#endif