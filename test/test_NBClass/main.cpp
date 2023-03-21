#include <ArduinoFake.h>
#include <unity.h>
#include "MockBC95.h"
#include "NB.h"

MockBC95 modem;
NBClass NB(&modem);

using namespace fakeit;

void test_NB_begin_auto()
{
    TEST_ASSERT_EQUAL(NB_IDLE, NB.begin());
}

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
    RUN_TEST(test_NB_begin_auto);
    UNITY_END();
    return 0;
}