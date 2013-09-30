import time
import sys
from os.path import abspath, dirname, join
from nark import *

print("Trying to import leap guessing what dll's to use")

if platform() == platforms.MAC:
  target = sys.executable
  success, out = run('otool', '-L', target, capture_output=True)
  lines = [x.strip() for x in out.split('\n')]
  lines = filter(lambda x: ('Python' in x or 'python' in x) and ':' not in x, lines)

  path = join(dirname(abspath(__file__)), 'mac')
  dll = join(path, 'LeapPython.so')
  success, out = run('otool', '-L', dll, capture_output=True)
  dlines = [x.strip() for x in out.split('\n')]
  dlines = filter(lambda x: ('Python' in x or 'python' in x) and ':' not in x and 'LeapPython' not in x, dlines)

  python_lib = lines[0].split(' ')[0]
  leap_lib = dlines[0].split(' ')[0]
  if python_lib != leap_lib:
    print('Attempting to patch dll...')
    run('install_name_tool', '-change', leap_lib, python_lib, dll)

  sys.path.append(path)
  import Leap

if platform() == platforms.WINDOWS:
  base = join(dirname(abspath(__file__)), 'win')
  sys.path.append(join(base, 'x64'))
  try:
    import Leap
  except:
    sys.path.pop()
    sys.path.append(join(base, 'x86'))
    import Leap
