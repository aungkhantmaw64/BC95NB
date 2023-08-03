#include <gtest/gtest.h>
#include <ArduinoFake.h>

#define DEFAULT_RESET_PIN 4

using namespace fakeit;

class Modem
{
public:
    Modem(uint8_t _resetPin, bool _activeHigh)
        : m_resetPin(_resetPin),
          m_activeHigh(_activeHigh){

          };
    void reset()
    {
        digitalWrite(m_resetPin, m_activeHigh);
        delay(200);
        digitalWrite(m_resetPin, !m_activeHigh);
        delay(200);
    };

private:
    uint8_t m_resetPin;
    bool m_activeHigh;
};

TEST(Modem, RebootsDueToHardwareResetWithActiveHigh)
{
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), delay)).AlwaysReturn();

    bool activeHigh = HIGH;
    const uint8_t resetPin = 12;
    Modem modem(resetPin, activeHigh);

    modem.reset();

    Verify(Method(ArduinoFake(), digitalWrite).Using(resetPin, HIGH) +
           Method(ArduinoFake(), delay) +
           Method(ArduinoFake(), digitalWrite).Using(resetPin, LOW) +
           Method(ArduinoFake(), delay));
}

TEST(Modem, RebootsDueToHardwareResetWithActiveLow)
{
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), delay)).AlwaysReturn();

    bool activeHigh = HIGH;
    const uint8_t resetPin = 12;
    Modem modem(resetPin, activeHigh);

    modem.reset();

    Verify(Method(ArduinoFake(), digitalWrite).Using(resetPin, LOW) +
           Method(ArduinoFake(), delay) +
           Method(ArduinoFake(), digitalWrite).Using(resetPin, HIGH) +
           Method(ArduinoFake(), delay));
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