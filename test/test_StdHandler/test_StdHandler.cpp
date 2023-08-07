#include <gtest/gtest.h>
#include "setup.h"
#include <ctype.h>
#include <cstring>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

Modem *modem = nullptr;

#define STD_HANDLER_IMSI_LENGTH 16

typedef struct
{
    int8_t fun;
} CFUN_t;

typedef struct
{
    char imsi[STD_HANDLER_IMSI_LENGTH];
} CIMI_t;

typedef struct
{
    int8_t n;
    int8_t stat;
} CEREG_t;
class StdHandler : public ModemResponseHandler
{
public:
    StdHandler()
    {
        m_cfun.fun = -1;
        memset(m_cimi.imsi, 0, STD_HANDLER_IMSI_LENGTH);
        memset(&m_cereg, -1, sizeof(CEREG_t));
    }
    ~StdHandler(){};
    void onReceive(String &_response) override
    {
        if (_response.indexOf("+CFUN:") != -1)
        {
            m_cfun.fun = _response.charAt(_response.indexOf(":") + 1) - '0';
        }
        else if (_response.indexOf("+CEREG:") != -1)
        {
            m_cereg.n = _response.charAt(_response.indexOf(":") + 1) - '0';
            m_cereg.stat = _response.charAt(_response.indexOf(",") + 1) - '0';
        }
        else if (isdigit(_response.charAt(0)) && (_response.length() == (STD_HANDLER_IMSI_LENGTH - 1)))
        {
            memcpy(m_cimi.imsi, _response.c_str(), STD_HANDLER_IMSI_LENGTH);
        }
    }
    void getCFUN(CFUN_t *_result)
    {
        *_result = m_cfun;
    }
    void getCIMI(CIMI_t *_result)
    {
        memcpy(_result->imsi, m_cimi.imsi, STD_HANDLER_IMSI_LENGTH);
    }
    void getCEREG(CEREG_t *_result)
    {
        *_result = m_cereg;
    }

private:
    CFUN_t m_cfun;
    CIMI_t m_cimi;
    CEREG_t m_cereg;
};

class StdHandlerTest : public ::testing::Test
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

TEST_F(StdHandlerTest, handleCFUN)
{
    modem = modemBuilder->buildModem();
    StdHandler stdHandler;
    modem->addResponseHandler(&stdHandler);
    testSupport.putRxBuffer("\r\n+CFUN:0\r\nOK\r\n");

    CFUN_t cfun;
    stdHandler.getCFUN(&cfun);
    EXPECT_LT(cfun.fun, 0);

    modem->waitForResponse(1000);
    stdHandler.getCFUN(&cfun);
    EXPECT_EQ(cfun.fun, 0);
}

TEST_F(StdHandlerTest, handleCIMI)
{
    Modem *modem = modemBuilder->buildModem();
    modem = modemBuilder->buildModem();
    StdHandler stdHandler;
    modem->addResponseHandler(&stdHandler);

    testSupport.putRxBuffer("\r\n460111174590523\r\nOK\r\n");

    CIMI_t cimi;
    stdHandler.getCIMI(&cimi);
    EXPECT_EQ(strlen(cimi.imsi), 0);

    modem->waitForResponse(1000);
    stdHandler.getCIMI(&cimi);
    EXPECT_STREQ(cimi.imsi, "460111174590523");
}

TEST_F(StdHandlerTest, handleCEREG)
{
    Modem *modem = modemBuilder->buildModem();
    modem = modemBuilder->buildModem();
    StdHandler stdHandler;
    modem->addResponseHandler(&stdHandler);
    testSupport.putRxBuffer("\r\n+CEREG:0,1\r\nOK\r\n");

    CEREG_t cereg;
    stdHandler.getCEREG(&cereg);
    EXPECT_LT(cereg.n, 0);
    EXPECT_LT(cereg.stat, 0);

    modem->waitForResponse(1000);
    stdHandler.getCEREG(&cereg);
    EXPECT_EQ(cereg.n, 0);
    EXPECT_EQ(cereg.stat, 1);
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