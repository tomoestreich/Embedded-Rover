#!/usr/bin/env python

#===============================================================================
# Filename: 	GameControl.py
# Date:			10/24/2016
# Created By:	Ben Johnson
# Modified:		BCJ - 11/14/16
# Version:		2.0
# Info:			This document contains the data and all backend 
#				functionality for the Maze control system used in the
#				Theseus and the Minotaur rover game as well as the UI.
#===============================================================================

# Import Libraries
import os
import sys
import time
from random import randint
import socket
import subprocess

# Add Rover Libraries from other directories
sys.path.append("/apps/pycontrol/comm")
from comm_ctrl import *

# Global coordinate variables
tX = 0
tY = 0
mX = 7
mY = 7
trX = 0
trY = 0
maze = []
hwalls = []
vwalls = []

# Global Variables
mazeFile = "/apps/pycontrol/cfg/maze_one.config"

# General Functionality functions
#===============================================================================
# Function for getting available options
#===============================================================================
def getOptions():
	opt = 15
	x = tX
	y = tY
	v = vwalls
	h = hwalls
	
	# Check right
	if x == 7:
		opt -= 1
	elif v[y][x]:
		opt -= 1
	# Check Left	
	if x == 0:
		opt -= 2
	elif v[y][x-1]:
		opt -= 2
	# Check forward 
	if y == 7:
		opt -= 4
	elif h[y][x]:
		opt -= 4
	# Check Back
	if y == 0:
		opt -= 8
	elif h[y-1][x]:
		opt -= 8
	return opt
	
#===============================================================================
# Function for getting the symbol based on maze obj id #
#===============================================================================
def getSymbol(id):
	id = int(id)
	if not id:
		return ' '
	elif id == 1:
		return 'T'
	elif id == 2:
		return 'M'
	elif id == 3:
		return 'X'

