## ctypes binding for leap motion

This is a simple ctypes binding for the leap motion, because the existing
SWIG based on is so hard to use.

Specifically, linking dynamically to the python framework on OSX means
that you can't use pyinstaller with leap code.

Which sucks.

So what is this? It's a new library with a C binding (libcleap) and a ctypes
binding for that. 

### Quick start

    c = leap_controller()
    l = leap_listener(500)
    leap_add_listener(c, l)

    try:
      while True:
        event = leap_poll_listener(l)
        if event:
          if any(e.frame.hands):
            print(e.frame.hands[0])
    except: 
      pass

    leap_remove_listener(c, l)
    leap_listener_dispose(l)
    leap_controller_dispose(c)

See tests/test.py for a complete example.

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
