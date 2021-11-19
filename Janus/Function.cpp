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
// Title:      Janus/Function
// Class:      Function
// Module:     Function.cpp
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file Function.cpp
 *
 * A Function instance holds in its allocated memory alphanumeric data
 * derived from a \em function element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  Each function has optional
 * description, optional provenance, and either a simple input/output values
 * or references to more complete (possible multiple) input, output, and
 * function data elements.
 *
 * The Function class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 */

// Local Includes
#include "DomFunctions.h"
#include "Function.h"
#include "Janus.h"

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aOptional.h>

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

  Function::Function() :
      XmlElementDefinition(),
      janus_( 0),
      elementType_( ELEMENT_NOTSET),
      hasProvenance_( false),
      isSimpleIO_( false),
      isCompleteIO_( false),
      isAllInterpolationLinear_( true),
      dependentVarRef_( 0)
  {
  }

  Function::Function(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition) :
      XmlElementDefinition(),
      janus_( janus),
      elementType_( ELEMENT_NOTSET),
      hasProvenance_( false),
      isSimpleIO_( false),
      isCompleteIO_( false),
      isAllInterpolationLinear_( true),
      dependentVarRef_( 0)
  {
    initialiseDefinition( janus, elementDefinition);
  }

//------------------------------------------------------------------------//

  void Function::initialiseDefinition(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "Function::initialiseDefinition()");
    janus_ = janus;

    /*
     * Retrieve the element attributes
     */
    try {
      name_ = DomFunctions::getAttribute( elementDefinition, "name", true);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - Function without \"name\" attribute.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve the description associated with the variable
     */
    description_ = DomFunctions::getChildValue( elementDefinition, "description");

    /*
     * Retrieve the optional Provenance associated with the element
     */
    elementType_ = ELEMENT_PROVENANCE;
    try {
      DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING, "provenance", "provenanceRef", "provID",
        false);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
        << excep.what()
      );
    }

    /*
     * Check whether using simple input/output or complete input/output
     */
    isSimpleIO_ = DomFunctions::isChildInNode( elementDefinition, "independentVarPts");
    isCompleteIO_ = DomFunctions::isChildInNode( elementDefinition, "independentVarRef");

    if ( !isSimpleIO_ && !isCompleteIO_) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName)
        << "\n - does not have either simple or complete input/output elements."
      );
    }

    /*
     * Retrieve simple input/output
     */
    if ( isSimpleIO_) {
      /*
       * Retrieve independent variable points data
       */
      elementType_ = ELEMENT_INDEPENDENTVARPTS;
      try {
        DomFunctions::initialiseChildren( this, elementDefinition, name_, "independentVarPts", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }

      /*
       * Retrieve dependent variable points
       */
      elementType_ = ELEMENT_DEPENDENTVARPTS;
      try {
        DomFunctions::initialiseChild( this, elementDefinition, name_, "dependentVarPts", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }
    }

    /*
     * Retrieve complete input/output
     */
    if ( isCompleteIO_) {
      /*
       * Retrieve independent variable references
       */
      independentVarRefList_ = DomFunctions::getChildren( elementDefinition, "independentVarRef");
      elementType_ = ELEMENT_INDEPENDENTVARREF;
      try {
        DomFunctions::initialiseChildrenOrRefs( this, elementDefinition, name_, "variableDef", EMPTY_STRING,
          "independentVarRef", "varID", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }

      /*
       * Retrieve dependent variable reference
       */
      elementType_ = ELEMENT_DEPENDENTVARREF;
      dependentVarRef_ = DomFunctions::getChild( elementDefinition, "dependentVarRef");
      try {
        DomFunctions::initialiseChildOrRef( this, elementDefinition, name_, "variableDef", "dependentVarRef", "varID",
          true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }

      /*
       * Retrieve function definition element
       */
      elementType_ = ELEMENT_FUNCTIONDEFN;
      try {
        DomFunctions::initialiseChild( this, elementDefinition, name_, "functionDefn", true);
      }
      catch ( exception &excep) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
          << excep.what()
        );
      }
    }

    /*
     * Check the number of independent variables defined for the function is
     * compatible with the number of breakpoint definitions within the gridded table.
     */
    if ( getTableType() == ELEMENT_GRIDDEDTABLE) {
      size_t gtRef = getTableRef();

      size_t n = janus_->getGriddedTableDef()[ gtRef].getBreakpointRef().size();
      if ( n != independentVarRefList_.size()) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName)
          << "\n - The number of independent variables ("
          << independentVarRefList_.size()
          << ") in function \n      \""
          << name_ << "\"\n   does not match the number of independent variables in the gridded data table ("
          << n
          << ")."
        );
      }
    }

    /*
     * Check that the number of independent variables defined for the function is
     * compatible with the number of independent variables defined in the ungridded
     * data table.
     */
    if ( getTableType() == ELEMENT_UNGRIDDEDTABLE) {
      size_t gtRef = getTableRef();

      size_t n = janus_->getUngriddedTableDef()[ gtRef].getIndependentVarCount();
      if ( n != independentVarElement_.size()) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName)
          << "\n - The number of independent variables ("
          << independentVarElement_.size()
          << ") in function \n      \""
          << name_ << "\"\n   does not match the number of independent variables in the ungridded data table ("
          << n
          << ")."
        );
      }
    }

    for ( size_t i = 0; i < independentVarElement_.size(); i++) {
      /*
       * Set a single flag if purely discrete, floor or ceiling linear
       * interpolation is to be used.
       * This is used to minimise run time delays (defaults to LINEAR)
       */
      InterpolateMethod enumReference = independentVarElement_[ i].getInterpolationMethod();
      if ( !( ( INTERPOLATE_LINEAR == enumReference) || ( INTERPOLATE_DISCRETE == enumReference)
        || ( INTERPOLATE_FLOOR == enumReference) || ( INTERPOLATE_CEILING == enumReference))) {
        isAllInterpolationLinear_ = false;
      }

      /*
       * Set the cross reference to the variableDef element
       */
      aOptionalSizeT xRef = janus_->crossReferenceId( ELEMENT_VARIABLE, independentVarElement_[ i].getVarID());
      independentVarElement_[ i].setVariableReference( xRef);
    }
  }

