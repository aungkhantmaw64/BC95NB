#include <gtest/gtest.h>
#include <ArduinoFake.h>

#define DEFAULT_RESET_PIN 4

using namespace fakeit;

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
    void setupMocks(void)
    {
        When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
        When(Method(ArduinoFake(), delay)).AlwaysReturn();

        When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *))).AlwaysReturn();
    }
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

    modem->send(cmd);

    Verify(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *)).Using(cmd));
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