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
// Title:      Janus/StatespaceFn
// Class:      StatespaceFn
// Module:     StatespaceFn.cpp
// First Date: 2015-11-18
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file StatespaceFn.cpp
 *
 * An StatespaceFn instance holds in its allocated memory alphanumeric data
 * derived from a dynamic systems model \em statespaceFn element of a DOM
 * corresponding to a DAVE-ML compliant XML dataset source file.
 *
 * It includes entries arranged as follows:
 * ...
 *
 * The StatespaceFn class is only used within the janus namespace, and should only
 * be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 */

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aMessageStream.h>

// Local Includes
#include "DomFunctions.h"
#include "JanusConstants.h"
#include "StatespaceFn.h"

using namespace std;
using namespace dstoute;

namespace janus
{

  //------------------------------------------------------------------------//

  StatespaceFn::StatespaceFn() :
    XmlElementDefinition(),
    elementType_( ELEMENT_NOTSET),
    isProvenanceRef_( false),
    hasProvenance_( false)
  {
  }

  StatespaceFn::StatespaceFn(
    const DomFunctions::XmlNode& elementDefinition) :
      XmlElementDefinition(),
      elementType_( ELEMENT_NOTSET),
      isProvenanceRef_( false),
      hasProvenance_( false)
  {
    initialiseDefinition( elementDefinition);
  }

  //------------------------------------------------------------------------//

  void StatespaceFn::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "StatespaceFn::initialiseDefinition()");

    /*
     * Retrieve attributes for the element's Definition
     */
    name_ = DomFunctions::getAttribute( elementDefinition, "name");
    ssID_ = DomFunctions::getAttribute( elementDefinition, "ssID");

    /*
     * Retrieve the description associated with the element
     */
    description_ = DomFunctions::getChildValue( elementDefinition,
                                                 "description");

    /*
     * Retrieve to state derivative equation and output equation matrices
     */
    elementType_ = ELEMENT_VARIABLE;
    try {
      DomFunctions::initialiseChildrenOrRefs( this,
                                              elementDefinition,
                                              ssID_,
                                              "variableDef",
                                              EMPTY_STRING,
                                              "variableRef",
                                              "varID",
                                              true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Error initialising \"variableRef\" elements.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve the optional Provenance associated with the element
     */
    elementType_ = ELEMENT_PROVENANCE;
    try {
      DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING,
        "provenance", "provenanceRef", "provID", false);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName) << "\n - for statespaceFn \"" << name_ << "\"\n - "
        << excep.what()
      );
    }
  }

  //------------------------------------------------------------------------//

  void StatespaceFn::exportDefinition(
    DomFunctions::XmlNode& documentElement,
    const bool &isReference)
  {
    /*
     * Create a child node in the DOM for the StatespaceFn element
     */
    DomFunctions::XmlNode childElement;
    if ( isReference) {
      childElement = DomFunctions::setChild( documentElement, "statespaceFnRef");
    }
    else {
      childElement = DomFunctions::setChild( documentElement, "statespaceFn");
    }

    /*
     * Add attributes to the StatespaceFn child
     */
    if ( !ssID_.empty()) {
      DomFunctions::setAttribute( childElement, "ssID", ssID_);
    }

    /*
     * Add entries if not a reference
     */
    if ( !isReference) {
      /*
       * Add attributes to the TransferFn child
       */
      if ( !name_.empty()) {
        DomFunctions::setAttribute( childElement, "name", name_);
      }

      /*
       * Add description element
       */
      if ( !description_.empty()) {
        DomFunctions::setChild( childElement, "description", description_);
      }

      /*
       * Add the references to the state derivative equation and output equation
       * coefficient matrices
       */
      DomFunctions::XmlNode variableRefElement;
      for ( size_t i = 0; i < statespaceVarRefs_.size(); i++) {
        if ( !statespaceVarRefs_[i].empty()) {
          variableRefElement = DomFunctions::setChild( childElement, "variableRef");
          DomFunctions::setAttribute( variableRefElement, "varID",
                                      statespaceVarRefs_[ i]);
        }
      }

      /*
       * Add the optional provenance entry to the GriddedTableDef child
       */
      if ( hasProvenance_) {
        provenance_.exportDefinition( childElement, isProvenanceRef_);
      }
    }
  }

  void StatespaceFn::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    static const aString functionName(
      "StatespaceFn::readDefinitionFromDom()");

    switch ( elementType_) {
      case ELEMENT_PROVENANCE:
        provenance_.initialiseDefinition( xmlElement);
        hasProvenance_ = true;
        break;

      default:
        break;
    }

    return;
  }

  bool StatespaceFn::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& /* documentElementReferenceIndex */)
  {
    switch ( elementType_) {
    case ELEMENT_PROVENANCE:
      if ( DomFunctions::getAttribute( xmlElement, "provID") != elementID) {
        return false;
      }
      isProvenanceRef_ = true;
      break;

    case ELEMENT_VARIABLE:
      if ( DomFunctions::getAttribute( xmlElement, "varID") != elementID) {
        return false;
      }
      statespaceVarRefs_.push_back( elementID);
      break;

    default:
      return false;
      break;
    }

    readDefinitionFromDom( xmlElement);

    return true;
  }

  //------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const StatespaceFn &statespaceFn)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display StatespaceFn contents:" << endl << "-----------------------------------" << endl;

    os << "  name               : " << statespaceFn.getName() << endl
       << "  ssID               : " << statespaceFn.getSSID() << endl
       << "  description        : " << statespaceFn.getDescription() << endl
       << "  state vector      (varID) : " << statespaceFn.getStateVectorID() << endl
       << "  stateDeriv vector (varID) : " << statespaceFn.getStateDerivVectorID() << endl
       << "  state MatrixID    (varID) : " << statespaceFn.getStateMatrixID() << endl
       << "  input MatrixID    (varID) : " << statespaceFn.getInputMatrixID() << endl
       << "  output MatrixID   (varID) : " << statespaceFn.getOutputMatrixID() << endl
       << "  direct MatrixID   (varID) : " << statespaceFn.getDirectMatrixID() << endl
       << "  disturbance vector (varID)     : " << statespaceFn.getDisturbanceVectorID() << endl
       << "  SD disturbanceF Matrix (varID) : " << statespaceFn.getStateDerivDisturbanceMatrixID() << endl
       << "  Output disturbanceH Matrix (varID) : " << statespaceFn.getOutputDisturbanceMatrixID() << endl
       << endl;

    /*
     * Provenance data for the Class
     */
    if ( statespaceFn.hasProvenance()) {
      os << statespaceFn.getProvenance() << endl;
    }

    return os;
  }

} // namespace janus
