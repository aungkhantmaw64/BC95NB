#include <gtest/gtest.h>
#include "setup.h"
#include <vector>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

typedef struct CFUNStruct
{
    int8_t fun;
} CFUN_t;

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