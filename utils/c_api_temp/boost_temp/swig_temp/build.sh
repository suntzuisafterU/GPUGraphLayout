# Works on example, trying to write for python3.*
g++ -c -fPIC code.cpp
swig -c++ -python code.i
g++ -c -fPIC code_wrap.cxx  -I/usr/include/python2.7 -I/usr/lib/python2.7
g++ -shared -Wl,-soname,_code.so -o _code.so code.o code_wrap.o
