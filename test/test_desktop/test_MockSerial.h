#include <ArduinoFake.h>
#include <unity.h>
#include "mocks/MockSerial.h"

using namespace fakeit;

namespace MockSerialTest
{
    void test_MockSerial_ClearTxBufferAfterCreate()
    {
        MockSerial mock_serial;

        TEST_ASSERT_EQUAL_STRING("", mock_serial.getTxBuffer().c_str());
    }

    void test_MockSerial_PutsDataOnTxBufferAfterPrint(void)
    {
        MockSerial mock_serial;

        mock_serial.begin();

        Stream *stream = ArduinoFakeMock(Stream);

        int totalPrintedBytes = 0;
        totalPrintedBytes = stream->print("Data");

        TEST_ASSERT_EQUAL(4, totalPrintedBytes);
        TEST_ASSERT_EQUAL_STRING("Data", mock_serial.getTxBuffer().c_str());
    }

    void test_MockSerial_CleanTxBuffer(void)
    {
        MockSerial mock_serial;
        Stream *stream = ArduinoFakeMock(Stream);

        mock_serial.begin();

        stream->print("Data");
        TEST_ASSERT_EQUAL_STRING("Data", mock_serial.getTxBuffer().c_str());

        mock_serial.cleanTxBuffer();

        TEST_ASSERT_EQUAL_STRING("", mock_serial.getTxBuffer().c_str());
    }

    void test_MockSerial_AppendsStringInBufferIfPrintedConsecutively(void)
    {
        MockSerial mock_serial;
        Stream *stream = ArduinoFakeMock(Stream);
        mock_serial.begin();
        stream->print("D");
        TEST_ASSERT_EQUAL_STRING("D", mock_serial.getTxBuffer().c_str());
        stream->print("A");
        TEST_ASSERT_EQUAL_STRING("DA", mock_serial.getTxBuffer().c_str());
        stream->print("T");
        TEST_ASSERT_EQUAL_STRING("DAT", mock_serial.getTxBuffer().c_str());
        stream->print("A");
        TEST_ASSERT_EQUAL_STRING("DATA", mock_serial.getTxBuffer().c_str());
    }

    void test_MockSerial_CleanRxBufferEmptyIfNotSet(void)
    {
        MockSerial mock_serial;

        TEST_ASSERT_EQUAL_STRING("", mock_serial.getRxBuffer().c_str());
    }

    void test_MockSerial_SetRxBufferToGivenData(void)
    {
    }
    void run_tests(void)
    {
        RUN_TEST(test_MockSerial_ClearTxBufferAfterCreate);
        RUN_TEST(test_MockSerial_PutsDataOnTxBufferAfterPrint);
        RUN_TEST(test_MockSerial_AppendsStringInBufferIfPrintedConsecutively);
        RUN_TEST(test_MockSerial_CleanRxBufferEmptyIfNotSet);
    }
}