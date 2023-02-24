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

        TEST_ASSERT_EQUAL_STRING("", mock_serial.popRxBuffer().c_str());
    }

    void test_MockSerial_SetRxBufferToGivenData(void)
    {
        MockSerial mock_serial;
        mock_serial.addRxContents("OK");

        TEST_ASSERT_EQUAL_STRING("OK", mock_serial.popRxBuffer().c_str());
    }
    void test_MockSerial_ReducesRxBufferSizeByOneEverytimeStreamReadIsCalled(void)
    {
        MockSerial mock_serial;
        Stream *stream = ArduinoFakeMock(Stream);
        mock_serial.addRxContents("data");
        mock_serial.begin();
        TEST_ASSERT_EQUAL_CHAR('d', stream->read());
        TEST_ASSERT_EQUAL(3, mock_serial.popRxBuffer().length());
        TEST_ASSERT_EQUAL_CHAR('a', stream->read());
        TEST_ASSERT_EQUAL(2, mock_serial.popRxBuffer().length());
        TEST_ASSERT_EQUAL_CHAR('t', stream->read());
        TEST_ASSERT_EQUAL(1, mock_serial.popRxBuffer().length());
        TEST_ASSERT_EQUAL_CHAR('a', stream->read());
        TEST_ASSERT_EQUAL(0, mock_serial.popRxBuffer().length());
    }

    void test_MockSerial_ReturnsRxBufferSizeWhenSerialAvailableIsCalled()
    {
        MockSerial mock_serial;
        Stream *stream = ArduinoFakeMock(Stream);
        mock_serial.begin();
        TEST_ASSERT_EQUAL(0, stream->available());
        mock_serial.addRxContents("OK");
        TEST_ASSERT_EQUAL(2, stream->available());
        mock_serial.addRxContents("Hello");
        TEST_ASSERT_EQUAL(5, stream->available());
    }

    void test_MockSerial_PutNegativeOneOnRxBufferWhenNoDataAvailable(void)
    {
        MockSerial mock_serial;
        Stream *stream = ArduinoFakeMock(Stream);
        mock_serial.begin();

        TEST_ASSERT_EQUAL(-1, stream->read());
        TEST_ASSERT_EQUAL(-1, stream->read());
    }

    void test_MockSerial_StringReplace(void)
    {
        String response = "\r\nAT\nOK\r\n";
        response.trim();
        response.replace("AT", "");
        response.replace("\n", "");

        TEST_ASSERT_EQUAL_STRING("OK", response.c_str());
    }

    void run_tests(void)
    {
        RUN_TEST(test_MockSerial_ClearTxBufferAfterCreate);
        RUN_TEST(test_MockSerial_PutsDataOnTxBufferAfterPrint);
        RUN_TEST(test_MockSerial_AppendsStringInBufferIfPrintedConsecutively);
        RUN_TEST(test_MockSerial_CleanRxBufferEmptyIfNotSet);
        RUN_TEST(test_MockSerial_SetRxBufferToGivenData);
        RUN_TEST(test_MockSerial_ReducesRxBufferSizeByOneEverytimeStreamReadIsCalled);
        RUN_TEST(test_MockSerial_ReturnsRxBufferSizeWhenSerialAvailableIsCalled);
        RUN_TEST(test_MockSerial_PutNegativeOneOnRxBufferWhenNoDataAvailable);
        RUN_TEST(test_MockSerial_StringReplace);
    }
}