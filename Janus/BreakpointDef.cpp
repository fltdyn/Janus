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
// Title:      Janus/BreakpointDef
// Class:      BreakpointDef
// Module:     BreakpointDef.cpp
// First Date: 2011-12-13
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file BreakpointDef.cpp
 *
 * A BreakpointDef instance holds in its allocated memory alphanumeric data
 * derived from a \em breakpointDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes numeric
 * break points for gridded tables, and associated alphanumeric identification
 * data.
 *
 * A \em breakpointDef is where gridded table breakpoints are defined; that is,
 * a set of independent variable values associated with one dimension of a
 * gridded table of data. An example would be the Mach or angle-of-attack
 * values that define the coordinates of each data point in a
 * two-dimensional coefficient value table. These are separate from function
 * data, and thus they may be reused. The \em independentVarPts element used
 * within some DAVE-ML \em functionDefn elements is equivalent to a
 * \em breakpointDef element, and is also represented as a BreakpointDef
 * within Janus.
 *
 * The BreakpointDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 *
 * Janus exists to handle data for a modelling process.
 * Therefore, in normal computational usage it is unnecessary (and undesirable)
 * for a calling program to be aware of the existence of this class.
 * However, functions do exist to access BreakpointDef contents directly,
 * which may be useful during dataset development.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Ute Includes
#include <Ute/aMessageStream.h>

// Local Includes
#include "JanusConstants.h"
#include "DomFunctions.h"
#include "BreakpointDef.h"

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

  BreakpointDef::BreakpointDef() :
    XmlElementDefinition()
  {
  }

  BreakpointDef::BreakpointDef(
    const DomFunctions::XmlNode& elementDefinition) :
    XmlElementDefinition()
  {
    initialiseDefinition( elementDefinition);
  }

//------------------------------------------------------------------------//

  void BreakpointDef::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "BreakpointDef::initialiseDefinition()");

    /*
     * Retrieve attributes for the element's Definition
     */
    name_  = DomFunctions::getAttribute( elementDefinition, "name");
    units_ = DomFunctions::getAttribute( elementDefinition, "units");
    try {
      bpID_ = DomFunctions::getAttribute( elementDefinition, "bpID", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Function without \"bpID\" attribute.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve the description associated with the element
     */
    description_ = DomFunctions::getChildValue( elementDefinition,
      "description");

    /*
     * Retrieve the breakpoint values associated with the element
     */
    try {
      DomFunctions::initialiseChild( this, elementDefinition, name_,
        "bpVals", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Function without \"bpVals\" elements.\n - "
        << excep.what()
      );
    }
  }

//------------------------------------------------------------------------//
  void BreakpointDef::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the BreakpointDef element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild(
      documentElement, "breakpointDef");

    /*
     * Add attributes to the BreakpointDef child
     */
    if ( !name_.empty()) {
      DomFunctions::setAttribute( childElement, "name", name_);
    }

    DomFunctions::setAttribute( childElement, "bpID", bpID_);

    if ( !units_.empty()) {
      DomFunctions::setAttribute( childElement, "units", units_);
    }

    /*
     * Add description element
     */
    if ( !description_.empty()) {
      DomFunctions::setChild( childElement, "description", description_);
    }

    /*
     * Add breakpoint values
     */
    aString bpValsStr;

    for ( size_t i = 0; i < bpVals_.size(); i++) {
      bpValsStr += aString( "%").arg( bpVals_[ i]);
      bpValsStr += ",";
    }
    DomFunctions::setChild( childElement, "bpVals", bpValsStr);
  }

  void BreakpointDef::readDefinitionFromDom(
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName(
      "BreakpointDef::readDefinitionFromDom()");

    /*
     * Initially read the breakpoint value data as a string table.
     * This is converted to a vector of breakpoint values, which are
     * checked for correctness, i.e. they are numeric entries.
     */
    aStringList breakpointList =
      DomFunctions::getCData( elementDefinition).toStringList( JANUS_DELIMITERS,
        true);
    for ( size_t i = 0; i < breakpointList.size(); ++i) {
      aString dataEntry = breakpointList[ i];
      if ( dataEntry.isNumeric()) {
        double val = dataEntry.toDouble();
        if ( i && val < bpVals_.back()) {
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n Shak says that breakpoint values must increase for \"" << bpID_ << "\". Invalid value " << val << " after " << bpVals_.back() << "."
          );
        }
        bpVals_.push_back( val);
      }
      else if ( !dataEntry.empty()) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n Breakpoint value is non-numeric \"" << dataEntry << "\"."
        );
      }
    }
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const BreakpointDef &breakpointDef)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display BreakpointDef contents:" << endl
      << "-----------------------------------" << endl;

    os << "  name               : " << breakpointDef.getName() << endl
      << "  bpID               : " << breakpointDef.getBpID() << endl
      << "  units              : " << breakpointDef.getUnits() << endl
      << "  description        : " << breakpointDef.getDescription() << endl
      << endl;

    /*
     * Data associated with Class
     */
    const vector< double>& bpVals = breakpointDef.getBpVals();
    for ( size_t i = 0; i < bpVals.size(); i++) {
      os << "  bpVal " << i << " = " << bpVals[ i] << endl;
    }

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus
