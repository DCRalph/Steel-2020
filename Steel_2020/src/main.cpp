/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       William Giles                                              */
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
// BackLeft             motor         16
// BackRight            motor         17
// LeftClaw             motor         1
// RightClaw            motor         2
// Indexer              motor         10
// shitter              motor         9
// Vision               vision        20
// ball                 bumper        H
// frontLeftLine        line          F
// frontRightLine       line          G
// backLeftLine         line          E
// backRightLine        line          D
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "iostream"
#include "vex.h"
#include <string>

using namespace vex;
using signature = vision::signature;
competition Competition;

float move_max = .9;
float move_min = .3;
float spin = 1;

float move = move_max;

int dead_zone = 20;

bool intake = false;

bool automatic = true;
bool ball_found = false;

#define NONE 2

int my_color;
int ball_color = NONE;

double last_pic;

double now;

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
#define CLOSED 0
#define OPEN 1

int line_value = 66;

bool open_or_closed = CLOSED;

int power_usage = 0;

bool screen = false;

bool isCursorOn = false;
int tempStatus = 0;
int autoTempStatus = 0;

int F_Left;
int B_Left;
int F_Right;
int B_Right;

int Ch1;
int Ch2;
int Ch3;
int Ch4;

void delay(int amount_eeeeee) { task::sleep(amount_eeeeee); }

//===============================================MADNESS===============================================

std::string robotStatus[4] = {"Robot Disabled", "Driver Control",
                              "Auton Control ", "MODE ERROR    "};

int maxMenusIndex[maxMenus] = {2, 7, 3};
int configuration[maxMenus] = {0, 0, 0};

std::string menuTypes[maxMenus] = {"Color: ", "Option: ", "Drive: "};

