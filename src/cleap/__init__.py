from __future__ import absolute_import
from . import _os
import os
import ctypes
from os.path import join

path = os.environ.get('LEAP_DLL_PATH', None)
if not path:
  raise Exception('Unable to locate leap DLLs')

# Determine file based on system
if _os.platform() == _os.platforms.WINDOWS:
  lib = '.dll'
elif _os.platform() == _os.platforms.MAC:
  lib = '.dylib'
else:
  lib = '.so'
lib = 'libcleap' + lib

# Load DLL
LEAP = ctypes.CDLL(join(path, lib))
if LEAP is None:
  raise Exception('Unable to load leap DLLs')
