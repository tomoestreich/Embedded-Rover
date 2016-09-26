#!/usr/bin/env python
#=====================================================================
# Filename: 	dhcp_addclient.py
# Date: 	09/26/2016
# Created by: 	Ben Johnson
# Modified:
# Version:	1.0
# Info:		Python script for adding a static IP address to the 
#		udhcpd.conf file
#			
#=====================================================================		

# Libraries
import os
import sys
import time
import argparse

#---------------------------------------------------------------------
# Function for checking ip address validity
#---------------------------------------------------------------------
def checkIP(addr):
	ip = addr.split('.')
	if len(addr) > 15 or len(addr) < 7:
		print "Invalid IP address. Please use IPv4 format."
		return -1
	if ip[0] != "192" or ip[1] != "168" or ip[2] != "42":
		print "Invalid IP address. Out of range."
		return -1
	elif int(ip[3]) < 60 or int(ip[3]) > 254:
		print "Invalid address. Out of range."
		return -1
	return 1

#---------------------------------------------------------------------
# MAIN FUNCTION
#---------------------------------------------------------------------
def main():
	# Set up arguments and parse the provided values
	parser = argparse.ArgumentParser()
	parser.add_argument("--IP", "-i", help="Provide the desired"+ 
		" static IP address")
	parser.add_argument("--MAC", "-m", help="Provide the devices"+
		" MAC Address")
	args = parser.parse_args()
	if checkIP(args.IP) < 0:
		return;
	print args.IP
if __name__ == "__main__":
	main()


