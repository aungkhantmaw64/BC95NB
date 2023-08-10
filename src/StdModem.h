#ifndef BC95NB_SRC_STD_MODEM_H
#define BC95NB_SRC_STD_MODEM_H

#include "Modem.h"
#include <Arduino.h>

#define STD_MODEM_MAX_IMSI_LENGTH 16

enum class STD_AtCmd
{
    CFUN,
    CIMI,
    CEREG,
    CGATT,
    CGPADDR
};

typedef struct CFUN
{
    int8_t fun;
} CFUN_t;

typedef struct CIMI
{
    char imsi[STD_MODEM_MAX_IMSI_LENGTH];
} CIMI_t;

typedef struct CEREG
{
    int8_t n;
    int8_t stat;
} CEREG_t;

typedef struct CGATT
{
    uint8_t state;
} CGATT_t;

typedef struct CGPADDR
{
    uint8_t cid;
    char ipaddr[20];
} CGPADDR_t;

class StdModem
{
public:
    StdModem();
    ~StdModem();
    void attach(Modem *_modem);
    void readCmd(STD_AtCmd _cmd);
    void wait(uint32_t _timeoutMs, STD_AtCmd _cmd);
    void getCFUN(CFUN_t *_result);
    void getCIMI(CIMI_t *_result);
    void getCEREG(CEREG_t *_result);
    void getCGATT(CGATT_t *_result);
    void getCGPADDR(CGPADDR_t *_result);

private:
    Modem *m_modem;
    CFUN_t m_cfun;
    CEREG_t m_cereg;
    CIMI_t m_cimi;
    CGATT_t m_cgatt;
    CGPADDR_t m_cgpaddr;
};

#endif