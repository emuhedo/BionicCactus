#ifdef UNIT_TEST

#include <Arduino.h>
#include <unity.h>
#include <MockTimeProvider.hpp>
#include <Clock.hpp>
#include <Light.hpp>

// Clock Tests
void test_mock_provider() {
  // 10/24/2017 11:50:15 PM GMT
  time_t testTime = 1508889015;
  MockTime mocktime(testTime);
  time_t currentTime = mocktime.getCurrentTime();
  TEST_ASSERT_EQUAL(testTime, currentTime);
}

void test_get_current_time() {
  // 10/24/2017 11:50:15 PM GMT
  time_t testTime = 1508889015;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, 0);
  time_t currentTime = aclock.getCurrentTime();
  TEST_ASSERT_EQUAL(testTime, currentTime);
}

void test_get_time_now_chr() {
  // 10/24/2017 11:50:15 PM GMT
  time_t testTime = 1508889015;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, 0);
  char timeNow[9];
  aclock.getTimeNowChr(timeNow);
  const char* correctTime = "23:50:15";
  TEST_ASSERT_EQUAL_STRING(correctTime, timeNow);
}

void test_get_time_now_chr_est() {
  // 10/24/2017 11:50:15 PM GMT
  time_t testTime = 1508889015;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, -4);
  char timeNow[9];
  aclock.getTimeNowChr(timeNow);
  const char* correctTime = "19:50:15";
  TEST_ASSERT_EQUAL_STRING(correctTime, timeNow);
}

void test_past_time() {
  // 10/25/2017 1:00:00 PM GMT
  time_t testTime = 1508936400;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, 0);
  TEST_ASSERT_TRUE(aclock.isAtOrPastTime("9:00:00"));
}

void test_not_past_time() {
  // 10/25/2017 1:00:00 PM GMT
  time_t testTime = 1508936400;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, 0);
  TEST_ASSERT_FALSE(aclock.isAtOrPastTime("14:00:00"));
}

// Light Tests
void test_light_on_after_settime() {
  //10/25/2017 1:00:00 PM GMT
  time_t testTime = 1508936400;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, 0);
  Light alight(aclock, D5);
  alight.setTimeOn("09:00:00");
  alight.setTimeOff("14:00:00");
  alight.loop();
  TEST_ASSERT_EQUAL(PWMRANGE, alight.getLastOutput());
}

void test_light_on_after_settime_half_bright() {
  //10/25/2017 1:00:00 PM GMT
  time_t testTime = 1508936400;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, 0);
  Light alight(aclock, D5);
  alight.setTimeOn("09:00:00");
  alight.setTimeOff("14:00:00");
  alight.setBrightness(50);
  alight.loop();
  TEST_ASSERT_EQUAL(PWMRANGE / 2, alight.getLastOutput());
}

void test_light_off_after_settime() {
  //10/25/2017 1:00:00 PM GMT
  time_t testTime = 1508936400;
  MockTime mocktime(testTime);
  Clock aclock(&mocktime, 0);
  Light alight(aclock, D5);
  alight.setTimeOn("09:00:00");
  alight.setTimeOff("11:00:00");
  alight.loop();
  TEST_ASSERT_EQUAL(0, alight.getLastOutput());
}

void setup() {
    delay(2000);

    UNITY_BEGIN();

    // Clock Tests
    RUN_TEST(test_mock_provider);
    RUN_TEST(test_get_current_time);
    RUN_TEST(test_get_time_now_chr);
    RUN_TEST(test_get_time_now_chr_est);
    RUN_TEST(test_past_time);
    RUN_TEST(test_not_past_time);

    // Light Tests
    RUN_TEST(test_light_on_after_settime);
    RUN_TEST(test_light_on_after_settime_half_bright);
    RUN_TEST(test_light_off_after_settime);

    UNITY_END();
}

void loop() {

}

#endif
