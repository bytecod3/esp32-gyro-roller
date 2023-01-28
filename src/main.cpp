#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "defs.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

void _set_display_settings(){
  /*
  Set display parameters
  */
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Clear the buffer.
  display.clearDisplay();

}

void _draw_target_point_circle(){
  /*
  Draw target like circle on the screen
  */
  uint8_t _target_point_radius = 10;
  uint8_t _target_point_x_offset = (SCREEN_WIDTH/2);
  uint8_t _y_offset = 10;
  uint8_t _target_point_line_marker_height = 6;
  uint8_t _center_circle_radius = 2;
  
  // draw target outer circle
  display.drawCircle(_target_point_x_offset, _y_offset+_target_point_radius, _target_point_radius+_target_point_line_marker_height/2, WHITE);

  // lines around the circle
  display.drawFastVLine(SCREEN_WIDTH/2, _target_point_radius-_target_point_line_marker_height, _target_point_line_marker_height, WHITE); // vertical top
  display.drawFastVLine(SCREEN_WIDTH/2, (_y_offset + _target_point_radius*2), _target_point_line_marker_height, WHITE); // vertical bottom
  display.drawFastHLine((SCREEN_WIDTH/2) - _target_point_radius-_target_point_line_marker_height, (_y_offset + _target_point_radius), _target_point_line_marker_height, WHITE ); // horizontal left
  display.drawFastHLine((SCREEN_WIDTH/2) + _target_point_radius, (_y_offset + _target_point_radius), _target_point_line_marker_height, WHITE ); // horizontal right

  // draw center point circle
  display.drawCircle(SCREEN_WIDTH/2, _y_offset + _target_point_radius, _center_circle_radius, WHITE);
    
}

void setup(){               
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // loop forever
  }

  _set_display_settings();
  _draw_target_point_circle();

  display.display();

}


void loop() {
  // put your main code here, to run repeatedly:
}