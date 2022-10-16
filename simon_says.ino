/*
  File: Simon Says master Arduino code
  File Name: simon_says.ino
  Author: Matthew VonWahlde & Cameron Zheng
  Date: 10/15/2022
  Event: Gonzaga ACM Hackathon
*/

// header files
#include <Wire.h>
// slave arduino COM port
#define SLAVE_ADDR 6 

// note frequency for melodies
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_C6 1047

// messages for LCD
char welcomeMessage[32] = "Welcome to      Simon Says!";
char correctMessage[32] = "Correct!";
char wrongMessage[32] = "Wrong!";
char winMessage[32] = "Congrats!       Score: ";
char loseMessage[32] = "You lost...     Score: ";
char roundMessage[32] = "Round ";

// function definitions
void writeMessage(char[], bool);
void setUpLEDs();
void setUpButtons();
void setUpPattern();
void readButtons();
int calcButton();
void printButtons();
void testLEDs();
void testIndLED(int);
void printPattern();
void setVariables();
bool simonSays();
bool userInputs();
void userWon();
void userLost();
void playMelody(int[]);
void displayPattern();
void lightUpLED(int, int);

// global const variables
const int buttonPin[] = {2,3,4,5,6};
const int ledPin[] = {8,9,10,11,12};
const int buzzerPin = 13;
const int numLEDs = 5;
const int maxRound = 15;
const int timeBetweenRounds = 2000; //ms

// global variables
int buttonState[] = {0,0,0,0,0};
int pattern[maxRound];
int displayLEDOnTime = 1000; //ms
int displayLEDOffTime = 500; //ms
int timeLEDButtonClick = 100; //ms
int currentRound;

// arrays of melodies
int winningMelody[] = {7, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_G5, NOTE_E5, NOTE_C5};
int losingMelody[] = {4, NOTE_G3, NOTE_FS3, NOTE_F3, NOTE_E3};
int correctMelody[] = {2, NOTE_C5, NOTE_G5};
int noteDuration = 500;
int restDuration = 100;

// ---------- Setup Function ----------
// Purpose: Initializes arduino
// Input: N/A
// Output: N/A
void setup() {
  Serial.begin(9600);
  Wire.begin();
  randomSeed(analogRead(0));

  // setting up LEDs and buttons on breadboard
  setUpLEDs();
  setUpButtons();
}

// ---------- Loop Function ----------
// Purpose: Arduino loop function 
// Input: N/A
// Output: N/A
void loop() {
  // start message
  writeMessage(welcomeMessage, false);
  delay(2000);
  // resets global variables
  setVariables();
  // running the simonSays game until loss or win
  if(simonSays() == false){
    userLost();
  } else {
    userWon();
  }

}

// ---------- writeMessage Function ----------
// Purpose: Writes message to LCD
// Input: character array of message, boolean tells function if there 
//        should be a round number (i.e. welcome message does not use round number)
// Output: N/A
void writeMessage(char mess[], bool printRound){

  // convert currentRound to tens and ones values
  int tens = currentRound / 10;
  int ones = currentRound % 10;

  // convert tens and ones to char
  char tensChar = (char)tens + 48;
  char onesChar = (char)ones + 48;

  // start transmission to slave arduino
  Wire.beginTransmission(SLAVE_ADDR);
  // send message to slave arduino
  Wire.write(mess);
  // send round number to slave arduino
  if(printRound){
    // if there is a tens digit, send value
    if(tens > 0)
      Wire.write(tensChar);
    // send ones value
    Wire.write(onesChar);
  }
  // end transmission to slave arduino
  Wire.endTransmission();
}

// ---------- simonSays Function ----------
// Purpose: Main game function
// Input: N/A
// Output: N/A
bool simonSays(){

  // boolean value (true) to run the game
  while(true){
    
    // sends the round message to display on LCD
    writeMessage(roundMessage, true);
    // displays the light pattern on breadboard
    displayPattern();

    // 
    if(userInputs() == false)
      return false;

    if(currentRound == maxRound)
      return true;
    currentRound++;
  }
}

// ---------- userInputs Function ----------
// Purpose: Reads the button inputs on the breadboard
// Input: N/A
// Output: N/A
bool userInputs(){
  // value of button
  int button;
  for(int i = 0; i < currentRound; i++){
    // while loop to make sure no buttons are pressed
    // ensures button values are set to 0
    do{
      readButtons();
      // sets button to correlate to the button pressed on breadboard
      button = calcButton();
      delay(50);
    } while(button != 0);

    // while loop to check until button is pressed
    do{
      readButtons();
      // sets button to correlate to the button pressed on breadboard
      button = calcButton();
      delay(50);
    } while(button == 0);

    // light up LED that correlates to the pressed button
    lightUpLED(button, timeLEDButtonClick);

    // wrong button pressed
    if(button != pattern[i]){
      writeMessage(wrongMessage, false);
      return false;
    }
  }

  // correctly pushed button
  writeMessage(correctMessage, false);
  playMelody(correctMelody);
  delay(1000);

  return true;
}

// ---------- setVariables Function ----------
// Purpose: Initializig random pattern array and current round
// Input: N/A
// Output: N/A
void setVariables(){
  setUpPattern();
  currentRound = 1;
}

// ---------- userWon Function ----------
// Purpose: Occurs when user wins the game
// Input: N/A
// Output: N/A
void userWon(){
  // sends winning message to slave arduino
  writeMessage(winMessage, true);
  playMelody(winningMelody);
  // lights up the LED in cascade fashion
  for(int i = 0; i < 20; i++){
    for(int i = 0; i < numLEDs; i++)
      lightUpLED(i+1, 100);
      delay(50);
  }
}

