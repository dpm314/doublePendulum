//compile with:
//	gcc -shared -o doublePendulum.so -I/usr/include/python2.7 -lpython2.7 -fPIC doublePendulum.c

#include <Python.h>
#include <stdio.h>
#include <numpy/arrayobject.h>
//#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <math.h>

#define DT .005

#define DEBUG 0
static PyObject *doublePendulum(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
    {"doublePendulum", doublePendulum, METH_VARARGS},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initdoublePendulum(void) {
    PyObject *m = Py_InitModule("doublePendulum", module_methods);
    if (m == NULL)
        return;
    // Load numpyfunctionality.
    import_array();
}

void f(float* vars, float* derivs, float h) {
	//http://scienceworld.wolfram.com/physics/DoublePendulum.html
	/*
	 calculate derivatives of theta1, theta2, p1 and p2
	 vars[0] is theta1
	 vars[1] is theta2
	 vars[2] is p1
	 vars[3] is p2
	 derivs[0] to derivs[3] are their instantaneous time derivatives
	 h is time step
	 */
	float t1, t2, p1, p2;
	float dt1, dt2, dp1, dp2;
	float delta, C1, C2; 
	delta = t1 - t2; 
	t1 = vars[0]; t2 = vars[1]; p1 = vars[2]; p2 = vars[3];

	C1 = p1*p2*sin(delta) / (1.0 + sin(delta)*sin(delta)); 
	C2 = (p1*p1 + 2.0*p2*p2 - p1*p2*cos(delta) )*sin(2.0*delta) / (2.0*(1.0 + sin(delta)*sin(delta) )*(1.0 + sin(delta)*sin(delta) ));
	dt1 = (p1 - p2*cos(delta))		/(1.0 + sin(delta)*sin(delta));
	dt2 = (2.0*p2 - p1*cos(delta))	/(1.0 + sin(delta)*sin(delta));
	dp1 = -2.0*sin(t1) - C1 + C2; 
	dp2 = -sin(t2)     + C1 - C2; 

	derivs[0] = dt1; derivs[1] = dt2; derivs[2] = dp1; derivs[3] = dp2;
}

void rk4(float* vars, float* soln, float h) {
	//Runge-Kutta 4th order numerical integration: 
	//	y_(n+1) = y_n + (h/6)*(k1 + 2*k2 + 2*k3 + k4)
	//vars is previous timestep solution y_n
	//	t_(n+1) = t_n + h
	float vars0[4];	float vars1[4];	float vars2[4];	float vars3[4];
	float k1[4]; float k2[4]; float k3[4]; float k4[4];
	float h2 = (h / 2.0);

	int i = 0;
	
	//load previous step
	for(i = 0; i <= 3; i++) {
		vars0[i] = vars[i]; 
	}
	
	if(DEBUG){
		printf("	rk4 vars: %2.4f, %2.4f, %2.4f, %2.4f \n",vars[0], vars[1], vars[2], vars[3]);
	}
	//calculate k1
	f(vars0, k1, h);
	if(DEBUG){
		printf("	k1 : %2.4f, %2.4f, %2.4f, %2.4f \n",k1[0], k1[1], k1[2], k1[3]);
	}

	//calculate k2
	vars1[0] = vars0[0] + (h2)*k1[0];
	vars1[1] = vars0[1] + (h2)*k1[1];
	vars1[2] = vars0[2] + (h2)*k1[2];
	vars1[3] = vars0[3] + (h2)*k1[3];
	f(vars1, k2, h); 
	if(DEBUG){
		printf("	k2 : %2.4f, %2.4f, %2.4f, %2.4f \n",k2[0], k2[1], k2[2], k2[3]);
	}

	//calculate k3
	vars2[0] = vars0[0] + (h2)*k2[0];
	vars2[1] = vars0[1] + (h2)*k2[1];
	vars2[2] = vars0[2] + (h2)*k2[2];
	vars2[3] = vars0[3] + (h2)*k2[3];
	f(vars2, k3, h); 

	if(DEBUG){
		printf("	k3 : %2.4f, %2.4f, %2.4f, %2.4f \n",k3[0], k3[1], k3[2], k3[3]);
	}

	//calculate k4
	vars3[0] = vars0[0] + (h2)*k3[0];
	vars3[1] = vars0[1] + (h2)*k3[1];
	vars3[2] = vars0[2] + (h2)*k3[2];
	vars3[3] = vars0[3] + (h2)*k3[3];
	f(vars3, k4, h);

	if(DEBUG){
		printf("	k4 : %2.4f, %2.4f, %2.4f, %2.4f \n",k4[0], k4[1], k4[2], k4[3]);
	}

	if(DEBUG){
		printf("	before update: soln %2.4f, %2.4f, %2.4f, %2.4f \n",vars[0], vars[1], vars[2], vars[3]);
	}

	for(i = 0; i <= 3; i++) 
	{
		soln[i] = vars[i] + (h/6.0)*(k1[i] + 2.0*k2[i] + 2.0*k3[i] + k4[i]);
	}

	if(DEBUG){
		printf("	h, soln : %2.8f, %2.8f, %2.8f, %2.8f, %2.8f \n",h, soln[0], soln[1], soln[2], soln[3]);
	}
}

void compute(float* vars, float* soln, int numTimesteps) {
	//soln is complete solution for all timesteps
	//solnStep is intermediate solution for passing between rk4() calls
	//vars are intermediate solution from previous step
	
	//compute(in,         out,         outSize/4);
	float h = DT; 
	int t = 0; 
	float solnStep[4];
	solnStep[0] = 0.0;	solnStep[1] = 0.0; solnStep[2] = 0.0; solnStep[3] = 0.0;

	//assign initial condition to first step of solution
	soln[0] = vars[0];
	soln[1] = vars[1];
	soln[2] = vars[2];
	soln[3] = vars[3];

	//main loop over time, runge-kutta 4th order integrating of double pendulum
	for(t = 0; t < numTimesteps; t++)	{
		rk4(vars, solnStep, h);
		vars = solnStep;
		//record solution for return to python
		soln[4*t + 0] = solnStep[0]; 
		soln[4*t + 1] = solnStep[1]; 
		soln[4*t + 2] = solnStep[2]; 
		soln[4*t + 3] = solnStep[3];
		if(DEBUG){
			printf("solnStep[%i]: %2.4f, %2.4f, %2.4f, %2.4f \n",t,solnStep[0], solnStep[1], solnStep[2], solnStep[3]);
		}
	}
}

static PyObject *doublePendulum(PyObject *self, PyObject *args) {
	int outSize;
	float* in;
	float* out;
	// variables with _ preface are python objects corresponding to c primitives with same name
	PyArrayObject* _in = NULL;
	PyArrayObject* _out = NULL;

	if (!PyArg_ParseTuple(args, "iO!", &outSize, &PyArray_Type, &_in))
	{
		printf("\n\nError with PyArg_ParseTuples, did not recieve (nparray,int) as args\n\n"); 
		return NULL;
	}
	
	//create numpy array for return
	_out = (PyArrayObject*)PyArray_FromDims(1, &outSize, NPY_FLOAT);
	//point local pointer out to data of _out 
	out = (float*)_out->data;
	//assign local float* x to point at data of input numpy array
	in = (float*)_in->data;

	compute(in, out, outSize/4);
	
	return PyArray_Return(_out);
}
