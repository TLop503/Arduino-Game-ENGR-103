#include <Adafruit_CircuitPlayground.h>

volatile bool switchFlag;
volatile bool buttonFlagL;
volatile bool buttonFlagR;
const int inSwitchPin = 7;
const int inButtonPinL = 4;
const int inButtonPinR = 5;
bool switchState;


void setup() {
  CircuitPlayground.begin(10);
  
  Serial.begin(9600);

  //Set up interrupt pins
  pinMode(inSwitchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(inSwitchPin), switchInt, CHANGE);
  pinMode(inButtonPinL, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(inButtonPinL), buttLInt, FALLING);
  pinMode(inButtonPinR, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(inButtonPinR), buttRInt, FALLING);
}

void switchInt(){
  switchFlag = !switchFlag;
}
void buttLInt(){
  buttonFlagL = true;
}
void buttRInt(){
  buttonFlagR = true;
}

float speed = 1.00;
bool isCorrect = true;
int goal;
bool color;

void resetFlags(){
  buttonFlagL = false;
  buttonFlagR = false;
}


void generateGoal(){
  goal = random(10);
  int temp = random(2);
  if (temp == 0){
    color = true;
  }
  else {
    color = false;
  }
}


void generateLights(int goal, int index){
  CircuitPlayground.clearPixels();
  if (color) {
    CircuitPlayground.setPixelColor(goal, 255, 0, 0);
  }
  else {
      CircuitPlayground.setPixelColor(goal, 0, 0, 255);
  }
  CircuitPlayground.setPixelColor(index, 255, 255, 255);
}


void loop() {
  if (switchFlag) {
    if (isCorrect) {
      generateGoal();
      isCorrect = false;
    }
    else {
      for (int index = 0; index < 10; index++) {
        generateLights(goal, index);
        if (((color && buttonFlagL) || (!color && buttonFlagR)) && (index == goal)) {
          speed = 1.5 * speed;
          CircuitPlayground.playTone(700,100);
          isCorrect = true;
          resetFlags();
        }
        else if (buttonFlagL || buttonFlagR) {
          CircuitPlayground.playTone(300,100);
          speed = 1;
          resetFlags();
        } 
        delay(500 / speed);
      }
      for (int index = 9; index > -1; index--) {
        generateLights(goal, index);
        if (((color && buttonFlagL) || (!color && buttonFlagR)) && (index == goal)) {
          CircuitPlayground.playTone(700,100);
          speed = 1.5 * speed;
          isCorrect = true;
          resetFlags();
        }
        else if (buttonFlagL || buttonFlagR) {
          CircuitPlayground.playTone(300,100);
          speed = 1;
          resetFlags();
        }
        delay(500 / speed);    
      }
    }
  }
  else {
    CircuitPlayground.clearPixels();
    speed = 1;
  }
}