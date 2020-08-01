/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       williamgiles                                              */
/*    Created:      Fri Jul 31 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// FrontLeft            motor         11              
// FrontRight           motor         12              
// BackLeft             motor         20              
// BackRight            motor         19              
// fricken_yeet         motor         5               
// LeftClaw             motor         1               
// RightClaw            motor         6               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;
vex::competition Competition;

float move = 1;
int dead_zone = 30;

#define btnNONE 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnRIGHT 4
#define btnA 5
#define btnB 6
#define btnX 7
#define btnY 8
#define btnR1 9
#define btnR2 10
#define btnL1 11
#define btnL2 12
#define bounceDelay 20
#define refreshDelay 150
#define screenTextWidth 16
#define screenTextHeight 3
#define maxMenus 3
#define maxOptions 3
#define modeDisabled 0
#define modeDriver 1
#define modeAuton 2
#define modeError 3
#define RED 0
#define BLUE 1
#define FRONT 0
#define BACK 1
#define SKILLS 2
#define RC 0
#define TANK 1
#define RC2 2
#define AUTON_COLOR 0
#define AUTON_TYPE 1
#define AUTON_DRIVE 2

int armlimitup = 900;
int armlimitdown = 130;

bool isCursorOn = false;
int tempStatus = 0;

int F_Left;
int B_Left;
int F_Right;
int B_Right;

int Ch1;
int Ch2;
int Ch3;
int Ch4;

void delay(int amount_eeeeee) { vex::task::sleep(amount_eeeeee); }

//===============================================MADNESS===============================================

std::string robotStatus[4] = {"Robot Disabled", "Driver Control",
                              "Auton Control ", "MODE ERROR    "};

int maxMenusIndex[maxMenus] = {3, 3, 3};
int configuration[maxMenus] = {0, 0, 0};

std::string menuTypes[maxMenus] = {"Color: ", "Option: ", "Drive: "};

std::string menuOptions[maxMenus][maxOptions] = {{"Red", "Blue", "Hmmm"},
                                                 {"Front", "Back", "Skills"},
                                                 {"RC", "Tank", "RC2"}};

int keyPressedRaw() {
  if (Controller1.ButtonUp.pressing() == true)
    return btnUP;
  if (Controller1.ButtonDown.pressing() == true)
    return btnDOWN;
  if (Controller1.ButtonLeft.pressing() == true)
    return btnLEFT;
  if (Controller1.ButtonRight.pressing() == true)
    return btnRIGHT;
  if (Controller1.ButtonA.pressing() == true)
    return btnA;
  if (Controller1.ButtonB.pressing() == true)
    return btnB;
  if (Controller1.ButtonX.pressing() == true)
    return btnX;
  if (Controller1.ButtonY.pressing() == true)
    return btnY;
  if (Controller1.ButtonR1.pressing() == true)
    return btnR1;
  if (Controller1.ButtonR2.pressing() == true)
    return btnR2;
  if (Controller1.ButtonL1.pressing() == true)
    return btnL1;
  if (Controller1.ButtonL2.pressing() == true)
    return btnL2;
  else
    return btnNONE;
}
int keyPressed() {
  int noBounceKey = keyPressedRaw();
  vex::task::sleep(bounceDelay);
  if (noBounceKey == keyPressedRaw()) {
    return noBounceKey;
  } else {
    return btnNONE;
  }
}
void clearLine(int l_row) {
  if (isCursorOn == true) {
    Controller1.Screen.setCursor(l_row + 1, 2);
    Controller1.Screen.print("               ");
  } else
    Controller1.Screen.clearLine();
}
void print(std::string text, int row, int col) {
  clearLine(row);
  if (isCursorOn == true) {
    Controller1.Screen.setCursor(row + 1, col + 2);
  } else {
    Controller1.Screen.setCursor(row + 1, col + 1);
  }
  Controller1.Screen.print(text.c_str());
}
void selector(int row) {
  isCursorOn = true;
  for (int i = 1; i <= screenTextHeight; i++) {
    Controller1.Screen.setCursor(i, 1);
    Controller1.Screen.print("|");
  }
  int showCursor = (row % screenTextHeight) + 1;
  Controller1.Screen.setCursor(showCursor, 1);
  Controller1.Screen.print(">");
}
void notificationHUD(std::string str) {
  clearLine(0);
  print(str, 0, 0);
}
int currStatus() {
  int status;
  if (Competition.isEnabled()) {
    if (Competition.isAutonomous()) {
      status = modeAuton;
    } else if (Competition.isDriverControl()) {
      status = modeDriver;
    } else {
      status = modeError;
    }
  } else {
    status = modeDisabled;
  }
  return status;
}
void statusHUD() {
  std::string temp;
  print(robotStatus[currStatus()], 1, 0);
  temp = menuOptions[0][configuration[0]] + " " +
         menuOptions[1][configuration[1]] + " " +
         menuOptions[2][configuration[2]];
  print(temp, 2, 0);
}
void displayMenu(int currRow, int configuration[]) {
  std::string temp;
  selector(currRow);
  for (int i = 0; i < screenTextHeight; i++) {
    temp = menuTypes[i] + menuOptions[i][configuration[i]];
    print(temp, i, 0);
  }
}
int getValues(int wantConfig) {
  if (wantConfig > maxMenus) {
    return NULL;
  } else {
    return configuration[wantConfig];
  }
}
void menuCONFIG() {
  isCursorOn = true;
  int currCursorMenu = 0;
  int currCursorOptions = 0;
  bool isAutonSelectScreen = true;
  while (isAutonSelectScreen == true) {
    displayMenu(currCursorMenu, configuration);
    bool isValidButton = false;
    while (isValidButton == false) {
      switch (keyPressed()) {
      case (btnUP):
        isValidButton = true;
        currCursorMenu--;
        currCursorMenu = (currCursorMenu + maxMenus) % maxMenus;
        currCursorOptions = configuration[currCursorMenu];
        break;
      case (btnDOWN):
        isValidButton = true;
        currCursorMenu++;
        currCursorMenu = (currCursorMenu + maxMenus) % maxMenus;
        currCursorOptions = configuration[currCursorMenu];
        break;
      case (btnLEFT):
        isValidButton = true;
        currCursorOptions--;
        currCursorOptions =
            (currCursorOptions + maxMenusIndex[currCursorMenu]) %
            maxMenusIndex[currCursorMenu];
        configuration[currCursorMenu] = currCursorOptions;
        break;
      case (btnRIGHT):
        isValidButton = true;
        currCursorOptions++;
        currCursorOptions =
            (currCursorOptions + maxMenusIndex[currCursorMenu]) %
            maxMenusIndex[currCursorMenu];
        configuration[currCursorMenu] = currCursorOptions;
        break;
      case (btnA):
        isValidButton = true;
        isAutonSelectScreen = false;
        isCursorOn = false;
        break;
      default:
        break;
      }
    }
  }
}

//===============================================END OF
// MADNESS===============================================

void calabrate(void) {

  FrontLeft.resetRotation();
  FrontRight.resetRotation();
  BackLeft.resetRotation();
  BackRight.resetRotation();

  fricken_yeet.resetRotation();
}

void donut(void) {
  // does a donut
  Ch1 = -100;
  Ch3 = 0;
  Ch4 = 40;

  F_Left = Ch3 + Ch4 + Ch1;
  B_Left = Ch3 + Ch4 - Ch1;
  F_Right = Ch3 - Ch4 - Ch1;
  B_Right = Ch3 - Ch4 + Ch1;

  FrontLeft.spin(vex::directionType::fwd, F_Left, vex::velocityUnits::pct);
  BackLeft.spin(vex::directionType::fwd, B_Left, vex::velocityUnits::pct);
  FrontRight.spin(vex::directionType::fwd, F_Right, vex::velocityUnits::pct);
  BackRight.spin(vex::directionType::fwd, B_Right, vex::velocityUnits::pct);

  vex::task::sleep(2071);
}

void Move(int Ch1_, int Ch3_, int Ch4_) {
  Ch1 = Ch1_;
  Ch3 = Ch3_;
  Ch4 = Ch4_;

  F_Left = Ch3 + Ch4 + Ch1;
  B_Left = Ch3 + Ch4 - Ch1;
  F_Right = Ch3 - Ch4 - Ch1;
  B_Right = Ch3 - Ch4 + Ch1;

  FrontLeft.spin(vex::directionType::fwd, F_Left, vex::velocityUnits::pct);
  BackLeft.spin(vex::directionType::fwd, B_Left, vex::velocityUnits::pct);
  FrontRight.spin(vex::directionType::fwd, F_Right, vex::velocityUnits::pct);
  BackRight.spin(vex::directionType::fwd, B_Right, vex::velocityUnits::pct);
}

void pre_auton(void) {
  menuCONFIG();

  if (getValues(AUTON_COLOR) == RED) {
    Brain.Screen.setFillColor(red);
  } else if (getValues(AUTON_COLOR) == BLUE) {
    Brain.Screen.setFillColor(blue);
  }

  Brain.Screen.drawRectangle(-10, -10, 500, 500);

  Brain.Screen.setPenColor(color::white);
  Brain.Screen.setFont(prop60);
  Brain.Screen.printAt(150, 120, "2903 S");

  Controller1.rumble(".");
}

void autonFrontRow() {
  // win
  vex::task::sleep(2000);
}

void autonBackRow() {
  // win
  vex::task::sleep(2000);
}

void autonSkills() {
  // win
  vex::task::sleep(2000);
}

void auton(void) {
  if (getValues(AUTON_TYPE) == FRONT) {
    autonFrontRow();
  } else if (getValues(AUTON_TYPE) == BACK) {
    autonBackRow();
  } else if (getValues(AUTON_TYPE) == SKILLS) {
    autonSkills();
  }

  fricken_yeet.rotateTo(830, degrees);

  fricken_yeet.rotateTo(10, degrees);

  notificationHUD("Auton: DONE");
  Controller1.rumble(".");
}

void RCDrive(void) {
  Ch1 = Controller1.Axis1.position(percent) * move;
  Ch3 = Controller1.Axis3.position(percent) * move;
  Ch4 = Controller1.Axis4.position(percent) * move;

  if(Ch1 < dead_zone && Ch1 > -dead_zone){
    Ch1 = 0;
  }
  if(Ch2 < dead_zone && Ch2 > -dead_zone){
    Ch2 = 0;
  }
  if(Ch3 < dead_zone && Ch3 > -dead_zone){
    Ch3 = 0;
  }
  if(Ch4 < dead_zone && Ch4 > -dead_zone){
    Ch4 = 0;
  }

  F_Left = Ch3 + Ch4 + Ch1;
  B_Left = Ch3 + Ch4 - Ch1;
  F_Right = Ch3 - Ch4 - Ch1;
  B_Right = Ch3 - Ch4 + Ch1;

  FrontLeft.spin(vex::directionType::fwd, F_Left, vex::velocityUnits::pct);
  BackLeft.spin(vex::directionType::fwd, B_Left, vex::velocityUnits::pct);
  FrontRight.spin(vex::directionType::fwd, F_Right, vex::velocityUnits::pct);
  BackRight.spin(vex::directionType::fwd, B_Right, vex::velocityUnits::pct);
}

void RCDrive2(void) {
  Ch1 = Controller1.Axis1.position(percent) * move;
  Ch3 = Controller1.Axis3.position(percent) * move;
  Ch4 = Controller1.Axis4.position(percent) * move;

  if(Ch1 < dead_zone && Ch1 > -dead_zone){
    Ch1 = 0;
  }
  if(Ch2 < dead_zone && Ch2 > -dead_zone){
    Ch2 = 0;
  }
  if(Ch3 < dead_zone && Ch3 > -dead_zone){
    Ch3 = 0;
  }
  if(Ch4 < dead_zone && Ch4 > -dead_zone){
    Ch4 = 0;
  }

  F_Left = Ch3 + Ch4 + Ch1;
  B_Left = Ch3 + Ch4 - Ch1;
  F_Right = Ch3 - Ch4 - Ch1;
  B_Right = Ch3 - Ch4 + Ch1;

  FrontLeft.spin(vex::directionType::fwd, F_Left, vex::velocityUnits::pct);
  BackLeft.spin(vex::directionType::fwd, B_Left, vex::velocityUnits::pct);
  FrontRight.spin(vex::directionType::fwd, F_Right, vex::velocityUnits::pct);
  BackRight.spin(vex::directionType::fwd, B_Right, vex::velocityUnits::pct);
}

void tankDrive(void) {
  Ch2 = Controller1.Axis2.position(percent) * move;
  Ch3 = Controller1.Axis3.position(percent) * move;
  Ch4 = Controller1.Axis4.position(percent) * move;

  if(Ch1 < dead_zone && Ch1 > -dead_zone){
    Ch1 = 0;
  }
  if(Ch2 < dead_zone && Ch2 > -dead_zone){
    Ch2 = 0;
  }
  if(Ch3 < dead_zone && Ch3 > -dead_zone){
    Ch3 = 0;
  }
  if(Ch4 < dead_zone && Ch4 > -dead_zone){
    Ch4 = 0;
  }

  F_Left = Ch3 + Ch4;
  B_Left = Ch3 - Ch4;
  F_Right = Ch2 - Ch4;
  B_Right = Ch2 + Ch4;

  FrontLeft.spin(vex::directionType::fwd, F_Left, vex::velocityUnits::pct);
  BackLeft.spin(vex::directionType::fwd, B_Left, vex::velocityUnits::pct);
  FrontRight.spin(vex::directionType::fwd, F_Right, vex::velocityUnits::pct);
  BackRight.spin(vex::directionType::fwd, B_Right, vex::velocityUnits::pct);
}

void user(void) {

  FrontLeft.setStopping(hold);
  FrontRight.setStopping(hold);
  BackLeft.setStopping(hold);
  BackRight.setStopping(hold);

  while (1) {

    Brain.Screen.setCursor(10, 10);
    Brain.Screen.print(fricken_yeet.rotation(deg));

    if (keyPressed() == btnB && 1 == 2) {

      Move(0, 20, 0);

      delay(2000);

      Move(20, 0, 0);

      delay(2000);

      Move(0, 30, 0);

      delay(2500);
    }

    if (keyPressed() == btnY) {
      fricken_yeet.rotateTo(830, degrees);

      fricken_yeet.rotateTo(10, degrees);
    }

     if (keyPressed() == btnUP) {
      move = 1;
    }
    if (keyPressed() == btnDOWN) {
      move = .2;
    }

    switch (getValues(AUTON_DRIVE)) {
    case RC:
      RCDrive();
      break;
    case TANK:
      tankDrive();
      break;
    case RC2:
      RCDrive2();
      break;
    default:
      break;
    }
    if(keyPressed() == btnL1){
      LeftClaw.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
      RightClaw.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
    } else if(keyPressed() == btnL2){
      LeftClaw.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);
      RightClaw.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);
    } else {
      LeftClaw.stop();
      RightClaw.stop();
    }

  }
}

int main() {
  vexcodeInit();

  calabrate();

  pre_auton();

  Competition.autonomous(auton);
  Competition.drivercontrol(user);

  while (1) {
    if (tempStatus != currStatus()) {
      statusHUD();
      tempStatus = currStatus();
    }
    vex::task::sleep(200);
  }
}
