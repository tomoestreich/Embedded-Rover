#!/usr/bin/env python
#=====================================================================
# Filename: 	comm_ctrl.py
# Date: 	09/26/2016
# Created by: 	Ben Johnson
# Modified:	BCJ - 10/l8/2016
#			BCJ - 10/24/2016
#			BCJ - 11/8/2016
# Version:	2.0
# Info:		Class implementations for the overall Comm Controller
#		and the individual connection handlers for each board
# 		classes will be explained in further detail in their 
#		individual areas
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

# IP, Port combos for each WiFly
theseusControlSim = ('localhost', 2001)
theseusControl = ('192.168.42.2', 2001)
theseusMotorSim = ('localhost', 2002)
theseusMotor = ('192.168.42.2', 2002)
minControlSim = ('localhost', 2003)
minControl = ('192.168.42.2', 2003)
minMotorSim = ('localhost', 2004)
minMotor = ('192.168.42.2', 2004)
piControl = ('localhost', 2005)
defaultHosts = [theseusControl, theseusMotor, minControl, minMotor,\
		piControl]

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
NAMES = ['', 'Th. Line', 'Th. Cont.', 'Th. Mtr.', 'M. Line', 'M. Cont.',\
		'M. Mtr.', 'R. Pi', '', '', '', '', 'DBG', 'Pack Err', 'Ping',\
		'N/A']

# Lists of acceptable 
boards = {"2001": "Theseus Control", "2002": "Theseus Motor", \
	"2003": "Minotaur Control", "2004": "Minotaur Motor", \
	"2005": "Pi Control"}
board1 = [THESEUS_CONTROL]
board2 = [THESEUS_MOTOR, THESEUS_LINE]
board3 = [MINOTAUR_CONTROL]
board4 = [MINOTAUR_MOTOR, MINOTAUR_LINE]
pi = [RPI, DEFAULT, DEBUG, PACKET_ERROR, PING]

# Message Queues for RX and TX purposes
txQueues = {"2001":[], "2002":[], "2003":[], "2004":[], "2005":[]}

# Global Logging Variable
seq_num = {"2001":0, "2002":0, "2003":0, "2004":0, "2005":0}
seq_errors = {"2001":0, "2002":0, "2003":0, "2004":0, "2005":0}
pack_errors = {"2001":0, "2002":0, "2003":0, "2004":0, "2005":0}

#---------------------------------------------------------------------
# Function for creating a message given the desired src, dst and data
#---------------------------------------------------------------------
def buildMsg(src, dst, seq, data):
	# Get integer value of message to send
	msg = (src << 4) | dst
	msg = (msg << 8) | seq
	msg = (msg << 8) | data
	msg = (msg << 8) | calcChecksum(msg << 8)
	
	# Parse the int value to a string of ascii characters
	retStr = []
	for i in range(0, 4):
		temp = (msg >> (i*8)) & 255
		retStr.append(chr(temp))
	retStr.reverse()
	retStr = array('B', map(ord, retStr)).tostring()
	return retStr

#---------------------------------------------------------------------
# Function for calculating the checksum given the 1st 3 msg bytes
#---------------------------------------------------------------------
def calcChecksum(message):
	if(type(message) == int):
		msg = message
	else:
		msg = int(message.encode('hex'), 16)
	chksm = 0
	msg = msg >> 8
	binary = bin(msg)
	return binary.count('1')

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
	src = NAMES[src]
	dst = NAMES[dst]
	
	# Return a formatted string
	return "[%s][%s] Seq:%d Data:%d Chk:%d" %(src,dst,seq,data,chksm)

