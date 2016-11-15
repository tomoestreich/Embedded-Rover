#!/usr/bin/env python
#=====================================================================
# Filename: 	comm_ctrl.py
# Date: 	09/26/2016
# Created by: 	Ben Johnson
# Modified:	BCJ - 10/l8/2016
#		BCJ - 10/24/2016
# Version:	1.2
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
from array import array

# Add additional directories to the system path
sys.path.append('/apps/pycontrol/game')

# Import Maze game libraries

# IP, Port combos for each WiFly
serv_one = ('192.168.42.2', 2001)
serv_two = ('localhost', 2002)
#serv_two = ('192.168.42.2', 2002)
serv_three = ('192.168.42.2', 2003)
serv_four = ('192.168.42.2', 2004)

# Thread Address Constants
THESEUS_LINE = 1
THESEUS_CONTROL = 2
THESEUS_MOTOR = 3
MINOTAUR_LINE = 4
MINOTAUR_CONTROL = 5
MINOTAUR_MOTOR = 6
RPI = 7
DEBUG = 12
PACKET_ERROR = 13
PING = 14
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
board1 = [THESEUS_CONTROL]
board2 = [THESEUS_MOTOR, THESEUS_LINE]
board3 = [MINOTAUR_CONTROL]
board4 = [MINOTAUR_MOTOR, MINOTAUR_LINE]
pi = [RPI, DEFAULT, DEBUG, PACKET_ERROR, PING]

# Message Queues for RX and TX purposes
txQueues = {"tCont":[], "tMotor":[], "mCont":[], "mMotor":[], "pi":[]}
sendLock = {"2001":0, "2002":0, "2003":0, "2004":0}

# Global Logging Variable
log = None
waitForMtr = 0
pack_err_seq = 0 

#---------------------------------------------------------------------
# Function for creating a message given the desired src, dst and data
#---------------------------------------------------------------------
def buildMsg(src, dst, seq, data):
	# Get integer value of message to send
	msg = (src << 4) | dst
	msg = (msg << 8) | seq
	msg = (msg << 8) | data
	msg = (msg << 8) | calcChecksum(msg)

	# Parse the int value to a string of ascii characters
	retStr = []
	for i in range(0, 4):
		temp = (msg >> (i*8)) & 0xff
		retStr.append(chr(temp))
	retStr.reverse()
	retStr = array('B', map(ord, retStr)).tostring()
	return retStr

#---------------------------------------------------------------------
# Function for calculating the checksum given the 1st 3 msg bytes
#---------------------------------------------------------------------
def resendMessage():
	global waitForMtr
	global pack_err_seq
	if waitForMtr:
		msg = msgBuild(PACKET_ERROR, THESEUS_CONTROL,\
			pack_err_seq, 0x33)
		pack_err_seq += 1
		txQueues["2001"].append	
		
#---------------------------------------------------------------------
# Function for calculating the checksum given the 1st 3 msg bytes
#---------------------------------------------------------------------
def calcChecksum(message):
	chksm = 0
	for i in range(0, 3):
		for j in range(0, 8):
			if (message >> ((i*8) + j)) & 0x1:
				chksm += 1	
	return chksm | 0x00

#---------------------------------------------------------------------
# Function for getting a list of message components
#---------------------------------------------------------------------
def unpackMessage(msg):
	# Get the four message fields
	frmt = "B B B B"
	unpacker = struct.Struct(frmt)
	mFields = unpacker.unpack(msg)
	
	# Parse the fields
	src = (mFields[0] >> 4) & 0xf
	dst = mFields[0] & 0xf
	seq = mFields[1]
	data = mFields[2]
	chksm = mFields[3]
	return [src, dst, seq, data, chksm]

#---------------------------------------------------------------------
# Function for formatting a message into a readable string
#---------------------------------------------------------------------
def messageString(msg):
	# Get the four message fields
	frmt = "B B B B"
	unpacker = struct.Struct(frmt)
	mFields = unpacker.unpack(msg)

	# Parse the fields
	src = (mFields[0] >> 4) & 0xf
	dst = mFields[0] & 0xf
	seq = mFields[1]
	data = mFields[2]
	chksm = mFields[3]
	
	# Return a formatted string
	return "[%d][%d] Seq:%d Data:%d Chk:%d" %(src,dst,seq,data,chksm)

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
		global sendLock
		while(sendLock[str(port)]):
			continue
		sendLock[str(port)] = 1
		conn.sendall('ee'.decode('hex'))
		sendLock[str(port)] = 0
		connections[str(port)] = True
		pings[str(port)] = False
		time.sleep(5)

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
def processMessages(conn, port):
	# loop continuously processing messages
	while True:
		# exit if disconn. -- else try to receive messages
		if not connections[str(port)]:
			return
		
		# If data ready to transmit send a message
		sendMessage(conn, port)

		# Attempt to receive a message
		try:
			msg = conn.recv(4)
		except socket.timeout:
			continue	
		# Process setup/conn messages
		print "Message received: ", msg
		if msg == "*HEL" or msg == "LO*":
			continue
		elif msg == 'ee'.decode('hex'):
			pings[str(port)] = True
		elif msg.startswith('ee'.decode('hex')):
			pings[str(port)] = True

		# Device has just begun running
		elif msg == "STRT":
			start(conn, port)

		# Actual message handling
		elif msg and msg != "*CLOS*" and len(msg) == 4:
			print messageString(msg)
			routeMessage(msg)	

#---------------------------------------------------------------------
# Function for sending a message from the desired queue to des. PIC
#---------------------------------------------------------------------
def sendMessage(conn, port):
	# Based on port and contents of queue send the next message
	global sendLock
	while(sendLock[str(port)]):
		continue
	sendLock[str(port)] = 1
	if port == 2001 and len(txQueues["tCont"]):
		conn.sendall(txQueues["tCont"].pop(0))
	elif port == 2002 and len(txQueues["tMotor"]):
		conn.sendall(txQueues["tMotor"].pop(0))
	elif port == 2003 and len(txQueues["mCont"]):
		conn.sendall(txQueues["mCont"].pop(0))
	elif port == 2004 and len(txQueues["mMotor"]):
		conn.sendall(txQueues["mMotor"].pop(0))
	sendLock[str(port)] = 0
		
#---------------------------------------------------------------------
# Function for reading messages and routing accordingly
#---------------------------------------------------------------------
def routeMessage(msg):
	# Unpack the two address fields
	if len(msg) != 4:
		invalidMsg(msg)	
	frmt = "B B B B"
	unpacker = struct.Struct(frmt)
	mFields = unpacker.unpack(msg)
	
	# Get the destination and route to the desired queue 
	dst = (mFields[0] & 0xf)
	if dst in board1:
		txQueues["tCont"].append(msg)
	elif dst in board2:
		txQueues["tMotor"].append(msg)
	elif dst in board3:
		txQueues["mCont"].append(msg)
	elif dst in board4:
		txQueues["mMotor"].append(msg)
	elif dst in pi:
		txQueues["pi"].append(msg)

#---------------------------------------------------------------------
# Function for reading messages and routing accordingly
#---------------------------------------------------------------------
def invalidMsg(msg):
	timeStr = datetime.datetime.now().strftime("%H%M%S")
	log.write("[%s] ERROR: Invalid Message: %s" %(timeStr, msg))

#---------------------------------------------------------------------
# Function for connecting to a WiFly and handling its interactions
#---------------------------------------------------------------------
def controlComm(ip, port):
	# Open the log file
	dstr = datetime.datetime.now().strftime("_%Y%m%d_%H%M%S")	
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
		conn.settimeout(0.01)
		processMessages(conn, port)
	
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


