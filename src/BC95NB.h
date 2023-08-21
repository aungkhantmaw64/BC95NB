#ifndef BC95NB_SRC_BC95NB_H
#define BC95NB_SRC_BC95NB_H

#include "StdModem.h"

/**
 * @brief Used to indicate the state of network communication
 *
 */
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
    /**
     * @brief Get the current state of the network connection
     *
     * @return BC95NBState
     */
    BC95NBState getState();
    /**
     * @brief Set the current state of the network connection
     *
     * @param _state
     */
    void setState(BC95NBState _state);
    /**
     * @brief Establish a connection with the network.
     *
     * @return BC95NBState
     */
    BC95NBState begin();
    /**
     * @brief Get IMSI of the modem
     *
     * @param _imsi
     */
    void getIMSI(char *_imsi);
    /**
     * @brief Get the Ip Addressof the modem
     *
     * @param _ipAddr
     */
    void getIpAddress(char *_ipAddr);

private:
    BC95NBState m_state;
    StdModem m_stdModem;

    char m_imsi[STD_MODEM_MAX_IMSI_LENGTH];
    char m_ipAddress[STD_MODEM_MAX_IP_ADDR_LENGTH];
};

#endif