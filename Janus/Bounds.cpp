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
// Title:      Janus/Bounds
// Class:      Bounds
// Module:     Bounds.cpp
// First Date: 2011-12-05
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Bounds.cpp
 *
 * A Bounds instance holds in its allocated memory alphanumeric data
 * derived from a \em bounds element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The element contains some
 * description of the statistical limits to the values the citing parameter
 * element might take on. This can be in the form of a scalar value, a
 * \em variableDef that provides a functional definition of the bound, a
 * \em variableRef that refers to such a functional definition, or a
 * private table whose elements correlate with those of a tabular function
 * defining the citing parameter.  The class also
 * provides the functions that allow a calling Janus instance to access
 * these data elements.
 *
 * The Bounds class is only used within the janus namespace, and should
 * only be referenced indirectly through the Uncertainty class or through
 * the variable functions within the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif

#include <Ute/aMath.h>
#include <Ute/aString.h>
#include <Ute/aMessageStream.h>
#include <Ute/aOptional.h>

#include "Janus.h"
#include "DomFunctions.h"
#include "Bounds.h"
#include "VariableDef.h"
#include "Function.h"

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

  Bounds::Bounds() :
    XmlElementDefinition(),
    janus_( 0),
    elementType_( ELEMENT_VARIABLE),
    isDataTable_( false),
    isVariableDef_( false),
    isVariableRef_( false),
    scalar_( dstomath::nan()),
    varIndex_( aOptionalSizeT::invalidValue()),
    variableDef_( 0)
  {
  }

  Bounds::Bounds(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition) :
    XmlElementDefinition(),
    janus_( janus),
    elementType_( ELEMENT_VARIABLE),
    isDataTable_( false),
    isVariableDef_( false),
    isVariableRef_( false),
    scalar_( dstomath::nan()),
    varIndex_( aOptionalSizeT::invalidValue()),
    variableDef_( 0)
  {
    initialiseDefinition( janus_, elementDefinition);
  }

  Bounds::~Bounds()
  {
    delete variableDef_;
  }

//------------------------------------------------------------------------//

  void Bounds::initialiseDefinition(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "Bounds::initialiseDefinition()");

    janus_ = janus;

    /*
     * Retrieve the normalPDF | uniformPDF
     */
    isDataTable_   = DomFunctions::isChildInNode( elementDefinition, "dataTable");
    isVariableDef_ = DomFunctions::isChildInNode( elementDefinition, "variableDef");
    isVariableRef_ = DomFunctions::isChildInNode( elementDefinition, "variableRef");

    if ( !isDataTable_ && !isVariableDef_ && !isVariableRef_) {
      scalar_ = DomFunctions::getCData( elementDefinition).toDouble();
    }
    else if ( isDataTable_) {
      elementType_ = ELEMENT_DATATABLE;
      try {
        DomFunctions::initialiseChild( this, elementDefinition, EMPTY_STRING,
          "dataTable", false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - Function without \"dataTable\" element.\n - "
          << excep.what()
        );
      }
    }
    else { // variableDef or variableRef
      elementType_ = ELEMENT_VARIABLE;
      try {
        DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING,
          "variableDef", "variableRef", "varID", false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - Function without \"variableDef\" or \"variableRef\" elements.\n - "
          << excep.what()
        );
      }
    }
  }

//------------------------------------------------------------------------//

  double Bounds::getBound( const aOptionalSizeT& functionIndex) const
  {
    double result = dstomath::nan();

    if ( !isDataTable_ && !isVariableDef_ && !isVariableRef_) {
      return scalar_;
    }
    else if ( isVariableDef_) {
      // Manage this as a pointer to a locally allocated
      // memory block of type variableDef.
      // This should avoid the circular declaration compile problem
      return variableDef_->getValue();
    }
    else if ( isVariableRef_) {
      return janus_->getVariableDef( varIndex_).getValue();
    }
    else if ( isDataTable_ && functionIndex.isValid()) {
      Function& thisFunction = janus_->getFunction( functionIndex);

      if ( thisFunction.getTableType() == ELEMENT_GRIDDEDTABLE) {
        // gridded numeric
        if ( thisFunction.isAllInterpolationLinear()) {
          result = janus_->getLinearInterpolation( thisFunction, dataTable_);
        }
        else {
          result = janus_->getPolyInterpolation( thisFunction, dataTable_);
        }
      }
      else {
        // cout << "@TODO review this to support multiple dependent variables" << endl;
        result = janus_->getUngriddedInterpolation( thisFunction, dataTable_);
      }
    }

    return result;
  }

