#ifndef BC95NB_SRC_BC95NB_H
#define BC95NB_SRC_BC95NB_H

#include "StdModem.h"

enum class BC95NBState
{
    QUERY_RF_FUNC,
    WAIT_QUERY_RF_FUNC_RESPONSE,
    QUERY_IMSI,
    WAIT_QUERY_IMSI_RESPONSE,
    QUERY_NET_REGISTRATION,
    WAIT_QUERY_NET_REGISTRATION_RESPONSE,
    QUERY_NET_ATTACHMENT,
    WAIT_QUERY_NET_ATTACHMENT_RESPONSE,
    QUERY_IP_ADDR,
    WAIT_QUERY_IP_ADDR_RESPONSE,
    NET_READY
};

class BC95NB
{
public:
    BC95NB(Modem *modem);
    ~BC95NB();
    BC95NBState getState();
    void setState(BC95NBState _state);
    BC95NBState begin();
    void getIMSI(char *_imsi);
    void getIpAddress(char *_ipAddr);

private:
    BC95NBState m_state;
    StdModem m_stdModem;

    char m_imsi[STD_MODEM_MAX_IMSI_LENGTH];
    char m_ipAddress[STD_MODEM_MAX_IP_ADDR_LENGTH];
};

#endif