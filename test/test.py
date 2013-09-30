import os
import sys
import time
from os.path import dirname, join, abspath
sys.path.append(join(dirname(__file__), '..', 'src'))

# Set DLL path for leap libraries
os.environ['LEAP_DLL_PATH'] = abspath(join(dirname(__file__), '..', 'dll'))

# Import leap
from cleap.leap import *

c = leap_controller()
l = leap_listener(500)
leap_add_listener(c, l)
leap_enable_background(c)

counter = 0
while counter < 1000:
  waiting = False
  while not waiting:
    event = leap_poll_listener(l)
    if event:
      e = Event(event)
      counter += 1
      print(e)
    else:
      waiting = True
  time.sleep(0.01)

leap_remove_listener(c, l)
leap_listener_dispose(l)
leap_controller_dispose(c)
