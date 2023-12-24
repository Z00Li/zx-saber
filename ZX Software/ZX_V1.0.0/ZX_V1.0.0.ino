// -------------------- Libraries --------------------
#include <Arduino.h>                // necessary arduino library
#include <DFRobotDFPlayerMini.h>    // audio player
#include <SoftwareSerial.h>         // audio transmit and receiver
#include <Wire.h>                   // serial communication with audio transmitter
#include <MPU6050.h>                // gyrosensor and acceleration meter
#include <FastLED.h>                // addressable LED library
#include <EEPROM.h>                 // save to storage


// -------------------- Setup --------------------
#define FPSerial softSerial
DFRobotDFPlayerMini audio;
MPU6050 accelgyro;


// -------------------- Settings --------------------
audio.volume(4);    // set volume from 0 to 30 (may not work over 5 with usb cable)
#define debug 1     // set 0 to turn off, set 1 to turn on


// -------------------- Pins --------------------
#define NUM_LEDS 60                 // set how many leds are in the neopixel chain
#define DATA_PIN 6                  // data pin for leds
SoftwareSerial softSerial(10, 11);  // TX and RX ports on mp3 player


// -------------------- Sounds --------------------
int power_on = 1;
int power_off = 2;
int hum = 3;
int swing = 4;
int clash = 5;


// -------------------- Variables --------------------
int16_t ax, ay, az;
int16_t gx, gy, gz;
unsigned long ACC, GYR, COMPL;
int gyroX, gyroY, gyroZ, accelX, accelY, accelZ, freq, freq_f = 20;
bool ls_state = false;



void setup() {
  Wire.begin();
  Serial.begin(9600);
  FPSerial.begin(9600);
  audio.begin(softSerial);


  // IMU check  -- for debug
  if (debug == 1) {
    if (accelgyro.testConnection()) 
      Serial.println(F("MPU6050 OK"));
    else
      Serial.println(F("MPU6050 fail"));
  }

  // IMU initialization
  accelgyro.initialize();
  accelgyro.setFullScaleAccelRange(16);
  accelgyro.setFullScaleGyroRange(250);

}

void loop() {

  if (millis() > 200){
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);       

    // find absolute and divide on 100
    gyroX = abs(gx / 100);
    gyroY = abs(gy / 100);
    gyroZ = abs(gz / 100);
    accelX = abs(ax / 100);
    accelY = abs(ay / 100);
    accelZ = abs(az / 100);

    // vector sum
    ACC = sq((long)accelX) + sq((long)accelY) + sq((long)accelZ);
    ACC = sqrt(ACC);
    GYR = sq((long)gyroX) + sq((long)gyroY) + sq((long)gyroZ);
    GYR = sqrt((long)GYR);
    COMPL = ACC + GYR;

    if (debug == 1){  
      Serial.print("$");
      Serial.print(gyroX);
      Serial.print(" ");
      Serial.print(gyroY);
      Serial.print(" ");
      Serial.print(gyroZ);
      Serial.println(";");
      Serial.print(accelY);
    }

    StateGesture();
  }
}

void StateGesture() {
  if (ls_state == false) {
    if (accelY > 180) {
      delay(100);
      if (accelY < 60) {
        audio.play(power_on);
        ls_state = true;
      }
    }
  }
}