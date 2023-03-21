#include <ArduinoFake.h>
#include <unity.h>
#include "mocks/MockClock.h"

using namespace fakeit;

namespace MockClockTest
{
    void test_MockClock_IncrementsMillisWithGivenStartTimeAndStep(void)
    {
        MockClock mock_clock(0, 1);
        mock_clock.begin();

        TEST_ASSERT_EQUAL(0, millis());
        TEST_ASSERT_EQUAL(1, millis());
        TEST_ASSERT_EQUAL(2, millis());
    }
    void run_tests()
    {
        RUN_TEST(test_MockClock_IncrementsMillisWithGivenStartTimeAndStep);
    }
}