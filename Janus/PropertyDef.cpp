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
// Title:      Janus/PropertyDef
// Class:      PropertyDef
// Module:     PropertyDef.cpp
// First Date: 2015-11-17
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file PropertyDef.h
 *
 * A PropertyDef instance holds in its allocated memory data
 * derived from a \em propertyDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
 * alphanumeric identification and cross-reference data.
 * This class sets up a structure that manages the \em propertyDef content.
 *
 * The PropertyDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  G Brian
 *
 */

// Ute Includes
#include <Ute/aMessageStream.h>

// Local Includes
#include "DomFunctions.h"
#include "PropertyDef.h"

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

PropertyDef::PropertyDef() :
  XmlElementDefinition(),
  janus_( 0),
  elementType_( ELEMENT_PROPERTY),
  isProvenanceRef_( false),
  hasProvenance_( false)
{
}

PropertyDef::PropertyDef(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
  :
  XmlElementDefinition(),
  janus_( janus),
  elementType_( ELEMENT_PROPERTY),
  isProvenanceRef_( false),
  hasProvenance_( false)
{
  initialiseDefinition( janus, elementDefinition);
}

//------------------------------------------------------------------------//

void PropertyDef::initialiseDefinition(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
{
  static const aString functionName( "PropertyDef::initialiseDefinition()");

  janus_ = janus;

  /*
   * Retrieve attributes for the element's Definition
   */
  name_  = DomFunctions::getAttribute( elementDefinition, "name");
  ptyID_ = DomFunctions::getAttribute( elementDefinition, "ptyID");
  refID_ = DomFunctions::getAttribute( elementDefinition, "refID");


  /*
   * Retrieve the description associated with the element
   */
  description_ = DomFunctions::getChildValue( elementDefinition, "description");

  /*
   * Retrieve the property entry for the element
   */
  try {
    propertyList_ = DomFunctions::getChildrenValues( elementDefinition, "property", "propertyDef", true);
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
    DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING, "provenance", "provenanceRef",
      "provID", false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName) << "\n - for propertyDef \"" << name_ << "\"\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//
void PropertyDef::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the PropertyDef element
   */
  DomFunctions::XmlNode childElement = DomFunctions::setChild(
    documentElement, "propertyDef");

  /*
   * Add attributes to the BreakpointDef child
   */
  if ( !name_.empty()) {
    DomFunctions::setAttribute( childElement, "name", name_);
  }

  DomFunctions::setAttribute( childElement, "ptyID", ptyID_);

  /*
   * Add description element
   */
  if ( !description_.empty()) {
    DomFunctions::setChild( childElement, "description", description_);
  }

  /*
   * Add property element
   */
  if ( !propertyList_.empty()) {
    for ( size_t i = 0; i < propertyList_.size(); ++i) {
      DomFunctions::setChild( childElement, "property", propertyList_[ i]);
    }
  }

  /*
   * Add the optional provenance entry to the GriddedTableDef child
   */
  if ( hasProvenance_) {
    provenance_.exportDefinition( childElement);
  }
}

void PropertyDef::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName(
    "PropertyDef::readDefinitionFromDom()");

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

bool PropertyDef::compareElementID(
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

  default:
    return false;
    break;
  }

  return true;
}


void PropertyDef::resetJanus( Janus *janus)
{
  janus_ = janus;
}

//------------------------------------------------------------------------//

ostream& operator<<(
  ostream &os,
  const PropertyDef &propertyDef)
{
  /*
   * General properties of the Class
   */
  os << endl << endl << "Display BreakpointDef contents:" << endl
     << "-----------------------------------" << endl;

  os << "  name               : " << propertyDef.getName() << endl
     << "  ptyID              : " << propertyDef.getPtyID() << endl
     << "  description        : " << propertyDef.getDescription() << endl
     << "  property           : " << propertyDef.getProperty() << endl
     << endl;

  /*
   * Provenance data for the Class
   */
  if ( propertyDef.hasProvenance()) {
    os << propertyDef.getProvenance() << endl;
  }

  return os;
}

//------------------------------------------------------------------------//

}// namespace janus
