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

add_library(Ute STATIC 
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

target_compile_features(Ute PRIVATE cxx_std_11)

if (UNIX) 
	target_compile_options(Ute
		PRIVATE 
			$<$<CONFIG:RELEASE>:-O3>
	)
elseif (MSVC)
	target_compile_options(Ute
		PRIVATE 
			$<$<CONFIG:RELEASE>:-O2>
			-D_CRT_SECURE_NO_WARNINGS
			-DNOMINMAX
	)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(Ute PRIVATE -fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(Ute PRIVATE -fcolor-diagnostics)
endif()

target_include_directories(Ute
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
)

if (MSVC)
	target_include_directories(Ute
		PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/dirent>
	)	

	set_target_properties(Ute PROPERTIES COMPILE_FLAGS "/bigobj")
endif()