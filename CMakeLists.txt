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

cmake_minimum_required(VERSION 3.1)

project(Janus
	VERSION 2.3.0 
	LANGUAGES C CXX
)

set(CMAKE_DEBUG_POSTFIX "D")

find_program(CCACHE ccache)
if (CCACHE)
	set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif()

add_subdirectory(Ute)
add_subdirectory(Janus)

option(BUILD_TOOLS "Build tools executables" OFF)
if (BUILD_TOOLS)
  add_subdirectory(Tools)
endif()

option(BUILD_EXAMPLES "Build example executables" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif()