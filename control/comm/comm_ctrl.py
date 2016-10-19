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
def closeConnection(sock, conn):
	conn.close()
	sock.shutdown(socket.SHUT_RDWR)
	sock.close()

#---------------------------------------------------------------------
# Function for rx/tx of messages from a socket and processing them 
# AUGMENT THIS FUNCTION TO IMPLEMENT ADDITONAL PROCESSING OR 
# SIMULATION AS INDICATED IN CAPS BELOW
#---------------------------------------------------------------------
def processMessages(log, conn, port):
	# loop continuously processing messages
	while True:
		msg = conn.recv(4)
		if msg == "*HEL" or "LO*":
			continue
		elif msg and msg != "*CLOS*":
			hex_one = binascii.hexlify(msg)
			log.write(msg)
			print "%s: Message Received: %s: %s" % \
				(port, msg, hex_one)
			
			# MESSAGE RECVD IS IN "msg" - PROCESS IT BELOW
			# ADD ADDITIONAL FUNCTIONS OR SCRIPTS HERE
			# ADD SIMPLE FUNCTION CALLS OF EXTERNAL METHODS
			# DO NOT SIMPLY ADD CODE HERE

			# Function for sending whatever is in "msg"
			conn.sendall(msg)
			# END MSG PROC./SIM. MODIFICATIONS
		else:
			print "Connection on port %s closed by \
				client" % port
			break

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
def c