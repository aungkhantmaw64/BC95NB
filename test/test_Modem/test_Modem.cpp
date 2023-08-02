#include <gtest/gtest.h>

#define MODEM_DEFAULT_BAUDRATE 9600
template <typename Stream>
class Modem
{
public:
    Modem(Stream &_stream)
        : m_stream(_stream)
    {
    }
    Modem(Stream &_stream, uint8_t _resetPin, bool _activeHigh)
        : m_stream(_stream),
          m_baudrate(MODEM_DEFAULT_BAUDRATE){

          };
    void setBaudrate(uint32_t baud)
    {
        m_baudrate = baud;
    }
    void begin(void)
    {
        m_stream.begin(m_baudrate);
    }

private:
    Stream &m_stream;
    uint32_t m_baudrate;
};

class MockStream
{
public:
    MockStream(){};
    ~MockStream(){};
    uint32_t getBaudrate(void)
    {
        return m_baudrate;
    }
    void begin(uint32_t baud)
    {
        m_baudrate = baud;
    };

private:
    uint32_t m_baudrate;
};

TEST(Modem, SetsBaudrate)
{
    MockStream stream;
    Modem<MockStream> modem(stream);
    uint32_t baudrate = 9600;
    modem.setBaudrate(baudrate);
    modem.begin();
    EXPECT_EQ(baudrate, stream.getBaudrate())
        << "The stream's baudrate is not set to " << baudrate;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    if (RUN_ALL_TESTS())
        ;
    return 0;
}