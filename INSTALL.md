## OGDF

See the instructions on github: https://github.com/ogdf/ogdf/blob/master/doc/build.md
Note that you must build from source.  This repo includes the build system and ogdf as a submodule, so all you have to do is:
```
git submodule init && git submodule update
cd ogdf
cmake -DOGDF_WARNING_ERRORS=OFF .  # OR: `cmake3` if you have multiple versions. 
```

Then the build system should work for all executables.
