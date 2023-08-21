#include "BC95NB.h"

BC95NB::BC95NB(Modem *modem)
    : m_state(BC95NBState::QUERY_RF_FUNC)
{
    m_stdModem.attach(modem);
    memset(m_imsi, 0, STD_MODEM_MAX_IMSI_LENGTH);
}

BC95NB::~BC95NB()
{
}

BC95NBState BC95NB::getState()
{
    return m_state;
}

void BC95NB::setState(BC95NBState _state)
{
    m_state = _state;
}

BC95NBState BC95NB::begin()
{
    switch (m_state)
    {
    case BC95NBState::QUERY_RF_FUNC:
    {
        m_stdModem.readCmd(STD_AtCmd::CFUN);
        m_state = BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE;
        break;
    }
    case BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE:
    {
        m_stdModem.wait(300, STD_AtCmd::CFUN);
        CFUN_t cfun;
        m_stdModem.getCFUN(&cfun);
        if (cfun.fun == 1)
            m_state = BC95NBState::QUERY_IMSI;
        else
            m_state = BC95NBState::QUERY_RF_FUNC;
        break;
    }
    case BC95NBState::QUERY_IMSI:
    {
        m_stdModem.readCmd(STD_AtCmd::CIMI);
        m_state = BC95NBState::WAIT_QUERY_IMSI_RESPONSE;
        break;
    }
    case BC95NBState::WAIT_QUERY_IMSI_RESPONSE:
    {
        m_stdModem.wait(300, STD_AtCmd::CIMI);
        CIMI_t cimi;
        m_stdModem.getCIMI(&cimi);
        if (cimi.imsi[0] != 0)
        {
            strncpy(m_imsi, cimi.imsi, STD_MODEM_MAX_IMSI_LENGTH);
            m_state = BC95NBState::QUERY_NET_REGISTRATION;
        }
        break;
    }
    case BC95NBState::QUERY_NET_REGISTRATION:
    {
        m_stdModem.readCmd(STD_AtCmd::CEREG);
        m_state = BC95NBState::WAIT_QUERY_NET_REGISTRATION_RESPONSE;
        break;
    }
    case BC95NBState::WAIT_QUERY_NET_REGISTRATION_RESPONSE:
    {
        m_stdModem.wait(300, STD_AtCmd::CEREG);
        CEREG_t cereg;
        m_stdModem.getCEREG(&cereg);
        if (cereg.stat == 1)
            m_state = BC95NBState::QUERY_NET_ATTACHMENT;
        else
            m_state = BC95NBState::QUERY_NET_REGISTRATION;
        break;
    }
    case BC95NBState::QUERY_NET_ATTACHMENT:
    {
        m_stdModem.readCmd(STD_AtCmd::CGATT);
        m_state = BC95NBState::WAIT_QUERY_NET_ATTACHMENT_RESPONSE;
        break;
    }
    case BC95NBState::WAIT_QUERY_NET_ATTACHMENT_RESPONSE:
    {
        m_stdModem.wait(300, STD_AtCmd::CGATT);
        CGATT_t cgatt;
        m_stdModem.getCGATT(&cgatt);
        if (cgatt.state == 1)
            m_state = BC95NBState::QUERY_IP_ADDR;
        else
            m_state = BC95NBState::QUERY_NET_ATTACHMENT;
        break;
    }
    case BC95NBState::QUERY_IP_ADDR:
    {
        m_stdModem.readCmd(STD_AtCmd::CGPADDR);
        m_state = BC95NBState::WAIT_QUERY_IP_ADDR_RESPONSE;
        break;
    }
    case BC95NBState::WAIT_QUERY_IP_ADDR_RESPONSE:
    {
        m_stdModem.wait(300, STD_AtCmd::CGPADDR);
        CGPADDR_t cgpaddr;
        m_stdModem.getCGPADDR(&cgpaddr);
        if (cgpaddr.ipaddr[0] != 0)
        {
            m_state = BC95NBState::NET_READY;
            strcpy(m_ipAddress, cgpaddr.ipaddr);
        }
        break;
    }
    }
    return m_state;
}

void BC95NB::getIMSI(char *_imsi)
{
    strncpy(_imsi, m_imsi, STD_MODEM_MAX_IMSI_LENGTH);
}

void BC95NB::getIpAddress(char *_ipAddr)
{
    strcpy(_ipAddr, m_ipAddress);
}