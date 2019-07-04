
.PHONY: original clean

py3:
	g++ -c -fPIC code.cpp
	swig -c++ -python code.i
	g++ -c -fPIC code_wrap.cxx  -I$(CONDA_PYTHON_EXE)
	g++ -shared -Wl,-soname,_code.so -o _code.so code.o code_wrap.o

original:
	g++ -c -fPIC code.cpp
	swig -c++ -python code.i
	g++ -c -fPIC code_wrap.cxx  -I/usr/include/python2.7 -I/usr/lib/python2.7
	g++ -shared -Wl,-soname,_code.so -o _code.so code.o code_wrap.o

clean:
	rm -rf _code.so code.o code_wrap.* code.pyc __pycache__/ code.py
