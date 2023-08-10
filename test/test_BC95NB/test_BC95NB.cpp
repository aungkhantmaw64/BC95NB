#include "setup.h"
#include <gtest/gtest.h>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

enum class BC95NBState
{
    QUERY_RF_FUNC,
    WAIT_QUERY_RF_FUNC_RESPONSE,
    QUERY_IMSI,
    WAIT_QUERY_IMSI_RESPONSE
};

class BC95NB
{
public:
    BC95NB(Modem *modem)
        : m_state(BC95NBState::QUERY_RF_FUNC)
    {
        m_stdModem.attach(modem);
    };
    ~BC95NB(){};
    BC95NBState getState()
    {
        return m_state;
    }
    void setState(BC95NBState _state)
    {
        m_state = _state;
    }
    BC95NBState begin()
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
            break;
        }
        case BC95NBState::QUERY_IMSI:
        {
            m_stdModem.readCmd(STD_AtCmd::CIMI);
            m_state = BC95NBState::WAIT_QUERY_IMSI_RESPONSE;
            break;
        }
        }
        return m_state;
    }

private:
    BC95NBState m_state;
    StdModem m_stdModem;

    char m_imsi[STD_MODEM_MAX_IMSI_LENGTH];
};

class BC95NBTest : public ::testing::Test
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

TEST_F(BC95NBTest, begin_ShouldReturnQUERY_RF_FUNC)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NB nb(modem);
    EXPECT_EQ(nb.getState(), BC95NBState::QUERY_RF_FUNC);
}

TEST_F(BC95NBTest, begin_ShouldReturnWAIT_QUERY_RF_FUNC_RESPONSE)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NB nb(modem);
    BC95NBState state = nb.begin();
    EXPECT_EQ(state, BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), "AT+CFUN?\r\n");
}

TEST_F(BC95NBTest, begin_ShouldReturnQUERY_IMSI)
{
    std::cout << "When receiving: "
              << "\r\n+CFUN:1\r\n\r\nOK\r\n"
              << std::endl;
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n+CFUN:1\r\nOK\r\n");
    BC95NB nb(modem);
    nb.setState(BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE);
    BC95NBState state = nb.begin();
    EXPECT_EQ(state, BC95NBState::QUERY_IMSI);
}

TEST_F(BC95NBTest, begin_ShouldReturnWAIT_QUERY_IMSI_RESPONSE)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NB nb(modem);
    nb.setState(BC95NBState::QUERY_IMSI);
    BC95NBState state = nb.begin();
    EXPECT_EQ(state, BC95NBState::WAIT_QUERY_IMSI_RESPONSE);
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