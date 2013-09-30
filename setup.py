#!/usr/bin/env python
from setuptools import setup, find_packages

setup (
  name = "cleap",
  version = "0.1",
  description="ctypes binding for leap motion",
  author="Douglas Linder",
  author_email="", # Removed to limit spam harvesting.
  url="",
  package_dir = {'': 'src'},
  packages = find_packages("src", exclude="tests"),
  zip_safe = True
)
