#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <max6675.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define THERMO_DO 3
#define THERMO_CS 5
#define THERMO_CLK 6

#define RELAY_PIN 7
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);
float cur_temp; 
float target_temp = 40.00f;
float activation_diff = 0.91f;  //lower activation value
bool relay_on = false;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  delay(200);
  display.clearDisplay();
  display.display();
  Serial.println("Done setup");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Thread is imminent\nEliminate humans");
  display.display();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  delay(10);
}

void loop() {
  cur_temp = thermocouple.readCelsius();

  /*target_temp += 1;  //debug loop
  if (target_temp > 45){
    target_temp = 20;
  }*/

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("curr:");
  display.setTextSize(2);
  display.print(cur_temp);
  display.drawCircle(95, 10, 3, WHITE);  // circle for c
  display.setCursor(100, 10);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("targ:");
  display.setTextSize(2);
  display.print(target_temp);
  display.drawCircle(95, 30, 3, WHITE);
  display.setCursor(100, 30);
  display.print("C");

  
  if(isnan(cur_temp) || cur_temp < 10) {  // turn off relay if probe is disconnected or something went wrong
    relay_on = false;
  } 
  else {
    if (cur_temp > target_temp && relay_on) {   // Hysteresis loop, aka activate heating element after certaion temperature drop and not right away
      relay_on = false;
      
    } else if (cur_temp < (target_temp * activation_diff) && !relay_on) {
      relay_on = true;
      
    }
  } 
  

  if (relay_on){
    digitalWrite(RELAY_PIN, HIGH);
    display.setTextSize(1);
    display.setCursor(30, 50);
    display.print("Relay is on");
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  display.display();
  delay(2000);  // update each 2 secs
}

