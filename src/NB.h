#ifndef D_BC95NB_H
#define D_BC95NB_H

#include "Modem.h"

enum
{
    NB_IDLE,
    NB_MODEM_AWAKEN,
    NB_NETWORK_REGISTERED,
    NB_NETWORK_ATTACHED
};

class NBClass
{
public:
    NBClass(Modem *modem);
    ~NBClass();
    int begin();
    int begin(int band);
    String getIPAddress();
    String getIMSI();

private:
    Modem *modem_;
    int connectionState_;
    String buffer_;
    String ip_;
    String imsi_;
};

#endif
