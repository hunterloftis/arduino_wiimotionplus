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
  wmc.invert(false, false, false);
  wmc.interval(10);
  wmc.start();
  wmc.calibrate();
}

void loop() {
  wmc.update();
  unsigned long now = millis();
  if (now - last_message > message_interval) {
    float* pos = wmc.getPosition();
    sendMessage(pos[3], pos[4], pos[5]);
    last_message = now;
  }
}

