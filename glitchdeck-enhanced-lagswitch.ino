// glitchdeck - enhanced lagswitch, 2020
// credit given to sevseg libraries, math libraries

//pin init.
const int ETHO1 = 3;
const int ETHO2 = 4;
const int POT = A0;

//init.
int totalPotReads = 30;
int totalPotValue = 0;
int potReads[30];

int readInt = 0;

float potValue = 0;
int potRounded = 0;
float theo = 0;
float sec = 0;
bool delaying = false;

int roundint = 1;
int maxSeconds = 30;

unsigned long previousMillis = 0;
unsigned long previousMillisForInterval = 0;
unsigned long previousMillisForDelaying = 0;
unsigned long delaytime = 0;

const long intervalDelaying = 200;
const long intervalOverall = 10; 

//grab the sevseg libraries
#include "SevSeg.h"
SevSeg sevseg;

//grab the math libraries
#include <math.h>

void setup() {
  byte numDigits = 4;
  byte digitPins[] = {A2, A3, A4, A5};
  byte segmentPins[] = {5, 6, 7, 8, 9, 10, 11, 12};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  pinMode(ETHO1, OUTPUT);
  pinMode(ETHO2, OUTPUT);
  Serial.begin(9600);
  Serial.println("enhanced lagswitch by glitchdeck by lucas wein, on oct. 20, 2020");

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalOverall) {
    previousMillis = currentMillis;
    
    potValue = analogRead(POT);
    potValue = -1  * (potValue - 1023);

    if (readInt <= (totalPotReads-1)) {
      potReads[readInt] = potValue;
    //  reversePotReads[readInt] = -1*(readInt-totalPotReads);
      readInt += 1;

    }
    if (readInt > (totalPotReads-1)) {
      readInt = 0;
    }

   // Serial.println(round(reversePotReads[15]+potReads[15])/2);

    totalPotValue = 0;
    potReads[-1] = potValue;
    for (byte i=0; i<totalPotReads; i+=1) {
      totalPotValue += potReads[i];
    }
    potValue = (totalPotValue / totalPotReads) -.5;
    if (potValue < 0.6) {
      potValue = 0;
    }
    if (potValue > 1023) {
      potValue = 1023;
    }
    
    //potValue = round(((potReads[0]+potReads[1]+potReads[2]+potReads[3]+potReads[4])/5)-.5);
    if (potValue > 0) {
      theo = (potValue/1023) * maxSeconds;
      theo /= roundint;
      theo = round(theo)*roundint;
      
    }
    const long interval = int(theo);
    //Serial.println(theo);
  }

  if (theo <= 0) {
    sevseg.setChars("Off"); //  here
  } else {
  sevseg.setNumber(theo);
  }
  if (theo > 0) {
    if (currentMillis - previousMillisForInterval >= int(theo)*1000) {
      previousMillisForInterval = currentMillis;
      delaying = true;
    }
  }
  if (theo <= 0) {
    delaying = false;
  }

 if (delaying == true) {
    if (currentMillis < previousMillisForDelaying + 200) {
      digitalWrite(ETHO1,LOW);
      digitalWrite(ETHO2,LOW);
    } 
    if (currentMillis >= previousMillisForDelaying + 200) {
      delaying = false;
    }
  }
  if (delaying == false) {
    previousMillisForDelaying = currentMillis;
    digitalWrite(ETHO1,HIGH);
    digitalWrite(ETHO2,HIGH);
  }
  

  sevseg.refreshDisplay();
}
