from ctypes import *
from . import LEAP

LEAP_ON_INIT = 0x0001
LEAP_ON_CONNECT = 0x0002
LEAP_ON_DISCONNECT = 0x0003
LEAP_ON_EXIT = 0x0004
LEAP_ON_FRAME = 0x0005
LEAP_ON_FOCUS_GAINED = 0x0006
LEAP_ON_FOCUS_LOST = 0x0007


class LEAP_VECTOR(Structure):
  _fields_ = [
    ('points', c_float * 3),
  ]

class LEAP_FINGER(Structure):
  _fields_ = [
    ('id', c_int),
    ('position', LEAP_VECTOR),
    ('velocity', LEAP_VECTOR),
    ('direction', LEAP_VECTOR),
  ]

class LEAP_HAND(Structure):
  _fields_ = [
    ('finger_count', c_int),
    ('fingers', LEAP_FINGER * 5),
    ('palm_position', LEAP_VECTOR),
    ('palm_normal', LEAP_VECTOR),
    ('direction', LEAP_VECTOR),
    ('sphere_radius', c_float),
  ]

class LEAP_BOUNDS(Structure):
  _fields_ = [
    ('center', LEAP_VECTOR),
    ('size', LEAP_VECTOR),
  ]

class LEAP_FRAME(Structure):
  _fields_ = [
    ('id', c_long),
    ('timestamp', c_long),
    ('hand_count', c_int),
    ('hands', LEAP_HAND * 2),
    ('bounds', LEAP_BOUNDS),
  ]


class LEAP_EVENT(Structure):
  _fields_ = [
    ('event_code', c_int),
    ('frame', LEAP_FRAME)
  ]


class Vector(object):
  def __init__(self, vector):
    self.points = (vector.points[0], vector.points[1], vector.points[2])

  def __str__(self):
    return '<{0} {1:.2f} {2:.2f} {3:.2f}>'.format(self.__class__, *self.points)


class Finger(object):
  def __init__(self, finger):
    self.id_ = finger.id
    self.position = Vector(finger.position)
    self.velocity = Vector(finger.velocity)
    self.direction = Vector(finger.direction)

  def __str__(self):
    return '<{0} {1} position:{2} velocity:{3} direction:{4}>'.format(
      self.__class__,
      self.id_,
      self.position,
      self.velocity,
      self.direction
    )


class Hand(object):
  def __init__(self, hand):
    self.sphere_radius = hand.sphere_radius
    self.palm_position = Vector(hand.palm_position)
    self.palm_normal = Vector(hand.palm_normal)
    self.direction = Vector(hand.direction)
    self.fingers = []
    for i in range(hand.finger_count):
      self.fingers.append(Finger(hand.fingers[i]))

  def __str__(self):
    rtn = (
      '<{0} sphere_radius:{1} palm_position:{2} '
      'palm_normal:{3} direction:{4}'
    ).format(
      self.__class__,
      self.sphere_radius,
      self.palm_position,
      self.palm_normal,
      self.direction
    )
    for f in self.fingers:
     rtn += ' ' + str(f)
    rtn += '.'
    return rtn


class Bounds(object):
  def __init__(self, bounds):
    self.center = Vector(bounds.center)
    self.size = Vector(bounds.size)

  def __str__(self):
    return '<{0} size:{1} center{2}>'.format(
      self.__class__, self.size, self.center
    )


class Frame(object):
  def __init__(self, frame):
    self.id_ = frame.id
    self.timestamp = frame.timestamp
    self.bounds = Bounds(frame.bounds)
    self.hands = []
    for i in range(frame.hand_count):
      self.hands.append(Hand(frame.hands[i]))

  def __str__(self):
    rtn = '<{0} {1} timestamp:{2} bounds:{3}'.format(
      self.__class__,
      self.id_,
      self.timestamp,
      str(self.bounds)
    )
    for h in self.hands:
      rtn += ' ' + str(h)
    rtn += '>'
    return rtn


class Event(object):
  def __init__(self, event):
    event = event.contents
    self.code = event.event_code
    self.frame = Frame(event.frame)

  def __str__(self):
    return '<{0} frame:{1}>'.format(self.code, self.frame)


# Create a controller
leap_controller = LEAP.leap_controller
leap_controller.restype = c_void_p

# Enable tracking when focus is lost on a controller
leap_enable_background = LEAP.leap_enable_background
leap_enable_background.argtypes = [c_void_p]

# Dispose of a controller
leap_controller_dispose = LEAP.leap_controller_dispose
leap_controller_dispose.argtypes = [c_void_p]

# Create a listener with a sized internal buffer.
# At most the most recent size elements will be queued.
leap_listener = LEAP.leap_listener
leap_listener.argtypes = [c_int]
leap_listener.restype = c_void_p

# Dispose of a listener
leap_listener_dispose = LEAP.leap_listener_dispose
leap_listener_dispose.argtypes = [c_void_p]

# Bind a listener to a controller
leap_add_listener = LEAP.leap_add_listener
leap_add_listener.argtypes = [c_void_p, c_void_p]

# Remove a listener from a controller
leap_remove_listener = LEAP.leap_remove_listener
leap_remove_listener.argtypes = [c_void_p, c_void_p]

# Fetch the next queued event
leap_poll_listener = LEAP.leap_poll_listener
leap_poll_listener.argtypes = [c_void_p]
leap_poll_listener.restype = POINTER(LEAP_EVENT)
