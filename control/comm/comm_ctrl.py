#!/usr/bin/env python
#=====================================================================
# Filename: 	comm_ctrl.py
# Date: 	09/26/2016
# Created by: 	Ben Johnson
# Modified:
# Version:	1.0
# Info:		Python script for connecting to the four PIC32s and 
#		facillitating the comm. between them. A GUI will 
#		eventually be added for debugging purposes.
#
#=====================================================================		

# Libraries
import os
import sys
import time
import argparse
import socket

#---------------------------------------------------------------------
# MAIN FUNCTION
#---------------------------------------------------------------------
def main():
	# create socket and address objects
	sock_one = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	#sock_two = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serv_one = ('192.168.42.2', 2001)
	serv_two = ('192.168.42.2', 2002)
	serv_three = ('192.168.42.2', 2003)
	serv_four = ('192.168.42.2', 2004)
	
	# Bind the socket and print status -- listen until a connection is made
	print "Starting socket %s:%s" % serv_one
	sock_one.bind(serv_one)
	sock_one.listen(1)
	#print "Starting socket %s:%s" % serv_two
	#sock_two.bind(serv_two)
	#sock_two.listen(1)

	# loop continuously waiting for information
	while(1):
		print "Waiting for a connection(s)..."
		conn_one, addr_one = sock_one.accept()
		#conn_two, addr_two = sock_two.accept()

		# Receive data from client
		try:
			print "Connection(s) found: ", addr_one
			while True:
				msg_one = conn_one.recv(16)
				if msg_one == "*HELLO*":
					continue
				elif msg_one and msg_one != "*CLOS*":
					print "WiFly1 Message Received: %s" % msg_one
					print "Sending Message to WiFly2..."
					conn_one.sendall(msg_one[0])
					print "Message sent."
				elif msg_one == "*CLOS*":
					print "Connection closed by WiFly one"
					break
				else:
					print "Data transfer complete. WiFlys Closing..."
					break	
		finally:
			conn_one.close()
			#conn_two.close()
			break

if __name__ == "__main__":
	main()


