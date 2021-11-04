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
// Title:      Janus/TransferFn
// Class:      TransferFn
// Module:     TransferFn.cpp
// First Date: 2015-11-18
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file TransferFn.cpp
 *
 * An TransferFn instance holds in its allocated memory alphanumeric data
 * derived from a dynamic systems model \em transferFn element of a DOM
 * corresponding to a DAVE-ML compliant XML dataset source file.
 *
 * It includes entries arranged as follows:
 *  Entries for the numerator and denominator of the transfer function, which
 *  subsequently contain coefficient data for each parameter.
 *
 * The TransferFn class is only used within the janus namespace, and should only
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
#include "TransferFn.h"

using namespace std;
using namespace dstoute;

namespace janus
{

  //------------------------------------------------------------------------//

  TransferFn::TransferFn() :
    XmlElementDefinition(),
    elementType_( ELEMENT_NOTSET),
    order_( 0),
    isProvenanceRef_( false),
    hasProvenance_( false)
  {
  }

  TransferFn::TransferFn(
    const DomFunctions::XmlNode& elementDefinition) :
      XmlElementDefinition(),
      elementType_( ELEMENT_NOTSET),
      order_( 0),
      isProvenanceRef_( false),
      hasProvenance_( false)
  {
    initialiseDefinition( elementDefinition);
  }

  //------------------------------------------------------------------------//

  void TransferFn::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "TransferFn::initialiseDefinition()");

    /*
     * Retrieve attributes for the element's Definition
     */
    name_ = DomFunctions::getAttribute( elementDefinition, "name");
    tfID_ = DomFunctions::getAttribute( elementDefinition, "tfID");

    aString tempString;
    tempString = DomFunctions::getAttribute( elementDefinition, "order");
    if ( tempString.isNumeric()) {
      order_ = tempString.toSize_T();
    }

    /*
     * Retrieve the description associated with the element
     */
    description_ = DomFunctions::getChildValue( elementDefinition,
                                                 "description");

    /*
     * Retrieve numerator and denominator elements
     */
    elementType_ = ELEMENT_NUMERATOR;
    try {
      DomFunctions::initialiseChild(
        this,
        elementDefinition,
        tfID_,
        "numerator",
        true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - "
        << excep.what()
      );
    }

    elementType_ = ELEMENT_DENOMINATOR;
    try {
      DomFunctions::initialiseChild(
        this,
        elementDefinition,
        tfID_,
        "denominator",
        true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - "
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
        setFunctionName( functionName) << "\n - for transerFn \"" << name_ << "\"\n - "
        << excep.what()
      );
    }
  }

  //------------------------------------------------------------------------//

  void TransferFn::exportDefinition(
    DomFunctions::XmlNode& documentElement,
    const bool &isReference)
  {
    /*
     * Create a child node in the DOM for the TransferFn element
     */
    DomFunctions::XmlNode childElement;
    if ( isReference) {
      childElement = DomFunctions::setChild( documentElement, "transferFnRef");
    }
    else {
      childElement = DomFunctions::setChild( documentElement, "transferFn");
    }

    /*
     * Add attributes to the TransferFn child
     */
    if ( !tfID_.empty()) {
      DomFunctions::setAttribute( childElement, "tfID", tfID_);
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

      DomFunctions::setAttribute( childElement, "order", aString("%").arg( order_));

      /*
       * Add description element
       */
      if ( !description_.empty()) {
        DomFunctions::setChild( childElement, "description", description_);
      }

      /*
       * Add Numerator and Denominator elements
       */
      numerator_.exportDefinition( childElement, "numerator");
      denominator_.exportDefinition( childElement, "denominator");

      /*
       * Add the optional provenance entry to the GriddedTableDef child
       */
      if ( hasProvenance_) {
        provenance_.exportDefinition( childElement, isProvenanceRef_);
      }
    }
  }

  void TransferFn::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    static const aString functionName(
      "TransferFn::readDefinitionFromDom()");

    switch ( elementType_) {
      case ELEMENT_NUMERATOR:
        numerator_.initialiseDefinition( xmlElement);
        break;

      case ELEMENT_DENOMINATOR:
        denominator_.initialiseDefinition( xmlElement);
        break;

      case ELEMENT_PROVENANCE:
        provenance_.initialiseDefinition( xmlElement);
        hasProvenance_ = true;
        break;

      default:
        break;
    }

    return;
  }

  bool TransferFn::compareElementID(
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
    const TransferFn &transferFn)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display TransferFn contents:" << endl << "-----------------------------------" << endl;

    os << "  name               : " << transferFn.getName() << endl
       << "  tfID               : " << transferFn.getTFID() << endl
       << "  order              : " << transferFn.getOrder() << endl
       << "  description        : " << transferFn.getDescription() << endl
       << endl;

    /*
     * Numerator and Denominator data for the Class
     */
    os << "Numerator" << endl;
    os << transferFn.getNumerator() << endl << endl;

    os << "Denominator" << endl;
    os << transferFn.getDenominator() << endl;

    /*
     * Provenance data for the Class
     */
    if ( transferFn.hasProvenance()) {
      os << transferFn.getProvenance() << endl;
    }

    return os;
  }

} // namespace janus
