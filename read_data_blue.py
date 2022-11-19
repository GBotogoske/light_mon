import serial

encoding = 'utf-8'

if __name__ == '__main__':
	serialPort = serial.Serial(port='/dev/rfcomm0', baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False)
	print_string = ""
	while 1:
		
		data_bin = serialPort.readline()
		if data_bin:
			data_str = data_bin.decode(encoding)
			print_string = print_string + data_str
			if data_str.find('\n'):
				print(print_string)

