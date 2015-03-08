all: build

test:
	python testDeps.py
	# test for gcc, and any other system libs needed

clean:
	rm -f doublePendulum.so

build: clean test
	python testDeps.py
	gcc -shared -o doublePendulum.so -I/usr/include/python2.7 -lpython2.7 -fPIC doublePendulum.c
	python setup.py build_ext --inplace
	# Build complete. run with 'make run' or 'python doublePendulumAnimation.py'

run:
	python doublePendulumAnimation.py