#===============================================================================
# Class for the Theseus and The Minotaur maze and its gameplay
#===============================================================================
class GameControl:
	#---------------------------------------------------------------------------
	# Contstuctor
	#---------------------------------------------------------------------------
	def __init__(self, headless=True):
		# Initialize variables
		self.serv_addr = ('localhost', 2005)
		self.initVariables()

		# Initialize the actual maze and then print (Terminal)
		self.initMaze()
		self.printMaze() # Add input to block during GUI mode

	#---------------------------------------------------------------------------
	# Function for initializing/resetting all game variables
	#---------------------------------------------------------------------------
	def initVariables(self):
		self.runnign = False
		self.processing = False
		self.seq = 0
		self.treasureFound = 0
		self.lastRcv = time.time()
		
	#---------------------------------------------------------------------------
	# Function for starting processing once all else is ready
	#---------------------------------------------------------------------------
	def startSimulations(self):
		# Start whichever simulations are desired based on input 
		self.thMtrSim = MotorSimulator(2002)
		while True:
			if not self.thMtrSim.connected and self.thMtrSim.started:
				time.sleep(1)
				print 'Reconnecting Theseus Motor Simulator.'
				self.thMtrSim.reconnect()

	#---------------------------------------------------------------------------
	# Board Initializing function
	#---------------------------------------------------------------------------
	def initMaze(self):
		self.initWalls()
		for i in range(0, 8):
			maze.append([])
			for j in range(0, 8):
				maze[i].append(0)
		
		# Place the treasure
		trX = randint(0, 7)
		trY = randint(0, 7)
		while (trX == mX and trY == mY) or (trX == tX and trY == tY):
			trX = randint(0, 7)
			trY = randint(0, 7)
		maze[tY][tX] = 1
		maze[mY][mX] = 2
		maze[trY][trX] = 3
	
	#-------------------------------------------------------------
	# Walls Initializing function
	#-------------------------------------------------------------
	def initWalls(self):
		# read in the file with the config
		with open(mazeFile) as inFile:
			setup = inFile.readlines()
			inFile.close()
	
		# Read in vertical walls, then horizontal
		global vwalls, hwalls
		vwalls = []
		hwalls = []
		for i in range(0, 8):
			cnt = 0
			vwalls.append([])
			for val in setup[i]:
				if val == '1' or val == '0':
					val = int(val)
					vwalls[i].append(val)
		for i in range(0, 7):
			cnt = 0
			hwalls.append([])
			for val in setup[i+9]:
				if val == '1' or val == '0':
					val = int(val)
					hwalls[i].append(val)
	
	#-------------------------------------------------------------
	# Function for printing the currect status of the maze
	#-------------------------------------------------------------
	def printMaze(self):
		# Loop through rows printing walls and current loc.s
		print "#   #---#---#---#---#---#---#---#"
		for row in range(0, len(maze)-1):
			# Vertical Elements
			self.printVert(row)

			# Horizontal Components
			self.printHor(row)
		self.printVert(-1)
		print "#---#---#---#---#---#---#---#---#"

	#-------------------------------------------------------------
	# Function for printing a row of vertical maze symbols
	#-------------------------------------------------------------
	def printVert(self, row):
		temp = []
		temp.append('| ')
		for col in range(0, len(maze)-1):
			temp.append(getSymbol(maze[row][col]))
			if vwalls[row][col]:
				temp.append(' | ')
			else:
				temp.append('   ')
		temp.append(getSymbol(maze[row][-1]))
		temp.append(' |')
		print ''.join(temp)

	#-------------------------------------------------------------
	# Function for checking if treasure was just found
	#-------------------------------------------------------------
	def checkTreasure(self):
		if (tX == trX) and (tY == trY):
			self.sock.sendall(buildMsg(RPI,\
				THESEUS_CONTROL, 0, 1))
			self.running = 0
			self.processing = 1
			print "Treasure Found at (%d, %d)" %(tX, tY)
			self.treasureFound = 1
			return True
		return False

	#-------------------------------------------------------------
	# Function for checking if the game was won
	#-------------------------------------------------------------
	def checkWin(self):
		if(tX == 0) and (tY == 0) and self.treasureFound:
			msg = buildMsg(RPI, THESEUS_CONTROL, 1, 0xf)
			self.sock.sendall(msg)
			self.running = 0
			self.processing = 0
			print "GAME OVER: THESEUS WON!"
			return True
		return False

	#-------------------------------------------------------------
	# Function for printing a row of horizontal maze symbols
	#-------------------------------------------------------------
	def printHor(self, row):
		temp = []
		temp.append('#')
		for col in range(0, len(maze)-1):
			if hwalls[row][col]:
				temp.append('---')
			else:
				temp.append('   ')
			temp.append('#')
		if hwalls[row][-1]:
			temp.append('---')
		else:
			temp.append('   ')
		temp.append('#')
		print ''.join(temp)

	#-------------------------------------------------------------
	# Function for handling general simulation of communicate
	#-------------------------------------------------------------
	def simulateComm(self):
		# Open the socket
		self.startSimSocket()

		# Open a thread for send and for receive
		self.rcv = threading.Thread(target=self.simulateReceive,\
			args=())
		self.rcv.daemon = True
		self.rcv.start()
		self.trn = threading.Thread(target=self.simulateTransmit,
			args=())
		self.trn.daemon = True
		self.trn.start()
	
	#-------------------------------------------------------------
	# Function for opening a thread to wait and send option data
	#-------------------------------------------------------------
	def sendOptions(self):
		# if not already generating options -- send them
		if not self.running and not self.processing:
			self.running = 1
			opt = threading.Thread(target=self.getOptions,args=())
			opt.daemon = True
			opt.start()

