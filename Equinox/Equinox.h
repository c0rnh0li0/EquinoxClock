bool is_summer = false;
bool is_winter = false;
bool is_spring = false;
bool is_autumn = false;

uint8_t second_sat = 255;
uint8_t minute_sat = 255;
uint8_t hour_sat = 255;

void goInverse(int hour_hue) {
  if (to_inverse > 0) {
    if (going_inverse) {
      if (inverse_fade <= led_brightness) {
        inverse_fade += 5;
      }

      if (inverse_fade > led_brightness)
        inverse_fade = led_brightness;

      if (inverse_fade == led_brightness) {
        going_inverse = false;
        is_inverse = true;
      }
    }
  }
  else {
    if (going_inverse) {
      if (inverse_fade > 0) {
        inverse_fade -= 5;
      }   
      
      if (inverse_fade < 0)
        inverse_fade = 0;

      if (inverse_fade == 0) {
        going_inverse = false;
        is_inverse = false;
      }
    }
  }
  
  for (int i = 0; i < NUM_LEDS; i++)
    leds[map_pixel_time[i]] = CHSV(hour_hue, 255, inverse_fade);
}

void draw_minute_hour(int pos, int factor, uint8_t hue, bool is_hour) {
  uint8_t the_sat = is_hour ? hour_sat : minute_sat;
  
  int prevpos_2 = pos - 2 < 0 ? NUM_LEDS + pos - 2 : pos - 2;
  int prevpos_1 = pos - 1 < 0 ? NUM_LEDS + pos - 1 : pos - 1;

  int nextpos_1 = pos + 1 > NUM_LEDS ? (pos) - NUM_LEDS : pos + 1;
  int nextpos_2 = pos + 2 > NUM_LEDS ? (pos + 1) - NUM_LEDS : pos + 2;
  
  leds[ map_pixel_time[prevpos_2]] = CHSV(hue, the_sat, is_inverse ? map(factor, 0, 59, 0, 255) : 255 - map(factor, 0, 59, 0, 255));
  leds[ map_pixel_time[prevpos_1]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, the_sat, led_brightness );
  leds[ map_pixel_time[pos]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, the_sat, led_brightness );
  leds[ map_pixel_time[nextpos_1]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, the_sat, led_brightness );
  leds[ map_pixel_time[nextpos_2]] = CHSV(hue, the_sat, is_inverse ? 255 - map(factor, 0, 59, 0, 255) : map(factor, 0, 59, 0, 255));

  if (is_hour) {
    int prevpos_3 = pos - 3 < 0 ? NUM_LEDS + pos - 3 : pos - 3;
    int nextpos_3 = pos + 3 > NUM_LEDS ? (pos + 2) - NUM_LEDS : pos + 3;
    
    leds[ map_pixel_time[prevpos_2]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, the_sat, led_brightness);
    leds[ map_pixel_time[prevpos_3]] = CHSV(hue, the_sat, 255 - map(factor, 0, 59, 0, 255));

    leds[ map_pixel_time[nextpos_2]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, the_sat, led_brightness);
    leds[ map_pixel_time[nextpos_3]] = CHSV(hue, the_sat, map(factor, 0, 59, 0, 255));

    if (is_inverse) {
      int prevpos_4 = pos - 4 < 0 ? NUM_LEDS + pos - 4 : pos - 4;
      int nextpos_4 = pos + 4 > NUM_LEDS ? (pos + 3) - NUM_LEDS : pos + 4;

      leds[ map_pixel_time[prevpos_3]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[prevpos_4]] = CHSV(hue, the_sat, map(factor, 0, 59, 0, 255));

      leds[ map_pixel_time[nextpos_3]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[nextpos_4]] = CHSV(hue, the_sat, 255 - map(factor, 0, 59, 0, 255));
    }
  }
  else {
    if (is_inverse) {
      int prevpos_3 = pos - 3 < 0 ? NUM_LEDS + pos - 3 : pos - 3;
      int nextpos_3 = pos + 3 > NUM_LEDS ? (pos + 2) - NUM_LEDS : pos + 3;
      
      leds[ map_pixel_time[prevpos_2]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[prevpos_3]] = CHSV(hue, the_sat, map(factor, 0, 59, 0, 255));
  
      leds[ map_pixel_time[nextpos_2]] = CHSV(0, 0, 0);
      leds[ map_pixel_time[nextpos_3]] = CHSV(hue, the_sat, 255 - map(factor, 0, 59, 0, 255));
    }
  }
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
  
  leds[ map_pixel_time[prevpos_1]] = CHSV( hue, second_sat, is_inverse ? snext : sprev); // mid    
  leds[ map_pixel_time[pos]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, second_sat, led_brightness); // mid  
  leds[ map_pixel_time[nextpos_1]] = is_inverse ? CHSV(0, 0, 0) : CHSV( hue, second_sat, led_brightness); // mid
  leds[ map_pixel_time[nextpos_2]] = CHSV( hue, second_sat, is_inverse ? sprev : snext ); // mid

  if (is_inverse) {
    leds[ map_pixel_time[prevpos_1]] = CHSV(0, 0, 0); // mid
    leds[ map_pixel_time[prevpos_2]] = CHSV( hue, second_sat, snext); // mid

    int nextpos_3 = pos + 3 > NUM_LEDS ? (pos + 2) - NUM_LEDS : pos + 3;
    leds[ map_pixel_time[nextpos_2]] = CHSV( 0, 0, 0); // mid
    leds[ map_pixel_time[nextpos_3]] = CHSV( hue, second_sat, sprev); // mid
  }
}

