#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor FrontLeft = motor(PORT19, ratio18_1, false);
motor FrontRight = motor(PORT9, ratio18_1, true);
motor BackLeft = motor(PORT20, ratio18_1, false);
motor BackRight = motor(PORT10, ratio18_1, true);
motor LeftClaw = motor(PORT18, ratio18_1, true);
motor RightClaw = motor(PORT8, ratio18_1, false);
motor Indexer = motor(PORT15, ratio18_1, true);

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