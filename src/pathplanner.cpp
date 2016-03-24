#include "ros/ros.h"
#include "roycobot/position2d.h"
#include "roycobot/rijsignaal.h"
#include "topics.h"

// --- sleep
#include <unistd.h>
#define SLEEP(seconds) usleep(seconds * 1000000);

// --- Rijd ROS node ---
#define DRIVEMACRO(name, value) \
    roycobot::rijsignaal msg; \
    msg.naam = #name ; \
    msg.waarde = value ; \
    chatter_sub_drive.publish(msg);

ros::Publisher chatter_sub_drive;
void driveForward(void){
    DRIVEMACRO(rijden, 1);
}

void driveBackward(void){
    DRIVEMACRO(rijden, -1);
}

void driveTurnLeft(void){
    DRIVEMACRO(draaien, -1);
}

void driveTurnRight(void){
    DRIVEMACRO(draaien, 1);
}

void driveStop(void){
    DRIVEMACRO(stop, 0);
}

void driveInit(void){
    DRIVEMACRO(useless, 0);
}

#undef DRIVEMACRO
// -----------------


int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "pathplanner");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

   // init ros node "Rijden"
   chatter_sub_drive = n.advertise<roycobot::rijsignaal>(robotdrive, 10);
   sleep(2);
   

   // ros loop
   while(ros::ok()){
	  driveTurnLeft();
	  sleep(10);
	  driveStop();
	  return 0;
   }

  return 0;
}