#=====================================================================		
# Class for handling a ocnnection between the Pi and PIC
#=====================================================================		
class ConnHandler:
	def __init__(self, address, port):
		# initialize class variables
		self.server = (address, port)
		self.connected = False
		self.started = False
		self.lastPing = time.time()
		txQueues[str(self.server[1])] = []

		# Create and open logfile for port
		dstr = datetime.datetime.now().strftime("_%Y%m%d")	
		self.log = open("/apps/logs/CommCtrl_" + dstr + ".log", "a")

		# create the socket for listening -- continue until success
		while not self.createSocket():
			continue

	#-----------------------------------------------------------------
	# Function for creating the stock socket on the desired port
	#-----------------------------------------------------------------
	def createSocket(self):
		# Create the socket and open it with desired settings
		try:
			# Create the socket and begin listening
			print "Creating socket on port", self.server[1]
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
			self.sock.bind(self.server)
			self.sock.listen(1)
			
			# Start the receive thread
			rx = threading.Thread(target=self.receiveHandler, args=())
			rx.daemon = True
			rx.start()
		except Exception as err:
			print "Failed to open socket on port %d" % self.server[1]
			print str(err)
			return False
		return True

	#-----------------------------------------------------------------
	# Function for creating the connection and preparing for exit
	#-----------------------------------------------------------------
	def close(self):
		# Try to close the conn and socket -- make sure to signal disc.
		try:
			self.sock.shutdown(socket.SHUT_RDWR)
			self.sock.close()
			self.conn.close()
			print "Connection closed on port", self.server[1]
			self.log.close()
		except Exception as err:
			print "Error closing connection on port", self.server[1]
			print str(err)
		self.connected = False

	#-----------------------------------------------------------------
	# Function for pinging the client to maintain connectivity
	#-----------------------------------------------------------------
	def ping(self):
		# Loop continously checking for conn and sending pings @ 3hz
		last = 0
		while True:
			# Stop pinging if disconnected
			if not self.connected:
				break
			
			# Send ping if it has been 300 ms since last
			if (time.time() - last) > 1.1:
				txQueues[str(self.server[1])].append('ee'.decode('hex'))
				last = time.time()
			time.sleep(0.05)
		print "Pings stopped for port", self.server[1]
		
	#-----------------------------------------------------------------
	# Function for carrying out message transmissions
	#-----------------------------------------------------------------
	def transmitHandler(self):
		# Loop continuously 
		while True:
			# Exit if disc. else transmit any available messages
			if not self.connected:
				return
			
			# If messages are present then send them via the socket
			if len(txQueues[str(self.server[1])]):
				try:
					self.conn.sendall(txQueues[str(self.server[1])].pop(0))
				except Exception as err:
					print str(err)
					continue

	#-----------------------------------------------------------------
	# Function for the general message receive/processor handling
	#-----------------------------------------------------------------
	def receiveHandler(self):
		# Make the connection before begin infinite message reception
		self.conn, self.addr = self.sock.accept()
		self.conn.settimeout(0.01)
		print "Connection found: %s:%s" %self.addr			
		
		# Run "forever"
		while True:
			# Check pings for maintaining connectivity
			if self.connected and (time.time()-self.lastPing > 5):
				print "Disconnected from %s:%s" %self.addr
				self.connected = False
				return		

			# Otherwise try to receive a message and see what it is
			try:
				msg = self.conn.recv(4)
				msg = msg
			except socket.timeout:
				continue
			except Exception as err:
				print str(err)
				continue

			# Process according if successful
			if (msg == '*HEL') or (msg == 'LO*') or not len(msg):
				continue
			elif (msg == 'ee'.decode('hex')) or \
						msg.startswith('ee'.decode('hex')):
				self.lastPing = time.time()
			elif msg == 'STRT':
				print 'Received start on port', self.server[1]
				seq_num[str(self.server[1])] = 0
				self.start()
			elif not msg:
				self.lastPing = 0		# Signal disc. if socket broken		

			# If not a control message ==> Route it accordingly
			else:
				if not self.invalidMsg(msg):
					self.routeMessage(msg)

	#---------------------------------------------------------------------
	# Function for reading messages and routing accordingly
	#---------------------------------------------------------------------
	def routeMessage(self, msg):
		# Unpack the two address fields
		self.logMessage(msg)
		frmt = "B B B B"
		unpacker = struct.Struct(frmt)
		mFields = unpacker.unpack(msg)
		
		# Get the destination and route to the desired queue 
		dst = (mFields[0] & 0xf)
		if dst in board1:
			txQueues["2001"].append(msg)
		elif dst in board2:
			txQueues["2002"].append(msg)
		elif dst in board3:
			txQueues["2003"].append(msg)
		elif dst in board4:
			txQueues["2004"].append(msg)
		elif dst in pi:
			txQueues["2005"].append(msg)
		
		# Check the sequence number
		if mFields[1] != seq_num[str(self.server[1])]:
			seq_num[str(self.server[1])] = mFields[1]
			seq_errors[str(self.server[1])] += 1
		seq_num[str(self.server[1])] += 1

	#---------------------------------------------------------------------
	# Function for writing a formatted message to the logfile
	#---------------------------------------------------------------------
	def logMessage(self, msg):
		mStr = messageString(msg)
		timeStr = datetime.datetime.now().strftime("%H:%M:%S")
		print "[%s] %s" %(self.server[1], mStr)
		self.log.write("[%s] %s\n" % (timeStr, mStr))

	#---------------------------------------------------------------------
	# Function for reading messages and routing accordingly
	#---------------------------------------------------------------------
	def invalidMsg(self, message):
		# check checksum -- print error if invalid and return 1
		# Try block catches conversion issues
		try:
			msg = int(message.encode('hex'), 16)
			if (msg & 0xff) != calcChecksum(msg):
				timeStr = datetime.datetime.now().strftime("%H:%M:%S")
				print "[%s] Invalid message: %s" \
						%(boards[str(self.server[1])], message.encode('hex'))
				self.log.write("[%s][%s] ERROR: Invalid Message: %s\n" \
						%(timeStr, boards[str(self.server[1])],  msg))
				pack_errors[str(self.server[1])] += 1
				return 1
		except Exception as err:
			print 'Error checking message validitiy.'
			print str(err)
			return 1
		return 0

	#-----------------------------------------------------------------
	# Function for starting message processing for a connection
	#-----------------------------------------------------------------
	def start(self):
		# Set status variables
		self.lastPing = time.time()
		self.connected = True
		self.started = True
		txQueues[str(self.server[1])] = []
		txQueues[str(self.server[1])].append('STRT')
		
		# Start a thread for pings, tx and rx then return
		pings = threading.Thread(target=self.ping, args=())					
		tx = threading.Thread(target=self.transmitHandler, args=())					
		pings.daemon = True
		tx.daemon = True
		pings.start()
		tx.start()

