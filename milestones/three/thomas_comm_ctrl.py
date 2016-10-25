#!/usr/bin/env python
#=====================================================================
# Filename: 	comm_ctrl.py
# Date: 	09/26/2016
# Created by: 	Ben Johnson
# Modified:	BCJ - 10/l8/2016
# Version:	1.1
# Info:		Python script for connecting to the four PIC32s and 
#		facillitating the comm. between them. A GUI will 
#		eventually be added for debugging purposes.
#
# NOTE:		ONLY MODIFY THE "processMessages" FUNCTION WHEN 
#		ADDING SIMULATIONS OR PROCESSING. 
#		**DO NOT MODIFY ANY OTHER FUNCTION OR CODE OUTSIDE OF
#		**THE INDICATED SECTION IN THE FUNCTION UNLESS YOU 
#		**KNOW EXACTLY WHAT YOU ARE DOING!
#		**ANY ADDITIONAL FUNCTIONS OR PROCESSES SHOULD BE 
#		**MADE IN A SEPARATE FILE AND IMPORTED HERE
#		**THIS FILE IS ONLY FOR CONNECTION MGMT FUNCTIONALITY
#
#=====================================================================		

# Libraries
import os
import sys
import time
import argparse
import socket
import binascii
import struct
import datetime
import threading

import thomas_M3_test

# IP, Port combos for each WiFly
serv_one = ('192.168.42.2', 2001)
serv_two = ('192.168.42.2', 2002)
serv_three = ('192.168.42.2', 2003)
serv_four = ('192.168.42.2', 2004)

# Thread Address Constants
THESEUS_EYES = 1
THESEUS_CONTROL = 2
THESEUS_MOTOR = 3
MINOTAUR_EYES = 4
MINOTAUR_CONTROL = 5
MINOTAUR_MOTOR = 6
RPI = 6
DEFAULT = 15

# Dict of port number to PIC relationship
boards = {"2001": "Theseus Control", "2002": "Theseus Motor", \
	"2003": "Minotaur Control", "2004": "Minotaur Motor"}

# Connectivity status dicts
connections = {"2001": True, "2002": True, 
	"2003": True, "2004": True}
pings = {"2001": False, "2002": False, 
	"2003": False, "2004": False}
started = {"2001": False, "2002": False, 
	"2003": False, "2004": False}

# Lists of acceptable 
board1 = [THESEUS_EYES, THESEUS_CONTROL]
board2 = [THESEUS_MOTOR]
board3 = [MINOTAUR_EYES, MINOTAUR_CONTROL]
board4 = [MINOTAUR_MOTOR]
pi = [RPI, DEFAULT]

#---------------------------------------------------------------------
# Function for creating a socket on a specified port 
#---------------------------------------------------------------------
def connect(ip, port):
	# Create the socket and attempt to connect
	server = (ip, port)
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	print "Creating socket on port %s" % port
	sock.bind(server)
	sock.listen(1)
	return sock

#---------------------------------------------------------------------
# Function for closing a connection when desired
#---------------------------------------------------------------------
def closeConnection(port, sock, conn):
	print "Closing socket on port %s" % port
	conn.close()
	sock.close()
	#sock.shutdown(socket.SHUT_RDWR)

#---------------------------------------------------------------------
# Function for pinging the PIC in order to maintain conn. status
#---------------------------------------------------------------------
def ping(conn, port):
	while(1):
		# If ping has not been answered mark the board as disconn.
		if not pings[str(port)]:
			connections[str(port)] = False
			print "Disconnected from", boards[str(port)]
			return
	
		# Otherwise perform the ping
		connections[str(port)] = True
		conn.sendall('00'.decode('hex'))
		pings[str(port)] = False
		time.sleep(1)
		

#---------------------------------------------------------------------
# Function for closing a connection when desired
#---------------------------------------------------------------------
def start(conn, port):
	# Initialize connection status values and start ping 
	pings[str(port)] = True
	connections[str(port)] = True	
	started[str(port)] = True
	w_ping = threading.Thread(target=ping, args=(conn, port))
	w_ping.daemon = True
	w_ping.start()
	print "Beginning data processing for", boards[str(port)], \
		"device"
	