#===============================================================================
# Class for simulating the operation of a motor control board
#===============================================================================
class MotorSimulator:
	#---------------------------------------------------------------------------
	# Constructor	
	#---------------------------------------------------------------------------
	def __init__(self, device=2002):
		# set necessary variables
		self.name = boards[str(device)]
		if device == 2002:
			self.server = theseusMotorSim
		elif device == 2004:
			self.server = minMotorSim
		else:
			print "Invalid device ID for motor simulations."
			return
		self.lastPing = time.time()
		self.lastSend = time.time()
		self.connected = False
		self.started = False
		self.stop = False
		self.txQueue = []
		self.options = 'ee'.decode('hex')
		self.waiting = False
		self.seq = 0

		# Connect to the server then begin listening
		self.connect()

	#---------------------------------------------------------------------------
	# Function for connecting to the server
	#---------------------------------------------------------------------------
	def connect(self):
		print self.name, 'simulator attempting to connect to server.'
		while True:
			# Attempt to connect the server on the desired port
			try:
				# Connect to the socket
				self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
				self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
				self.sock.connect(self.server)
				self.sock.settimeout(0.01)
				
				# Start the RX thread
				self.rxThread = threading.Thread(target=self.receive, args=[])
				self.rxThread.daemon = True
				self.rxThread.start()

			# handle expected errors
			except KeyboardInterrupt:
				print "Simulation halted by user. Exiting now."
				self.ocnnected = False
				sys.exit(0)
			except:
				continue
	
			# Start the receive thread
			self.connected = True
			print self.name, "simulator connected to the server."
			break

	#---------------------------------------------------------------------------
	# Function for reconnecting the socket to the server
	#---------------------------------------------------------------------------
	def reconnect(self):
		# Attempt to receonnect
		try:
			self.connect()
			return True
		except Exception as err:
			return False

	#---------------------------------------------------------------------------
	# Message reception handler for the motor simulator
	#---------------------------------------------------------------------------
	def receive(self):
		# Run infinitely until interrupted
		self.sock.sendall('STRT')
		self.lastPing = time.time()
		self.start()
		while True:
			# Check ping and connection status
			if (self.connected and self.started and (time.time()-self.lastPing > 5))\
						 or (not self.connected and self.started) or self.stop:
				print self.name, 'disconnected from server.'
				self.connected = False
				self.close()
				return

			# Try to receive a message
			try:
				msg = self.sock.recv(4)
			except:
				continue

			# respond if ping
			if (msg == 'ee'.decode('hex')) or msg.startswith('ee'.decode('hex')):
				self.lastPing = time.time()
				self.txQueue.append(msg.encode('hex'))
				self.connected = True
			else:
				self.processMessage(msg)

	#---------------------------------------------------------------------------
	# Function for processing inbound messages from ctrl -- only error checking
	#---------------------------------------------------------------------------
	def processMessage(self, message):
		# Check the validity of the message
		if len(message) != 4:
			return
		msg = unpackMessage(message)
		if msg[4] != calcChecksum(message):
			return

		# If valid update status bits
		ack = buildMsg(THESEUS_LINE, THESEUS_CONTROL, self.seq, 254)
		self.seq = (self.seq + 1) % 250
		self.txQueue.append(ack.encode('hex'))
		self.txQueue.append(ack.encode('hex'))
		self.waiting = False

	#---------------------------------------------------------------------------
	# Function for starting motor thread simulation
	#---------------------------------------------------------------------------
	def start(self):
		# Start the transmition/generation of messages
		self.txThread = threading.Thread(target=self.transmit, args=[])
		self.txThread.daemon = True
		self.txThread.start()

		# Start the thread for generating options messages
		self.optThread = threading.Thread(target=self.sendOptions, args=[])
		self.optThread.daemon = True
		self.optThread.start()
		self.started = True
		self.lastPing = time.time()
		self.connected = True

	#---------------------------------------------------------------------------
	# Function for closing the socket
	#---------------------------------------------------------------------------
	def close(self):
		# Close the socket/connection
		try:
			self.sock.close()
			print 'Simulation closed for', self.name
		except Exception as err:
			print 'Error closing simulation for', self.name
			print str(err)
		self.connected = False

	#---------------------------------------------------------------------------
	# Function for carrying out the transmission thread
	#---------------------------------------------------------------------------
	def transmit(self):
		# loop "foreer"
		print self.name, "transmit thread started."
		while True:
			# if closed exit
			if not self.connected:
				return

			# If connected to the server, transmit available messages
			if self.connected:
				while len(self.txQueue):
					try:
						self.sock.sendall(self.txQueue.pop(0).decode('hex'))
					except Exception as err:
						print str(err)
						self.connected = False
						break
	
	#---------------------------------------------------------------------------
	# Function for processing incoming decisions and updating the board
	#---------------------------------------------------------------------------
	def sendOptions(self):
		while True:
			# Stop when instructed
			if not self.connected:
				return 
 
			# If options need to be sent, generate them and add to txQueue
			if not self.waiting and self.connected:
				time.sleep(float(randint(2, 6)/1.38))
				self.options = getOptions()
				msg = buildMsg(THESEUS_LINE, THESEUS_CONTROL, self.seq, self.options)
				self.txQueue.append(msg.encode('hex'))
				self.seq = (self.seq + 1) % 250
				self.lastSend = time.time()
				self.waiting = True
			
			# Otherwise check to see if it has been more than 5s of waiting
			elif self.waiting and self.connected and (time.time()-self.lastSend > 1):
				self.options = getOptions()
				msg = buildMsg(THESEUS_LINE, THESEUS_CONTROL, self.seq, self.options)
				self.seq = (self.seq + 1) % 250
				self.lastSend = time.time()
				self.txQueue.append(msg.encode('hex'))

#===============================================================================
# Main Function -- Only for use in headless mode
#===============================================================================
def main():
	try:
		print "Starting Theseus and the Minotaur Game. Press CTRL+C to exit."
		Control = GameControl()
		
		# run the simulations in a separate thread for monitoring/restarting
		clients = threading.Thread(target=Control.startSimulations, args=[])
		clients.daemon = True
		clients.start()
	except Exception as err:
		print str(err)
	
	# Loop forever once threads are started -- try catch for user exit
	try: 
		while True:
			time.sleep(30)
	except KeyboardInterrupt:
		print "Game halted by user. Exiting now."
		sys.exit(0)

if __name__ == "__main__":
	main()





				
