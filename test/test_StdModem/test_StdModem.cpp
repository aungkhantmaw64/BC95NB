#include <gtest/gtest.h>
#include "setup.h"
#include <vector>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

#define STD_MODEM_MAX_IMSI_LENGTH 16
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
class StdModem
{
public:
    StdModem(Modem &_modem)
        : m_modem(_modem){

          };
    ~StdModem(){};

    void getCFUN(CFUN_t *_result)
    {
        String response;
        m_modem.send("AT+CFUN?\r\n");
        m_modem.waitForResponse(300, &response);
        if (response.indexOf("+CFUN") != -1)
            _result->fun = response.charAt(response.indexOf(":") + 1) - '0';
        else
            _result->fun = -1;
    }

    void getCIMI(CIMI_t *_result)
    {
        String response;
        m_modem.send("AT+CIMI\r\n");
        ResponseCode retCode = m_modem.waitForResponse(300, &response);
        response.replace("\r\n", "");
        response.trim();
        if (isdigit(response.charAt(0)) && (retCode == ResponseCode::OK))
        {
            response.remove(STD_MODEM_MAX_IMSI_LENGTH - 1, response.length() - 1);
            strncpy(_result->imsi, response.c_str(), STD_MODEM_MAX_IMSI_LENGTH);
        }
    }

    void getCEREG(CEREG_t *_result)
    {
        String response;
        m_modem.send("AT+CEREG?\r\n");
        ResponseCode retCode = m_modem.waitForResponse(300, &response);
        if (retCode == ResponseCode::OK)
        {
            int8_t n_index = response.indexOf("+CEREG:") + strlen("+CEREG:");
            int8_t stat_index = response.indexOf(",", n_index) + 1;
            _result->n = response.charAt(n_index) - '0';
            _result->stat = response.charAt(stat_index) - '0';
        }
        else
        {
            _result->n = -1;
            _result->stat = -1;
        }
    }

private:
    Modem &m_modem;
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

    CEREG_t cereg;
    StdModem stdModem(*modem);
    stdModem.getCEREG(&cereg);
    EXPECT_EQ(cereg.n, 0);
    EXPECT_EQ(cereg.stat, 1);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CEREG?\r\n");
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