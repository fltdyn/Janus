//
// DST Janus Library (Janus DAVE-ML Interpreter Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2021 Commonwealth of Australia
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

//------------------------------------------------------------------------//
// Title:      Janus/InDependentVarDef
// Class:      InDependentVarDef
// Module:     InDependentVarDef.cpp
// First Date: 2011-11-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file InDependentVarDef.cpp
 *
 * This code is used during initialisation of the Janus class, and
 * provides access to the In-Dependent variable definitions contained in a DOM that
 * complies with the DAVE-ML DTD.
 *
 * A breakpointDef is where gridded table breakpoints are given.
 * Since these are separate from function data, they may be reused.
 *
 * bpVals is a set of breakpoints; that is, a set of independent
 * variable values associated with one dimension of a gridded table
 * of data. An example would be the Mach or angle-of-attack values
 * that define the coordinates of each data point in a
 * two-dimensional coefficient value table.
 */

/*
 * Author:  D. M. Newman,  Quantitative Aeronautics
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Local Includes
#include "JanusConstants.h"
#include "DomFunctions.h"
#include "InDependentVarDef.h"

// C++ Includes
#include <cfloat>

// Ute Includes
#include <Ute/aBiMap.h>
#include <Ute/aMessageStream.h>
#include <Ute/aMath.h>
#include <Ute/aOptional.h>

using namespace std;
using namespace dstoute;
using namespace dstomath;

namespace janus {

typedef aBiMap< aString, int > EnumStringBiMap;
const EnumStringBiMap extrapolateAttributes = ( EnumStringBiMap()
  << EnumStringBiMap::pair( "neither", EXTRAPOLATE_NEITHER)
  << EnumStringBiMap::pair( "min",     EXTRAPOLATE_MINEX)
  << EnumStringBiMap::pair( "max",     EXTRAPOLATE_MAXEX)
  << EnumStringBiMap::pair( "both",    EXTRAPOLATE_BOTH)
);

const EnumStringBiMap interpolateAttributes = ( EnumStringBiMap()
  << EnumStringBiMap::pair( "discrete",        INTERPOLATE_DISCRETE)
  << EnumStringBiMap::pair( "floor",           INTERPOLATE_FLOOR)
  << EnumStringBiMap::pair( "ceiling",         INTERPOLATE_CEILING)
  << EnumStringBiMap::pair( "linear",          INTERPOLATE_LINEAR)
  << EnumStringBiMap::pair( "quadraticSpline", INTERPOLATE_QSPLINE)
  << EnumStringBiMap::pair( "cubicSpline",     INTERPOLATE_CSPLINE)
);

//------------------------------------------------------------------------//

InDependentVarDef::InDependentVarDef( )
 : XmlElementDefinition(),
   isIndependentVarDef_(true),
   extrapolateMethod_( EXTRAPOLATE_NEITHER),
   interpolateMethod_( INTERPOLATE_LINEAR),
   min_( -DBL_MAX),
   max_( DBL_MAX),
   varRef_( aOptionalSizeT::invalidValue())
{
}

InDependentVarDef::InDependentVarDef( const DomFunctions::XmlNode& elementDefinition )
 : XmlElementDefinition(),
   isIndependentVarDef_(true),
   extrapolateMethod_( EXTRAPOLATE_NEITHER),
   interpolateMethod_( INTERPOLATE_LINEAR),
   min_( -DBL_MAX),
   max_( DBL_MAX),
   varRef_( aOptionalSizeT::invalidValue())
{
  initialiseDefinition( elementDefinition );
}

//------------------------------------------------------------------------//

void InDependentVarDef::initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
                                              const bool& isIndependentVarDef )
{
  static const aString functionName( "InDependentVarDef::initialiseDefinition()");
  aString dataEntry;

  isIndependentVarDef_ = isIndependentVarDef;

  /*
   * Retrieve attributes for the element's Definition
   */
  try {
    varID_ = DomFunctions::getAttribute( elementDefinition, "varID", true);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Function without \"varID\" attribute.\n - "
      << excep.what()
    );
  }

  name_  = DomFunctions::getAttribute( elementDefinition, "name");
  units_ = DomFunctions::getAttribute( elementDefinition, "units");
  sign_  = DomFunctions::getAttribute( elementDefinition, "sign");

  dataEntry = DomFunctions::getAttribute( elementDefinition, "min");
  if ( !dataEntry.empty()) {
    if ( dataEntry.isNumeric()) {
      min_ = dataEntry.toDouble();
    }
    else {
      throw_message( std::invalid_argument,
        setFunctionName( functionName)
        << "\n - Invalid \"min\" value for independentVarRef \"" << varID_ << "\"."
      );
    }
  }

  dataEntry = DomFunctions::getAttribute( elementDefinition, "max");
  if ( !dataEntry.empty()) {
    if ( dataEntry.isNumeric()) {
      max_ = dataEntry.toDouble();
    }
    else {
      throw_message( std::invalid_argument,
        setFunctionName( functionName)
        << "\n - Invalid \"max\" value for independentVarRef \"" << varID_ << "."
      );
    }
  }

  /*
   * Retrieve the Extrapolate attribute
   */
  aString attributeStr = DomFunctions::getAttribute( elementDefinition, "extrapolate");
  if ( attributeStr.empty()) {
    attributeStr = extrapolateAttributes[ EXTRAPOLATE_NEITHER];
  }
  if ( extrapolateAttributes.find( attributeStr) == extrapolateAttributes.end()) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - invalid \"extrapolate\" attribute \"" << attributeStr << "\"."
    );
  }
  extrapolateMethod_ = ExtrapolateMethod( extrapolateAttributes[ attributeStr]);

  /*
   * Check for DSTO legacy Interpolate syntax
   */
  attributeStr = DomFunctions::getAttribute( elementDefinition, "interpolationType");
  if ( !attributeStr.empty()) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << " - \"interpolationType\" attribute no longer supported.\n"
      << "   Please update file to comply with DAVE-ML V2.0 RC3.0 + syntax."
    );
  }

  /*
   * Retrieve the Interpolate attribute
   */
  attributeStr = DomFunctions::getAttribute( elementDefinition, "interpolate");
  if ( attributeStr.empty()) {
    attributeStr = interpolateAttributes[ INTERPOLATE_LINEAR];
  }
  if ( interpolateAttributes.find( attributeStr) == interpolateAttributes.end()) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - invalid \"interpolate\" attribute \"" << attributeStr << "\"."
    );
  }
  interpolateMethod_ = InterpolateMethod( interpolateAttributes[ attributeStr]);

  /*
   * Retrieve the data values associated with the element
   */
  try {
    readDefinitionFromDom( elementDefinition);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for independentVarRef \"" << varID_ << "\"\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//
void InDependentVarDef::exportDefinition( DomFunctions::XmlNode& documentElement,
                                          const bool& asPts)
{
  /*
   * Create a child node in the DOM for the InDependentVarDef element
   */
  aString dataPointStr;
  if (asPts) {
    for ( size_t i = 0; i < dataPoint_.size(); i++) {
      dataPointStr += aString("%").arg( dataPoint_[i]);
      dataPointStr += ",";
    }
  }

  DomFunctions::XmlNode childElement;
  if ( isIndependentVarDef_) {
    if ( asPts) {
      childElement = DomFunctions::setChild( documentElement,
                                             "independentVarPts", dataPointStr);
    }
    else {
      childElement = DomFunctions::setChild( documentElement, "independentVarRef");
    }
  }
  else {
    if ( asPts) {
      childElement = DomFunctions::setChild( documentElement,
                                             "dependentVarPts", dataPointStr);
    }
    else {
      childElement = DomFunctions::setChild( documentElement, "dependentVarRef");
    }
  }

  /*
   * Add varID attribute to the child
   */
  DomFunctions::setAttribute( childElement, "varID", varID_);

  /*
   * If an independentVarRef, independentVarPts, or dependentVarPts element then
   * additional attributes are to be added to the child
   */
  if ( !isIndependentVarDef_ && !asPts) {
    return;
  }
  else if ( asPts) { // independentVarPts or dependentVarPts
    DomFunctions::setAttribute( childElement, "name",  name_);
    DomFunctions::setAttribute( childElement, "units", units_);
    DomFunctions::setAttribute( childElement, "sign", sign_);
  }
  else { // independentVarRef
    if ( !dstomath::isnan( max_)) {
      DomFunctions::setAttribute( childElement, "max",  aString("%").arg( max_));
    }
    if ( !dstomath::isnan( min_)) {
      DomFunctions::setAttribute( childElement, "min",  aString("%").arg( min_));
    }
  }

  if ( isIndependentVarDef_) {
    DomFunctions::setAttribute( childElement, "extrapolate",
                                extrapolateAttributes[ extrapolateMethod_]);
    DomFunctions::setAttribute( childElement, "interpolate",
                                interpolateAttributes[interpolateMethod_]);
  }
}

void InDependentVarDef::readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition)
{
  /*
   * Initially read the dependent and independent value data as a string table.
   * This is converted to a vector of values, which are
   * checked for correctness, i.e. they are numeric entries.
   */
  aStringList dataList = DomFunctions::getCData( elementDefinition).toStringList( JANUS_DELIMITERS);
  for ( size_t i = 0; i < dataList.size(); ++i) {
    aString dataEntry = dataList[i];
    if ( dataEntry.isNumeric()) {
      dataPoint_.push_back( dataEntry.toDouble());
    }
    else if ( !dataEntry.empty()) {
      throw_message( std::invalid_argument,
        setFunctionName( "InDependentVarDef::readDefinitionFromDom()")
        << " Error: In-Dependent data value is non-numeric \"" << dataEntry << "\"."
      );
    }
  }
}

//------------------------------------------------------------------------//

ostream& operator<< ( ostream &os,
                      const InDependentVarDef &inDependentVarDef)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display InDependentVarDef contents:" << endl
     << "-----------------------------------" << endl;

  os << "  name               : " << inDependentVarDef.getName() << endl
     << "  varId              : " << inDependentVarDef.getVarID() << endl
     << "  units              : " << inDependentVarDef.getUnits() << endl
     << "  sign               : " << inDependentVarDef.getSign() << endl
     << "  extrapolate enum   : " << inDependentVarDef.getExtrapolationMethod() << endl
     << "  interpolate enum   : " << inDependentVarDef.getInterpolationMethod() << endl
     << "  min                : " << inDependentVarDef.getMin() << endl
     << "  max                : " << inDependentVarDef.getMax() << endl
     << "  variable reference : " << inDependentVarDef.getVariableReference() << endl
     << endl;

  /*
   * Data associated with Class
   */
  const vector<double>& dataPoint = inDependentVarDef.getData();
  for ( size_t i = 0; i < dataPoint.size(); i++) {
    os << "  dataPoint " << i << " = " << dataPoint[i] << endl;
  }

  return os;
}

//------------------------------------------------------------------------//

} // End namespace janus

