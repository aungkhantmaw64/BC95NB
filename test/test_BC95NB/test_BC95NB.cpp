#include "setup.h"
#include <gtest/gtest.h>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

enum class BC95NBState
{
    QUERY_RF_FUNC,
    WAIT_QUERY_RF_FUNC_RESPONSE
};

class BC95NB
{
public:
    BC95NB(Modem &modem)
        : m_state(BC95NBState::QUERY_RF_FUNC){};
    ~BC95NB(){};
    BC95NBState getState()
    {
        return m_state;
    }
    BC95NBState begin()
    {
        switch (m_state)
        {
        case BC95NBState::QUERY_RF_FUNC:
        {

            break;
        }
        case BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE:
        {
            break;
        }
        }
        return m_state;
    }

private:
    BC95NBState m_state;
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

TEST_F(BC95NBTest, beginsWithQueryRadioFunctionality)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NB nb(*modem);
    EXPECT_EQ(nb.getState(), BC95NBState::QUERY_RF_FUNC);
}

TEST_F(BC95NBTest, entersWaitForQueryRadioFunctionalityResponse)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NB nb(*modem);
    BC95NBState state = nb.begin();
    EXPECT_EQ(state, BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE);
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