#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
// #include <Fonts/FreeSans9pt7b.h>
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

  // set font
  // display.setFont(&FreeSans9pt7b);

  // Clear the buffer.
  display.clearDisplay();

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

void  _update_helm_and_tiller(){
  // if change is +ve, rotate the wheel clockwise
  // otherwise rotate the wheel anticlockwise
  // using polar coordinates

  // this rectangle willl fill the whole screen 
  display.fillRect(0,0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

  // draw target outer circle
  display.drawCircle(_target_point_x_offset, _y_offset+_target_point_radius, _target_point_radius+_target_point_line_marker_height/2, WHITE);
  
  // lines around the target circle
  display.drawFastVLine(SCREEN_WIDTH/2, _target_point_radius-_target_point_line_marker_height, _target_point_line_marker_height, WHITE); // vertical top
  display.drawFastVLine(SCREEN_WIDTH/2, (_y_offset + _target_point_radius*2), _target_point_line_marker_height, WHITE); // vertical bottom
  display.drawFastHLine((SCREEN_WIDTH/2) - _target_point_radius-_target_point_line_marker_height, (_y_offset + _target_point_radius), _target_point_line_marker_height, WHITE ); // horizontal left
  display.drawFastHLine((SCREEN_WIDTH/2) + _target_point_radius, _y_offset + _target_point_radius, _target_point_line_marker_height, WHITE ); // horizontal right
  display.drawCircle(SCREEN_WIDTH/2, _y_offset + _target_point_radius, _center_circle_radius, WHITE); // center most circle

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

  display.display(); // enable displaying
}

void setup(){               
  Serial.begin(115200);
  
  // attempt to initialize OLED screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    debugln(F("SSD1306 allocation failed"));
    for (;;); // loop forever
  }

  _set_display_settings();
  _draw_side_levellers();
  _init_MPU6050();

  // enable OLED screen to display
  display.display();

}

void loop() {
  // get gyroscope readings
  sensors_event_t a, g, temp;
  gyroscope.getEvent(&a, &g, &temp);

  acc_x = a.acceleration.x;
  acc_y = a.acceleration.y;
  acc_z = a.acceleration.z;

  // measure and convert rotational angles
  roll_angle = (atan(acc_y/sqrt(acc_z*acc_z + acc_x*acc_x))) / conversion_factor;
  pitch_angle = (atan(-acc_x/sqrt(acc_y*acc_y + acc_z*acc_z))) / conversion_factor;

  // calculate the polar coordinates for the roll
  x_roll = cos(radians(roll_angle))*_marker_radius;
  y_roll = -sin(radians(roll_angle))*_marker_radius;

  x_roll_inverse = -cos(radians(roll_angle))*_marker_radius;
  y_roll_inverse = sin(radians(roll_angle))*_marker_radius;

  // find the change in roll angle
  change_in_roll_angle = roll_angle - old_roll_angle;

  // update the old_roll_angle to whatever is being read now
  old_roll_angle = roll_angle;

  // update screen helm and tiller - the wheel (^.^)
  _update_helm_and_tiller();

  display.fillCircle(
    (SCREEN_WIDTH/2 + x_roll)-2,
    _y_offset+_target_point_radius + y_roll + _marking_triangle_width,
    2,
    WHITE
  );

  // draw mirrored marker
  display.fillCircle(
    (SCREEN_WIDTH/2 + x_roll_inverse)-2,
    _y_offset+_target_point_radius + y_roll_inverse + _marking_triangle_width,
    2,
    WHITE
  );

  display.fillRect(
    (SCREEN_WIDTH/2 + x_roll)-2,
    _y_offset+_target_point_radius + y_roll + _marking_triangle_width,
    3,
    3,
    WHITE
  );

  display.drawLine(
    (SCREEN_WIDTH/2 + x_roll)-2,
    _y_offset+_target_point_radius + y_roll + _marking_triangle_width,
    (SCREEN_WIDTH/2 + x_roll_inverse)-2,
    _y_offset+_target_point_radius + y_roll_inverse + _marking_triangle_width,
    WHITE);

  // update the roll angle value on the screen
  display.setCursor(SCREEN_WIDTH/2 - 45, 0);
  display.print("Flight Controller"); 

  display.setCursor(SCREEN_WIDTH/2-25, SCREEN_HEIGHT-10);
  display.print("Roll:"); 
  display.print(roll_angle);
  
  display.display();

  delay(100);

}