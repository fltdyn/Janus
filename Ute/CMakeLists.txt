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

SET(UteSrcFiles
	aBiMap.h
	aCrypt.cpp
	aCrypt.h
	aFile.h
	aFileBuffer.h
	aIntegrator.cpp
	aIntegrator.h
	aList.h
	aMap.h
	aMath.h
	aMatrix.h
	aMessageStream.cpp
	aMessageStream.h
	aMutex.h
	aOptional.h
	aRealTimeClock.h
	aString.cpp
	aString.h
	aUnits.cpp
	aUnits.h
	exprtk_matrix.hpp
	UteConfig.h
)

SOURCE_GROUP("Source Files" FILES ${UteSrcFiles})

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-DNOMINMAX)

INCLUDE_DIRECTORIES(..)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/..)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # for UteConfig.h

ADD_LIBRARY(Ute STATIC ${UteSrcFiles})
SET_TARGET_PROPERTIES(Ute PROPERTIES PREFIX "lib")

TARGET_INCLUDE_DIRECTORIES(Ute PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/..")
TARGET_INCLUDE_DIRECTORIES(Ute PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

IF(MSVC)
	# MSVC builds require a ported version of dirent.h
	SET(_dirent_directory "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/dirent")
	TARGET_INCLUDE_DIRECTORIES(Ute PUBLIC "${_dirent_directory}")
	INSTALL(DIRECTORY "${_dirent_directory}/" DESTINATION "include" FILES_MATCHING PATTERN "*.h")

    	SET_SOURCE_FILES_PROPERTIES(aString.cpp PROPERTIES COMPILE_FLAGS /bigobj)
ENDIF()

INSTALL(DIRECTORY . DESTINATION "include/Ute" FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
INSTALL(TARGETS Ute DESTINATION lib)
