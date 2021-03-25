using namespace vex;

extern brain Brain;

using signature = vision::signature;

// VEXcode devices
extern controller Controller1;
extern motor FrontLeft;
extern motor FrontRight;
extern motor BackLeft;
extern motor BackRight;
extern motor LeftClaw;
extern motor RightClaw;
extern motor Indexer;
extern motor shitter;
extern signature Vision__RED_BALL;
extern signature Vision__BLUE_BALL;
extern signature Vision__SIG_3;
extern signature Vision__SIG_4;
extern signature Vision__SIG_5;
extern signature Vision__SIG_6;
extern signature Vision__SIG_7;
extern vision Vision;
extern bumper ball;
extern line frontLeftLine;
extern line frontRightLine;
extern line backLeftLine;
extern line backRightLine;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );