

/*
  WriteSingleField
  
  Description: Writes a value to a channel on ThingSpeak every 20 seconds.
  
  Hardware: ESP32 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details. 
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

#include <ThingSpeak.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include "secrets.h"
 // always include thingspeak header file after other header files and custom macros

#define LED_PIN 10 // creates the variable LED_PIN as Pin 10 on the M5 which is the LED pin

float accX = 0.0F; // sets the variable for the accelerometer for the X axis
float accY = 0.0F; // sets the variable for the accelerometer for the Y axis
float accZ = 0.0F; // sets the variable for the accelerometer for the Z axis

float maxX = 0.0F; // sets the variable to hold the maximum X axis value
float maxY = 0.0F; // sets the variable to hold the maximum Y axis value
float maxZ = 0.0F; // sets the variable to hold the maximum Z axis value

float magSq = 0.0F; // the variable that holds the square of the magnitude value
float mag = 0.0F;   // the variable that holds the magnitude value
float maxMag = 0.0F;// the variable that holds the maximum magnitude value

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int number = 0;

void setup() {

Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  } 
  M5.begin();             // Init M5StickC Plus.  
  M5.Imu.Init();          // Init IMU.  
  M5.Lcd.setRotation(3);  // Rotate the screen. 
  M5.Lcd.fillScreen(BLACK); //sets the background as black
  M5.Lcd.setTextSize(2);    // sets the font size

  pinMode(LED_PIN, OUTPUT);     //sets the mode of the LED pin as an output
  digitalWrite(LED_PIN, HIGH); // sets the LED pin as high because of the backwards logic

  // Connect to WEP network
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

   M5.update();
  if (M5.BtnA.wasReleased()) {
    maxMag = 0.0F; //resets the maximum magnitude when button A is pressed
  }

  M5.IMU.getAccelData(&accX, &accY, &accZ); //gets the values from the accellerometer for the x,y, and z axis.

  float xSq = sq(accX); // finds the square for the x axis
  float ySq = sq(accY); // finds the square for the y axis
  float zSq = sq(accZ); // finds the square for the z axis
 
  magSq = xSq + ySq + zSq; // adds all of the squares
  mag = sqrt(magSq); // finds the square root

  // updates the maximum magnitude vallue
  if (mag > maxMag) {
    maxMag = mag;
  }

  M5.Lcd.setCursor(30, 30);
  M5.Lcd.print("mag");
  M5.Lcd.printf(" %5.2F  ", mag); //prints the mag value

  M5.Lcd.setCursor(30, 60);
  M5.Lcd.print("maxMag");
  M5.Lcd.printf(" %5.2F  ", maxMag); // prints the maximum mag value

  //makes the LED blink 5 times 
  if (mag > 5) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN, LOW); // sets the LED pin to off
      delay(500);
      digitalWrite(LED_PIN, HIGH); // sets the LED pin to on 
      delay(500);
    }
  }

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeField(myChannelNumber, 1,(" %5.2F  ", maxMag), myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the value
  number++;
  if(number > 99){
    number = 0;
  }
  
  delay(15000); // Wait 15 seconds to update the channel again
}