//------------------------------------------------------------------------//
  void Bounds::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the Bound element
     */
    DomFunctions::XmlNode childElement;

    if ( !isDataTable_ && !isVariableDef_ && !isVariableRef_) {
      DomFunctions::setChild( documentElement, "bounds",
        aString( "%").arg( scalar_));
      return;
    }
    else {
      childElement = DomFunctions::setChild( documentElement, "bounds");
    }

    if ( isVariableRef_) {
      DomFunctions::XmlNode variableRefElement = DomFunctions::setChild(
        childElement, "variableRef");
      DomFunctions::setAttribute( variableRefElement, "varID", varID_);
    }
    else if ( isVariableDef_) {
      variableDef_->exportDefinition( childElement);
    }
    else { // dataTable
      aString dataTableStr;

      for ( size_t i = 0; i < dataTable_.size(); i++) {
//      dataTableStr += ( aString("%").arg( dataTable_[i]) + ",");
        dataTableStr += aString( "%").arg( dataTable_[ i]);
        dataTableStr += ",";
      }
      DomFunctions::setChild( childElement, "dataTable", dataTableStr);
    }
  }

  void Bounds::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {

    if ( elementType_ == ELEMENT_DATATABLE) {
      /*
       * Initially read the data table as a string table. It will then
       * be converted to a numeric table.
       */
      aStringList stringData = DomFunctions::getCData( xmlElement).toStringList( JANUS_DELIMITERS);
      for ( size_t i = 0; i < stringData.size(); ++i) {
        aString dataEntry = stringData[ i];

        if ( dataEntry.isNumeric()) {
          dataTable_.push_back( dataEntry.toDouble());
        }
        else if ( !dataEntry.empty()) {
          throw_message( range_error,
            setFunctionName( "Bounds::readDefinitionFromDom()")
            << "Bounds \"" << dataEntry << "\" dataTable entry for bound is not numeric."
          );
        }
      }
    }

    if ( elementType_ == ELEMENT_VARIABLE) {
      /*
       * Read the in-line variableDef and store locally instead of in the global
       * list. This variableDef should NOT include an uncertainty element or
       * reference a gridded or ungridded data table that has an uncertainty
       * element, otherwise this may result in a circular reference.
       */
      // Manage this as a pointer to a locally allocated memory
      // block of type variableDef.
      // This should avoid the circular declaration compile problem
      variableDef_ = new VariableDef( janus_, xmlElement);
    }

    return;
  }

  bool Bounds::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& /*documentElementReferenceIndex*/)
  {
    aString varId = DomFunctions::getAttribute( xmlElement, "varID");
    if ( varId != elementID) {
      return false;
    }

    varIndex_ = janus_->crossReferenceId( ELEMENT_VARIABLE, elementID);
    varID_ = elementID;
    return true;
  }

//------------------------------------------------------------------------//

  void Bounds::resetJanus(
    Janus *janus)
  {
    janus_ = janus;

    /*
     * Reset the Janus pointer in the Bounds class
     */
    if ( variableDef_ != 0) {
      variableDef_->resetJanus( janus);
    }
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const Bounds &bounds)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display Bounds contents:" << endl
       << "-----------------------------------" << endl;

    os << "  isDataTable        : " << bounds.isDataTable_ << endl
       << "  isVariableDef      : " << bounds.isVariableDef_ << endl
       << "  isVariableRef      : " << bounds.isVariableRef_ << endl
       << "  varID              : " << bounds.getVarID() << endl
       << "  bound              : " << bounds.getBound() << endl << endl;

    /*
     * Data associated with Class
     */
    if ( bounds.isDataTable_) {
      for ( size_t i = 0; i < bounds.dataTable_.size(); i++) {
        os << "  dataPoint " << i << " = " << bounds.dataTable_[ i] << endl;
      }
    }

    if ( bounds.isVariableRef_) {
      os << "  varIndex           : " << bounds.varIndex_.value() << endl;
    }

    if ( bounds.isVariableDef_) {
      os << bounds.isVariableDef_ << endl;
    }

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus
