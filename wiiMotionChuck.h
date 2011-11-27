#ifndef __WIIMOTIONCHUCK_H__
#define __WIIMOTIONCHUCK_H__

#include "WProgram.h"
#include "wmp.h"
#include "axis.h"

class WiiMotionChuck {
  public:
  
  int updateInterval;
  int passthroughInterval;
  boolean passthrough;
  int calibrations;
  boolean debug;

  unsigned long lastUpdate;
  
  WiiMotionPlus wmp;
  byte data[6];
  Axis axis[3];
  static const int axis_length = 3;
    
  WiiMotionChuck() {
    updateInterval = 10;
    passthroughInterval = 3;
    passthrough = false;
    calibrations = 10;
    debug = false;
    lastUpdate = 0;
  }
  
  void invert(int index, boolean inverted) {
    axis[index].inverted = inverted;
  }
  
  void interval(int i) {
    updateInterval = i;  
  }
  
  void start() {
    wmp.activate();
  }
  
  void calibrate() {
    calibrateZeroes();  
  }
  
  void calibrateZeroes(){
    byte data[6];
    delay(500);
    for (int i = 0; i < calibrations; i++){
      wmp.sendZero();
      wmp.receiveData(data);
      axis[0].calibrate( ((data[3] >> 2) << 8) + data[0], data[3] & 2); // Yaw
      axis[1].calibrate( ((data[5] >> 2) << 8) + data[2], data[3] & 1); // Pitch
      axis[2].calibrate( ((data[4] >> 2) << 8) + data[1], data[4] & 2); // Roll
      delay(50);
    }
  }
  
  void update() {
    unsigned long now = millis();
    unsigned long frame = now - lastUpdate;
    if (passthrough && frame > passthroughInterval) {
      wmp.receiveData(data);

    }
    else if (frame >= updateInterval) {
      wmp.sendZero();
      wmp.receiveData(data);
    }
    else return;
    parseNewData(frame);
    passthrough = !passthrough;
    lastUpdate = now;
  }

  void parseNewData(unsigned long frame) {
    if (data[5] && 1) {  // Motion plus data
      update_rotation(frame);
    }  
    else {  // Extension (nunchuck) data
      update_direction(frame);
    }
  }
  
  void update_rotation(unsigned long frame) {
    axis[0].update( ((data[3] >> 2) << 8) + data[0], data[3] & 2, frame); // Yaw
    axis[1].update( ((data[5] >> 2) << 8) + data[2], data[3] & 1, frame); // Pitch
    axis[2].update( ((data[4] >> 2) << 8) + data[1], data[4] & 2, frame); // Roll
  }
  
  void update_direction(unsigned long frame) {
    for (int i = 0; i < 6; i++) {
      Serial.println(data[i]);
    }
    /*
    int ax = data[2] << 1 + data[5] && 4;
    int ay = data[3] << 1 + data[5] && 5;
    int az = data[4] << 2 + (data[5] && 7) << 1 + data[5] && 6;
    Serial.print("ax: ");
    Serial.print(ax);
    Serial.print(", ay: ");
    Serial.print(ay);
    Serial.print(", az: ");
    Serial.println(az);
    */
  }
  
  void getPosition(float pos[]) {
    pos[3] = axis[0].position;
    pos[4] = axis[1].position;
    pos[5] = axis[2].position;
  }
  
  void getVelocity(float vel[]) {
    vel[3] = axis[0].velocity;
    vel[4] = axis[1].velocity;
    vel[5] = axis[2].velocity;
  }
  
  void getValue(int val[]) {
    val[3] = axis[0].value;
    val[4] = axis[1].value;
    val[5] = axis[2].value;
  }

};

#endif