std::string menuOptions[maxMenus][7] = {
    {"Red", "Blue", ""},
    {"L 1", "L 2", "L 3", "R 1", "R 2", "R 3", "Skills"},
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
  task::sleep(bounceDelay);
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
void clearScreen(void) { Controller1.Screen.clearScreen(); }

void print(std::string text, int row, int col) {
  clearLine(row);
  if (isCursorOn == true) {
    Controller1.Screen.setCursor(row + 1, col + 2);
  } else {
    Controller1.Screen.setCursor(row + 1, col + 1);
  }
  Controller1.Screen.print(text.c_str());
}

void print2(std::string text, int row, int col) {
  Controller1.Screen.setCursor(row, 0);
  Controller1.Screen.print("               ");
  Controller1.Screen.setCursor(row, col);
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
         menuOptions[2][configuration[2]] + " ";
  temp += automatic ? "V-" + menuOptions[0][configuration[0]] : "V-OFF ";
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
}

void calabrate_vision(void) {
  while (keyPressed() != btnNONE)
    ;

  clearScreen();

  bool calabrating = true;
  bool wantRed = true;
  bool wantBlue = false;

  while (calabrating) {

    if (wantRed) {
      print2("Red ball plz", 1, 0);
      print2("Press A", 2, 0);

      if (keyPressed() == btnA && !ball.pressing()) {
        print2("Put in a ball", 1, 0);
        print2("Dumb shit!", 2, 0);
        delay(2000);
        while (keyPressed() != btnNONE)
          ;
      }

      if (keyPressed() == btnA && ball.pressing()) {
        // calbrate

        wantRed = false;
        wantBlue = true;
      }
    }

    if (wantBlue) {
      print2("Blue ball plz", 0, 0);
      print2("Press A", 2, 0);

      if (keyPressed() == btnA && !ball.pressing()) {
        print2("Put in a ball", 1, 0);
        print2("Dumb shit!", 2, 0);
        delay(2000);
        while (keyPressed() != btnNONE)
          ;
      }

      if (keyPressed() == btnA && ball.pressing()) {
        // calbrate

        signature Vision__BLUE_BALLj =
            signature(2, -3389, -1093, -2241, 1489, 15421, 8455, 0.8, 0);

        wantRed = false;
        wantBlue = false;
        calabrating = false;
      }
    }
  }
  clearScreen();
  print2("Done", 0, 0);
  delay(1000);
  statusHUD();
}

void intakeRoutine() {
  if (intake) {
    intake = false;
    LeftClaw.stop();
    RightClaw.stop();
  } else {
    intake = true;
    LeftClaw.spin(directionType::fwd, 100, velocityUnits::pct);
    RightClaw.spin(directionType::fwd, 100, velocityUnits::pct);
  }
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

  FrontLeft.spin(directionType::fwd, F_Left, velocityUnits::pct);
  BackLeft.spin(directionType::fwd, B_Left, velocityUnits::pct);
  FrontRight.spin(directionType::fwd, F_Right, velocityUnits::pct);
  BackRight.spin(directionType::fwd, B_Right, velocityUnits::pct);

  task::sleep(2071);
}

void Move(int Ch1_, int Ch3_, int Ch4_) {
  Ch1 = Ch1_;
  Ch3 = Ch3_;
  Ch4 = Ch4_;

  F_Left = Ch3 + Ch4 + Ch1;
  B_Left = Ch3 + Ch4 - Ch1;
  F_Right = Ch3 - Ch4 - Ch1;
  B_Right = Ch3 - Ch4 + Ch1;

  FrontLeft.spin(directionType::fwd, F_Left, velocityUnits::pct);
  BackLeft.spin(directionType::fwd, B_Left, velocityUnits::pct);
  FrontRight.spin(directionType::fwd, F_Right, velocityUnits::pct);
  BackRight.spin(directionType::fwd, B_Right, velocityUnits::pct);
}

void Move_f(int d, int s) {

  FrontLeft.rotateFor(d, rev, false);
  BackLeft.rotateFor(d, rev, false);
  FrontRight.rotateFor(d, rev, false);
  BackRight.rotateFor(d, rev, true);
}

void Move_d(float mm, int s) {

  float rot = mm / 330;

  rot *= 360;
  rot *= 1.41;

  FrontLeft.startRotateFor(rot, deg, s, velocityUnits::pct);
  BackLeft.startRotateFor(rot, deg, s, velocityUnits::pct);
  FrontRight.startRotateFor(rot, deg, s, velocityUnits::pct);
  BackRight.rotateFor(rot, deg, s, velocityUnits::pct);
}

void pre_auton(void) {
  menuCONFIG();

  Controller1.rumble(".");
}

void intakeOn() {
  LeftClaw.spin(directionType::fwd, 100, velocityUnits::pct);
  RightClaw.spin(directionType::fwd, 100, velocityUnits::pct);
}
void intakeOff() {
  LeftClaw.stop();
  RightClaw.stop();
}
void intakeOpen() {
  LeftClaw.startRotateFor(-100, rotationUnits::deg);
  RightClaw.startRotateFor(-100, rotationUnits::deg);
}
void intakeClose() {
  LeftClaw.startRotateFor(100, rotationUnits::deg);
  RightClaw.startRotateFor(100, rotationUnits::deg);
}
void indexerOn() {
  Indexer.spin(directionType::fwd, 100, velocityUnits::pct);
  shitter.spin(directionType::fwd, 100, velocityUnits::pct);
}
void indexerOff() {
  Indexer.stop();
  shitter.stop();
}

void autonLeft(int whenStop) {

  Move(40, 0, 0); // crab

  intakeOpen();

  while (backLeftLine.value(percentUnits::pct) > line_value) // wait for line
    ;

  Move(0, 0, 0); // stop drive

  delay(200);

  Move(0, 50, 0); // foward

  delay(500);

  intakeOn(); // intake on

  delay(200);

  Move(0, 0, 0); // stop drive

  indexerOn(); // indexer on

  delay(1000);

  indexerOff(); // indexer off

  delay(200);

  intakeOff(); // intake off

  delay(200);

  FrontRight.spin(directionType::rev, 50, velocityUnits::pct); // from here to
  BackRight.spin(directionType::rev, 50, velocityUnits::pct);

  FrontLeft.spin(directionType::rev, 50, velocityUnits::pct);
  BackLeft.spin(directionType::rev, 50, velocityUnits::pct);

  while (frontRightLine.value(percentUnits::pct) > line_value)
    ;

  FrontRight.stop();
  BackRight.stop();

  while (frontLeftLine.value(percentUnits::pct) > line_value)
    ;
  delay(100);

  FrontLeft.stop();
  BackLeft.stop();

  // here is the fancy rotate and wait for line

  if (whenStop == 1)
    return; // stop if only want to socre 1 ball

  delay(100);

  Move(0, 50, 0); // foward

  delay(500);

  Move(0, 0, 0); // stop

  delay(200);

  while (frontLeftLine.value(percentUnits::pct) >
         line_value) { //    from here to
    if (backLeftLine.value(percentUnits::pct) > line_value) {
      BackLeft.spin(directionType::fwd, 75, percentUnits::pct);
      BackRight.spin(directionType::rev, 75, percentUnits::pct);
      FrontLeft.spin(directionType::rev, 75, percentUnits::pct);
      FrontRight.spin(directionType::fwd, 75, percentUnits::pct);

    } else {
      BackLeft.stop();
      BackRight.stop();
      FrontLeft.spin(directionType::rev, 85, percentUnits::pct);
      FrontRight.spin(directionType::fwd, 85, percentUnits::pct);
    }
  }

  BackLeft.spin(directionType::fwd, 75, percentUnits::pct);
  BackRight.spin(directionType::rev, 75, percentUnits::pct);
  FrontLeft.spin(directionType::rev, 75, percentUnits::pct);
  FrontRight.spin(directionType::fwd, 75, percentUnits::pct);

  delay(60);

  Move(0, 0, 0); // here is fancy auto correct strafe from goal 1 to goal 2

  delay(200);

  Move(0, 50, 0); // forard

  delay(750);

  Move(0, 0, 0); // stop

  indexerOn(); // score

  delay(750);

  indexerOff();

  delay(200);

  Move(0, -50, 0); // back

  delay(300);

  Move(0, 0, 0); // stop

  if (whenStop == 2)
    return; // stop if only want to socre 2 ball

  delay(200);

  while (frontLeftLine.value(percentUnits::pct) >
         line_value) { //    from here to
    if (backLeftLine.value(percentUnits::pct) > line_value) {
      BackLeft.spin(directionType::fwd, 75, percentUnits::pct);
      BackRight.spin(directionType::rev, 75, percentUnits::pct);
      FrontLeft.spin(directionType::rev, 75, percentUnits::pct);
      FrontRight.spin(directionType::fwd, 75, percentUnits::pct);

    } else {
      BackLeft.stop();
      BackRight.stop();
      FrontLeft.spin(directionType::rev, 85, percentUnits::pct);
      FrontRight.spin(directionType::fwd, 85, percentUnits::pct);
    }
  }

  BackLeft.spin(directionType::fwd, 75, percentUnits::pct);
  BackRight.spin(directionType::rev, 75, percentUnits::pct);
  FrontLeft.spin(directionType::rev, 75, percentUnits::pct);
  FrontRight.spin(directionType::fwd, 75, percentUnits::pct);

  delay(60);

  Move(0, 0, 0); // here is fancy auto correct strafe from goal 2 to goal 3

  delay(800);

  Move(0, 0, 0);
}

void autonRight(int whenStop) {
  // win

  Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

  Move(-25, 0, 0); // crab
  delay(1100);
  Move(0, 0, 0);
  delay(200);

  LeftClaw.spin(directionType::fwd, 100, velocityUnits::pct);
  RightClaw.spin(directionType::fwd, 100, velocityUnits::pct); // on

  Move(0, 50, 0);
  delay(700);

  Indexer.stop();

  delay(300);
  Move(0, 0, 0);
  delay(100);

  Indexer.spin(directionType::fwd, 100, velocityUnits::pct);
  delay(200);

  LeftClaw.stop();
  RightClaw.stop();

  delay(500);
  Indexer.stop();

  Move(0, 50, 50);
  delay(500);
  Move(0, 0, 0);

  Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

  delay(100);
  Move(80, 0, 0);
  delay(1350);

  Move(0, 0, 0);
  delay(100);

  Move(0, 50, 0);
  delay(1000);
  Move(0, 0, 0);
  delay(100);

  delay(4000);
  Indexer.stop();

  Move(0, -100, 0);
  delay(500);
  Move(0, 0, 0);
}

void autonSkills() {
  // win

  Move_d(100, 20);
}

void auton(void) {
  FrontLeft.setStopping(hold);
  FrontRight.setStopping(hold);
  BackLeft.setStopping(hold);
  BackRight.setStopping(hold);

  LeftClaw.setStopping(hold);
  RightClaw.setStopping(hold);

  Indexer.setStopping(coast);
  shitter.setStopping(coast);

  if (configuration[1] == 0) { // L 1
    autonLeft(1);
  } else if (configuration[1] == 1) { // L 2
    autonLeft(2);
  } else if (configuration[1] == 2) { // L 3
    autonLeft(3);
  } else if (configuration[1] == 3) { // R 1
    autonLeft(1);
  } else if (configuration[1] == 4) { // R 2
    autonLeft(2);
  } else if (configuration[1] == 5) { // R 3
    autonRight(3);
  } else if (configuration[1] == 6) { // Skills
    autonSkills();
  }

  std::cout << configuration[1] << std::endl;

  notificationHUD("Auton: DONE");
  Controller1.rumble(".");
}

void RCDrive(void) {
  Ch1 = Controller1.Axis1.position(percent) * move;
  Ch3 = Controller1.Axis3.position(percent) * move;
  Ch4 = Controller1.Axis4.position(percent) * move;
  Ch4 = Ch4 * spin;

  if (Ch1 < dead_zone && Ch1 > -dead_zone) {
    Ch1 = 0;
  }
  if (Ch2 < dead_zone && Ch2 > -dead_zone) {
    Ch2 = 0;
  }
  if (Ch3 < dead_zone && Ch3 > -dead_zone) {
    Ch3 = 0;
  }
  if (Ch4 < dead_zone && Ch4 > -dead_zone) {
    Ch4 = 0;
  }

  F_Left = Ch3 + Ch4 + Ch1;
  B_Left = Ch3 + Ch4 - Ch1;
  F_Right = Ch3 - Ch4 - Ch1;
  B_Right = Ch3 - Ch4 + Ch1;

  FrontLeft.spin(directionType::fwd, F_Left, velocityUnits::pct);
  FrontRight.spin(directionType::fwd, F_Right, velocityUnits::pct);

  BackRight.spin(directionType::fwd, B_Right, velocityUnits::pct);
  BackLeft.spin(directionType::fwd, B_Left, velocityUnits::pct);
}

void RCDrive2(void) {
  Ch1 = Controller1.Axis1.position(percent) * move;
  Ch3 = Controller1.Axis3.position(percent) * move;
  Ch4 = Controller1.Axis4.position(percent) * move;

  if (Ch1 < dead_zone && Ch1 > -dead_zone) {
    Ch1 = 0;
  }
  if (Ch2 < dead_zone && Ch2 > -dead_zone) {
    Ch2 = 0;
  }
  if (Ch3 < dead_zone && Ch3 > -dead_zone) {
    Ch3 = 0;
  }
  if (Ch4 < dead_zone && Ch4 > -dead_zone) {
    Ch4 = 0;
  }

  F_Left = Ch3 + Ch4 + Ch1;
  B_Left = Ch3 + Ch4 - Ch1;
  F_Right = Ch3 - Ch4 - Ch1;
  B_Right = Ch3 - Ch4 + Ch1;

  FrontLeft.spin(directionType::fwd, F_Left, velocityUnits::pct);
  BackLeft.spin(directionType::fwd, B_Left, velocityUnits::pct);
  FrontRight.spin(directionType::fwd, F_Right, velocityUnits::pct);
  BackRight.spin(directionType::fwd, B_Right, velocityUnits::pct);
}

void tankDrive(void) {
  Ch2 = Controller1.Axis2.position(percent) * move;
  Ch3 = Controller1.Axis3.position(percent) * move;
  Ch4 = Controller1.Axis4.position(percent) * move;

  if (Ch1 < dead_zone && Ch1 > -dead_zone) {
    Ch1 = 0;
  }
  if (Ch2 < dead_zone && Ch2 > -dead_zone) {
    Ch2 = 0;
  }
  if (Ch3 < dead_zone && Ch3 > -dead_zone) {
    Ch3 = 0;
  }
  if (Ch4 < dead_zone && Ch4 > -dead_zone) {
    Ch4 = 0;
  }

  F_Left = Ch3 + Ch4;
  B_Left = Ch3 - Ch4;
  F_Right = Ch2 - Ch4;
  B_Right = Ch2 + Ch4;

  FrontLeft.spin(directionType::fwd, F_Left, velocityUnits::pct);
  BackLeft.spin(directionType::fwd, B_Left, velocityUnits::pct);
  FrontRight.spin(directionType::fwd, F_Right, velocityUnits::pct);
  BackRight.spin(directionType::fwd, B_Right, velocityUnits::pct);
}

void user(void) {

  while (keyPressed() != btnNONE)
    ;

  FrontLeft.setStopping(brake);
  FrontRight.setStopping(brake);
  BackLeft.setStopping(brake);
  BackRight.setStopping(brake);

  LeftClaw.setStopping(coast);
  RightClaw.setStopping(coast);

  Indexer.setStopping(coast);
  shitter.setStopping(coast);

  Vision.setLedMode(vision::ledMode::manual);
  Vision.setBrightness(100);
  Vision.setLedColor(0, 200, 0);

  // Controller1.ButtonL1.pressed(intakeRoutine);

  my_color = getValues(AUTON_COLOR);

  last_pic = Brain.timer(msec);

  while (1) {
    now = Brain.timer(msec);

    if (keyPressed() == btnY) {
      automatic = !automatic;
      while (keyPressed() == btnY)
        ;
    }

    if (keyPressed() == btnA) {
      // calabrate_vision();
    }

    if (keyPressed() == btnUP) {
      move = move_max;
    }
    if (keyPressed() == btnDOWN) {
      move = move_min;
    }

    if (keyPressed() == btnRIGHT && !Competition.isEnabled()) {
      auton();
    }

    if (keyPressed() == btnLEFT) {
      Move(-80, 0, 0);

      delay(500);

      while (FrontLeft.current() < 2.00 || FrontRight.current() < 2.00)
        ;

      Move(50, 0, 0);

      delay(500);

      Move(0, 0, 0);
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

    // if (Controller1.ButtonL2.pressing()) {
    //   LeftClaw.spin(directionType::rev, 100 * move, velocityUnits::pct);
    //   RightClaw.spin(directionType::rev, 100 * move, velocityUnits::pct);
    // } else {
    //   if (intake) {
    //     LeftClaw.spin(directionType::fwd, 100, velocityUnits::pct);
    //     RightClaw.spin(directionType::fwd, 100, velocityUnits::pct);
    //   } else {
    //     LeftClaw.stop();
    //     RightClaw.stop();
    //   }
    // }

    if (Controller1.ButtonL1.pressing()) {
      LeftClaw.setStopping(coast);
      RightClaw.setStopping(coast);

      LeftClaw.spin(directionType::fwd, 100, velocityUnits::pct);
      RightClaw.spin(directionType::fwd, 100, velocityUnits::pct);

      open_or_closed = CLOSED;

    } else if (Controller1.ButtonL2.pressing()) {

      if (open_or_closed == CLOSED) {
        LeftClaw.setStopping(hold);
        RightClaw.setStopping(hold);

        LeftClaw.setVelocity(100, percentUnits::pct);
        RightClaw.setVelocity(100, percentUnits::pct);

        LeftClaw.startRotateFor(-100, rotationUnits::deg);
        RightClaw.startRotateFor(-100, rotationUnits::deg);
      }

      open_or_closed = OPEN;

    } else {
      if (open_or_closed == CLOSED) {
        LeftClaw.stop();
        RightClaw.stop();
      }
    }

    if (automatic) { //======================================================

      if (now - last_pic > 500 || ball.pressing()) {
        last_pic = now;

        ball_found = false;
        ball_color = NONE;

        Vision.takeSnapshot(Vision__RED_BALL);

        //&& Vision.largestObject.centerX > 90 && Vision.largestObject.centerX <
        // 160 && Vision.largestObject.centerY > 90

        if (Vision.largestObject.exists && Vision.largestObject.centerX > 20 &&
            Vision.largestObject.centerX < 200 && ball_found == false) {
          ball_found = true;

          ball_color = RED;

          Vision.setLedColor(255, 0, 0);
        }
        Vision.takeSnapshot(Vision__BLUE_BALL);

        if (Vision.largestObject.exists && Vision.largestObject.centerX > 20 &&
            Vision.largestObject.centerX < 200 && ball_found == false) {
          ball_found = true;

          ball_color = BLUE;
          Vision.setLedColor(0, 0, 255);
        }
        if (!ball_found) {
          Vision.setLedColor(0, 255, 0);
        }
      }

      if (Controller1.ButtonX.pressing()) {

        if (Controller1.ButtonR1.pressing()) {
          if (ball_color == NONE || ball_color != my_color) {

            Indexer.spin(directionType::fwd, 100, velocityUnits::pct);
            shitter.spin(directionType::rev, 100, velocityUnits::pct);

          } else {

            Indexer.spin(directionType::fwd, 100, velocityUnits::pct);
            shitter.spin(directionType::fwd, 100, velocityUnits::pct);
          }
        } else if (Controller1.ButtonR2.pressing()) {
          if (ball_color == NONE ||
              ball_color != my_color) { // wrong ball or no ball

            shitter.spin(directionType::rev, 100, velocityUnits::pct);

            if (ball.pressing()) {

              Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

            } else {

              Indexer.spin(directionType::rev, 100, velocityUnits::pct);
            }

          } else {

            Indexer.spin(directionType::rev, 100, velocityUnits::pct);

            if (ball.pressing()) {

              shitter.spin(directionType::fwd, 100, velocityUnits::pct);

            } else {

              shitter.spin(directionType::rev, 100, velocityUnits::pct);
            }
          }
        } else {
          Indexer.stop();
          shitter.stop();
        }

      } else {
        if (Controller1.ButtonR1.pressing()) {
          if (ball_color == NONE || ball_color != my_color) {

            Indexer.spin(directionType::fwd, 100, velocityUnits::pct);
            shitter.spin(directionType::rev, 100, velocityUnits::pct);

          } else {

            Indexer.spin(directionType::fwd, 100, velocityUnits::pct);
            shitter.spin(directionType::fwd, 100, velocityUnits::pct);
          }
        } else if (Controller1.ButtonR2.pressing()) {
          if (ball_color == NONE ||
              ball_color != my_color) { // wrong ball or no ball

            shitter.spin(directionType::rev, 100, velocityUnits::pct);

            if (ball.pressing()) {

              Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

            } else {

              Indexer.spin(directionType::rev, 100, velocityUnits::pct);
            }

          } else {

            Indexer.spin(directionType::rev, 100, velocityUnits::pct);

            if (ball.pressing()) {

              shitter.spin(directionType::fwd, 100, velocityUnits::pct);

            } else {

              shitter.spin(directionType::rev, 100, velocityUnits::pct);
            }
          }
        } else {
          Indexer.stop();
          shitter.stop();
        }
      }

    } else { // auto ^^^^ .       manuel vvvvvv

      if (Controller1.ButtonX.pressing()) {
        if (Controller1.ButtonR1.pressing()) {
          shitter.spin(directionType::rev, 100, velocityUnits::pct);

          Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

        } else if (Controller1.ButtonR2.pressing()) {
          shitter.spin(directionType::rev, 100, velocityUnits::pct);

          if (ball.pressing()) {
            Indexer.spin(directionType::fwd, 100, velocityUnits::pct);
          } else {
            Indexer.spin(directionType::rev, 100, velocityUnits::pct);
          }

        } else {
          Indexer.stop();
          shitter.stop();
        }
      } else {
        if (Controller1.ButtonR1.pressing()) {

          shitter.spin(directionType::fwd, 100, velocityUnits::pct);

          Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

        } else if (Controller1.ButtonR2.pressing()) {
          Indexer.spin(directionType::rev, 100, velocityUnits::pct);

          if (!ball.pressing()) {
            shitter.spin(directionType::rev, 100, velocityUnits::pct);
          } else {
            // shitter.stop();
            shitter.spin(directionType::fwd, 100, velocityUnits::pct);
          }

        } else {
          Indexer.stop();
          shitter.stop();
        }
      }
    }
    // looooooooooooooooooooooooooooooooooooooop
  }
}

int main() {
  vexcodeInit();

  calabrate();

  Vision.setLedMode(vision::ledMode::manual);
  Vision.setLedBrightness(100);

  pre_auton();

  Competition.autonomous(auton);
  Competition.drivercontrol(user);

  std::cout << "Hello World" << std::endl;

  while (1) {
    power_usage = 0;
    power_usage += FrontLeft.power(watt);
    power_usage += FrontRight.power(watt);
    power_usage += BackLeft.power(watt);
    power_usage += BackRight.power(watt);

    power_usage += Indexer.power(watt);

    power_usage += LeftClaw.power(watt);
    power_usage += RightClaw.power(watt);

    if (Brain.Screen.pressing() && screen == false) {
      screen = true;
      Brain.Screen.clearScreen();
    }

    if (screen) {
      Brain.Screen.setFillColor(blue);
      Brain.Screen.drawRectangle(5, 20, 60, 30);

      Brain.Screen.setPenColor(color::white);
      Brain.Screen.setFont(prop20);
      Brain.Screen.printAt(25, 39, "Back");

      Brain.Screen.setCursor(1, 10);
      Brain.Screen.print(my_color);
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(1, 15);
      Brain.Screen.print(ball_color);
      Brain.Screen.print("  ");

      Brain.Screen.setCursor(1, 20);
      Brain.Screen.print(ball.pressing());
      Brain.Screen.print("  ");

      Brain.Screen.setCursor(2, 10);
      Brain.Screen.print(LeftClaw.position(deg));
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(2, 15);
      Brain.Screen.print(RightClaw.position(deg));
      Brain.Screen.print("  ");

      Brain.Screen.setCursor(3, 10);
      Brain.Screen.print(frontLeftLine.value(percentUnits::pct));
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(3, 15);
      Brain.Screen.print(frontRightLine.value(percentUnits::pct));
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(3, 20);
      Brain.Screen.print(backLeftLine.value(percentUnits::pct));
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(3, 25);
      Brain.Screen.print(backRightLine.value(percentUnits::pct));
      Brain.Screen.print("  ");

      Brain.Screen.setCursor(4, 10);
      Brain.Screen.print(FrontLeft.current());
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(4, 15);
      Brain.Screen.print(FrontRight.current());
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(4, 20);
      Brain.Screen.print(BackLeft.current());
      Brain.Screen.print("  ");
      Brain.Screen.setCursor(4, 25);
      Brain.Screen.print(BackRight.current());
      Brain.Screen.print("  ");

      if (Brain.Screen.pressing()) {

        int xPos = Brain.Screen.xPosition();
        int yPos = Brain.Screen.yPosition();
        while (Brain.Screen.pressing())
          ;

        if (xPos > 5 && xPos < 5 + 60 && yPos > 20 && yPos < 20 + 30) {
          screen = false;
          Brain.Screen.clearScreen();
        }
      }

    } else {
      if (getValues(AUTON_COLOR) == RED) {
        Brain.Screen.setFillColor(red);
      } else if (getValues(AUTON_COLOR) == BLUE) {
        Brain.Screen.setFillColor(blue);
      }

      Brain.Screen.drawRectangle(-10, -10, 500, 500);

      Brain.Screen.setPenColor(color::white);
      Brain.Screen.setFont(prop60);
      Brain.Screen.printAt(150, 120, "2903 S");
    }

    if (tempStatus != currStatus() || autoTempStatus != automatic) {
      statusHUD();
      tempStatus = currStatus();
      autoTempStatus = automatic;
    }
    task::sleep(200);
  }
}
