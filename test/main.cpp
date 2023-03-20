#include <ArduinoFake.h>
#include <unity.h>
#include "test_BC95.h"
#include "test_MockSerial.h"
#include "test_MockClock.h"
#include "test_NBClass.h"

#define RUN_TEST_GROUP(TEST)                                                           \
    if (!std::getenv("TEST_GROUP") || (strcmp(#TEST, std::getenv("TEST_GROUP")) == 0)) \
    {                                                                                  \
        TEST::run_tests();                                                             \
    }

using namespace fakeit;

BC95 *driverUnderTest = NULL;
MockSerial *mockSerial = NULL;
MockClock *mockClock = NULL;
Stream *stream = NULL;
const int reset_pin = 23;
const unsigned long timeout_ms = 300;
const unsigned long initTime = 0;
const unsigned long timeStep = 1;

void setUp()
{
    stream = ArduinoFakeMock(Stream);
    driverUnderTest = new BC95(stream, reset_pin);
    mockSerial = new MockSerial();
    mockClock = new MockClock(initTime, timeStep);
}

void tearDown()
{
    ArduinoFakeReset();
    delete driverUnderTest;
    delete mockSerial;
    delete mockClock;
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST_GROUP(MockClockTest);
    RUN_TEST_GROUP(MockSerialTest);
    RUN_TEST_GROUP(BC95Test);
    RUN_TEST_GROUP(NBClassTest);
    UNITY_END();
    return 0;
}