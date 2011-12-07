#include <Wire.h>

#include "wiiMotionChuck.h"

const int baud = 19200;

WiiMotionChuck wmc = WiiMotionChuck();

unsigned long last_message = 0;
int message_interval = 10;

void sendMessage(float yaw, float pitch, float roll) {
  Serial.print(yaw);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.println(roll);
}

void setup(){
  Serial.begin(baud);
  wmc.interval(10);
  wmc.invert(0, true);  // Pitch sensor is inverted
  wmc.invert(1, true);  // Yaw sensor is inverted
  wmc.start();
  //wmc.calibrate();
}

void loop() {
  float pos[6];
  wmc.update();
  unsigned long now = millis();
  if (now - last_message > message_interval) {
    wmc.getPosition(pos);
    //sendMessage(pos[3], pos[4], pos[5]);
    last_message = now;
  }
}

