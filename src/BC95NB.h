#ifndef D_BC95NB_H
#define D_BC95NB_H

#include "Modem.h"
#include "BC95.h"

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
    /**
     * @brief Construct a new NBClass object
     *
     * @param modem
     */
    NBClass(Modem *modem);
    /**
     * @brief Destroy the NBClass object
     *
     */
    ~NBClass();
    /**
     * @brief
     *
     * @return int
     */
    int begin();
    /**
     * @brief
     *
     * @param band
     * @return int
     */
    int begin(int band);
    /**
     * @brief
     *
     * @return String
     */
    String getIPAddress();
    /**
     * @brief
     *
     * @return String
     */
    String getIMSI();
    /**
     * @brief
     *
     * @return int
     */
    int reset(void);

private:
    Modem *modem_;
    int connectionState_;
    String buffer_;
    String ip_;
    String imsi_;
};

#endif
