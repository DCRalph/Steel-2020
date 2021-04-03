#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor FrontLeft = motor(PORT11, ratio18_1, false);
motor FrontRight = motor(PORT12, ratio18_1, true);
motor BackLeft = motor(PORT16, ratio18_1, false);
motor BackRight = motor(PORT17, ratio18_1, true);
motor LeftClaw = motor(PORT1, ratio18_1, false);
motor RightClaw = motor(PORT2, ratio18_1, true);
motor Indexer = motor(PORT10, ratio18_1, true);
motor shitter = motor(PORT9, ratio18_1, false);
/*vex-vision-config:begin*/
signature Vision__RED_BALL = signature (1, 5791, 10559, 8174, -1277, 445, -416, 0.9, 0);
signature Vision__BLUE_BALL = signature (2, -3389, -1093, -2241, 1489, 15421, 8455, 0.8, 0);
vision Vision = vision (PORT20, 70, Vision__RED_BALL, Vision__BLUE_BALL);
/*vex-vision-config:end*/
bumper ball = bumper(Brain.ThreeWirePort.H);
line frontLeftLine = line(Brain.ThreeWirePort.F);
line frontRightLine = line(Brain.ThreeWirePort.G);
line backLeftLine = line(Brain.ThreeWirePort.E);
line backRightLine = line(Brain.ThreeWirePort.D);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}