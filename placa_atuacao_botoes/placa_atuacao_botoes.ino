#include <AutoPID.h>
#include <DualVNH5019MotorShield.h>

DualVNH5019MotorShield md;

double rudderPosition, rudderCmd, rudderDriverVel, throttlePosition, throttleCmd, throttleDriverVel;

#define OUTPUT_MIN -400
#define OUTPUT_MAX 400
#define KP 25
#define KI 0.1
#define KD 0

#define rudderCmd_MIN -150      // nomalized rudder angle requested
#define rudderCmd_MAX 150
#define rudderPos_MIN -30       // actual rudder angle range (angle sensor feedback)
#define rudderPos_MAX 30

#define reversed 0

#define OUTPUT_MIN_throttle -100
#define OUTPUT_MAX_throttle 100
#define KP_throttle 5
#define KI_throttle 0.01
#define KD_throttle 0

AutoPID rudderPID(&rudderPosition, &rudderCmd, &rudderDriverVel, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
AutoPID throttlePID(&throttlePosition, &throttleCmd, &throttleDriverVel, OUTPUT_MIN_throttle, OUTPUT_MAX_throttle, KP_throttle, KI_throttle, KD_throttle);

// puxar acelera
// abrir freia
// totalmente aberto - 0
// totalmente puxado - 100
// positivo acelera
// negativo desacelera

const int buttonAuto = 2;
const int buttonFwd = 3;
const int buttonBkd = 4;
const int buttonLeft = 5;
const int buttonRight = 6;

int buttonAutoState; 
int buttonFwdState;
int buttonBkdState;
int buttonLeftState;
int buttonRightState;

void setup(){
  Serial.setTimeout(10);           // control how fast data in serial port is checked
  Serial.begin(115200);
  rudderPID.setTimeStep(200);
  throttlePID.setTimeStep(200);
  pinMode(buttonAuto, INPUT);
  pinMode(buttonFwd, INPUT);
  pinMode(buttonBkd, INPUT);
  pinMode(buttonLeft, INPUT);
  pinMode(buttonRight, INPUT);
}

void loop(){
  readButtonsState();  
  if (buttonAutoState == HIGH){
    readSerialCmd();
    
    rudderPID.run();
    limitDriverCmd(rudderDriverVel);
    md.setM1Speed(rudderDriverVel);
  
    throttlePID.run();
    limitDriverCmd(throttleDriverVel);
    printData_throttle();
    md.setM2Speed(throttleDriverVel);
  } else {
    executeButtons();
  }
}

void readButtonsState(){
  buttonAutoState = digitalRead(buttonAuto);
  buttonFwdState = digitalRead(buttonFwd);
  buttonBkdState = digitalRead(buttonBkd);
  buttonLeftState = digitalRead(buttonLeft);
  buttonRightState = digitalRead(buttonRight);
}

void executeButtons(){
  if (buttonFwdState == HIGH){
    md.setM2Speed(100);
  } else {
    md.setM2Speed(0);
  }
  if (buttonBkdState == HIGH){
    md.setM2Speed(-100);
  } else {
    md.setM2Speed(0);
  }
  if (buttonLeftState == HIGH){
    md.setM1Speed(200);
  } else {
    md.setM1Speed(0);
  }
  if (buttonRightState == HIGH){
    md.setM1Speed(-200);
  } else {
    md.setM1Speed(0);
  }
}

void readSerialCmd(){
  String inputString = "";
  if (Serial.available() > 0) {                                           // check if there is data available to read
    char buffer[64];                                                      // create a buffer to store the incoming bytes
    int length = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1); // read the bytes up to the newline character
    buffer[length] = '\0';                                                // terminate the string with a null character
    inputString = String(buffer);                                         // convert the buffer to a string
    parseCommand(inputString);
//    printData();
  }
}

void parseCommand(String input){
  if(input.length() > 0){
    String cmd_code = input.substring(0,2);           // get cmd code
    String cmd_value_str = input.substring(2,10);     // get cmd value
    double cmd_value = double(cmd_value_str.toInt());
    
     if(cmd_code == "RP"){                            // current rudder position (given by rudder pos sensor)    
       rudderPosition = cmd_value;
     } else if (cmd_code == "RC"){                    // desired rudder position (calculated by ardupilot)
       rudderCmd = map(cmd_value, rudderCmd_MIN, rudderCmd_MAX, rudderPos_MIN, rudderPos_MAX);
//       Serial.print("RUDDER POSITION IS: ");
//       Serial.println(rudderCmd);
     } else if (cmd_code == "TC"){                    // desired throttle percentage (calculated by ardupilot)
       throttleCmd = cmd_value;
//       Serial.print("THROTTLE IS: ");
//       Serial.println(throttleCmd);
     } else if (cmd_code = "TP"){
       throttlePosition = cmd_value;
     }
  }
}

void printData(){
  Serial.print("RUDDER POSITION IS: ");
  Serial.println(rudderPosition, 2);
  Serial.print("RUDDER COMMAND IS: ");
  Serial.println(rudderCmd, 2);
  Serial.print("RUDDER OUTPUT IS: ");
  Serial.println(rudderDriverVel, 2);
}

void printData_throttle(){
//  Serial.print("THROTTLE POSITION IS: ");
//  Serial.println(throttlePosition, 2);
//  Serial.print("THROTTLE COMMAND IS: ");
//  Serial.println(throttleCmd, 2);
  Serial.print("THROTTLE OUTPUT IS: ");
  Serial.println(throttleDriverVel, 2);
}

double limitDriverCmd(double value){
  return constrain(value, OUTPUT_MIN, OUTPUT_MAX);
}