#---------------------------------------------------------------------
# Function for rx/tx of messages from a socket and processing them 
# AUGMENT THIS FUNCTION TO IMPLEMENT ADDITONAL PROCESSING OR 
# SIMULATION AS INDICATED IN CAPS BELOW
#---------------------------------------------------------------------
def processMessages(log, conn, port):
	# loop continuously processing messages
	while True:
		# INSERT SIMULATED DATA GENERATOR FUNCTION HERE
		# data = simulate()
		# conn.sendall(data)

		# exit if disconn. -- else try to receive messages
		if not connections[str(port)]:
			return
		try:
			msg = conn.recv(4)
		except socket.timeout:
			continue
		
		# Don't worry about hello message
		if msg == "*HEL" or msg == "LO*":
			continue

		# Ping received
		elif msg == '00'.decode('hex'):
			print "Ping received from:", boards[str(port)]
			pings[str(port)] = True
			continue

		# Device has just begun running
		elif msg == "STRT":
			start(conn, port)
			continue

		# Actual message handling
		elif msg and msg != "*CLOS*":
			hex_one = binascii.hexlify(msg)
			print "%s: Message Received: %s: %s" % \
				(boards[str(port)], msg, hex_one)
			
#---------------------------------------------------------------------
# Function for reading src and destination to route messages
#---------------------------------------------------------------------
def getData(message):
	# Unpack the two address fields
	if len(message) == 4:
		format = "B B B B"
	elif len(message) == 13:
		format = "B B B B B B B B B B B B B"
	else:
		return ""
	unpacker = struct.Struct(format)
	msg = unpacker.unpack(message)
	
	# Return a list of the source, destination and data
	src = (msg[0] >> 4)
	dst = (msg[0] & 0xf)
	print src, dst, msg[2] 
	return (src, dst, msg[2])

#---------------------------------------------------------------------
# Function for connecting to a WiFly and handling its interactions
#---------------------------------------------------------------------
def controlComm(ip, port):
	# Open the log file
	dstr = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")	
	log = open("/apps/logs/CommCtrl_"+str(port)+dstr+".log", "w")

	# Create the socket
	sock = connect(ip, port)

	# try to perform the comm
	try:
		# make the connection on the socket
		conn, addr = sock.accept()
		print "Connection found:", addr
		log.write("Connection found: %s:%s" %addr)

		# begin processing messages
		conn.settimeout(0.5)
		parseCommand()
		#processMessages(log, conn, port)
	
	# close the log and socket then exit and restart 
	except Exception as err:
		print str(err)
		#log.write("Connection on port %s closed." % port)
		#print "Connection closed on port %s" % port 
	closeConnection(port, sock, conn)
	log.close()

#---------------------------------------------------------------------
# Function for starting thread
#---------------------------------------------------------------------
def startThread(thread):
	connections[str(2000 + thread)] = True
	if thread == 1:
		wi_one = threading.Thread(target=controlComm, args=serv_one)
		wi_one.daemon = True
		wi_one.start()
	elif thread == 2:
		wi_two = threading.Thread(target=controlComm, args=serv_two)
		wi_two.daemon = True
		wi_two.start()
	elif thread == 3:
		wi_three = threading.Thread(target=controlComm, args=serv_three)
		wi_three.daemon = True
		wi_three.start()
	elif thread == 4:
		wi_four = threading.Thread(target=controlComm, args=serv_four)
		wi_four.daemon = True
		wi_four.start()
		
#---------------------------------------------------------------------
# MAIN FUNCTION
#---------------------------------------------------------------------
def main():
	# Fork seperate processes for each WiFly
	for i in range(1, 5):
		startThread(i)
	
	# run infinitely -- restart threads if connections are dropped 
	while True:
		if not connections["2001"]:
			time.sleep(2)
			startThread(1)
		if not connections["2002"]:
			time.sleep(2)
			startThread(2)
		if not connections["2003"]:
			time.sleep(2)
			startThread(3)
		if not connections["2004"]:
			time.sleep(2)
			startThread(4)
		time.sleep(1)
	
if __name__ == "__main__":
	main()


