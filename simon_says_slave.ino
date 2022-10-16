/*
  File: Simon Says slave Arduino code
  File Name: simon_says_slave.ino
  Author: Matthew VonWahlde & Cameron Zheng
  Date: 10/15/2022
  Event: Gonzaga ACM Hackathon
*/

// header files
#include <Wire.h>
#include <LiquidCrystal.h>
// slave arduino's COM port
#define SLAVE_ADDR 6

// initializing LCD display
LiquidCrystal lcd(7,8,9,10,11,12);

// function definitions
void receiveEvent();

// global variables
int LED = 8;
char message[32]; // array storing message

// ---------- setup Function ----------
// Purpose: Initializes slave arduino
// Input: N/A
// Output: N/A
void setup() {
  Serial.begin(9600);

  // starts the wire transfer between master and slave arduino
  Wire.begin(SLAVE_ADDR);

  // set up the LCD'S number of columns and rows
  lcd.begin(16, 2);
  // receiving information from master arduino
  Wire.onReceive(receiveEvent);
}

// ---------- loop Function ----------
// Purpose: Loop function for slave arduino
// Input: N/A
// Output: N/A
void loop() {
  // sets the cursor to column 0, line 1  
  lcd.setCursor(0, 0);
  delay(1000);
}

// ---------- receiveEvent Function ----------
// Purpose: Receives a command from master arduino
// Input: N/A
// Output: N/A
void receiveEvent(){

  // index for message array
  int i = 0;

  // while loop for receiving message
  while( Wire.available() ){
    message[i] = (char)Wire.read();
    i++;
  }

  // fill the rest of the message with spaces
  while(i < 32){
    message[i] = ' ';
    i++;
  }

  // sets the cursor to (0,0) on LCD
  lcd.setCursor(0,0);
  // writes the message on the first line of LCD
  for(int j = 0; j < 16; j++)
    lcd.write(message[j]);

  // writes the message on the second line of LCD
  lcd.setCursor(0,1);
  for(int j = 16; j < 32;j++)
    lcd.write(message[j]);
}