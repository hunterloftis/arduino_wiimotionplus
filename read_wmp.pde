#include <Wire.h>

const int baud = 19200;
const float wmpToDegreesPerSec = 16.0;
const float wmpFastMultiplier = 4.0;
const float frameToSec = 0.001;      // Acceleration is represented by degs/sec, but we update in ms instead of 1s (1000ms)
const int updateInterval = 10;
const int printInterval = 1000;
const int calibrations = 10;
const boolean debug = false;

unsigned long lastUpdate = 0;
unsigned long lastPrint = 0;

byte data[6]; //six data bytes
int yaw, pitch, roll; //three axes

bool slowYaw, slowPitch, slowRoll;

float slowPitchVal = 0.0;
float fastPitchVal = 0.0;

float yawZero, pitchZero, rollZero = 0.0;
float yawVelocity, pitchVelocity, rollVelocity = 0.0;
float yawPosition, pitchPosition, rollPosition = 0.0;


void wmpOn(){
  Wire.beginTransmission(0x53); //WM+ starts out deactivated at address 0x53
  Wire.send(0xfe); //send 0x04 to address 0xFE to activate WM+
  Wire.send(0x04);
  Wire.endTransmission(); //WM+ jumps to address 0x52 and is now active
}

void wmpSendZero(){
  Wire.beginTransmission(0x52); //now at address 0x52
  Wire.send(0x00); //send zero to signal we want info
  Wire.endTransmission();
}

void receiveData() {
  wmpSendZero(); //send zero before each request (same as nunchuck)
  Wire.requestFrom(0x52,6); //request the six bytes from the WM+
  for (int i=0;i<6;i++){
    data[i]=Wire.receive();
  }
}

void calibrateZeroes(){
  delay(500);
  for (int i=0;i<calibrations;i++){
    receiveVelocity();
    yawZero += yawVelocity / calibrations;
    pitchZero += pitchVelocity / calibrations;
    rollZero += rollVelocity / calibrations;
    delay(50);
  }
}

void receiveYawPitchRoll(){
  receiveData();
  
  yaw=((data[3]>>2)<<8)+data[0]; 
  pitch=((data[5]>>2)<<8)+data[2]; 
  roll=((data[4]>>2)<<8)+data[1]; 
  
  slowYaw = data[3] & 2;
  slowPitch = data[3] & 1;
  slowRoll = data[4] & 2;
}

void receiveVelocity() {
  receiveYawPitchRoll();
  
  yawVelocity = yaw / wmpToDegreesPerSec;
  pitchVelocity = pitch / wmpToDegreesPerSec;
  rollVelocity = roll / wmpToDegreesPerSec;
}

void receiveRelativeVelocity() {
  receiveVelocity();
  
  yawVelocity -= yawZero;
  pitchVelocity -= pitchZero;
  rollVelocity -= rollZero;
  
  if (!slowYaw) yawVelocity *= wmpFastMultiplier;
  if (!slowPitch) pitchVelocity *= wmpFastMultiplier;
  if (!slowRoll) rollVelocity *= wmpFastMultiplier;
}

void receivePosition(unsigned long frame) {
  receiveRelativeVelocity();

  float toSec = frame * frameToSec;
 
  // Integrete velocity into position
  
  yawPosition += yawVelocity * toSec;
  pitchPosition += pitchVelocity * toSec;
  rollPosition += rollVelocity * toSec;  
}

void printPosition() {
  Serial.print("POSIT --");
  Serial.print("yaw:");//see diagram on randomhacksofboredom.blogspot.com
  Serial.print(yawPosition); //for info on which axis is which
  Serial.print(" pitch:");
  Serial.print(pitchPosition);
  Serial.print(" roll:");
  Serial.println(rollPosition);  
}

void printVelocity() {
  Serial.print("VELOC --");
  Serial.print("yaw:");//see diagram on randomhacksofboredom.blogspot.com
  Serial.print(yawVelocity); //for info on which axis is which
  Serial.print(" pitch:");
  Serial.print(pitchVelocity);
  Serial.print(" roll:");
  Serial.println(rollVelocity);  
}

void printZeroes() {
  Serial.print("ZERO  --");
  Serial.print("yaw:");//see diagram on randomhacksofboredom.blogspot.com
  Serial.print(yawZero); //for info on which axis is which
  Serial.print(" pitch:");
  Serial.print(pitchZero);
  Serial.print(" roll:");
  Serial.println(rollZero);  
}

void printYawPitchRoll() {
  Serial.print("YPR   --");
  Serial.print("yaw:");//see diagram on randomhacksofboredom.blogspot.com
  Serial.print(yaw); //for info on which axis is which
  Serial.print(" pitch:");
  Serial.print(pitch);
  Serial.print(" roll:");
  Serial.println(roll);    
}

void printError() {
  float frame = abs(yawVelocity) + abs(pitchVelocity) + abs(rollVelocity); 
  float total = abs(yawPosition) + abs(pitchPosition) + abs(rollPosition);
  Serial.print("ERROR --");
  Serial.print("frame:");
  Serial.print(frame);
  Serial.print(", total:");
  Serial.println(total);
}

void message(float yaw, float pitch, float roll) {
  Serial.print(yaw);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.println(roll);
}

//see http://wiibrew.org/wiki/Wiimote/Extension_Controllers#Wii_Motion_Plus
//for info on what each byte represents
void setup(){
  Serial.begin(baud);
  if (debug) Serial.println("WM+ tester");
  Wire.begin();
  if (debug) Serial.println("3");
  wmpOn(); //turn WM+ on
  if (debug) Serial.println("2");
  calibrateZeroes(); //calibrate zeroes
  if (debug) Serial.println("1");
  delay(1000);
  if (debug) Serial.println("Starting!");
}

void loop(){
  unsigned long now = millis();
  unsigned long frameframe = now - lastUpdate;
  if (frameframe > updateInterval) {
    receivePosition(frameframe);
    lastUpdate = now;
    if (!debug) message(yawPosition, pitchPosition, rollPosition);
  }
  if (debug && now - lastPrint > printInterval) {
    //printPosition();
    Serial.println("=====");
    printYawPitchRoll();    
    printZeroes();
    printVelocity();
    printPosition();
    printError();
    lastPrint = now; 
  }
}

