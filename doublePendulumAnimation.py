import doublePendulum as dp
import numpy as np
import matplotlib.pyplot as plt
import pygame
import sys
from time import sleep

def drawPendulum(t): 
	circle1_color = (0,0,255)
	circle2_color = (0,255,0)
	#calculate coordinates of both pendula (x1,y1) and (x2,y2)
	#note, pixel coordinates must be integers
	x1 = np.int16( x1off + l*np.sin(b[t,0]) ) 
	y1 = np.int16( y1off + l*np.cos(b[t,0]) )
	x2 = np.int16( x1    + l*np.sin(b[t,1]) )
	y2 = np.int16( y1    + l*np.cos(b[t,1]) )
	
	#draw pendula as circles, with lines between them
	pygame.draw.circle(screen, circle1_color, (x1,y1), 10, 2)
	pygame.draw.aaline(screen, (255,0,0), (x1off, y1off), (x1, y1), 1)
	pygame.draw.circle(screen, circle2_color, (x2,y2), 10, 2)
	pygame.draw.aaline(screen, (255,0,0), (x1, y1), (x2, y2), 1)

def drawTails(t):
	TAIL_LENGTH = 500
	SKIP = 5
	tmin = max( t - TAIL_LENGTH, SKIP)
	#draw streaming tails behind pendula along their previous locations
	x1s = np.int16(x1off + l*np.sin(b[tmin:t:SKIP, 0]) )
	y1s = np.int16(y1off + l*np.cos(b[tmin:t:SKIP, 0]) )
	x2s = np.int16(x1s + l*np.sin(b[tmin:t:SKIP, 1]) )
	y2s = np.int16(y1s + l*np.cos(b[tmin:t:SKIP, 1]) )
	for i in xrange(1, len(x1s) ):
		pygame.draw.aaline( screen, (128, 0,255), (x1s[i], y1s[i]), (x1s[i-1], y1s[i-1]) )
		pygame.draw.aaline( screen, (255, 255,0), (x2s[i], y2s[i]), (x2s[i-1], y2s[i-1]) )

def drawPhaseSpace(t):
	AXIS_SIZE = 25
	PHASE_LENGTH = 5000
	#animate the evolution of the pendula momentum by plotting p1 vs p2 (momenta) 
	pygame.draw.aaline( screen, (255, 255, 255), (p1off - AXIS_SIZE, p2off), (p1off + AXIS_SIZE, p2off) )
	pygame.draw.aaline( screen, (255, 255, 255), (p1off, p2off - AXIS_SIZE), (p1off, p2off + AXIS_SIZE ) )
	
	SKIP = 5 #SKIP points to reduce number of line segments needed
	tmin = max( t - PHASE_LENGTH, SKIP)
	#extract momenta coordinates, dialate by pDilation, and cast as integers
	p1 = (b[tmin:t, 2]*pDilation + p1off).astype('int16')
	p2 = (b[tmin:t, 3]*pDilation + p2off).astype('int16')
	#draw the lines
	for i in xrange(SKIP, len(p1),SKIP):
		pygame.draw.aaline( screen, (192, 72,165), (p1[i], p2[i]), (p1[i-SKIP], p2[i-SKIP]) )
	
def printUsageAndExit():
	print '*****'
	print 'Usage: python doublePendulumAnimation [theta1] [theta2] [momentum1] [momentum2]'
	print '[theta1] [theta2] [momentum1] [momentum2] are optional  initial values of the angles and momenta of the first and second pendulum, must be convertable to float' 
	print '*****'
	sys.exit(0)

def parseArgs():
	initialconditions = []
	if len(sys.argv) == 1:
		initialconditions = np.array([3.1415, 3.1416, 0, 0], dtype=np.float32) #other nice default values are: [1.8,.2,-1.25,.5]
		print "No initial conditions entered as args, so using default values: ", str(initialconditions)
	elif len(sys.argv) == 5:
		try:
			initialconditions = np.array([float(sys.argv[1]), float(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4])], dtype=np.float32)
			print 'Using initial conditions: ', initialconditions
		except ValueError:
			print 'invalid arguments. should be floats theta1, theta2, momentum1, momentum2'
			printUsageAndExit()
	else:
		printUsageAndExit()
	return initialconditions

NUM_TIMESTEPS = 100000
outSize = np.array([NUM_TIMESTEPS*4]) #NUM_TIMESTEPS*4 is total size of output array 


#setup window in pygame
background_colour = (0,0,0) #black
(width, height) = (480, 640) #window size (pixels)
screen = pygame.display.set_mode( (width, height) )
screen.fill(background_colour)
pygame.display.set_caption( 'Double Pendulum' )
pygame.display.flip()


x1off = width/2.0 #offset in screen coordinates for first pendulum
y1off = height*(1.0/4.0)
l = height / 10.0  #length of penulum

#offset for origin of phase space plot (p1 vs p2) 
p1off = width/2.0
p2off = height*(2.0/3.0)
pDilation = 20

#get initial theta and momentum values from input arguments, 
#	or use defaults
initialconditions = parseArgs()

#calculate theta and momenta values for the double pendulum 
# calls the compiled c - extension, compiled from doublePendulum.c
b = dp.doublePendulum(outSize,initialconditions)
#reshape so that b[:,1], b[:,2], b[:,3], b[:,4] 
#	correspond to theta1, theta2, momentum1 and momentum2
b = np.reshape(b, [NUM_TIMESTEPS, 4])

#main loop
running = True
t = 0
while running:
	#check for exit condition
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			running = False
		elif event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				running = False # user pressed ESC

	screen.fill(background_colour)
	
	#drawing functions
	drawPendulum(t)
	drawTails(t)
	drawPhaseSpace(t)
	pygame.display.flip()

	t = t + 1
	if t >= NUM_TIMESTEPS:
		running = False
	sleep(.001)