// ---------- userLost Function ----------
// Purpose: Occurs when user loses
// Input: N/A
// Output: N/A
void userLost(){
  // amount of blinks for a loss
  int runs = 10;
  // setting the score
  currentRound -= 1;
  // sending message to slave arduino
  writeMessage(loseMessage, true);
  playMelody(losingMelody);

  // blinks LEDs all simultaneously
  for (int i = 0; i < runs; i++){
    digitalWrite(ledPin[0], HIGH);
    digitalWrite(ledPin[1], HIGH);
    digitalWrite(ledPin[2], HIGH);
    digitalWrite(ledPin[3], HIGH);
    digitalWrite(ledPin[4], HIGH);

    delay(100);

    digitalWrite(ledPin[0], LOW);
    digitalWrite(ledPin[1], LOW);
    digitalWrite(ledPin[2], LOW);
    digitalWrite(ledPin[3], LOW);
    digitalWrite(ledPin[4], LOW);

    delay(100);
  }
}

// ---------- setUpLEDs Function ----------
// Purpose: Initializing LEDs
// Input: N/A
// Output: N/A
void setUpLEDs() {
  pinMode(ledPin[0], OUTPUT);
  pinMode(ledPin[1], OUTPUT);
  pinMode(ledPin[2], OUTPUT);
  pinMode(ledPin[3], OUTPUT);
  pinMode(ledPin[4], OUTPUT);
}

// ---------- setUpButtons Function ----------
// Purpose: Initializes buttons
// Input: N/A
// Output: N/A
void setUpButtons() {
  pinMode(buttonPin[0], INPUT);
  pinMode(buttonPin[1], INPUT);
  pinMode(buttonPin[2], INPUT);
  pinMode(buttonPin[3], INPUT);
  pinMode(buttonPin[4], INPUT);
}

// ---------- setUpPattern Function ----------
// Purpose: Creates a random pattern
// Input: N/A
// Output: N/A
void setUpPattern(){
  // fills pattern array corresponding to random LED values
  for (int i = 0; i < maxRound; i++)
    pattern[i]= random(1, numLEDs+1);
}

// ---------- readButtons Function ----------
// Purpose: Digitally reads each button
// Input: N/A
// Output: N/A
void readButtons() {
  buttonState[0] = digitalRead(buttonPin[0]);
  buttonState[1] = digitalRead(buttonPin[1]);
  buttonState[2] = digitalRead(buttonPin[2]);
  buttonState[3] = digitalRead(buttonPin[3]);
  buttonState[4] = digitalRead(buttonPin[4]);
}

// ---------- calcButton Function ----------
// Purpose: Retrieve which button is pressed
// Input: N/A
// Output: N/A
int calcButton(){
  // traverses through the LED array to see which LED is pushed
  for(int i = 0; i < numLEDs; i++)
    if(buttonState[i] != 0)
      return i+1;
  return 0;
}

// ---------- displayPattern Function ----------
// Purpose: Displays the pattern on the LEDs
// Input: N/A
// Output: N/A
void displayPattern(){
  // delay time between rounds
  delay(timeBetweenRounds);
  // loops through the pattern to light up LEDs
  for(int i = 0; i < currentRound; i++) {
    if(i != 0)
      delay(displayLEDOffTime);
    lightUpLED(pattern[i], displayLEDOnTime);
  }
}

// ---------- lightUpLED Function ----------
// Purpose: Lights up individual LED
// Input: LED #, amount of time to stay on
// Output: N/A
void lightUpLED(int led, int timeOn){
  // turn on LED
  digitalWrite(ledPin[led-1], HIGH);
  // time for light to stay on
  delay(timeOn);
  // turn off LED
  digitalWrite(ledPin[led-1], LOW);
}

// ---------- playMelody Function ----------
// Purpose: Plays the melody that is assigned through an array
// Input: Array of melody (plays notes in order of array)
// Output: N/A
void playMelody(int melody[]){
  // traverses the array to play through notes
  for(int i = 1; i <= melody[0]; i++){
    tone(buzzerPin, melody[i], noteDuration);
    // duration between notes
    delay(restDuration);
  }
}

/*
// TESTING FUNCTIONS

// ---------- printButtons Function ----------
// Purpose: 
// Input: N/A
// Output: N/A
void printButtons(){
  Serial.println(buttonState[0]);
  Serial.println(buttonState[1]);
  Serial.println(buttonState[2]);
  Serial.println(buttonState[3]);
  Serial.println(buttonState[4]);
}

// ---------- testLEDs Function ----------
// Purpose: Tests the LEDs
// Input: N/A
// Output: N/A
void testLEDs() {
  for(int i = 0; i < numLEDs; i++) {
    testIndLED(i);
  }
}

// ---------- testIndLED Function ----------
// Purpose: Testing individual LEDs
// Input: N/A
// Output: N/A
void testIndLED(int led) {
  if(buttonState[led] == HIGH) {
    digitalWrite(ledPin[led], HIGH);
  } else {
    digitalWrite(ledPin[led],LOW);
  }
}

// ---------- printPattern Function ----------
// Purpose: Testing the random pattern
// Input: N/A
// Output: N/A
void printPattern() {
  for(int i = 0; i < maxRound; i++)
    Serial.println(pattern[i]);
}
*/