#include <gtest/gtest.h>
#include "setup.h"
#include <vector>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

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
    StdModem(Modem &_modem)
        : m_modem(_modem){

          };
    ~StdModem(){};

    void readCmd(STD_AtCmd cmd)
    {
        switch (cmd)
        {
        case STD_AtCmd::CFUN:
        {
            m_modem.send("AT+CFUN?\r\n");
            break;
        }
        case STD_AtCmd::CIMI:
        {
            m_modem.send("AT+CIMI\r\n");
            break;
        }
        case STD_AtCmd::CEREG:
        {
            m_modem.send("AT+CEREG?\r\n");
            break;
        }
        case STD_AtCmd::CGATT:
        {
            m_modem.send("AT+CGATT?\r\n");
            break;
        }
        case STD_AtCmd::CGPADDR:
        {
            m_modem.send("AT+CGPADDR\r\n");
            break;
        }
        default:
            break;
        }
    }
    void wait(uint32_t timeoutMs, STD_AtCmd cmd)
    {
        String response;
        ResponseCode retCode = m_modem.waitForResponse(timeoutMs, &response);
        response.trim();
        if (retCode == ResponseCode::OK)
        {
            switch (cmd)
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
                int8_t state_index = response.indexOf("+CGATT") + strlen("+CGATT:");
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
    void getCFUN(CFUN_t *_result)
    {
        *_result = m_cfun;
    }

    void getCIMI(CIMI_t *_result)
    {
        strncpy(_result->imsi, m_cimi.imsi, STD_MODEM_MAX_IMSI_LENGTH);
    }

    void getCEREG(CEREG_t *_result)
    {
        *_result = m_cereg;
    }
    void getCGATT(CGATT_t *_result)
    {
        *_result = m_cgatt;
    }
    void getCGPADDR(CGPADDR_t *_result)
    {
        *_result = m_cgpaddr;
    }

private:
    Modem &m_modem;
    CFUN_t m_cfun;
    CEREG_t m_cereg;
    CIMI_t m_cimi;
    CGATT_t m_cgatt;
    CGPADDR_t m_cgpaddr;
};

class StdModemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ArduinoFakeReset();
        modemBuilder = new ModemBuilder();
        testSupport.setupMocks();
    }
    void TearDown() override
    {
        delete modemBuilder;
        testSupport.reset();
    }
};

TEST_F(StdModemTest, getCFUN)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n+CFUN:1\r\nOK\r\n");

    StdModem stdModem(*modem);
    CFUN_t cfun;

    stdModem.readCmd(STD_AtCmd::CFUN);
    stdModem.wait(300, STD_AtCmd::CFUN);
    stdModem.getCFUN(&cfun);

    EXPECT_EQ(cfun.fun, 1);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CFUN?\r\n");
}

TEST_F(StdModemTest, getCIMI)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n460111174590523\r\nOK\r\n");

    StdModem stdModem(*modem);
    CIMI_t cimi;

    stdModem.readCmd(STD_AtCmd::CIMI);
    stdModem.wait(300, STD_AtCmd::CIMI);

    memset(cimi.imsi, 0, STD_MODEM_MAX_IMSI_LENGTH);
    stdModem.getCIMI(&cimi);

    EXPECT_STREQ(cimi.imsi, "460111174590523");
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CIMI\r\n");
}

TEST_F(StdModemTest, getCEREG)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n+CEREG:0,5\r\nOK\r\n");

    StdModem stdModem(*modem);
    CEREG_t cereg;

    stdModem.readCmd(STD_AtCmd::CEREG);
    stdModem.wait(300, STD_AtCmd::CEREG);
    stdModem.getCEREG(&cereg);

    EXPECT_EQ(cereg.n, 0);
    EXPECT_EQ(cereg.stat, 5);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CEREG?\r\n");
}

TEST_F(StdModemTest, getCGATT)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n+CGATT:1\r\nOK\r\n");

    StdModem stdModem(*modem);
    CGATT_t cgatt;

    stdModem.readCmd(STD_AtCmd::CGATT);
    stdModem.wait(300, STD_AtCmd::CGATT);
    stdModem.getCGATT(&cgatt);

    EXPECT_EQ(cgatt.state, 1);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CGATT?\r\n");
}

TEST_F(StdModemTest, getCGPADDR)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n+CGPADDR:0,10.169.241.288\r\nOK\r\n");

    StdModem stdModem(*modem);
    CGPADDR_t cgpaddr;

    stdModem.readCmd(STD_AtCmd::CGPADDR);
    stdModem.wait(300, STD_AtCmd::CGPADDR);
    stdModem.getCGPADDR(&cgpaddr);

    EXPECT_EQ(cgpaddr.cid, 0);
    EXPECT_STREQ(cgpaddr.ipaddr, "10.169.241.288");
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CGPADDR\r\n");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
        ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}