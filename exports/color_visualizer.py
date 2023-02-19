import pygame
import serial
import sys

if len(sys.argv) < 2:
	print('please provide a port number to connect to (format COM#)')
	sys.exit()

try:
	#try to open the serial port
	ser = serial.Serial(sys.argv[1], 115200)
except serial.serialutil.SerialException:
	print('could not find port %s' % sys.argv[1])
	sys.exit()

#create a 480x480 window
screen = pygame.display.set_mode((480,480))
#register a timer event to fire every 100ms
pygame.time.set_timer(pygame.USEREVENT, 100)

while True:
	#wait until an event fires
	event = pygame.event.wait()
	if event.type == pygame.QUIT:
		#user wants to close the window
		break
	elif event.type == pygame.USEREVENT:
		#timer fired, request some data, and update the display
		ser.write(bytes('?', 'utf-8'))
		s = ser.readline().decode('utf-8')
		color = [0]
		idx = 0
		
		#expected format: 'r=%04x g=%04x b=%04x\r\n'
		color = [s[2:6],s[9:13],s[16:20]]
		#pygame color expects values in the range 0-255, strip off the bottom eight bits of each color channel
		rgb = [int(v,16)>>8 for v in color]

		# fill the screen surface with the received color and update the display
		screen.fill(pygame.Color(rgb[0],rgb[1],rgb[2],255))
		pygame.display.flip()
pygame.quit()