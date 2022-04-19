#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>
#include <Braccio.h>

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo gripper;
Servo wrist_ver;

int base_angle = 10;
int shoulder_angle = 90;
int elbow_angle = 45;
int wrist_ver_angle = 180;
int wrist_rot_angle = 90;
int gripper_angle = 73;

bool armMode = false;
bool selectedPressed = false;
bool clawPressed = false;

int motorSpeed = 255;

enum Dir { None, Forward, Back, Left, Right, ForwardLeft, ForwardRight, BackLeft, BackRight };

void change_claw(){
  if (gripper_angle == 73)
    gripper_angle = 10;
  else
    gripper_angle = 73;  
}

void moveBraccio(){
  Braccio.ServoMovement(10, base_angle, shoulder_angle, elbow_angle, wrist_ver_angle, wrist_rot_angle, gripper_angle);
}

void increase_shoulder_angle(){ 
  if (shoulder_angle < 165){
    shoulder_angle++;
  }
  else{
    Serial.write("Increase impossible");
  }
}

void decrease_shoulder_angle(){
  if (shoulder_angle > 15){
    shoulder_angle--;
  }
  else{
    Serial.write("Decrease impossible");
  }
}

void increase_elbow_angle(){
  if (elbow_angle < 180){
    elbow_angle++;
  }
  else{
    Serial.write("Increase impossible");
  }
}

void decrease_elbow_angle(){
  if (elbow_angle > 0){
    elbow_angle--;
  }
  else{
    Serial.write("Decrease impossible");
  }
}

void increase_wrist_angle(){
  if (wrist_ver_angle < 180){
    wrist_ver_angle++;
  }
  else{
    Serial.write("Increase impossible");
  }
}

void decrease_wrist_angle(){
  if (wrist_ver_angle > 0){
    wrist_ver_angle--;
  }
  else{
    Serial.write("Decrease impossible");
  }
}



class Motor {
  
  private:
  int motorIN1;
  int motorIN2;
  int motorENA;
  
  public:
  Motor(int motorENAPin, int motorIN1Pin, int motorIN2Pin) {

    motorENA = motorENAPin;
    motorIN1 = motorIN1Pin;
    motorIN2 = motorIN2Pin;

    pinMode(motorENA, OUTPUT);
    pinMode(motorIN1, OUTPUT);
    pinMode(motorIN2, OUTPUT);
  }

  void breakMotor() {
    analogWrite(motorENA, 0);
    digitalWrite(motorIN1, 0); 
    digitalWrite(motorIN2, 0);
  }

  void runForward() {
    analogWrite(motorENA, motorSpeed);
    digitalWrite(motorIN1, 1); 
    digitalWrite(motorIN2, 0);
  }

  void runBack() {
    analogWrite(motorENA, motorSpeed);
    digitalWrite(motorIN1, 0); 
    digitalWrite(motorIN2, 1);
  }
};

Motor* motorForwardLeft;
Motor* motorForwardRight;
Motor* motorBackLeft;
Motor* motorBackRight;

Dir getDirection(String dir) {
  
  if (dir == "01")
    return Forward;

  if (dir == "0-1")
    return Back;

  if (dir == "10")
    return Right;

  if (dir == "-10")
    return Left;

  if (dir == "11")
    return ForwardRight;

  if (dir == "-11")
    return ForwardLeft;

  if (dir == "1-1")
    return BackRight; 

  if (dir == "-1-1")
    return BackLeft;

  return None;
}

void setup() {
  
  motorForwardLeft = new Motor(A1, 28, 26);
  motorForwardRight = new Motor(A0, 22, 24);
  motorBackLeft = new Motor(A2, 25, 23);
  motorBackRight = new Motor(A3, 27, 29);

  Braccio.begin();
  Serial.begin(9600);   
  Dabble.begin(9600);
}

void walk(int x, int y) {

  switch (getDirection((String(x) + String(y)))) {
    
    case Forward:
      motorForwardLeft->runForward();
      motorForwardRight->runForward();
      motorBackLeft->runForward();
      motorBackRight->runForward();
      break;

    case Back:
      motorForwardLeft->runBack();
      motorForwardRight->runBack();
      motorBackLeft->runBack();
      motorBackRight->runBack();
      break;

    case Right:
      motorForwardLeft->runForward();
      motorForwardRight->runBack();
      motorBackLeft->runBack();
      motorBackRight->runForward();
      break;

    case Left:
      motorForwardLeft->runBack();
      motorForwardRight->runForward();
      motorBackLeft->runForward();
      motorBackRight->runBack();
      break;

    case ForwardRight:
      motorForwardLeft->runForward();
      motorForwardRight->breakMotor();
      motorBackLeft->breakMotor();
      motorBackRight->runForward();
      break;

    case ForwardLeft:
      motorForwardLeft->breakMotor();
      motorForwardRight->runForward();
      motorBackLeft->runForward();
      motorBackRight->breakMotor();
      break;

    case BackRight:
      motorForwardLeft->breakMotor();
      motorForwardRight->runBack();
      motorBackLeft->runBack();
      motorBackRight->breakMotor();
      break;

    case BackLeft:
      motorForwardLeft->runBack();
      motorForwardRight->breakMotor();
      motorBackLeft->breakMotor();
      motorBackRight->runBack();
      break;
      
    default:
      motorForwardLeft->breakMotor();
      motorForwardRight->breakMotor();
      motorBackLeft->breakMotor();
      motorBackRight->breakMotor();
      break;
  }
}

void rotate(int rotation) {

  if (rotation < 0) {
    motorForwardLeft->runBack();
    motorForwardRight->runForward();
    motorBackLeft->runBack();
    motorBackRight->runForward();
    return;
  }

  if (rotation > 0) {
    motorForwardLeft->runForward();
    motorForwardRight->runBack();
    motorBackLeft->runForward();
    motorBackRight->runBack();
    return;
  }
}

void loop() {

  Dabble.processInput();

  if (GamePad.isSelectPressed()) {
    
    if (!selectedPressed)
      armMode = !armMode;

    selectedPressed = true;
  }
  else 
    selectedPressed = false;
  
  if (!armMode) {
    
    int x = (GamePad.isRightPressed() ? 1 : 0) + (GamePad.isLeftPressed() ? -1 : 0);
    int y = (GamePad.isUpPressed() ? 1 : 0) + (GamePad.isDownPressed() ? -1 : 0);
    int rotation = (GamePad.isCirclePressed() - GamePad.isSquarePressed());
  
    if (rotation != 0)
      rotate(rotation);
    else
      walk(x, y);
      
  } else {
    
    if (GamePad.isSquarePressed()) {

      if (!clawPressed)
        change_claw();

      clawPressed = true;
    }
    else
      clawPressed = false;

    if (GamePad.isUpPressed())
      increase_elbow_angle();

    if (GamePad.isDownPressed())
      decrease_elbow_angle();

    if (GamePad.isRightPressed())
      increase_wrist_angle();

    if (GamePad.isLeftPressed())
      decrease_wrist_angle();
      
    if (GamePad.isTrianglePressed())
      increase_shoulder_angle();

    if (GamePad.isCrossPressed())
      decrease_shoulder_angle();
      
    moveBraccio();
  }
}
