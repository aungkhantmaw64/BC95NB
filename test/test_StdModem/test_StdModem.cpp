#include <gtest/gtest.h>
#include "setup.h"
#include <vector>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

#define STD_MODEM_MAX_IMSI_LENGTH 16

enum class ModemCMD
{
    CFUN,
    CIMI,
    CEREG,
    CGATT
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

} CGATT_t;
class StdModem
{
public:
    StdModem(Modem &_modem)
        : m_modem(_modem){

          };
    ~StdModem(){};

    void readCmd(ModemCMD cmd)
    {
        switch (cmd)
        {
        case ModemCMD::CFUN:
        {
            m_modem.send("AT+CFUN?\r\n");
            break;
        }
        case ModemCMD::CIMI:
        {
            m_modem.send("AT+CIMI\r\n");
            break;
        }
        case ModemCMD::CEREG:
        {
            m_modem.send("AT+CEREG?\r\n");
            break;
        }
        case ModemCMD::CGATT:
        {
            m_modem.send("AT+CGATT?\r\n");
            break;
        }
        default:
            break;
        }
    }
    void wait(uint32_t timeoutMs, ModemCMD cmd)
    {
        String response;
        ResponseCode retCode = m_modem.waitForResponse(timeoutMs, &response);
        if (retCode == ResponseCode::OK)
        {
            switch (cmd)
            {
            case ModemCMD::CFUN:
            {
                if (response.indexOf("+CFUN") != -1)
                    m_cfun.fun = response.charAt(response.indexOf(":") + 1) - '0';
                else
                    m_cfun.fun = -1;
                break;
            }
            case ModemCMD::CIMI:
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
            case ModemCMD::CEREG:
            {
                int8_t n_index = response.indexOf("+CEREG:") + strlen("+CEREG:");
                int8_t stat_index = response.indexOf(",", n_index) + 1;
                m_cereg.n = response.charAt(n_index) - '0';
                m_cereg.stat = response.charAt(stat_index) - '0';
                break;
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

private:
    Modem &m_modem;
    CFUN_t m_cfun;
    CEREG_t m_cereg;
    CIMI_t m_cimi;
    CGATT_t m_cgatt;
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
    StdModem stdModem(*modem);
    testSupport.putRxBuffer("\r\n+CFUN:1\r\nOK\r\n");
    stdModem.readCmd(ModemCMD::CFUN);
    stdModem.wait(300, ModemCMD::CFUN);
    CFUN_t cfun;
    stdModem.getCFUN(&cfun);

    EXPECT_EQ(cfun.fun, 1);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CFUN?\r\n");
}

TEST_F(StdModemTest, getCIMI)
{
    Modem *modem = modemBuilder->buildModem();
    StdModem stdModem(*modem);
    testSupport.putRxBuffer("\r\n460111174590523\r\nOK\r\n");
    stdModem.readCmd(ModemCMD::CIMI);
    stdModem.wait(300, ModemCMD::CIMI);
    CIMI_t cimi;
    memset(cimi.imsi, 0, STD_MODEM_MAX_IMSI_LENGTH);
    stdModem.getCIMI(&cimi);

    EXPECT_STREQ(cimi.imsi, "460111174590523");
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CIMI\r\n");
}

TEST_F(StdModemTest, getCEREG)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n+CEREG:0,1\r\nOK\r\n");

    StdModem stdModem(*modem);
    stdModem.readCmd(ModemCMD::CEREG);
    stdModem.wait(300, ModemCMD::CEREG);
    CEREG_t cereg;
    stdModem.getCEREG(&cereg);

    EXPECT_EQ(cereg.n, 0);
    EXPECT_EQ(cereg.stat, 1);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CEREG?\r\n");
}

TEST_F(StdModemTest, getCGATT)
{
    CGATT_t cgatt;
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