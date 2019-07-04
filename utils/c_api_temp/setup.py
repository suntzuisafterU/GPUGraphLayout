from setuptools import setup, Extension


# For installing via python
# python3 setup.py install --record files.txt --user
setup(
    name='greet',
    version='1.0',
    description='Python Package with Hello World C Extension',
    ext_modules=[
        Extension(
            'greet',
            sources=['greetmodule.c'],
            py_limited_api=True)
    ],
)
