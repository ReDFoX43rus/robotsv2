#!/usr/bin/env python

import serial
import sys
import getopt
import os
import time

device = "/dev/ttyUSB0"

if len(sys.argv) != 2:
	print "file_send.py filename"
	sys.exit()

tty = serial.Serial(device, 115200)

def sendFile(filename):
	sfile = open(filename, "rb")

	sfile.seek(0, os.SEEK_END)
	size = sfile.tell()
	sfile.seek(0, os.SEEK_SET)

	print "Filename: " + os.path.basename(filename)
	#tty.write("filereceive " + "/fat/" + os.path.basename(filename) + " " + str(size) + "\n")
	tty.write("update " + str(size) + "\n");

	time.sleep(0.5)

	# TODO: Check return value

	block = size
	while size > 0:
		data = sfile.read(block if size > block else size)
		tty.write(data)
		#time.sleep(0.025)
		size -= block
		print(size)

	sfile.close()

sendFile(sys.argv[1])

tty.close()
