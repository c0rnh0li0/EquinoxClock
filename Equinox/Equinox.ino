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

  the_second = second(local);
  the_minute = minute(local);
  the_hour = hour(local); 
  
  minsec_factor = (float) NUM_LEDS / 60.00;

  //is_inverse = true;
  
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  setSyncInterval(1);
  last_second = second(local);

  Serial.begin(9600);
}

void loop() {
  FastLED.clear();
  local = CE.toLocal(now(), &tcr);
  
  the_second = second(local);
  the_minute = minute(local);
  the_hour = hour(local); 
  
  int pos_hour = (the_hour % 12) * 5 + floor(the_minute / 12);
  
  float gradient = the_minute * 99 / 59 + (the_hour % 3) * 100;
  uint8_t second_hue = map(gradient, 0, 299, 0, 255);
  uint8_t minute_hue = second_hue + 35;
  uint8_t hour_hue = minute_hue + 35;
  
  if (the_second != last_second) {
    mill = 0;
    last_time = millis();
    last_second = the_second;
  } else {
    mill = map((millis() - last_time) % 1000, 0, 1000, 0, 255);
  }  

  if (is_inverse) {
    for (int i = 0; i < NUM_LEDS; i++)
      leds[ map_pixel_time[i]] = CHSV( hour_hue, 255, 255 );

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

void draw_second(int pos, uint8_t hue) {
  if (prev_sec_pos != pos) {
    sec_calls = 0;
    prev_sec_pos = pos;
  }
  else {
    sec_calls++;
  }

  uint8_t calls_to_mill = map(sec_calls, 0, 7, 0, 255);
  
  uint8_t sprev = map(255 - calls_to_mill, 0, 255, 0, 255);
  uint8_t snext = map(calls_to_mill, 0, 255, 0, 255);

  int prevpos_2 = pos - 2 < 0 ? NUM_LEDS + pos - 2 : pos - 2;
  int prevpos_1 = pos - 1 < 0 ? NUM_LEDS + pos - 1 : pos - 1;

  int nextpos_1 = pos + 1 > NUM_LEDS ? (pos) - NUM_LEDS : pos + 1;
  int nextpos_2 = pos + 2 > NUM_LEDS ? (pos + 1) - NUM_LEDS : pos + 2;
  
  leds[ map_pixel_time[prevpos_1]] = CHSV( hue, 255, is_inverse ? snext : sprev); // mid    
  leds[ map_pixel_time[pos]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, 255, 255); // mid  
  leds[ map_pixel_time[nextpos_1]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, 255, 255); // mid
  leds[ map_pixel_time[nextpos_2]] = CHSV( hue, 255, is_inverse ? sprev : snext ); // mid

  if (is_inverse) {
    leds[ map_pixel_time[prevpos_1]] = CHSV(0, 0, 0); // mid
    leds[ map_pixel_time[prevpos_2]] = CHSV( hue, 255, snext); // mid

    int nextpos_3 = pos + 3 > NUM_LEDS ? (pos + 2) - NUM_LEDS : pos + 3;
    leds[ map_pixel_time[nextpos_2]] = CHSV( 0, 0, 0); // mid
    leds[ map_pixel_time[nextpos_3]] = CHSV( hue, 255, sprev); // mid
  }
}

void draw_minute(int pos, uint8_t hue) {
  draw_minute_hour(pos, the_second, hue, false);
}

void draw_hour(float float_pos, uint8_t hue) {
  int pos = floor(float_pos);

  draw_minute_hour(pos, the_minute, hue, true);
}

void draw_minute_hour(int pos, int factor, uint8_t hue, bool is_hour) {
  int prevpos_2 = pos - 2 < 0 ? NUM_LEDS + pos - 2 : pos - 2;
  int prevpos_1 = pos - 1 < 0 ? NUM_LEDS + pos - 1 : pos - 1;

  int nextpos_1 = pos + 1 > NUM_LEDS ? (pos) - NUM_LEDS : pos + 1;
  int nextpos_2 = pos + 2 > NUM_LEDS ? (pos + 1) - NUM_LEDS : pos + 2;
  
  leds[ map_pixel_time[prevpos_2]] = CHSV(hue, 255, is_inverse ? map(factor, 0, 59, 0, 255) : 255 - map(factor, 0, 59, 0, 255));
  leds[ map_pixel_time[prevpos_1]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, 255, 255 );
  leds[ map_pixel_time[pos]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, 255, 255 );
  leds[ map_pixel_time[nextpos_1]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, 255, 255 );
  leds[ map_pixel_time[nextpos_2]] = CHSV(hue, 255, is_inverse ? 255 - map(factor, 0, 59, 0, 255) : map(factor, 0, 59, 0, 255));

  if (is_hour) {
    int prevpos_3 = pos - 3 < 0 ? NUM_LEDS + pos - 3 : pos - 3;
    int nextpos_3 = pos + 3 > NUM_LEDS ? (pos + 2) - NUM_LEDS : pos + 3;
    
    leds[ map_pixel_time[prevpos_2]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, 255, 255);
    leds[ map_pixel_time[prevpos_3]] = CHSV(hue, 255, 255 - map(factor, 0, 59, 0, 255));

    leds[ map_pixel_time[nextpos_2]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, 255, 255);
    leds[ map_pixel_time[nextpos_3]] = CHSV(hue, 255, map(factor, 0, 59, 0, 255));

    if (is_inverse) {
      int prevpos_4 = pos - 4 < 0 ? NUM_LEDS + pos - 4 : pos - 4;
      int nextpos_4 = pos + 4 > NUM_LEDS ? (pos + 3) - NUM_LEDS : pos + 4;

      leds[ map_pixel_time[prevpos_3]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[prevpos_4]] = CHSV(hue, 255, map(factor, 0, 59, 0, 255));

      leds[ map_pixel_time[nextpos_3]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[nextpos_4]] = CHSV(hue, 255, 255 - map(factor, 0, 59, 0, 255));
    }
  }
  else {
    if (is_inverse) {
      int prevpos_3 = pos - 3 < 0 ? NUM_LEDS + pos - 3 : pos - 3;
      int nextpos_3 = pos + 3 > NUM_LEDS ? (pos + 2) - NUM_LEDS : pos + 3;
      
      leds[ map_pixel_time[prevpos_2]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[prevpos_3]] = CHSV(hue, 255, map(factor, 0, 59, 0, 255));
  
      leds[ map_pixel_time[nextpos_2]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[nextpos_3]] = CHSV(hue, 255, 255 - map(factor, 0, 59, 0, 255));
    }
  }
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

