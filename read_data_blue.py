import serial
import time

import numpy as np

encoding = 'unicode_escape'

if __name__ == '__main__':
	t=0;

	serialPort=list()
	serialPort.append(serial.Serial(port='/dev/rfcomm1', baudrate=9600, timeout=t, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False))
	serialPort.append(serial.Serial(port='/dev/rfcomm2', baudrate=9600, timeout=t, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False))
	serialPort.append(serial.Serial(port='/dev/rfcomm3', baudrate=9600, timeout=t, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False))
	serialPort.append(serial.Serial(port='/dev/rfcomm4', baudrate=9600, timeout=t, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False))
	print_string = ""
	data_str=""
	
	while 1:
	
		for i in range(1):#len(serialPort)):
			ser = serialPort[i]
			if(ser.isOpen()):
				data_str=""
				while True:
					data_str += ser.readline().decode(encoding)
					if(data_str.find('START\n')):
						break
				while True:
					data_bin = ser.readline()
					if data_bin:
						data_str += data_bin.decode(encoding)
						#print_string = print_string + data_str
						#if data_str.find('\n'):
						#print("\n")
						#print_string=""
					if(data_str.find('END\n')):
						break
						
				print(data_str, end="\n")	
			time.sleep(10)	

