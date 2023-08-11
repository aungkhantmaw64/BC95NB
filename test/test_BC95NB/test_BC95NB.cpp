#include "setup.h"
#include <gtest/gtest.h>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

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

static void BC95NB_BEGIN_QUERY_TEST(const char *_expectedCmd, BC95NBState _givenState, BC95NBState _expectedState)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NB nb(modem);
    nb.setState(_givenState);
    BC95NBState state = nb.begin();
    EXPECT_EQ(state, _expectedState);
    EXPECT_STREQ(testSupport.getTxBuffer().c_str(), _expectedCmd);
}

static BC95NB *BC95NB_BEGIN_QUERY_RESPONSE_TEST(const char *_givenResponse, BC95NBState _givenState, BC95NBState _expectedState)
{
    std::cout << "When receiving: "
              << _givenResponse
              << std::endl;
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer(_givenResponse);
    BC95NB *nb = new BC95NB(modem);
    nb->setState(_givenState);

    BC95NBState state = nb->begin();

    EXPECT_EQ(state, _expectedState);
    return nb;
}

TEST_F(BC95NBTest, begin_ShouldReturnQUERY_RF_FUNC)
{
    Modem *modem = modemBuilder->buildModem();
    BC95NB nb(modem);
    EXPECT_EQ(nb.getState(), BC95NBState::QUERY_RF_FUNC);
}

TEST_F(BC95NBTest, begin_ShouldReturnWAIT_QUERY_RF_FUNC_RESPONSE)
{
    const char *expectedCmd = "AT+CFUN?\r\n";
    BC95NBState givenState = BC95NBState::QUERY_RF_FUNC;
    BC95NBState expectedState = BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE;
    BC95NB_BEGIN_QUERY_TEST(expectedCmd, givenState, expectedState);
}

TEST_F(BC95NBTest, begin_ShouldReturnQUERY_IMSI)
{
    const char *givenResponse = "\r\n+CFUN:1\r\n\r\nOK\r\n";
    BC95NBState givenState = BC95NBState::WAIT_QUERY_RF_FUNC_RESPONSE;
    BC95NBState expectedState = BC95NBState::QUERY_IMSI;
    BC95NB *nb = BC95NB_BEGIN_QUERY_RESPONSE_TEST(givenResponse, givenState, expectedState);
    delete nb;
}

TEST_F(BC95NBTest, begin_ShouldReturnWAIT_QUERY_IMSI_RESPONSE)
{
    const char *expectedCmd = "AT+CIMI\r\n";
    BC95NBState givenState = BC95NBState::QUERY_IMSI;
    BC95NBState expectedState = BC95NBState::WAIT_QUERY_IMSI_RESPONSE;
    BC95NB_BEGIN_QUERY_TEST(expectedCmd, givenState, expectedState);
}

TEST_F(BC95NBTest, begin_ShouldReturnQUERY_NET_REGISTRATION)
{
    const char *givenResponse = "\r\n460111174590523\r\n\r\nOK\r\n";
    BC95NBState givenState = BC95NBState::WAIT_QUERY_IMSI_RESPONSE;
    BC95NBState expectedState = BC95NBState::QUERY_NET_REGISTRATION;
    BC95NB *nb = BC95NB_BEGIN_QUERY_RESPONSE_TEST(givenResponse, givenState, expectedState);

    char imsi[STD_MODEM_MAX_IMSI_LENGTH];
    nb->getIMSI(imsi);
    EXPECT_STREQ("460111174590523", imsi);
    delete nb;
}

TEST_F(BC95NBTest, begin_ShouldReturnWAIT_QUERY_NET_REGISTRATION_RESPONSE)
{
    const char *expectedCmd = "AT+CEREG?\r\n";
    BC95NBState givenState = BC95NBState::QUERY_NET_REGISTRATION;
    BC95NBState expectedState = BC95NBState::WAIT_QUERY_NET_REGISTRATION_RESPONSE;
    BC95NB_BEGIN_QUERY_TEST(expectedCmd, givenState, expectedState);
}

TEST_F(BC95NBTest, begin_ShouldReturnQUERY_NET_ATTACHMENT)
{
    const char *givenResponse = "\r\n+CEREG:0,1\r\n\r\nOK\r\n";
    BC95NBState givenState = BC95NBState::WAIT_QUERY_NET_REGISTRATION_RESPONSE;
    BC95NBState expectedState = BC95NBState::QUERY_NET_ATTACHMENT;
    BC95NB *nb = BC95NB_BEGIN_QUERY_RESPONSE_TEST(givenResponse, givenState, expectedState);
    delete nb;
}

TEST_F(BC95NBTest, begin_ShouldReturnWAIT_QUERY_NET_ATTACHMENT_RESPONSE)
{
    const char *expectedCmd = "AT+CGATT?\r\n";
    BC95NBState givenState = BC95NBState::QUERY_NET_ATTACHMENT;
    BC95NBState expectedState = BC95NBState::WAIT_QUERY_NET_ATTACHMENT_RESPONSE;
    BC95NB_BEGIN_QUERY_TEST(expectedCmd, givenState, expectedState);
}

TEST_F(BC95NBTest, begin_ShouldReturnQUERY_IP_ADDR)
{
    const char *givenResponse = "\r\n+CGATT:1\r\n\r\nOK\r\n";
    BC95NBState givenState = BC95NBState::WAIT_QUERY_NET_ATTACHMENT_RESPONSE;
    BC95NBState expectedState = BC95NBState::QUERY_IP_ADDR;
    BC95NB *nb = BC95NB_BEGIN_QUERY_RESPONSE_TEST(givenResponse, givenState, expectedState);
    delete nb;
}

TEST_F(BC95NBTest, begin_ShouldReturnWAIT_QUERY_IP_ADDR_RESPONSE)
{
    const char *expectedCmd = "AT+CGPADDR\r\n";
    BC95NBState givenState = BC95NBState::QUERY_IP_ADDR;
    BC95NBState expectedState = BC95NBState::WAIT_QUERY_IP_ADDR_RESPONSE;
    BC95NB_BEGIN_QUERY_TEST(expectedCmd, givenState, expectedState);
}

TEST_F(BC95NBTest, begin_ShouldReturnNET_READY)
{
    const char *givenResponse = "\r\n+CGPADDR:0,10.169.241.248\r\n\r\nOK\r\n";
    BC95NBState givenState = BC95NBState::WAIT_QUERY_IP_ADDR_RESPONSE;
    BC95NBState expectedState = BC95NBState::NET_READY;
    BC95NB *nb = BC95NB_BEGIN_QUERY_RESPONSE_TEST(givenResponse, givenState, expectedState);

    char ipAddress[30];
    nb->getIpAddress(ipAddress);
    EXPECT_STREQ("10.169.241.248", ipAddress);
    delete nb;
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