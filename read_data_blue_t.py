import threading
from threading import Lock
import time
import serial
import os
from termcolor import colored

encoding = 'unicode_escape'
data_read=[None]*4
first_lock=[None]*4

data_process=[None]*4
second_lock=[None]*4

class Serial_Thread (threading.Thread):
	def __init__(self, threadID, name_serial):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.serialPort = serial.Serial(port=name_serial, baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1, xonxoff =False, rtscts = False, dsrdtr = False)
		
	def run(self):
		#while(not (self.threadID == 3)):
			#time.sleep(0)
		while(not self.serialPort.isOpen()):
			print(self.threadID)
			time.sleep(0)
		while 1:
			first_lock[self.threadID-1].acquire()
			data_read[self.threadID-1].append(self.serialPort.readline())
			first_lock[self.threadID-1].release()
			
			time.sleep(0)	
			#if data_read[self.threadID-1]:
				#data_str = data_read[self.threadID-1].decode(encoding)
				#print(data_str,end='')
				
			
class Handle_Thread (threading.Thread):
	def __init__(self, threadID):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.data=''
		self.s=False
		self.e=False
		
		
	def run(self):
		#while(not (self.threadID == 3)):
			#time.sleep(0)
			
		while(1):
			first_lock[self.threadID-1].acquire()
			if(data_read[self.threadID-1]):
				self.data += data_read[self.threadID-1].pop(0).decode(encoding)

			first_lock[self.threadID-1].release()
			
			if not self.s:
				if "START" in self.data :
					self.s=True
					self.data=self.data[self.data.find("START")+6:]
					
			if (not self.e) and self.s :
				if "END" in self.data :
					self.e=True
					
					second_lock[self.threadID-1].acquire()
					data_process[self.threadID-1].append(self.data[:self.data.find("END")-1])
					#print(self.data[:self.data.find("END")-1])
					second_lock[self.threadID-1].release()
					
					self.data=self.data[self.data.find("END"):]
					
					#print(self.data,end='')
					self.s=False
					self.e=False
						
						
			time.sleep(0)
			
class Terminal_Thread (threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.colors= ['red','green','blue','yellow']
      
	def run(self):
		data = [None]*4;
		while(1):
			for i in range(4):
			
				second_lock[i].acquire()
				if data_process[i] :
					data[i] = data_process[i].pop(0)
				second_lock[i].release()
				
				if not data[i] == None:
					print(colored("\nMonitor ",self.colors[i]), colored(i+1,self.colors[i]),colored(":",self.colors[i]))
					print(colored(data[i],self.colors[i]))
			
				
			time.sleep(0.5)
			os.system('clear')

if __name__ == '__main__':
	threads = []

	
	first_lock[0]=Lock()
	first_lock[1]=Lock()
	first_lock[2]=Lock()
	first_lock[3]=Lock()
	
	second_lock[0]=Lock()
	second_lock[1]=Lock()
	second_lock[2]=Lock()
	second_lock[3]=Lock()
	
	data_read[0]=list()
	data_read[1]=list()
	data_read[2]=list()
	data_read[3]=list()
	
	data_process[0]=list()
	data_process[1]=list()
	data_process[2]=list()
	data_process[3]=list()
	
	
	# Create new threads
	thread1 = Serial_Thread(1, '/dev/rfcomm1')
	thread2 = Serial_Thread(2, '/dev/rfcomm2')
	thread3 = Serial_Thread(3, '/dev/rfcomm3')
	thread4 = Serial_Thread(4, '/dev/rfcomm4')
	
	thread5 = Handle_Thread(1)
	thread6 = Handle_Thread(2)
	thread7 = Handle_Thread(3)
	thread8 = Handle_Thread(4)
	
	thread9 = Terminal_Thread()

	# Start new Threads
	thread1.start()
	thread2.start()
	thread3.start()
	thread4.start()
	thread5.start()
	thread6.start()
	thread7.start()
	thread8.start()
	thread9.start()

	# Add threads to thread list
	threads.append(thread1)
	threads.append(thread2)
	threads.append(thread3)
	threads.append(thread4)
	threads.append(thread5)
	threads.append(thread6)
	threads.append(thread7)
	threads.append(thread8)
	threads.append(thread9)


	
	# Wait for all threads to complete
	for t in threads:
	   t.join()
	 
