#include <gtest/gtest.h>
#include <ArduinoFake.h>

#define DEFAULT_RESET_PIN 4

using namespace fakeit;

#include <stdarg.h>

#define MODEM_CMD_MAX_SIZE 100

enum ResponseCode
{
    EMPTY,
    OK,
    ERROR,
    CME_ERROR
};

class Modem
{
public:
    Modem(Stream &_stream, uint8_t _resetPin, bool _activeHigh)
        : m_stream(_stream),
          m_resetPin(_resetPin),
          m_activeHigh(_activeHigh){

          };
    void reset()
    {
        digitalWrite(m_resetPin, m_activeHigh);
        delay(200);
        digitalWrite(m_resetPin, !m_activeHigh);
        delay(200);
    };
    void send(const char *cmd)
    {
        m_stream.print(cmd);
    }
    ResponseCode waitForResponse(uint32_t timeoutMs)
    {
        String response;
        ResponseCode code = ResponseCode::EMPTY;
        uint32_t startMillis = millis();
        while (millis() - startMillis < timeoutMs)
        {
            if (m_stream.available())
            {
                char ch = m_stream.read();
                response += ch;
                if (ch == '\n')
                {
                    if (response.indexOf("OK") != -1)
                        code = ResponseCode::OK;
                    else if (response.indexOf("+CME") != -1)
                        code = ResponseCode::CME_ERROR;
                    else if (response.indexOf("ERROR") != -1)
                        code = ResponseCode::ERROR;
                    response = "";
                }
            }
        }
        return code;
    }

private:
    Stream &m_stream;
    uint8_t m_resetPin;
    bool m_activeHigh;
};

class ModemBuilder
{
public:
    Modem *buildModem()
    {
        return new Modem(*m_stream, m_resetPin, m_activeHigh);
    }
    ModemBuilder *setResetPin(uint8_t _resetPin)
    {
        m_resetPin = _resetPin;
        return this;
    }
    ModemBuilder *setActiveHigh(bool _activeHigh)
    {
        m_activeHigh = _activeHigh;
        return this;
    }

private:
    Stream *m_stream = ArduinoFakeMock(Stream);
    uint8_t m_resetPin = DEFAULT_RESET_PIN;
    bool m_activeHigh = HIGH;
};

ModemBuilder *modemBuilder = nullptr;

class TestSupport
{
public:
    const uint8_t RESET_PIN = 14;
    void setClock(uint32_t _startTime, uint32_t _timeStep)
    {
        m_startTime = _startTime;
        m_timeStep = _timeStep;
    }
    uint32_t getTicks(void)
    {
        unsigned long currentTime = m_startTime;
        m_startTime += m_timeStep;
        return currentTime;
    }
    void putRxBuffer(const char *_str)
    {
        m_rxBuffer = String(_str);
    }
    String getRxBuffer(void)
    {
        return m_rxBuffer;
    }
    char popRxBuffer(void)
    {
        char chr = m_rxBuffer.charAt(0);
        m_rxBuffer.remove(0, 1); // remove(0) doesn't work
        return chr;
    }
    void setupMocks(void)
    {
        // Digital I/O
        When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

        // Stream and serial communications
        When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *)))
            .AlwaysDo([this](const char *str) -> int
                      { m_txBuffer = String(str);
                        return strlen(str); });
        When(Method(ArduinoFake(Stream), read))
            .AlwaysDo([this]() -> char
                      { return popRxBuffer(); });
        When(Method(ArduinoFake(Stream), available))
            .AlwaysDo([this]() -> int
                      { return m_rxBuffer.length(); });

        // Time services
        When(Method(ArduinoFake(), delay)).AlwaysReturn();

        When(Method(ArduinoFake(), millis))
            .AlwaysDo([this]() -> unsigned long
                      { return getTicks(); });
    }
    void reset(void)
    {
        m_txBuffer = "";
        m_rxBuffer = "";
        m_startTime = 0;
        m_timeStep = 1;
    }

private:
    String m_txBuffer;
    String m_rxBuffer;
    uint32_t m_startTime = 0;
    uint32_t m_timeStep = 1;
};

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