//------------------------------------------------------------------------//

  aOptionalSizeT Function::getDependentVarRef() const
  {
    return janus_->crossReferenceId( ELEMENT_VARIABLE, dependentVarElement_.getVarID());
  }

//------------------------------------------------------------------------//

  aOptionalSizeT Function::getIndependentVarRef( const aOptionalSizeT& index) const
  {
    if ( index < getIndependentVarCount()) {
      return janus_->crossReferenceId( ELEMENT_VARIABLE, independentVarElement_[ index].getVarID());
    }
    return aOptionalSizeT();
  }

//------------------------------------------------------------------------//

  aOptionalSizeT Function::getTableRef() const
  {
    if ( !functionDefn_.getTableIndex().isValid()) {
      functionDefn_.setTableIndex(
        janus_->crossReferenceId( functionDefn_.getTableType(), functionDefn_.getTableReference()));
    }
    return functionDefn_.getTableIndex();
  }

//------------------------------------------------------------------------//

  const vector< double>& Function::getData() const
  {
    aOptionalSizeT gtRef = getTableRef();

    switch ( functionDefn_.getTableType()) {
      case ELEMENT_UNGRIDDEDTABLE:
      {
        size_t utDataColumn = functionDefn_.getDependentDataColumnNumber();
        return janus_->getUngriddedTableDef()[ gtRef].getDependentData( utDataColumn);
      }
      break;

    case ELEMENT_GRIDDEDTABLE:
    default:
//    return janus_->getGriddedTableDef()[ gtRef).getData(); // Moved outside to eliminate compile warnings
      break;
    }

    return janus_->getGriddedTableDef()[ gtRef].getData();
  }

