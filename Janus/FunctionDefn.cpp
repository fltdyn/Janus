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
// Title:      Janus/FunctionDefn
// Class:      FunctionDefn
// Module:     FunctionDefn.cpp
// First Date: 2011-11-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file FunctionDefn.cpp
 *
 * A FunctionDefn instance holds in its allocated memory alphanumeric data
 * derived from a \em functionDefn element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  Each function stores
 * function data elements.

 * The FunctionDefn class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes
#include <stdexcept>
#include <sstream>
#include <iostream>

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aOptional.h>

#include "DomFunctions.h"
#include "FunctionDefn.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

  FunctionDefn::FunctionDefn() :
      XmlElementDefinition(),
      janus_( 0),
      elementType_( ELEMENT_NOTSET),
      tableType_( ELEMENT_NOTSET),
      tableIndex_( aOptionalSizeT::invalidValue()),
      dependentDataColumnNumber_( 0)
  {
  }

  FunctionDefn::FunctionDefn(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition) :
      XmlElementDefinition(),
      janus_( janus),
      elementType_( ELEMENT_NOTSET),
      tableType_( ELEMENT_NOTSET),
      tableIndex_( aOptionalSizeT::invalidValue()),
      dependentDataColumnNumber_( 0)
  {
    initialiseDefinition( janus, elementDefinition);
  }

//------------------------------------------------------------------------//

  void FunctionDefn::initialiseDefinition(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const string functionName( "FunctionDefn::initialiseDefinition()");
    janus_ = janus;

    /*
     * Retrieve the element attributes
     */
    name_ = DomFunctions::getAttribute( elementDefinition, "name");

    /*
     * Check which data table syntax has been used
     */
    bool isGriddedTable = DomFunctions::isChildInNode( elementDefinition, "griddedTable");
    bool isGriddedTableDef = DomFunctions::isChildInNode( elementDefinition, "griddedTableDef");
    bool isGriddedTableRef = DomFunctions::isChildInNode( elementDefinition, "griddedTableRef");
    bool isUngriddedTable = DomFunctions::isChildInNode( elementDefinition, "ungriddedTable");
    bool isUngriddedTableDef = DomFunctions::isChildInNode( elementDefinition, "ungriddedTableDef");
    bool isUngriddedTableRef = DomFunctions::isChildInNode( elementDefinition, "ungriddedTableRef");

    if ( !isGriddedTable && !isGriddedTableDef && !isGriddedTableRef && !isUngriddedTable && !isUngriddedTableDef
      && !isUngriddedTableRef) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "FunctionDef element"
        << "\" does not have a valid data table element."
      );
    }

    /*
     * Retrieve gridded table data - Def, Ref or Table
     */
    elementType_ = ELEMENT_GRIDDEDTABLE;
    if ( isGriddedTableDef || isGriddedTableRef) {
      try {
        DomFunctions::initialiseChildOrRef( this, elementDefinition, name_, "griddedTableDef", "griddedTableRef",
          "gtID", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }
    }
    else if ( isGriddedTable) {
      try {
        DomFunctions::initialiseChild( this, elementDefinition, name_, "griddedTable", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }
    }

    /*
     * Retrieve ungridded table data - Def, Ref or Table
     */
    elementType_ = ELEMENT_UNGRIDDEDTABLE;
    if ( isUngriddedTableDef || isUngriddedTableRef) {
      try {
        DomFunctions::initialiseChildOrRef( this, elementDefinition, name_, "ungriddedTableDef", "ungriddedTableRef",
          "utID", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }

      /*
       * Retrieve and check that the dependentDataColumn is appropriate
       */
      if ( isUngriddedTableRef) {
        aString dependentDataColumnStr = DomFunctions::getAttribute(
          DomFunctions::getChild( elementDefinition, "ungriddedTableRef"), "dependentDataColumn", false);

        if ( !dependentDataColumnStr.empty()) {
          dependentDataColumnNumber_ = dependentDataColumnStr.toSize_T();
        }

        aOptionalSizeT utRef = janus_->crossReferenceId( tableType_, tableReference_);
        size_t n = janus_->getUngriddedTableDef()[ utRef].getIndependentVarCount();
        size_t m = janus_->getUngriddedTableDef()[ utRef].getDataTableColumnCount();

        if ( dependentDataColumnNumber_ >= ( m - n)) {
          throw_message( std::invalid_argument,
            setFunctionName( functionName) << "FunctionDefn element\n"
            << "\" The number of the dependent data column for the ungridded table\n"
            << "\" exceeds the available data range."
          );
        }
      }
    }
    else if ( isUngriddedTable) {
      try {
        DomFunctions::initialiseChild( this, elementDefinition, name_, "ungriddedTable", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }
    }
  }

//------------------------------------------------------------------------//

  void FunctionDefn::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the Function element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild( documentElement, "functionDefn");

    /*
     * Add attributes to the Function child
     */
    DomFunctions::setAttribute( childElement, "name", name_);

    /*
     * Add the reference to the associated gridded or ungridded table
     */
    DomFunctions::XmlNode tableRefElement;
    switch ( tableType_) {
    case ELEMENT_GRIDDEDTABLE:
      tableRefElement = DomFunctions::setChild( childElement, "griddedTableRef");
      DomFunctions::setAttribute( tableRefElement, "gtID", tableReference_);
      break;

    case ELEMENT_UNGRIDDEDTABLE:
      tableRefElement = DomFunctions::setChild( childElement, "ungriddedTableRef");
      DomFunctions::setAttribute( tableRefElement, "utID", tableReference_);
      DomFunctions::setAttribute( tableRefElement, "dependentDataColumn",
        aString( "%").arg( dependentDataColumnNumber_));
      break;

    default:
      return;
      break;
    }
  }

  void FunctionDefn::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    switch ( elementType_) {
    case ELEMENT_GRIDDEDTABLE:
      janus_->getGriddedTableDef().push_back( GriddedTableDef( janus_, xmlElement));
      tableReference_ = janus_->getGriddedTableDef().back().getGtID();
      break;

    case ELEMENT_UNGRIDDEDTABLE:
      janus_->getUngriddedTableDef().push_back( UngriddedTableDef( janus_, xmlElement));
      tableReference_ = janus_->getUngriddedTableDef().back().getUtID();
      break;

    default:
      return;
      break;
    }

    tableType_ = elementType_;
    return;
  }

  bool FunctionDefn::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& /*documentElementReferenceIndex*/)
  {
    aString crossRefElementId;

    switch ( elementType_) {
    case ELEMENT_GRIDDEDTABLE:
      crossRefElementId = DomFunctions::getAttribute( xmlElement, "gtID");
      break;

    case ELEMENT_UNGRIDDEDTABLE:
      crossRefElementId = DomFunctions::getAttribute( xmlElement, "utID", true);
      break;

    default:
      return false;
      break;
    }

    if ( crossRefElementId != elementID) {
      return false;
    }

    tableReference_ = crossRefElementId;
    tableType_ = elementType_;
    return true;
  }

//------------------------------------------------------------------------//

}// namespace janus

