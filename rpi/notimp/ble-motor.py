import bluetooth
import RPi.GPIO as GPIO
import time
 
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
 
port = 1
server_sock.bind(("",port))
server_sock.listen(1)
 
client_sock,address = server_sock.accept()
print "Accepted connection from ",address

GPIO.setup(23, GPIO.OUT)
GPIO.setup(24, GPIO.OUT)
GPIO.setup(18, GPIO.OUT)
GPIO.setup(17, GPIO.OUT)

while True:
 
 data = client_sock.recv(1024)
 print "received [%s]" % data
 if (data == "8"):
 	print ("Going forward")
 	GPIO.output(23,GPIO.HIGH)	
	GPIO.output(24,GPIO.LOW)
	GPIO.output(18,GPIO.HIGH)
	GPIO.output(17,GPIO.LOW)

 if (data == "2"):
 	print ("Going backward")
 	GPIO.output(23,GPIO.LOW)
	GPIO.output(24,GPIO.HIGH)
	GPIO.output(18,GPIO.LOW)
	GPIO.output(17,GPIO.HIGH)
	
 if (data == "4"):
 	print ("Going backward")
 	GPIO.output(23,GPIO.HIGH)
	GPIO.output(24,GPIO.LOW)
	GPIO.output(18,GPIO.LOW)
	GPIO.output(17,GPIO.LOW)
	
 if (data == "6"):
 	print ("Going backward")
 	GPIO.output(23,GPIO.LOW)
	GPIO.output(24,GPIO.LOW)
	GPIO.output(18,GPIO.HIGH)
	GPIO.output(17,GPIO.LOW)

 if (data == "0"):
	print ("Stopping vehicle")
 	GPIO.output(23,GPIO.LOW)
	GPIO.output(24,GPIO.LOW)
	GPIO.output(18,GPIO.LOW)
	GPIO.output(17,GPIO.LOW)
	
 if (data == "e"):
	print ("Exit")
 	GPIO.output(23,GPIO.LOW)
	GPIO.output(24,GPIO.LOW)
	GPIO.output(18,GPIO.LOW)
	GPIO.output(17,GPIO.LOW)	
 	break
 
client_sock.close()
server_sock.close()
