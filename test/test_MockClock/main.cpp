#include <Arduino.h>
#include <ArduinoFake.h>
#include <unity.h>
#include "MockClock.h"

using namespace fakeit;

void test_MockClock_IncrementsMillisWithGivenStartTimeAndStep(void)
{
    MockClock mock_clock(0, 1);
    mock_clock.begin();

    TEST_ASSERT_EQUAL(0, millis());
    TEST_ASSERT_EQUAL(1, millis());
    TEST_ASSERT_EQUAL(2, millis());
}

void setUp()
{
    ArduinoFakeReset();
}

void tearDown()
{
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_MockClock_IncrementsMillisWithGivenStartTimeAndStep);
    UNITY_END();
    return 0;
}