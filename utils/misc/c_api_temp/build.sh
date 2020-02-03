gcc -fpic --shared $(python3-config --includes) greetmodule.c -o greet.abi3.so
# can also use $(pkg-config --cflags python-3.5)
# or
# python3 setup.py install --record files.txt --user
