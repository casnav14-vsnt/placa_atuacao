#include <AutoPID.h>
#include <DualVNH5019MotorShield.h>

DualVNH5019MotorShield md;

double rudderPosition, rudderCmd, throttleFront, throttleBack, driverVel;

#define OUTPUT_MIN -400
#define OUTPUT_MAX 400
#define KP 1
#define KI 0
#define KD 0

#define rudderCmd_MIN 1000
#define rudderCmd_MAX 2000
#define rudderPos_MIN -50
#define rudderPos_MAX 50

AutoPID rudderPID(&rudderPosition, &rudderCmd, &driverVel, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

void setup() {
  Serial.setTimeout(2);
  Serial.begin(115200);
  rudderPID.setTimeStep(200);
}

void loop() {
  String inputString = "";
  if (Serial.available() > 0) { // check if there is data available to read
    char buffer[64]; // create a buffer to store the incoming bytes
    int length = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1); // read the bytes up to the newline character
    buffer[length] = '\0'; // terminate the string with a null character
    inputString = String(buffer); // convert the buffer to a string
    parseCommand(inputString);
  }
  rudderPID.run();
}

void parseCommand(String input){
  if(input.length() > 0){
    String cmd_code = input.substring(0,2);
    String cmd_value_str = input.substring(2,10);
    double cmd_value = double(cmd_value_str.toInt());
    
     if(cmd_code == "RP"){
       rudderPosition = cmd_value;
     } else if (cmd_code == "RC"){
       rudderCmd = map(cmd_value, rudderCmd_MIN, rudderCmd_MAX, rudderPos_MIN, rudderPos_MAX);
     } else if (cmd_code == "TF"){
       throttleFront = cmd_value;
     } else if (cmd_code == "TB"){
       throttleBack = cmd_value;
     }
  }
}
