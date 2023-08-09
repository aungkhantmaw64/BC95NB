#include "setup.h"
#include <gtest/gtest.h>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

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