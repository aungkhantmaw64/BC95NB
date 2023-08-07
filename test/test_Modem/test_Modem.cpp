#include <gtest/gtest.h>
#include "setup.h"
#include <vector>

ModemBuilder *modemBuilder = nullptr;

TestSupport testSupport;

class ModemTest : public ::testing::Test
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

class FooModemHandler : public ModemResponseHandler
{

public:
    String getResponse(int _index)
    {
        return m_responses[_index];
    }
    void onReceive(String _response) override
    {
        m_responses.push_back(_response);
    }

private:
    std::vector<String> m_responses;
};

static void VerifyReset(uint8_t resetPin, bool activeHigh)
{
    Verify(Method(ArduinoFake(), digitalWrite).Using(resetPin, activeHigh) +
           Method(ArduinoFake(), delay) +
           Method(ArduinoFake(), digitalWrite).Using(resetPin, !activeHigh) +
           Method(ArduinoFake(), delay));
}

TEST_F(ModemTest, RebootsDueToHardwareResetWithActiveHigh)
{
    Modem *modem = modemBuilder->setResetPin(testSupport.RESET_PIN)
                       ->setActiveHigh(HIGH)
                       ->buildModem();

    modem->reset();

    VerifyReset(testSupport.RESET_PIN, HIGH);
}

TEST_F(ModemTest, RebootsDueToHardwareResetWithActiveLow)
{
    Modem *modem = modemBuilder->setResetPin(testSupport.RESET_PIN)
                       ->setActiveHigh(LOW)
                       ->buildModem();

    modem->reset();

    VerifyReset(testSupport.RESET_PIN, LOW);
}

TEST_F(ModemTest, SendsACommandToSerialPort)
{

    const char *cmd = "AT";
    Modem *modem = modemBuilder->buildModem();

    modem->send("AT");

    Verify(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *)).Using(cmd));
}

TEST_F(ModemTest, ReturnEMPTYWhenWaitForResponse)
{
    Modem *modem = modemBuilder->buildModem();

    ResponseCode retCode = modem->waitForResponse(300);
    EXPECT_EQ(ResponseCode::EMPTY, retCode);
}

TEST(MockSerial, DoesWhatSerialShouldDo)
{
    Stream *stream = ArduinoFakeMock(Stream);
    testSupport.putRxBuffer("OK\r\nHELLO");
    ASSERT_STREQ("OK\r\nHELLO", testSupport.getRxBuffer().c_str());
    ASSERT_EQ(9, stream->available());
    ASSERT_EQ('O', stream->read());
    ASSERT_EQ(8, stream->available());
    ASSERT_EQ('K', stream->read());
    ASSERT_EQ(7, stream->available());
    ASSERT_EQ('\r', stream->read());
    ASSERT_EQ(6, stream->available());
    ASSERT_EQ('\n', stream->read());
    ASSERT_EQ(5, stream->available());
    ASSERT_EQ('H', stream->read());
    ASSERT_EQ(4, stream->available());
    ASSERT_EQ('E', stream->read());
    ASSERT_EQ(3, stream->available());
    ASSERT_EQ('L', stream->read());
    ASSERT_EQ(2, stream->available());
    ASSERT_EQ('L', stream->read());
    ASSERT_EQ(1, stream->available());
    ASSERT_EQ('O', stream->read());
    ASSERT_EQ(0, stream->available());
    ASSERT_EQ(0, stream->read());
    ASSERT_EQ(0, stream->available());
    ASSERT_EQ(0, stream->read());
}

TEST_F(ModemTest, ReturnOKWhenWaitForResponse)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\nOK\r\n");
    testSupport.setClock(0, 1);
    ResponseCode retCode = modem->waitForResponse(300);
    EXPECT_EQ(ResponseCode::OK, retCode);
}

TEST_F(ModemTest, ReturnERRORWhenWaitForResponse)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\nERROR\r\n");
    testSupport.setClock(0, 1);
    ResponseCode retCode = modem->waitForResponse(300);
    EXPECT_EQ(ResponseCode::ERROR, retCode);
}

TEST_F(ModemTest, ReturnCME_ERRORWhenWaitForResponse)
{
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\n+CME ERROR: 50\r\n");
    testSupport.setClock(0, 1);
    ResponseCode retCode = modem->waitForResponse(300);
    EXPECT_EQ(ResponseCode::CME_ERROR, retCode);
}

TEST_F(ModemTest, CallsASingleResponseHandlerWhenReceivedStringResponse)
{
    FooModemHandler modemHandler;
    Modem *modem = modemBuilder->buildModem();
    testSupport.putRxBuffer("\r\nRESP1\r\nRESP2\r\nOK\r\n");
    testSupport.setClock(0, 1);

    modem->addResponseHandler(&modemHandler);
    ResponseCode retCode = modem->waitForResponse(1000);
    EXPECT_EQ(ResponseCode::OK, retCode);
    EXPECT_STREQ("RESP1", modemHandler.getResponse(0).c_str());
    EXPECT_STREQ("RESP2", modemHandler.getResponse(1).c_str());
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