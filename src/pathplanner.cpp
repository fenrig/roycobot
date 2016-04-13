#include "ros/ros.h"
#include "roycobot/imgPosition.h"
#include "roycobot/imgCanPosition.h"
#include "roycobot/rijsignaal.h"
#include "topics.h"
#include <limits.h>

// --- sleep
#include <unistd.h>
#define SLEEP(seconds) usleep(seconds * 1000000);
#define msleep(mseconds) usleep(mseconds * 1000)

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

void grijpOpen(void){
    DRIVEMACRO(grijpen, 0);
}

void grijpGesloten(void){
    DRIVEMACRO(grijpen, 1);
}

#undef DRIVEMACRO

// --- Beeldverwerking ---
ros::ServiceClient imgPositionClient;

struct position {
	unsigned int x;
	unsigned int y;
};

bool getPosition(struct position *pos){
	roycobot::imgPosition srv;
	srv.request.cmd = "getPos";
	if(imgPositionClient.call(srv)){
		ROS_INFO("Position: ( x = %u , y = %u )", srv.response.x, srv.response.y);
		pos->x = srv.response.x;
		pos->y = srv.response.y;
		return true;
		
	}else{
		ROS_ERROR("Failed to call service position");
		return false;
	}
}

ros::ServiceClient imgCanPositionClient;

int getCanPosition(void){
	roycobot::imgCanPosition srv;
	srv.request.cmd = "findcan";
	if(imgCanPositionClient.call(srv)){
		ROS_INFO("Position: ( rot = %d )", srv.response.rot);
		return srv.response.rot;
		
	}else{
		ROS_ERROR("Failed to call service position");
		return INT_MAX;
	}
}

ros::ServiceClient canDistanceClient;

int getCanDistance(void){
	roycobot::imgCanPosition srv;
	srv.request.cmd = "finddistance";
	if(canDistanceClient.call(srv)){
		ROS_INFO("Distance: ( dist = %d )", srv.response.rot);
		return srv.response.rot;
		
	}else{
		ROS_ERROR("Failed to call service position");
		return INT_MAX;
	}
}
// -----------------
typedef enum {SEARCHINGCAN, APPROACHINGCAN, GOTCAN} state_type;


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
   grijpOpen();
   driveStop();
   
   canDistanceClient = n.serviceClient<roycobot::imgCanPosition>(candistance);
   int distance;
  
   // init ros node "Beeldverwerking"
   imgPositionClient = n.serviceClient<roycobot::imgPosition>(robotposition);
   struct position pos;
   imgCanPositionClient = n.serviceClient<roycobot::imgCanPosition>(canposition);
   int rotation;
   unsigned int frame;
   
   state_type state = SEARCHINGCAN;
   
   // ros loop
   while(ros::ok()){
//	  driveStop();
//	  getPosition(&pos);
          switch(state){
                case SEARCHINGCAN:
                  rotation = getCanPosition();
                  if(rotation == INT_MAX){
                      driveTurnLeft();
                      msleep(1000);
                  }else if(rotation < 2 && rotation > -2){
                        distance = getCanDistance();
                        if(distance < 300){
                                driveForward();
                                msleep(1000);
                        }else{
                                state = APPROACHINGCAN;
                        }
                  }else if(rotation < 0){
	                if(rotation > -8){
		                driveTurnLeft();
		                msleep(600);
	                }else{
		                driveTurnLeft();
		                msleep(800);		
	                }
                  }else if(rotation > 0){
                  	if(rotation < 8){
		                driveTurnRight();
		                msleep(600);		
	                }else{
		                driveTurnRight();
		                msleep(800);		
	                }
                  }
                  driveStop();
                  msleep(100);
                  break;
	   case APPROACHINGCAN:
	        distance = getCanDistance();
                if(distance > 450){
                        driveForward();
                        msleep(800);
                }else if(distance > 500){
                       driveForward();
                        msleep(600); 
                }else if(distance > 600){
                        grijpGesloten();
                        driveTurnRight();
                        SLEEP(40);
                        state = GOTCAN;
                }
                driveStop();
	        msleep(100);
                break;
           case GOTCAN:
                ROS_INFO("GOT CAN, finishing");
           default:
                return 0;
           
        }
   }

  return 0;
}
