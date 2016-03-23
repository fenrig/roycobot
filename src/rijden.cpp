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
struct sp_port *port;

/*void Verzender(void){
    ss << "hello world " << count;
    msg.data = ss.str();
    ROS_INFO("%s", msg.data.c_str());
    chatter_pub.publish(msg);

    loop_rate.sleep();
    ++count;
}*/


void stop(void){
    strcpy(buf, "q 0 0\r");
    sp_nonblocking_write(port, buf, 6);

}

void rijden(int waarde){
///http://www.sigrok.org/api/libserialport/unstable/a00008.html
///'g [0 open/1close]' servo open close
///'q' [speed left -100 –- 100]
///[speed right -100 –- 100] speed left motor right motor
/// --> serieel sturen
	if(waarde > 0){
		strcpy(buf, "q 100 100\r\0\0");
	}
	else if(waarde < 0){
    		strcpy(buf, "q -100 -100\r");
	}
    sp_nonblocking_write(port, buf, 12);
}

void draaien(int waarde){
    if(waarde < 0){
        strcpy(buf, "q -100 100\r");
    }
    else if(waarde > 0){
        strcpy(buf, "q 100 -100\r");
    }
    sp_nonblocking_write(port, buf, 11);
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

int main(int argc, char **argv){

	printf("start\n");    
    ros::init(argc, argv, "rijden");
    ros::NodeHandle n;
    //ros::Publisher chatter_pub = n.advertise<std_msgs::String>(pathplanning, 10);
    
    struct sp_port **ports;

    sp_list_ports(&ports);
    for(int i=0; ports[i]; i++){
	printf("Possible port: %s\n", sp_get_port_name(ports[i]));
	if(strcmp(sp_get_port_name(ports[i]), "/dev/ttyUSB0") == 0){
		port = ports[i];
		if(sp_open(port, SP_MODE_READ_WRITE) == SP_OK){
			sp_set_baudrate(port, 38400);
			printf("Port Found: %s\n", sp_get_port_name(port));
			draaien(1);
			sleep(3);
			stop();
			sleep(3);
			rijden(20);
			sleep(6);
			stop();
			sleep(10);
		}else{
			printf("Kon poort niet openen\n");		
		}
		printf("the end\n");
		return 2;
	}
    }
	printf("the end\n");    
    int count = 0;
    ros::Subscriber sub = n.subscribe<roycobot::rijsignaal>(robotdrive, 10, Ontvanger);
    ros::Rate loop_rate(10);
    while (ros::ok()){
        //Verzender();
        ros::spinOnce();
    }
    return 0;
}
