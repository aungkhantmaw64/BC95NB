#include "StdModem.h"

StdModem::StdModem()
{
}

StdModem::~StdModem()
{
}

void StdModem::attach(Modem *_modem)
{
    m_modem = _modem;
}

void StdModem::readCmd(STD_AtCmd _cmd)
{
    switch (_cmd)
    {
    case STD_AtCmd::CFUN:
    {
        m_modem->send("AT+CFUN?\r\n");
        break;
    }
    case STD_AtCmd::CIMI:
    {
        m_modem->send("AT+CIMI\r\n");
        break;
    }
    case STD_AtCmd::CEREG:
    {
        m_modem->send("AT+CEREG?\r\n");
        break;
    }
    case STD_AtCmd::CGATT:
    {
        m_modem->send("AT+CGATT?\r\n");
        break;
    }
    case STD_AtCmd::CGPADDR:
    {
        m_modem->send("AT+CGPADDR\r\n");
        break;
    }
    default:
        break;
    }
}

void StdModem::wait(uint32_t _timeoutMs, STD_AtCmd _cmd)
{
    String response;
    ResponseCode retCode = m_modem->waitForResponse(_timeoutMs, &response);
    response.trim();
    if (retCode == ResponseCode::OK)
    {
        switch (_cmd)
        {
        case STD_AtCmd::CFUN:
        {
            if (response.indexOf("+CFUN") != -1)
                m_cfun.fun = response.charAt(response.indexOf(":") + 1) - '0';
            else
                m_cfun.fun = -1;
            break;
        }
        case STD_AtCmd::CIMI:
        {
            response.replace("\r\n", "");
            response.trim();
            if (response.indexOf("AT+CIMI") != -1)
            {
                response.replace("AT+CIMI", "");
            }
            if (isdigit(response.charAt(0)) && (retCode == ResponseCode::OK))
            {
                response.remove(STD_MODEM_MAX_IMSI_LENGTH - 1, response.length() - 1);
                strncpy(m_cimi.imsi, response.c_str(), STD_MODEM_MAX_IMSI_LENGTH);
            }
            break;
        }
        case STD_AtCmd::CEREG:
        {
            int8_t n_index = response.indexOf("+CEREG:") + strlen("+CEREG:");
            int8_t stat_index = response.indexOf(",", n_index) + 1;
            m_cereg.n = response.charAt(n_index) - '0';
            m_cereg.stat = response.charAt(stat_index) - '0';
            break;
        }
        case STD_AtCmd::CGATT:
        {
            int8_t state_index = response.indexOf("+CGATT:") + strlen("+CGATT:");
            m_cgatt.state = response.charAt(state_index) - '0';
        }
        case STD_AtCmd::CGPADDR:
        {
            int8_t cid_index = response.indexOf("+CGPADDR") + strlen("+CGPADDR:");
            int8_t ipaddr_start = response.indexOf(",", cid_index) + 1;
            int8_t ipaddr_end = ipaddr_start;
            while (isdigit(response.charAt(ipaddr_end)) || (response.charAt(ipaddr_end) == '.'))
            {
                ipaddr_end++;
            }
            m_cgpaddr.cid = response.charAt(cid_index) - '0';
            strcpy(m_cgpaddr.ipaddr, response.substring(ipaddr_start, ipaddr_end).c_str());
        }
        default:
            break;
        }
    }
}

void StdModem::getCFUN(CFUN_t *_result)
{
    *_result = m_cfun;
}

void StdModem::getCIMI(CIMI_t *_result)
{
    strncpy(_result->imsi, m_cimi.imsi, STD_MODEM_MAX_IMSI_LENGTH);
}

void StdModem::getCEREG(CEREG_t *_result)
{
    *_result = m_cereg;
}
void StdModem::getCGATT(CGATT_t *_result)
{
    *_result = m_cgatt;
}
void StdModem::getCGPADDR(CGPADDR_t *_result)
{
    *_result = m_cgpaddr;
}