void draw_minute(int pos, uint8_t hue) {
  draw_minute_hour(pos, the_second, hue, false);
}

void draw_hour(float float_pos, uint8_t hue) {
  int pos = floor(float_pos);

  draw_minute_hour(pos, the_minute, hue, true);
}

// spring: 21.03
// summer: 21.06
// autumn: 23.09
// winter: 21.12
void check_dates(time_t local) {  
  uint8_t the_day = day(local);
  uint8_t the_month = month(local);

  // check spring
  if (the_month >= 3) {
    is_spring = true;
    is_summer = false;
    is_autumn = false;
    is_winter = false;
    
    if (the_day < 21 && the_month == 3) {
      is_spring = false;
      is_summer = false;
      is_autumn = false;
      is_winter = true;
    }
  }
  // check summer
  else if (the_month >= 6) {
    is_spring = false;
    is_summer = true;
    is_autumn = false;
    is_winter = false;

    if (the_day < 21 && the_month == 6) {
      is_spring = true;
      is_summer = false;
      is_autumn = false;
      is_winter = false;
    }
  }
  // check autumn
  else if (the_month >= 9) {
    is_spring = false;
    is_summer = false;
    is_autumn = true;
    is_winter = false;

    if (the_day < 23 && the_month == 9) {
      is_spring = false;
      is_summer = true;
      is_autumn = false;
      is_winter = false;
    }
  }
  // check winter
   else if (the_month >= 12) {
    is_spring = false;
    is_summer = false;
    is_autumn = false;
    is_winter = true;

    if (the_day < 21 && the_month == 12) {
      is_spring = false;
      is_summer = false;
      is_autumn = true;
      is_winter = false;
    }
  }
  // check spring again
  else if (the_month <= 3) {
    is_spring = false;
    is_summer = false;
    is_autumn = false;
    is_winter = true;

    if (the_day >= 21 && the_month == 3) {
      is_spring = true;
      is_summer = false;
      is_autumn = false;
      is_winter = false;
    }
  }
}

int c_green = 0;
int c_yellow_green = 20;
int c_yellow = 50;
int c_warm_yellow = 60;
int c_light_orange = 70;
int c_orange = 80;
int c_red = 90;
int c_deep_pink = 100;
int c_light_purple = 110;
int c_red_purple = 120;
int c_purple = 140;
int c_blue_purple = 150;
int c_blue = 160;
int c_deep_blue = 170;
int c_light_blue = 190;
int c_aqua = 200;
int c_aqua_green = 210;
int c_light_green_aqua = 220;

uint8_t get_second_hue() {
  if (is_spring) return c_yellow; // yellow
  else if (is_summer) return c_light_green_aqua; // orange
  else if (is_autumn) return c_orange; // orange - yellow
  else if (is_winter) return c_light_green_aqua; // blue;
  else {
    second_sat = 0;
    return 128;
  }
}

uint8_t get_minute_hue() {
  if (is_spring) return c_blue_purple; // purple
  else if (is_summer) return c_blue; // orange
  else if (is_autumn) return c_light_orange; // orange - yellow
  else if (is_winter) return c_light_blue; // blue;
  else {
    minute_sat = 0;
    return 128;
  }
}

uint8_t get_hour_hue() {
  hour_sat = 255;
  if (is_spring) return c_green;
  else if (is_summer) return c_warm_yellow;
  else if (is_autumn) return c_red;
  else if (is_winter) {
    hour_sat = 0;
    return c_deep_blue;
  }
  else {
    hour_sat = 0;
    return 128;
  }
}

int check_should_inverse(time_t local) {
  // go inverse depending on the time of year (approx.)
  if (is_autumn || is_spring) {
    if (hour(local) == 18 && minute(local) == 30)
      return 1;
  }
  else if (is_summer) {
    if (hour(local) == 20 && minute(local) == 30)
      return 1;
  }
  else if (is_winter) {
    if (hour(local) == 16 && minute(local) == 30)
      return 1;
  }

  // revert to normal
  if (hour(local) == 6 && minute(local) == 0)
    return 0;

  return -1;
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