//------------------------------------------------------------------------//

  void Function::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the Function element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild( documentElement, "function");

    /*
     * Add attributes to the Function child
     */
    DomFunctions::setAttribute( childElement, "name", name_);

    /*
     * Add the optional description entry to the Function child
     */
    if ( !description_.empty()) {
      DomFunctions::setChild( childElement, "description", description_);
    }

    /*
     * Add the optional provenance entry to the Function child
     */
    if ( hasProvenance_) {
      provenance_.exportDefinition( childElement);
    }

    /*
     * Add the independent variable reference entries to the Function child
     */
    for ( size_t i = 0; i < getIndependentVarCount(); i++) {
      independentVarElement_[ i].exportDefinition( childElement, false);
    }

    /*
     * Add the dependent variable reference entry to the Function child
     */
    dependentVarElement_.exportDefinition( childElement, false);

    /*
     * Add the function definition entry to the Function child
     */
    functionDefn_.exportDefinition( childElement);
  }

  void Function::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    switch ( elementType_) {
    case ELEMENT_PROVENANCE:
      provenance_.initialiseDefinition( xmlElement);
      hasProvenance_ = true;
      break;

    case ELEMENT_INDEPENDENTVARREF:
    case ELEMENT_INDEPENDENTVARPTS:
      independentVarElement_.push_back( InDependentVarDef( xmlElement));

      if ( isSimpleIO_) {
        aString randomString;

        janus_->getBreakpointDef().push_back( BreakpointDef());
        BreakpointDef& breakpointDef = janus_->getBreakpointDef().back();

        breakpointDef.setName( independentVarElement_.back().getName());
        breakpointDef.setBpID( randomString.random()); // Create a random string for this.
        breakpointDef.setUnits( independentVarElement_.back().getUnits());
        breakpointDef.setDescription( name_ + independentVarElement_.back().getName());
        breakpointDef.setBpVals( independentVarElement_.back().getData());

        breakpointRef_.push_back( janus_->getBreakpointDef().size() - 1);
      }
      break;

    case ELEMENT_DEPENDENTVARREF:
    case ELEMENT_DEPENDENTVARPTS:
      dependentVarElement_.initialiseDefinition( xmlElement, false);

      if ( isSimpleIO_) {
        aString randomString;

        janus_->getGriddedTableDef().push_back( GriddedTableDef());
        GriddedTableDef& griddedTableDef = janus_->getGriddedTableDef().back();

        griddedTableDef.setJanus( janus_);
        griddedTableDef.setName( dependentVarElement_.getName());
        griddedTableDef.setGtID( randomString.random()); // Create a random string for this
        griddedTableDef.setUnits( dependentVarElement_.getUnits());
        griddedTableDef.setDescription( name_ + dependentVarElement_.getName());
        griddedTableDef.setBreakpointRefs( breakpointRef_);
        griddedTableDef.setTableData( dependentVarElement_.getData());

        functionDefn_.setTableType( ELEMENT_GRIDDEDTABLE);
        functionDefn_.setTableReference( janus_->getGriddedTableDef().back().getGtID());
      }
      break;

    case ELEMENT_FUNCTIONDEFN:
      functionDefn_.initialiseDefinition( janus_, xmlElement);
      break;

    default:
      break;
    }

    return;
  }

  bool Function::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& /*documentElementReferenceIndex*/)
  {
    switch ( elementType_) {
    case ELEMENT_PROVENANCE:
      if ( DomFunctions::getAttribute( xmlElement, "provID") != elementID) {
        return false;
      }
      readDefinitionFromDom( xmlElement);
      break;

    case ELEMENT_INDEPENDENTVARREF:
    case ELEMENT_DEPENDENTVARREF:
      if ( DomFunctions::getAttribute( xmlElement, "varID") != elementID) {
        return false;
      }

      if ( ELEMENT_INDEPENDENTVARREF == elementType_) {
        readDefinitionFromDom( independentVarRefList_[ independentVarElement_.size()]);
      }
      else {
        readDefinitionFromDom( dependentVarRef_);
      }
      break;

    default:
      return false;
      break;
    }

    return true;
  }

//------------------------------------------------------------------------//

  void Function::resetJanus(
    Janus *janus)
  {
    janus_ = janus;

    /*
     * Reset the Janus pointer in the FunctionDefn class
     */
    functionDefn_.resetJanus( janus);
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const Function &function)
  {
    /*
     * General properties of the Function
     */
    os << endl << endl << "Display Function contents:" << endl << "---------------------------" << endl;

    os << "  name              : " << function.getName() << endl << "  description       : "
      << function.getDescription() << endl << "  hasProvenance     : " << function.hasProvenance() << endl
      << "  functionDefn name : " << function.getDefnName() << endl << "  isAllInterpolationLinear : "
      << function.isAllInterpolationLinear() << endl << endl;

    /*
     * Provenance data for the Function
     */
    if ( function.hasProvenance()) {
      os << function.getProvenance() << endl;
    }

    /*
     * Table properties for the Function
     */
    size_t gtRef = function.getTableRef();
    os << " Table Properties" << "  table Ref: " << gtRef << endl << "  table type: ";

    if ( function.getTableType() == ELEMENT_GRIDDEDTABLE) {
      os << "    Gridded Table" << endl;
      os << function.janus_->getGriddedTableDef()[ gtRef] << endl;
    }
    else {
      os << "    Ungridded Table" << endl;
      os << function.janus_->getUngriddedTableDef()[ gtRef] << endl;
    }

    /*
     * independentVarDefs / independentVarRefs for the Function
     */
    const vector< InDependentVarDef>& independentVars = function.getInDependentVarDef();
    for ( size_t j = 0; j < independentVars.size(); j++) {
      os << "  independent var j: " << j << endl;
      os << independentVars[ j] << endl;
    }

    /*
     * dependentVarDefs / dependentVarRefs for the Function
     */
    os << function.dependentVarElement_ << endl;

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus
