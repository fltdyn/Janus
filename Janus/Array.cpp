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
// Title:      Janus/Array
// Class:      Array
// Module:     Array.h
// First Date: 2009-07-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Array.cpp
 *
 * An Array instance holds in its allocated memory alphanumeric data
 * derived from an \em array element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.
 *
 * It includes entries arranged as follows:
 *  Entries for a vector represent the row entries of that vector.
 *  Entries for a matrix are specified such that the column entries
 *  of the first row are listed followed by column entries for subsequent rows
 *  until the base matrix is complete. This sequence is repeated for higher order
 *  matrix dimensions until all entries of the matrix are specified.
 *
 * The Array class is only used within the janus namespace, and should only
 * be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 * Modified :  S. Hill
 *
 */

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aMessageStream.h>

// Local Includes
#include "DomFunctions.h"
#include "JanusConstants.h"
#include "Array.h"

using namespace std;
using namespace dstoute;

namespace janus
{

  //------------------------------------------------------------------------//

  Array::Array() :
    XmlElementDefinition()
  {
  }

  Array::Array(
    const DomFunctions::XmlNode& arrayElement) :
    XmlElementDefinition()
  {
    initialiseDefinition( arrayElement);
  }

  //------------------------------------------------------------------------//

  void Array::initialiseDefinition(
    const DomFunctions::XmlNode& arrayElement)
  {
    static const aString functionName( "Array::initialiseDefinition()");

    /*
     * uncertainty may be specified by a dataTable within this griddedTable
     * so can't just look for elements by tag name.  Have to look for
     * child dataTable elements only.
     */

    try {
      DomFunctions::initialiseChild( this, arrayElement, EMPTY_STRING, "dataTable", false);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName) << "\n - Function without \"dataTable\" attribute.\n - "
        << excep.what()
      );
    }
  }

  //------------------------------------------------------------------------//

  void Array::exportDefinition(
    DomFunctions::XmlNode& documentElement,
    const aString& elementTag)
  {
    /*
     * Create a child node in the DOM for the Array element
     */
    DomFunctions::XmlNode childElement;
    if ( elementTag.empty()) {
      childElement = DomFunctions::setChild( documentElement, "array");
    }
    else {
      childElement = DomFunctions::setChild( documentElement, elementTag);
    }

    /*
     * Add data table element
     */
    aString dataTableStr;
    for ( size_t i = 0; i < stringTableData_.size(); i++) {
      dataTableStr += stringTableData_[ i];
      dataTableStr += ",";
    }
    DomFunctions::setChild( childElement, "dataTable", dataTableStr);
  }

  void Array::readDefinitionFromDom(
    const DomFunctions::XmlNode& elementDefinition)
  {
    /*
     * Initially read the data table as a string table.
     * This will be converted to a numeric table after its size has been
     * checked for correctness and non-numeric entries have been evaluated.
     */
    stringTableData_ = DomFunctions::getCData( elementDefinition).toStringList( JANUS_DELIMITERS);

    return;
  }

  //------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const Array &array)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display Array contents:" << endl << "-----------------------------------" << endl;

    /*
     * Data associated with Class
     */
    const aStringList& stringDataTable = array.getStringDataTable();
    for ( size_t i = 0; i < stringDataTable.size(); i++) {
      os << "  dataPoint " << i << " = " << stringDataTable[ i] << endl;
    }

    return os;
  }

} // namespace janus
