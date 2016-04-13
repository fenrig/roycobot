#!/usr/bin/env python2

#import rospy
import socket
import struct
from msg.msg import position2d
from time import sleep

def talker(pub,x,y):
    rate = rospy.Rate(10) # 10hz
    #rospy.loginfo(hello_str)
    #pub.publish(hello_str)
    #rate.sleep()


def listener():
	pub = rospy.Publisher('otherposition', position2d, queue_size=10)
	rospy.init_node('receive', anonymous=True)
	
	UDP_PORT = 52000

	sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
	sock.bind(('', UDP_PORT))

	#while not rospy.is_shutdown() :
	while True :
		# Receive the data from the other group
		data, addr = sock.recvfrom(1024) 	# buffer size is 1024 bytes
		unpackedData = struct.unpack("!IIII", data)
		x = unpackedData[0]
		y = unpackedData[1]
		xInv = unpackedData[2] ^ 0xFFFFFFFF
		yInv = unpackedData[3] ^ 0xFFFFFFFF
		print "received message:", data
		print "unpackedData:", unpackedData
		print "Data:", x, y, xInv, yInv
		if(x == xInv and y == yInv)
			try:
				talker(pub,x,y)
			except rospy.ROSInterruptException:
				pass
		# Publish the received data, so that other nodes can use it		

if __name__ == '__main__':
	listener()
