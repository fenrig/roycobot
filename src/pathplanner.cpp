#include "ros/ros.h"
#include "roycobot/imgPosition.h"
#include "roycobot/imgCanPosition.h"
#include "roycobot/rijsignaal.h"
#include "roycobot/position2d.h"
#include "topics.h"
#include <limits.h>

// --- sleep
#include <unistd.h>
#define SLEEP(seconds) usleep(seconds * 1000000);
#define msleep(mseconds) usleep(mseconds * 1000)

// ---- Share pos ---
struct position {
	unsigned int x;
	unsigned int y;
};

ros::Publisher chatter_sharePos;

void sharePosition(struct position *pos){
        roycobot::position2d msg;

        msg.x = pos->x;
        msg.y = pos->y;

        ROS_INFO("Sharing position: ( x = %d , y = %d )", msg.x, msg.y);

        chatter_sharePos.publish(msg);
}

void otherrobotposFunc(const roycobot::position2d::ConstPtr& signaal){
        ROS_INFO("Sharing position: ( x = %d , y = %d )", signaal->x, signaal->y);
}
// ------------------

// ---- Sleep timings ---
// SEARCHINGCAN
#define LONGFORWARD 775

#define LONGTURN 900
#define MIDDLETURN 825
#define SHORTTURN 650

#define SMALLANGLE 2
#define MIDDLEANGLE 11

// APPROACHINGCAN
#define MIDDLEFORWARD 600
#define SHORTFORWARD 250

// ALL
#define SHORTSLEEP 25

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

   // init ros node "shareLoc"
   chatter_sharePos = n.advertise<roycobot::position2d>(robotsharepos, 10);
   ros::Subscriber sub = n.subscribe<roycobot::position2d>(otherrobotposition, 10, otherrobotposFunc);

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

   int zijnerbijna = 0;
   int prevdist = INT_MAX;

   state_type state = SEARCHINGCAN;

   // ros loop
   while(ros::ok()){
	  getPosition(&pos);
	  sharePosition(&pos);

          switch(state){
                case SEARCHINGCAN:
                  rotation = getCanPosition();
                  if(rotation == INT_MAX){
                      driveTurnLeft();
                      msleep(LONGTURN);
                  }else if(rotation < MIDDLEANGLE && rotation > -MIDDLEANGLE){
                        distance = getCanDistance();
                        if(distance < 125){
                                driveForward();
                                msleep(LONGFORWARD);
                                zijnerbijna++;
                        }else{
                                state = APPROACHINGCAN;
                                zijnerbijna = 0;
                        }
                  }else if(zijnerbijna > 2){
                        if(rotation < 0){
		                 driveTurnLeft();
		                 msleep(SHORTTURN);
                          }else if(rotation > 0){
		                driveTurnRight();
		                msleep(SHORTTURN);
                          }
                  }else if(rotation < 0){
                        zijnerbijna = 0;
	                if(rotation > -MIDDLEANGLE){
		                driveTurnLeft();
		                msleep(SHORTTURN);
	                }else{
		                driveTurnLeft();
		                msleep(MIDDLETURN);
	                }
                  }else if(rotation > 0){
                        zijnerbijna = 0;
                  	if(rotation < MIDDLEANGLE){
		                driveTurnRight();
		                msleep(SHORTTURN);
	                }else{
		                driveTurnRight();
		                msleep(MIDDLETURN);
	                }
                  }
                  driveStop();
                  msleep(SHORTSLEEP);
                  break;
	   case APPROACHINGCAN:
	        prevdist = distance;
	        distance = getCanDistance();

	        if(distance < 75){
	                state = SEARCHINGCAN;
	                zijnerbijna = 0;
	                break;
	        }else if(distance > 497 || (distance > 300 && prevdist > distance)){
                        state = GOTCAN;
	        }else if(distance > 190){
                        driveForward();
                        msleep(SHORTFORWARD);
                }else{
                       driveForward();
                        msleep(MIDDLEFORWARD);
                }
                driveStop();
	        msleep(SHORTSLEEP);
                break;
           case GOTCAN:
		 grijpGesloten();
		 SLEEP(1);
		 driveTurnRight();
		 SLEEP(10);
		 driveTurnLeft();
		 SLEEP(2);
		 driveTurnLeft();
		 SLEEP(10);
		 driveStop();
		 grijpOpen();
                ROS_INFO("GOT CAN, finishing");
           default:
                return 0;

        }
   }

  return 0;
}
