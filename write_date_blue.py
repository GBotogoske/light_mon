import serial  
import time
import struct

if __name__ == '__main__':
	serialPort1 = serial.Serial(port='/dev/rfcomm1', baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False)
	serialPort2 = serial.Serial(port='/dev/rfcomm2', baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False)
	serialPort3 = serial.Serial(port='/dev/rfcomm3', baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False)
	serialPort4 = serial.Serial(port='/dev/rfcomm4', baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False)
	
	tempo = int(time.time())
	print(tempo)
	serialPort1.write(bytes( str(tempo), 'utf-8'))
	serialPort2.write(bytes( str(tempo), 'utf-8'))
	serialPort3.write(bytes( str(tempo), 'utf-8'))	
	serialPort4.write(bytes( str(tempo), 'utf-8'))
