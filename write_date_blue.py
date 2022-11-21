import serial  
import time
import struct

if __name__ == '__main__':
	serialPort = serial.Serial(port='/dev/rfcomm0', baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False)
	
	tempo = int(time.time())
	print(tempo)
	serialPort.write(bytes( str(tempo), 'utf-8'))
	
		
