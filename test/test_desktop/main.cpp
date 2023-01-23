#include <ArduinoFake.h>
#include <unity.h>
#include "test_BC95.h"
#include "test_MockSerial.h"
#include "test_MockClock.h"

#define RUN_TEST_GROUP(TEST)                                                           \
    if (!std::getenv("TEST_GROUP") || (strcmp(#TEST, std::getenv("TEST_GROUP")) == 0)) \
    {                                                                                  \
        TEST::run_tests();                                                             \
    }

using namespace fakeit;

void setUp()
{
}

void tearDown()
{
    ArduinoFakeReset();
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST_GROUP(MockClockTest);
    RUN_TEST_GROUP(MockSerialTest);
    RUN_TEST_GROUP(BC95Test);
    UNITY_END();
    return 0;
}