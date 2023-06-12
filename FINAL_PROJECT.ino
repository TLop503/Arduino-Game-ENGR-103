#include <Adafruit_CircuitPlayground.h>

//vars for interrupts, pins
volatile bool switchFlag;
volatile bool buttonFlagL;
volatile bool buttonFlagR;
const int inSwitchPin = 7;
const int inButtonPinL = 4;
const int inButtonPinR = 5;
bool switchState;


void setup() {
  //boilerplate
  CircuitPlayground.begin(10);
  Serial.begin(9600);
  pinMode(inSwitchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(inSwitchPin), switchInt, CHANGE);
  pinMode(inButtonPinL, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(inButtonPinL), buttLInt, FALLING);
  pinMode(inButtonPinR, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(inButtonPinR), buttRInt, FALLING);
}

//ISRs
void switchInt(){
  switchFlag = !switchFlag;
}
void buttLInt(){
  buttonFlagL = true;
}
void buttRInt(){
  buttonFlagR = true;
}

//gobal vars used throughout loop()
//There is a better way to do this, but diminishing returns
float speed = 1.00;
bool isCorrect = true;
int goal;
bool color;

//resets flags from ISRs
void resetFlags(){
  buttonFlagL = false;
  buttonFlagR = false;
}

//Picks a random light and sets it to red or blue
void generateGoal(){
  goal = random(10);

  //I could probably just set color to temp
  int temp = random(2);
  if (temp == 0){
    color = true;
  }
  else {
    color = false;
  }
}

//sets goal to it's color, and the current position of the light to white
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

//this is definitely unoptimized but cest la vie
void loop() {
  //If the board is "on" play the game
  if (switchFlag) {
    //if the player clicked the right button at the right time set a new goal
    if (isCorrect) {
      generateGoal();
      isCorrect = false;
    }

    //otherwise continue checking for correct button
    else {
      
      //goes through loop counter clockwise
      for (int index = 0; index < 10; index++) {
        generateLights(goal, index);
        //if you clicked the correct button and matched the color
        //not OR is inclusive so clicking both buttons at the same time also works, which is nice for testing
        if (((color && buttonFlagL) || (!color && buttonFlagR)) && (index == goal)) {
          speed = 1.5 * speed;
          CircuitPlayground.playTone(700,100); //somewhat happy beep
          isCorrect = true;
          resetFlags();
        }
        else if (buttonFlagL || buttonFlagR) {
          CircuitPlayground.playTone(300,100);  //somewhat unhappy beep
          speed = 1;
          resetFlags();
        } 
        delay(500 / speed);
      }

      //and then clockwise
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

  //if board is off just clear pixels and reset speed.
  else {
    CircuitPlayground.clearPixels();
    speed = 1;
  }
}