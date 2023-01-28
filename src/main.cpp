#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include "defs.h"
#include "debug.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
Adafruit_MPU6050 gyroscope;

void _set_display_settings(){
  /*
  Set display parameters
  */
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Clear the buffer.
  display.clearDisplay();

  // top text 
  display.setCursor(SCREEN_WIDTH/2 - 27, 0);
  display.print("Gyro Scope");

  // reset cursor
  display.setCursor(0, 0);

  // draw left y ticks
  uint8_t _y_tick_spacing = 10;
  uint8_t _y_tick_width = 5;

  for(uint8_t j = 10; j < SCREEN_HEIGHT; j += _y_tick_spacing){
    display.drawFastHLine(0, j, _y_tick_width, WHITE);
  }

  // draw right y ticks
  for(uint8_t j = 10; j < SCREEN_HEIGHT; j += _y_tick_spacing){
    display.drawFastHLine(SCREEN_WIDTH-_y_tick_width, j, _y_tick_width, WHITE);
  }
}
void _draw_target_point_circle(){
  /*
  Draw target like circle on the screen
  */
  uint8_t _target_point_radius = 10;
  uint8_t _target_point_x_offset = (SCREEN_WIDTH/2);
  uint8_t _y_offset = 15;
  uint8_t _target_point_line_marker_height = 6;
  uint8_t _center_circle_radius = 2;
  
  // draw target outer circle
  display.drawCircle(_target_point_x_offset, _y_offset+_target_point_radius, _target_point_radius+_target_point_line_marker_height/2, WHITE);

  // lines around the target circle
  display.drawFastVLine(SCREEN_WIDTH/2, _target_point_radius-_target_point_line_marker_height, _target_point_line_marker_height, WHITE); // vertical top
  display.drawFastVLine(SCREEN_WIDTH/2, (_y_offset + _target_point_radius*2), _target_point_line_marker_height, WHITE); // vertical bottom
  display.drawFastHLine((SCREEN_WIDTH/2) - _target_point_radius-_target_point_line_marker_height, (_y_offset + _target_point_radius), _target_point_line_marker_height, WHITE ); // horizontal left
  display.drawFastHLine((SCREEN_WIDTH/2) + _target_point_radius, (_y_offset + _target_point_radius), _target_point_line_marker_height, WHITE ); // horizontal right

  // draw center point circle
  display.drawCircle(SCREEN_WIDTH/2, _y_offset + _target_point_radius, _center_circle_radius, WHITE);
    
}
void _draw_side_levellers(){
  /*
  Draw triangle pointers that show at the sides of the leveling line
  */
  uint8_t _marker_height = 10;
  uint8_t _marker_width = 10;

  uint8_t _y_scaler_offset = 50;

  // left marker
  display.fillTriangle(0, _y_scaler_offset, 0, _y_scaler_offset + _marker_height, _marker_width, _y_scaler_offset + _marker_height/2, WHITE);

  // right marker
  display.fillTriangle(SCREEN_WIDTH, _y_scaler_offset, SCREEN_WIDTH, _y_scaler_offset + _marker_height, SCREEN_WIDTH-_marker_width, _y_scaler_offset + _marker_height/2, WHITE);

  // leveling line
  uint8_t _levelling_line_width = SCREEN_WIDTH - 2*_marker_width;
  display.drawFastHLine(_marker_width, _y_scaler_offset+_marker_height/2, _levelling_line_width ,WHITE);

  // leveller line horizontal ticks
  uint8_t _tick_spacing = 10;
  uint8_t _tick_height = 6;

  for(uint8_t i = _marker_width; i < SCREEN_WIDTH-_marker_width; i += _tick_spacing ){
    display.drawFastVLine(i, _y_scaler_offset+_marker_height/2 - _tick_height/2, _tick_height, WHITE);
  }
}

void _init_MPU6050(){
  /*
    Initialize MPU6050
  */

  // attempt to initialize MPU6050
  if(!gyroscope.begin(0x68)){
    debugln("MPU6050 allocation failed!");
    for(;;);
  }

  gyroscope.setAccelerometerRange(MPU6050_RANGE_8_G);
  gyroscope.setGyroRange(MPU6050_RANGE_500_DEG);
  gyroscope.setFilterBandwidth(MPU6050_BAND_5_HZ);

  delay(SETUP_DELAY);

}

void setup(){               
  Serial.begin(115200);
  
  // attempt to initialize OLED screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    debugln(F("SSD1306 allocation failed"));
    for (;;); // loop forever
  }

  _set_display_settings();
  _draw_target_point_circle();
  _draw_side_levellers();
  _init_MPU6050();

  // enable OLED screen to display
  display.display();

}


void loop() {
  // get gyroscope readings
  sensors_event_t a, g, temp;
  gyroscope.getEvent(&a, &g, &temp);

  // debug acceleration readings on serial monitor
  debug("[+] Acceleration: ");
  debug("x: "); debug(a.acceleration.x); debug(", ");
  debug("y: "); debug(a.acceleration.y); debug(", ");
  debug("z: "); debug(a.acceleration.z); debug(" m/s^2 ");
  debugln();

  // debug gyroscope readings on serial monitor
  debug("[+] Angular velocity: ");
  debug("x: "); debug(g.gyro.x); debug(", ");
  debug("y: "); debug(g.gyro.y); debug(", ");
  debug("z: "); debug(g.gyro.z); debug(" rad/s ");
  debugln();

  delay(400);

}