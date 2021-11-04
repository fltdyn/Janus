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
// Title:      Janus/Reference
// Class:      Reference
// Module:     Reference.cpp
// First Date: 2009-05-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Reference.cpp
 *
 * A Reference instance holds in its allocated memory alphanumeric data
 * derived from a \em reference element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance describes an
 * external document relevant to the dataset.  The class also provides the
 * functions that allow a calling Janus instance to access these data
 * elements.
 *
 * The Reference class is only used within the janus namespace, and should
 * only be referenced indirectly through the FileHeader class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Local Includes
#include "DomFunctions.h"
#include "Reference.h"

using namespace std;

namespace janus {

//------------------------------------------------------------------------//

Reference::Reference()
  :
  XmlElementDefinition()
{
}

Reference::Reference(
  const DomFunctions::XmlNode& elementDefinition )
  :
  XmlElementDefinition()
{
  initialiseDefinition( elementDefinition );
}


//------------------------------------------------------------------------//

void Reference::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition )
{
  /*
   * Retrieve the element attributes
   */
  xlink_  = DomFunctions::getAttribute( elementDefinition, "xmlns:xlink");
  if ( xlink_.empty()) {
    xlink_ = "http://www.w3.org/1999/xlink";
  }

  type_   = DomFunctions::getAttribute( elementDefinition, "xlink:type");
  if ( type_.empty()) {
    type_ = "simple";
  }

  refID_  = DomFunctions::getAttribute( elementDefinition, "refID", true);
  author_ = DomFunctions::getAttribute( elementDefinition, "author", true);
  title_  = DomFunctions::getAttribute( elementDefinition, "title", true);
  classification_ = DomFunctions::getAttribute( elementDefinition, "classification");
  accession_      = DomFunctions::getAttribute( elementDefinition, "accession");
  date_           = DomFunctions::getAttribute( elementDefinition, "date", true);
  href_           = DomFunctions::getAttribute( elementDefinition, "href");
  
  /*
   * Retrieve the description associated with the variable
   */
  description_ = DomFunctions::getChildValue( elementDefinition, "description" );
}

//------------------------------------------------------------------------//

void Reference::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the Reference element
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "reference");

  /*
   * Add attributes to the Provenance child
   */
  DomFunctions::setAttribute( childElement, "xmlns:xlink", xlink_);
  DomFunctions::setAttribute( childElement, "xlink:type",  type_);
  DomFunctions::setAttribute( childElement, "refID",       refID_);
  DomFunctions::setAttribute( childElement, "author",      author_);
  DomFunctions::setAttribute( childElement, "title",       title_);

  if ( !classification_.empty()) {
    DomFunctions::setAttribute( childElement, "classification", classification_);
  }

  if ( !accession_.empty()) {
    DomFunctions::setAttribute( childElement, "accession", accession_);
  }

  if ( !date_.empty()) {
    DomFunctions::setAttribute( childElement, "date", date_);
  }

  if ( !href_.empty()) {
    DomFunctions::setAttribute( childElement, "xlink:href", href_);
  }

  /*
   * Add description element
   */
  if ( !description_.empty()) {
    DomFunctions::setChild( childElement, "description", description_);
  }
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const Reference &reference)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display Reference contents:" << endl
     << "-----------------------------------" << endl;

  os << "  xlink              : " << reference.getXLink() << endl
     << "  type               : " << reference.getXLinkType() << endl
     << "  refID              : " << reference.getRefID() << endl
     << "  author             : " << reference.getAuthor() << endl
     << "  title              : " << reference.getTitle() << endl
     << "  date               : " << reference.getDate() << endl
     << "  classification     : " << reference.getClassification() << endl
     << "  accession          : " << reference.getAccession() << endl
     << "  hRef               : " << reference.getHref() << endl
     << "  description        : " << reference.getDescription() << endl
     << endl;

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus
