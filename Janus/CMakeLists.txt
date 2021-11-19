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

add_library(qhull STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/Coordinates.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/PointCoordinates.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/Qhull.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullFacet.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullFacetList.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullFacetSet.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullHyperplane.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullPoint.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullPoints.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullPointSet.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullQh.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullRidge.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullSet.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullStat.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullVertex.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/QhullVertexSet.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/RoadError.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhullcpp/RoadLogEvent.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/global_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/stat_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/geom2_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/poly2_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/merge_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/libqhull_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/geom_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/poly_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/qset_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/mem_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/random_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/usermem_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/io_r.c
 	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src/libqhull_r/user_r.c
)

target_include_directories(qhull SYSTEM # suppress warnings
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/qhull/src>
)

target_compile_features(qhull PRIVATE cxx_std_11)

add_library(pugixml STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/pugixml/pugixml.cpp
)

target_include_directories(pugixml SYSTEM # suppress warnings
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/pugixml>
)

target_compile_features(pugixml PRIVATE cxx_std_11)

add_library(Janus STATIC 
 	${CMAKE_CURRENT_SOURCE_DIR}/Array.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Author.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Bounds.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/BreakpointDef.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/CheckData.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/CheckSignal.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/DimensionDef.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/DomFunctions.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/ExportMathML.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/FileHeader.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Function.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/FunctionDefn.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/GetDescriptors.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/GriddedTableDef.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/InDependentVarDef.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Janus.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/JanusVariable.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/JanusVariableManager.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/LinearInterpolation.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/MathMLDataClass.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Model.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Modification.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/ParseMathML.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/PolyInterpolation.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/PropertyDef.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Provenance.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Reference.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Signal.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/SignalDef.cpp	
 	${CMAKE_CURRENT_SOURCE_DIR}/Signals.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/SignalList.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/SolveMathML.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/StatespaceFn.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/StaticShot.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/TransferFn.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/Uncertainty.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/UngriddedInterpolation.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/UngriddedTableDef.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/VariableDef.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/VariableDefExprTkParseMathML.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/VariableDefExprTkScript.cpp
 	${CMAKE_CURRENT_SOURCE_DIR}/VariableDefLuaScript.cpp
)

target_include_directories(Janus 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_features(Janus PRIVATE cxx_std_11)
   

if (UNIX) 
	target_compile_options(Janus
		PRIVATE 
			$<$<CONFIG:RELEASE>:-O3>
	)
elseif (MSVC)
	target_compile_options(Janus
		PRIVATE 
			$<$<CONFIG:RELEASE>:-O2>
			-D_CRT_SECURE_NO_WARNINGS
			-DNOMINMAX
	)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(Janus PRIVATE -fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(Janus PRIVATE -fcolor-diagnostics)
endif()

if (MSVC)
    set_target_properties(Janus PROPERTIES COMPILE_FLAGS "/bigobj")
endif()

target_link_libraries(Janus 
    PUBLIC 
        Ute
        qhull
        pugixml 
)