///http://sigrok.org/wiki/Libserialport

#include "roycobot/rijsignaal.h"
#include "topics.h"
#include <sstream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <libserialport.h>
#include <stddef.h>
#include <stdio.h>

using namespace std_msgs;
using namespace ros;
using namespace std;

String msg;
stringstream ss;
char buf[15];

/*void Verzender(void){
    ss << "hello world " << count;
    msg.data = ss.str();
    ROS_INFO("%s", msg.data.c_str());
    chatter_pub.publish(msg);

    loop_rate.sleep();
    ++count;
}*/
/*

void stop(void){
    strcpy(buf, "q 0 0");
    sp_nonblocking_write(port, buf, sizeof(buf));

}

void rijden(int waarde){
///http://www.sigrok.org/api/libserialport/unstable/a00008.html
///'g [0 open/1close]' servo open close
///'q' [speed left -100 –- 100]
///[speed right -100 –- 100] speed left motor right motor
/// --> serieel sturen
	if(waarde > 0){
		strcpy(buf, "q 100 100");
	}
	else if(waarde < 0){
    		strcpy(buf, "q -100 -100");
	}
    sp_nonblocking_write(port, buf, sizeof(buf));
}

void draaien(int waarde){
    if(waarde < 0){
        strcpy(buf, "q -10 10");
    }
    else if(waarde > 0){
        strcpy(buf, "q 10 -10");
    }
    sp_nonblocking_write(port, buf, sizeof(buf));
}

void Ontvanger(const roycobot::rijsignaal::ConstPtr& signaal){//const std_msgs::String::ConstPtr& msg){
///Binnengekomen bericht: msg->data.c_str()
    if(signaal->naam == "stop"){
        ROS_INFO("STOP Signaal ontvangen");
        stop();
    }
    else if(signaal->naam == "rijden"){
        ROS_INFO("RIJDEN Signaal ontvangen");
        rijden(signaal->waarde);
    }
    else if(signaal->naam == "draaien"){
        ROS_INFO("DRAAI Signaal ontvangen");
        draaien(signaal->waarde);
    }
}
*/
int main(int argc, char **argv){

	printf("start\n");    
//ros::init(argc, argv, "rijden");
    //ros::NodeHandle n;
    ///ros::Publisher chatter_pub = n.advertise<std_msgs::String>("pathplanning", 1000);
    //ros::Subscriber sub = n.subscribe<roycobot::rijsignaal>(pathplanning, 1000, Ontvanger);
    //ros::Rate loop_rate(10);
    struct sp_port **ports;

    sp_list_ports(&ports);
    for(int i=0; ports[i]; i++){
	printf("found port: %s\n", sp_get_port_name(ports[i]));
    }
printf("the end\n");    
    int count = 0;
    while (ros::ok()){
        //Verzender();
        ros::spinOnce();
    }
    return 0;
}