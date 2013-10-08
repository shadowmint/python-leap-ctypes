## ctypes binding for leap motion

This is a simple ctypes binding for the leap motion, because the existing
SWIG based on is so hard to use.

Specifically, linking dynamically to the python framework on OSX means
that you can't use pyinstaller with leap code.

Which sucks.

So what is this? It's a new library with a C binding (libcleap) and a ctypes
binding for that. 

### Quick start

See tests/test.py

### Compiling libcleap

Use cmake. 

    mkdir cleap/build
    cd cleap/build
    cmake ..
    make

For windows you may want to use cmake-gui to generate a build file.

Notice that because the original leap DLL is built with visual studio
you must build libcleap with visual studio or visual studio express (free)
too.
