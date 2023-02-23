#include <AutoPID.h>
#include <DualVNH5019MotorShield.h>

DualVNH5019MotorShield md;

double rudderPosition, rudderCmd, throttleCmd, rudderDriverVel;

#define OUTPUT_MIN -400
#define OUTPUT_MAX 400
#define KP 7
#define KI 0.005
#define KD 0

#define rudderCmd_MIN -100
#define rudderCmd_MAX 100
#define rudderPos_MIN -50
#define rudderPos_MAX 50

int throttleCounter;

AutoPID rudderPID(&rudderPosition, &rudderCmd, &rudderDriverVel, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

void setup(){
  Serial.setTimeout(2);         // check if there is data available to read
  Serial.begin(115200);         // check if there is data available to read
  rudderPID.setTimeStep(200);   // check if there is data available to read
}

void loop(){
  readSerialCmd();
  rudderPID.run();
  limitDriverCmd(rudderDriverVel);
  md.setM1Speed(rudderDriverVel);
}

void readSerialCmd(){
  String inputString = "";
  if (Serial.available() > 0) {                                           // check if there is data available to read
    char buffer[64];                                                      // create a buffer to store the incoming bytes
    int length = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1); // read the bytes up to the newline character
    buffer[length] = '\0';                                                // terminate the string with a null character
    inputString = String(buffer);                                         // convert the buffer to a string
    parseCommand(inputString);
    printData();
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
     } else if (cmd_code == "TC"){                    // desired throttle percentage (calculated by ardupilot)
       throttleCmd = cmd_value;
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

double limitDriverCmd(double value){
  return constrain(value, -400, 400);
}
