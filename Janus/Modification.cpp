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
// Title:      Janus/Modification
// Class:      Modification
// Module:     Modification.cpp
// First Date: 2009-05-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Modification.cpp
 *
 * A Modification instance holds in its allocated memory alphanumeric data
 * derived from a \em modificationRecord element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance describes
 * the author and content of a modification to a dataset.
 * A modificationRecord associates a single letter (such as
 * modification "A") with modification author(s), address, and any
 * optional external reference documents, in keeping with the AIAA
 * draft standard. The class also provides the functions that allow
 * a calling Janus instance to access these data elements.
 *
 * The Modification class is only used within the janus namespace, and should
 * only be referenced indirectly through the FileHeader class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

#include <Ute/aMessageStream.h>

#include "DomFunctions.h"
#include "Modification.h"

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

Modification::Modification( )
  :
  XmlElementDefinition(),
  elementType_( ELEMENT_NOTSET)
{
}

Modification::Modification(
  const DomFunctions::XmlNode& elementDefinition )
  :
  XmlElementDefinition(),
  elementType_( ELEMENT_NOTSET)
{
  initialiseDefinition( elementDefinition );
}


//------------------------------------------------------------------------//

void Modification::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition )
{
  static const aString functionName( "Modification::initialiseDefinition()");

  /*
   * Retrieve attributes for the Variable Definition
   */
  try {
    modID_ = DomFunctions::getAttribute( elementDefinition, "modID", true);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Function without \"modID\" attribute.\n - "
      << excep.what()
    );
  }

  try {
    date_  = DomFunctions::getAttribute( elementDefinition, "date", true);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Function without \"date\" attribute.\n - "
      << excep.what()
    );
  }

  refID_ = DomFunctions::getAttribute( elementDefinition, "refID");

  /*
   * Retrieve the description associated with the variable
   */
  description_ = DomFunctions::getChildValue( elementDefinition, "description" );

  /*
   * Retrieve List of Authors
   */
  elementType_ = ELEMENT_AUTHOR;
  try {
    DomFunctions::initialiseChildren( this,
                                      elementDefinition,
                                      modID_,
                                      "author",
                                      true);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Function without \"author\" element.\n - "
      << excep.what()
    );
  }

  /*
   * Retrieve Document References
   */
  elementType_ = ELEMENT_REFERENCE;
  try {
    DomFunctions::initialiseChildrenOrRefs( this,
                                            elementDefinition,
                                            modID_,
                                            "reference",
                                            EMPTY_STRING,
                                            "extraDocRef",
                                            "refID",
                                            false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Error initialising \"extraDocRef\" elements.\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//
void Modification::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the ModificationRecord element
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "modificationRecord");

  /*
   * Add attributes to the ModificationRecord child
   */
  DomFunctions::setAttribute( childElement, "modID", modID_);
  DomFunctions::setAttribute( childElement, "date",  date_);

  if ( !refID_.empty()) {
    DomFunctions::setAttribute( childElement, "refID", refID_);
  }

  /*
   * Add author entries to the ModificationRecord child
   */
  for ( size_t i = 0; i < getAuthorCount(); i++) {
    author_[i].exportDefinition( childElement);
  }

  /*
   * Add description element
   */
  if ( !description_.empty()) {
    DomFunctions::setChild( childElement, "description", description_);
  }

  /*
   * Add extra document reference entries to the ModificationRecord child
   */
  DomFunctions::XmlNode extraDocRefElement;
  for ( size_t i = 0; i < getExtraDocCount(); i++) {
    extraDocRefElement = DomFunctions::setChild( childElement, "extraDocRef");
    DomFunctions::setAttribute( extraDocRefElement, "refID",
                                getExtraDocRefID( i));
  }
}

void Modification::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement )
{
  switch ( elementType_) {
    case ELEMENT_AUTHOR:
      author_.push_back( Author( xmlElement));
      break;

    default:
      break;
  }
}

bool Modification::compareElementID(
  const DomFunctions::XmlNode& xmlElement,
  const aString& elementID,
  const size_t& /*documentElementReferenceIndex*/)
{
  if ( DomFunctions::getAttribute( xmlElement, "refID") != elementID) {
    return false;
  }
  extraDocRefID_.push_back( elementID);
  return true;
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const Modification &modification)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display Modification contents:" << endl
     << "-----------------------------------" << endl;

  os << "  modId              : " << modification.getModID() << endl
     << "  date               : " << modification.getDate() << endl
     << "  refId              : " << modification.getRefID() << endl
     << "  description        : " << modification.getDescription() << endl
     << endl;

  /*
   * Data associated with Class
   */
  const vector<Author>& author = modification.getAuthor();
  for ( size_t i = 0; i < author.size(); i++) {
    os << "  Author " << i << endl;
    os << author[i] << endl;
  }

  size_t extraDocCount = modification.getExtraDocCount();
  for ( size_t i = 0; i < extraDocCount; i++) {
    os << "  extra doc refId " << i << " = " << modification.getExtraDocRefID(i) << endl;
  }

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus
