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
    msg.waarde = value ;

ros::Publisher chatter_sub_drive;
void driveForward(void){
    DRIVEMACRO(rijden, 1);
    chatter_sub_drive.publish(msg);
}

void driveBackward(void){
    DRIVEMACRO(rijden, -1);
    chatter_sub_drive.publish(msg);
}

void driveTurnLeft(void){
    DRIVEMACRO(draaien, 1);
    chatter_sub_drive.publish(msg);
}

void driveTurnRight(void){
    DRIVEMACRO(draaien, -1);
    chatter_sub_drive.publish(msg);
}

void driveStop(void){
    DRIVEMACRO(stop, 0);
    chatter_sub_drive.publish(msg);
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

  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to the subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */
   chatter_sub_drive = n.advertise<roycobot::rijsignaal>(robotdrive, 10);
   
   while(true){
          driveForward();
          SLEEP(3);
          driveStop();
          SLEEP(1);
          driveBackward();
          SLEEP(3);
          driveStop();
          SLEEP(1);
          driveTurnLeft();
          SLEEP(3);
          driveStop();
          SLEEP(1);
          driveTurnRight();
          SLEEP(3);
          driveStop();
          SLEEP(1);
   }
  

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */
  //ros::spin();

  return 0;
}