#=====================================================================		
# Class for coordinating the communication of all four devices
#=====================================================================		
class CommControl:
	#-----------------------------------------------------------------
	# Constructor
	#-----------------------------------------------------------------
	def __init__(self, servers=defaultHosts):
		# Initialize status variables
		self.statuses = ['Ready', 'Ready', 'Ready', 'Ready', 'Ready']
		self.servers = servers
		self.servers[1] = theseusMotorSim

	#-----------------------------------------------------------------
	# Function for starting connections when ready
	#-----------------------------------------------------------------
	def start(self):
		# Create the four connections 
		servers = self.servers
		print "Communication started. Press CTRL + C to stop."
		self.connOne = ConnHandler(servers[0][0], servers[0][1])
		self.connTwo = ConnHandler(servers[1][0], servers[1][1])
		self.connThree = ConnHandler(servers[2][0], servers[2][1])
		self.connFour = ConnHandler(servers[3][0], servers[3][1])
		self.connPi = ConnHandler(servers[4][0], servers[4][1])

		# Begin monitoring connection
		self.monitor()

	#-----------------------------------------------------------------
	# Function for checking connection statuses -- act accordingly
	#-----------------------------------------------------------------
	def checkConnections(self):
		# Check each connection and reopen if necessary
		servers = self.servers
		if not self.connOne.connected and self.connOne.started:
			self.connOne.close()
			self.connOne = ConnHandler(servers[0][0], servers[0][1])
		if not self.connTwo.connected and self.connTwo.started:
			self.connTwo.close()
			self.connTwo = ConnHandler(servers[1][0], servers[1][1])
		if not self.connThree.connected and self.connThree.started:
			self.connThree.close()
			self.connThree = ConnHandler(servers[2][0], servers[2][1])
		if not self.connFour.connected and self.connFour.started:
			self.connFour.close()
			self.connFour = ConnHandler(servers[3][0], servers[3][1])
		if not self.connPi.connected and self.connPi.started:
			self.connPi.close()
			self.connPi = ConnHandler(servers[4][0], servers[4][1])

	#-----------------------------------------------------------------
	# Function for continously monitoring/checking connection statuses
	#-----------------------------------------------------------------
	def monitor(self):
		# Loop continously -- allow for user to exit
		try:
			while True:	
				self.checkConnections()
				time.sleep(0.2)
				continue
		except KeyboardInterrupt:
			print "Program halted by user. Exiting now."
			try:
				sys.exit(0)
			except SystemExit:
				os._exit(0)

#=====================================================================		
# MAIN FUNCTION
#=====================================================================		
def main():
	# Start the communication controller -- should be done by UI 
	controller = CommControl()
	controller.start()
	return
	
	# Run forever or until stopped by user
	print "Communication started. Press CTRL + C to stop."
	try:
		while True:
			continue
	except KeyboardInterrupt:
		print "Program halted by user. Exiting now."
		try:
			sys.exit(0)
		except SystemExit:
			os._exit(0)

if __name__ == "__main__":
	main()


