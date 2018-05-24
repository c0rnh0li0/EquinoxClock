#include <FastLED.h>
#include <Wire.h>
#include <DS3232RTC.h>
#include <Time.h>
#include <Timezone.h>
#include <Math.h>

// FastLED setup
#define NUM_LEDS 144
#define DATA_PIN 7
CRGB leds[NUM_LEDS];

// TimeZone setup
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 0};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, -60};       //Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t local;
word mill = 0;
unsigned long last_time = millis();
uint8_t last_second;
float minsec_factor;

uint8_t the_second;
uint8_t the_minute;
uint8_t the_hour; 
int prev_sec_pos;
int sec_calls = 0;
bool is_inverse = false;
bool going_inverse = false;
int to_inverse = 0;
int inverse_fade = 0;
int led_brightness = 240;

const uint8_t map_pixel_time[144] = { 
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
  30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 
  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 
  70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
  80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 
  90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 
  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 
  110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 
  120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 
  130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 
  140, 141, 142, 143
};

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  minsec_factor = (float) NUM_LEDS / 60.00;
  
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  setSyncInterval(1);
  last_second = second(local);

  Serial.begin(9600);
}

#include "Equinox.h"

void loop() {
  FastLED.clear();
  local = CE.toLocal(now(), &tcr);
  
  the_second = second(local);
  the_minute = minute(local);
  the_hour = hour(local); 

  check_dates(local);

  int pos_hour = (the_hour % 12) * 5 + floor(the_minute / 12);

  uint8_t second_hue = get_second_hue();
  uint8_t minute_hue = get_minute_hue();
  uint8_t hour_hue = get_hour_hue();
  
  if (the_second != last_second) {
    mill = 0;
    last_time = millis();
    last_second = the_second;
  } else {
    mill = map((millis() - last_time) % 1000, 0, 1000, 0, 255);
  }  
  
  /*if (Serial.available() > 0) {
    String tmp = Serial.readString();
    if (tmp.toInt() != to_inverse) {
      to_inverse = tmp.toInt();
      going_inverse = true;
    }
  }*/

  int should_inverse = check_should_inverse(local);
  if (should_inverse != -1) {
    if (should_inverse == 1 && !is_inverse && !going_inverse) {
        to_inverse = should_inverse;
        going_inverse = true;
    }
    else if (should_inverse == 0 && is_inverse && !going_inverse) {
        to_inverse = should_inverse;
        going_inverse = true;
    }  
  }
  
  
  if (going_inverse) {
    goInverse(hour_hue);
  }
  
  if (is_inverse) {
    for (int i = 0; i < NUM_LEDS; i++)
      leds[ map_pixel_time[i]] = CHSV( hour_hue, 255, led_brightness );

    minute_hue = hour_hue;
    second_hue = hour_hue;
  }
  
  /*** draw seconds ***/
  float sec_mill_pos = mapf((float)mill, 0.0, 255.0, 0.0, 2.4);  
  float sec_pos = floor(sec_pos + sec_mill_pos);
  if (the_second > 0) {
    sec_pos = the_second * minsec_factor;    
    sec_pos = floor(sec_pos + sec_mill_pos) + 1;
  }
  draw_second((int)sec_pos, second_hue);
  /*** draw seconds end ***/

  /*** draw minutes ***/
  int min_pos = map(the_minute, 0, 59, 0, NUM_LEDS); 
  draw_minute(min_pos, minute_hue);
  /*** draw minutes end ***/

   /*** draw hours ***/
  draw_hour(pos_hour * minsec_factor, hour_hue);
  /*** draw hours end ***/
  
  int led_brightness = 255;
  FastLED.setBrightness(led_brightness);
  FastLED.show(); 
  FastLED.delay(50);
}
