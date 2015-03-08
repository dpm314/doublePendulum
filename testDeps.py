#!/usr/bin/python

import sys, imp

found = True
deps = ['numpy', 'matplotlib.pyplot', 'pygame']

for dep in deps:
	try:
		imp.find_module('numpy')
	except ImportError:
		print "'numpy' dependency is not found"
		found = False

if found == False:
	sys.exit(1)
else:
	print 'numpy dependence found, all is well'
	sys.exit(0)
