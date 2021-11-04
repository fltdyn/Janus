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
// Title:      Janus/DimensionDef
// Class:      DimensionDef
// Module:     DimensionDef.cpp
// First Date: 2010-07-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file DimensionDef.cpp
 *
 * A DimensionDef instance holds in its allocated memory alphanumeric data
 * derived from a \em dimensionDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
 * alphanumeric identification and cross-reference data.
 *
 * The DimensionDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 * Modified :  S. Hill
 */

#include <Ute/aMessageStream.h>

// Local Includes
#include "DomFunctions.h"
#include "DimensionDef.h"

using namespace std;
using namespace dstoute;

namespace janus
{

  DimensionDef::DimensionDef() :
    XmlElementDefinition(),
    isCurrent_( false)
  {
  }

  DimensionDef::DimensionDef(
    const DomFunctions::XmlNode& elementDefinition) :
    XmlElementDefinition(),
    isCurrent_( false)
  {
    initialiseDefinition( elementDefinition);
  }

//------------------------------------------------------------------------//
  void DimensionDef::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "DimensionDef::initialiseDefinition()");

    // Attributes
    dimID_ = DomFunctions::getAttribute( elementDefinition, "dimID");

    // Child Elements
    //   Dimension Record(s)
    dimRecords_.clear();
    try {
      DomFunctions::XmlNodeList dimRecords = DomFunctions::getChildren( elementDefinition, "dim", "dimID");
      for ( size_t i = 0; i < dimRecords.size(); i++) {
        dimRecords_.push_back( DomFunctions::getCData( dimRecords[ i]).toSize_T());
      }
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName) << "\n - Function without \"dim\" elements.\n - "
        << excep.what()
      );
    }

    return;
  }

//------------------------------------------------------------------------//

  void DimensionDef::exportDefinition(
    DomFunctions::XmlNode& documentElement,
    const bool &isReference)
  {
    /*
     * Create a child node in the DOM for the DimensionDef element
     */
    DomFunctions::XmlNode childElement;
    if ( isReference) {
      childElement = DomFunctions::setChild( documentElement, "dimensionRef");
    }
    else {
      childElement = DomFunctions::setChild( documentElement, "dimensionDef");
    }

    /*
     * Add attributes to the DimensionDef child
     */
    if ( !dimID_.empty()) {
      DomFunctions::setAttribute( childElement, "dimID", dimID_);
    }

    /*
     * Add dimension entries
     */
    if ( !isReference) {
      for ( size_t i = 0; i < dimRecords_.size(); i++) {
        DomFunctions::setChild( childElement, "dim", aString( "%").arg( dimRecords_[ i]));
      }
    }
  }

//------------------------------------------------------------------------//

  size_t DimensionDef::getTotal(
    const vector< size_t> &records) const
  {
    size_t total = 1;
    size_t numberOfRecords = records.size();

    for ( size_t index = 0; index < numberOfRecords; index++) {
      total *= records[ index];
    }

    if ( numberOfRecords > 0)
      return total;
    else
      return 0;
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const DimensionDef &dimensionDef)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display DimensionDef contents:" << endl << "-----------------------------------" << endl;

    os << "  dimID              : " << dimensionDef.getDimID() << endl << endl;

    /*
     * Data associated with Class
     */
    size_t dimCount = dimensionDef.getDimCount();
    for ( size_t i = 0; i < dimCount; i++) {
      os << "  dimension " << i << " = " << dimensionDef.getDim( i) << endl;
    }
    os << "  dim total          : " << dimensionDef.getDimTotal() << endl;

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus
