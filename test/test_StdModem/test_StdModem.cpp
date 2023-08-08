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
    }
};

TEST_F(StdModemTest, parseCFUN)
{
    Modem *modem = modemBuilder->buildModem();
    StdModem stdModem(*modem);
    testSupport.putRxBuffer("\r\n+CFUN:1\r\nOK\r\n");

    CFUN_t cfun;
    stdModem.getCFUN(&cfun);
    EXPECT_EQ(cfun.fun, 1);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CFUN?\r\n");
}

TEST_F(StdModemTest, parseCIMI)
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