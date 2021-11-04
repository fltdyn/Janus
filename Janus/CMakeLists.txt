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

readCfg(${CMAKE_CURRENT_SOURCE_DIR}/../configure "config_name;config_version_hex;config_version_dec")

SET(JANUS_LIBRARY_DESCRIPTION ${config_name})
SET(JANUS_VERSION ${config_version_hex})
SET(JANUS_VERSION_HEX ${config_version_hex})
SET(JANUS_VERSION_SHORT ${config_version_dec})
SET(JANUS_VERSION_LONG "Janus-${config_version_dec}")

SET(ThirdPartyDirectory ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty)

SET(QhullSrcFiles
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/Coordinates.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/Coordinates.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/PointCoordinates.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/PointCoordinates.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/Qhull.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/Qhull.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullFacet.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullFacet.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullFacetList.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullFacetList.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullFacetSet.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullFacetSet.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullHyperplane.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullHyperplane.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullPoint.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullPoint.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullPoints.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullPoints.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullPointSet.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullPointSet.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullQh.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullQh.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullRidge.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullRidge.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullSet.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullSet.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullStat.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullStat.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullVertex.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullVertex.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullVertexSet.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/QhullVertexSet.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/RoadError.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/RoadError.h
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/RoadLogEvent.cpp
	${ThirdPartyDirectory}/qhull/src/libqhullcpp/RoadLogEvent.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/global_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/stat_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/stat_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/geom2_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/poly2_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/merge_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/merge_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/libqhull_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/libqhull_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/geom_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/geom_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/poly_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/poly_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/qset_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/qset_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/mem_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/mem_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/random_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/random_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/usermem_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/io_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/io_r.h
	${ThirdPartyDirectory}/qhull/src/libqhull_r/user_r.c
	${ThirdPartyDirectory}/qhull/src/libqhull_r/user_r.h
)

SET(PugixmlSrcFiles
	${ThirdPartyDirectory}/pugixml/pugixml.cpp
	${ThirdPartyDirectory}/pugixml/pugixml.hpp
)

SET(JanusSrcFiles
	Array.cpp
	Array.h
	Author.cpp
	Author.h
	Bounds.cpp
	Bounds.h
	BreakpointDef.cpp
	BreakpointDef.h
	CheckData.cpp
	CheckData.h
	CheckInputs.h
	CheckOutputs.h
	CheckSignal.cpp
	CheckSignal.h
	Denominator.h
	DimensionDef.cpp
	DimensionDef.h
	DomFunctions.cpp
	DomFunctions.h
	DomTypes.h
	ElementDefinitionEnum.h
	ExportMathML.cpp
	ExportMathML.h
	FileHeader.cpp
	FileHeader.h
	Function.cpp
	Function.h
	FunctionDefn.cpp
	FunctionDefn.h
	GetDescriptors.cpp
	GriddedTableDef.cpp
	GriddedTableDef.h
	InDependentVarDef.cpp
	InDependentVarDef.h
	InternalValues.h
	Janus.cpp
	Janus.h
	JanusConstants.h
	JanusUtilities.h
	JanusVariable.cpp
	JanusVariable.h
	JanusVariableManager.cpp
	JanusVariableManager.h
	LinearInterpolation.cpp
	MathMLDataClass.cpp
	MathMLDataClass.h
	Model.cpp
	Model.h
	Modification.cpp
	Modification.h
	Numerator.h
	ParseMathML.cpp
	ParseMathML.h
	PolyInterpolation.cpp
	PropertyDef.cpp
	PropertyDef.h
	Provenance.cpp
	Provenance.h
	Reference.cpp
	Reference.h
	Signal.cpp
	Signal.h
	SignalDef.cpp
	SignalDef.h
	Signals.cpp
	Signals.h
	SignalList.cpp
	SignalList.h
	SolveMathML.cpp
	SolveMathML.h
	StatespaceFn.cpp
	StatespaceFn.h
	StaticShot.cpp
	StaticShot.h
	TransferFn.cpp
	TransferFn.h
	Uncertainty.cpp
	Uncertainty.h
	UngriddedInterpolation.cpp
	UngriddedTableDef.cpp
	UngriddedTableDef.h
	VariableDef.cpp
	VariableDef.h
	VariableDefExprTkParseMathML.cpp
	VariableDefExprTkScript.cpp
	VariableDefLuaScript.cpp
	XmlElementDefinition.h
	JanusConfig.h
)

SET(SrcFiles 
	${JanusSrcFiles} 
	"${QhullSrcFiles}"
	"${PugixmlSrcFiles}"
)

SOURCE_GROUP("Source Files" FILES ${SrcFiles})

# Needed to be able to access JanusConfig.h
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(Janus STATIC ${SrcFiles})
SET_TARGET_PROPERTIES(Janus PROPERTIES PREFIX "lib")

IF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	SET_TARGET_PROPERTIES(Janus PROPERTIES COMPILE_FLAGS "/bigobj")
ENDIF()

TARGET_INCLUDE_DIRECTORIES(Janus PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
TARGET_INCLUDE_DIRECTORIES(Janus PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
TARGET_INCLUDE_DIRECTORIES(Janus PUBLIC "${ThirdPartyDirectory}/qhull/src")
TARGET_INCLUDE_DIRECTORIES(Janus PUBLIC "${ThirdPartyDirectory}/pugixml")

TARGET_LINK_LIBRARIES(Janus Ute ${OPENSSL_LIBRARIES})

INSTALL(DIRECTORY . DESTINATION "include/Janus/Janus" FILES_MATCHING PATTERN "*.h")
INSTALL(TARGETS Janus DESTINATION lib)
