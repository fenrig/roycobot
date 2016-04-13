#!/usr/bin/env python2

#import rospy
import socket
import struct
#from std_msgs.msg import String
from time import sleep
from msg.msg import position2d

subName = "position"
x = 0
y = 0

def callback(data):
	print ""
	rospy.loginfo(rospy.get_caller_id() + "Rossend: %s", data.data)

def listener():
	UDP_IP = "192.168.1.135"
	UDP_PORT = 52000
	rospy.init_node('udp_sender', anonymous=True);
	rospy.Subscriber(subName, position2d, callback);

	
	print "UDP target IP:", UDP_IP
	print "UDP target port:", UDP_PORT
	
	while True :
		print "resend"
		sock = socket.socket(socket.AF_INET, # Internet
		                     socket.SOCK_DGRAM) # UDP

		# send message to other group
		my_bytes = bytearray(struct.pack('!IIII',x,y,x,y))
		for i in range(8,16) :
			my_bytes[i] ^= 0b11111111
		sock.sendto(my_bytes, (UDP_IP, UDP_PORT))
		sleep(5)
	

if __name__ == '__main__':
	listener()