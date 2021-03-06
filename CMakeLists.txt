#
# DST Janus Library (Janus DAVE-ML Interpreter Library)
#
# Defence Science and Technology (DST) Group
# Department of Defence, Australia.
# 506 Lorimer St
# Fishermans Bend, VIC
# AUSTRALIA, 3207
#
# Copyright 2005-2019 Commonwealth of Australia
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this
# software and associated documentation files (the "Software"), to deal in the Software
# without restriction, including without limitation the rights to use, copy, modify,
# merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be included in all copies
# or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
# OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#

PROJECT(Janus)
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

IF( NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS_DEBUG "-g")
SET(CMAKE_DEBUG_POSTFIX "D")

IF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
  SET(CMAKE_CXX_FLAGS "-Wall")
ELSE()
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
  SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
ENDIF()

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReadCFG.cmake)

ADD_SUBDIRECTORY(Ute)
ADD_SUBDIRECTORY(Janus)

OPTION(BUILD_TOOLS "Build tools executables" OFF)
IF(BUILD_TOOLS)
  ADD_SUBDIRECTORY(Tools)
ENDIF()

OPTION(BUILD_EXAMPLES "Build example executables" OFF)
IF(BUILD_EXAMPLES)
  ADD_SUBDIRECTORY(Examples)
ENDIF()
