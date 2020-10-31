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
// BackLeft             motor         16              
// BackRight            motor         17              
// LeftClaw             motor         1               
// RightClaw            motor         2               
// Indexer              motor         10              
// shitter              motor         9               
// Vision               vision        20              
// ball                 bumper        H               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include <string>

using namespace vex;
competition Competition;

float move_max = 1;
float move_min = .3;

float move = move_max;

int dead_zone = 20;

bool intake = false;

bool automatic = true;
bool ball_found = false;

#define NONE 2

int my_color;
int ball_color = NONE;

double last_pic;
;
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

int Fwd = 1;
int Rev = 2;

int Pct = 1;
int Rpm = 2;
int Dps = 3;

int armlimitup = 900;
int armlimitdown = 130;

int power_usage = 0;

bool screen = false;

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

void delay(int amount_eeeeee) { task::sleep(amount_eeeeee); }

//===============================================MADNESS===============================================

std::string robotStatus[4] = {"Robot Disabled", "Driver Control",
                              "Auton Control ", "MODE ERROR    "};

int maxMenusIndex[maxMenus] = {3, 3, 3};
int configuration[maxMenus] = {0, 0, 0};

std::string menuTypes[maxMenus] = {"Color: ", "Option: ", "Drive: "};

std::string menuOptions[maxMenus][maxOptions] = {{"Red", "Blue", "Hmmm"},
                                                 {"Left", "Right", "Skills"},
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

void autonLeftRow() {
  // win

  Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

  Move(25, 0, 0); // crab
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

  Move(0, -50, -50);
  delay(500);
  Move(0, 0, 0);

  Indexer.spin(directionType::fwd, 100, velocityUnits::pct);

  delay(100);
  Move(-80, 0, 0);
  delay(1350);

  Move(0, 0, 0);
  delay(100);

  Move(0, 50, 0);
  delay(1000);
  Move(0, 0, 0);
  delay(100);

  delay(4000);
  Indexer.stop();

  Move(0, -85, 0);
  delay(500);
  Move(0, 0, 0);
}

void autonRightRow() {
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

  // FrontLeft.startRotateFor(720, deg, 10, velocityUnits::pct);
  // //BackLeft.startRotateFor(720, deg, 10, velocityUnits::pct);
  // //FrontRight.startRotateFor(720, deg, 10, velocityUnits::pct);
  // BackRight.rotateFor(720, deg, 10, velocityUnits::pct);

  // delay(1000);

  // // FrontLeft.startRotateFor(720, deg, 10, velocityUnits::pct);
  // BackLeft.startRotateFor(720, deg, 10, velocityUnits::pct);
  // FrontRight.rotateFor(720, deg, 10, velocityUnits::pct);
  // // BackRight.rotateFor(720, deg, 10, velocityUnits::pct);
}

void auton(void) {
  if (getValues(AUTON_TYPE) == FRONT) {
    autonLeftRow();
  } else if (getValues(AUTON_TYPE) == BACK) {
    autonRightRow();
  } else if (getValues(AUTON_TYPE) == SKILLS) {
    autonSkills();
  }

  notificationHUD("Auton: DONE");
  Controller1.rumble(".");
}

void RCDrive(void) {
  Ch1 = Controller1.Axis1.position(percent) * move;
  Ch3 = Controller1.Axis3.position(percent) * move;
  Ch4 = Controller1.Axis4.position(percent) * move;
  Ch4 = Ch4 * .60;

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

  FrontLeft.setStopping(hold);
  FrontRight.setStopping(hold);
  BackLeft.setStopping(hold);
  BackRight.setStopping(hold);

  LeftClaw.setStopping(hold);
  RightClaw.setStopping(hold);

  Indexer.setStopping(hold);
  shitter.setStopping(hold);

  Vision.setLedMode(vision::ledMode::manual);
  Vision.setBrightness(100);
  Vision.setLedColor(0, 200, 0);

  // Controller1.ButtonL1.pressed(intakeRoutine);

  my_color = getValues(AUTON_COLOR);

  last_pic = Brain.timer(msec);

  while (1) {
    now = Brain.timer(msec);

    if (keyPressed() == btnY) {
    }

    if (keyPressed() == btnY) { // then reevaulate life choices
      automatic = !automatic;
      while (keyPressed() == btnY)
        ;
    }

    if (keyPressed() == btnUP) {
      move = move_max;
    }
    if (keyPressed() == btnDOWN) {
      move = move_min;
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
      LeftClaw.spin(directionType::fwd, 100, velocityUnits::pct);
      RightClaw.spin(directionType::fwd, 100, velocityUnits::pct);
    } else if (Controller1.ButtonL2.pressing()) {
      LeftClaw.spin(directionType::rev, 100 * move, velocityUnits::pct);
      RightClaw.spin(directionType::rev, 100 * move, velocityUnits::pct);
    } else {
      LeftClaw.stop();
      RightClaw.stop();
    }

    if (automatic) { //======================================================

      if (now - last_pic > 500 || ball.pressing()) {
        last_pic = now;

        ball_found = false;
        ball_color = NONE;

        Vision.takeSnapshot(Vision__RED_BALL);

        if (Vision.largestObject.exists && ball_found == false) {
          ball_found = true;

          ball_color = RED;
        }
        Vision.takeSnapshot(Vision__BLUE_BALL);

        if (Vision.largestObject.exists && ball_found == false) {
          ball_found = true;

          ball_color = BLUE;
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

  pre_auton();

  Competition.autonomous(auton);
  Competition.drivercontrol(user);

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

    if (tempStatus != currStatus()) {
      statusHUD();
      tempStatus = currStatus();
    }
    task::sleep(200);
  }
}
