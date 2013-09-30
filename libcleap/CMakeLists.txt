# Copyright 2013 Douglas Linder
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required (VERSION 2.8)
project (CLEAP)

# Determine leap library
if (APPLE)
  set(LEAP ${CMAKE_CURRENT_SOURCE_DIR}/bin/libLeap.dylib)
endif()

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build c api
file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(cleap SHARED ${SOURCES})
target_link_libraries(cleap ${LEAP})

# Test c api
add_executable(cleap_test ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
target_link_libraries(cleap_test cleap)

# Copy dll so it works
add_custom_command(TARGET cleap_test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LEAP} ${CMAKE_CURRENT_BINARY_DIR})
