Files: 
	doublePendulum.c
	doublePendulumAnimation.py
	setup.py
	testDeps.py
	Makefile
	readme.txt

Required: 
	Python 2.7
	python-dev package (sudo apt-get install python-dev)
	gcc compiler
	python libraries: matplotlib, numpy, pygame


This program displays an animation of the double pendulum system. 
A plot of the momenutum phase space is also animated, the two 
axes are each pendulum's linear momentum (speed). 
A numerical integration of the lagrangian equations of motion is 
completed in doublePendulum.c, which is compiled into a c-extension
callable directly by python. The following tutorials were used to 
add the c-extension to python:

http://dan.iel.fm/posts/python-c-extensions/
http://scipy-lectures.github.io/advanced/interfacing_with_c/interfacing_with_c.html

and the dynamics are a modified version of the derivations
presented here:  

http://scienceworld.wolfram.com/physics/DoublePendulum.html

Usage: 
make build
python doublePendulumAnimation.py [theta1] [theta2] [momentum1] [momentum2]

The (optional) arguments allow the user to start with arbitrary initial 
coniditions for both pendula's initial angle and momentum (speed). 
Very interesting and diverse dynamcs can be found trying different 
settings for these initial conditions. The numerics are not stable for 
high initial momentum values (greater than about 5).
