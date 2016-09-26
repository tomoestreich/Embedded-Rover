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
	sock_two = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serv_two = ('192.168.42.2', 2000)
	
	# Bind the socket and print status -- listen until a connection is made
	print "Starting socket %s:%s" % serv_two
	sock_two.bind(serv_two)
	sock_two.listen(1)

	# loop continuously waiting for information
	while(1):
		print "Waiting for a connection..."
		conn, addr_two = sock_two.accept()
		
		# Receive data from client
		try:
			print "Connection found: ", addr_two
			while True:
				msg_two = conn.recv(16)
				if msg_two and msg_two != "^":
					print "Message Received: %s" % msg_two
				elif msg_two == "^":
					print "Connection closed by WiFly two"
					break
				else:
					print "Data transfer complete. Exiting..."
					break	
		finally:
			conn.close()
			break

if __name__ == "__main__":
	main()


