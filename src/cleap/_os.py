# Copyright 2013 Douglas Linder
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import absolute_import
from .enum import enum
import platform as _platform


# OS constants
platforms = enum(
  WINDOWS="windows",
  CYGWIN="cygwin",
  LINUX="linux",
  MAC="darwin",
)


class UnknownOsError(Exception):
  pass


def platform():
  """ Best guess for simple OS type resolution """
  name = str(_platform.system()).strip().lower()
  if platforms.WINDOWS in name or platforms.CYGWIN in name:
    return platforms.WINDOWS
  elif platforms.LINUX in name:
    return platforms.LINUX
  elif platforms.MAC in name:
    return platforms.MAC
  else:
    raise UnknownOsError("Unknown OS type '%s'" % name)
