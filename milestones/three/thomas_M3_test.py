#!/usr/bin/env python

#=====================================================================
# Filename: 	thomas_M3_test.py
# Date: 		10/23/2016
# Created by: 	Thomas Lazor
# Version:		1.0
# Info:		Python script for simulating control thread messages to
#			motor_thread. The user will input 'left', 'right', 
#			'forward', 'backward', or 'stop' to control the
#			movement of the rover.
#
#=====================================================================

import fileinput

def parseCommand(conn):
	for line in fileinput.input():
		if line == "left":
			conn.sendall(0x230001)
		elif line == "right":
			conn.sendall(0x230002)
		elif line == "forward":
			conn.sendall(0x230003)
		elif line == "backward":
			conn.sendall(0x230004)
		elif line == "stop":
			conn.sendall(0x230000)
		elif line == "exit":
			return
		else:
			continue