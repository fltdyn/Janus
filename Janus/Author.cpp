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
// Title:      Janus/Author
// Class:      Author
// Module:     Author.cpp
// First Date: 2011-12-05
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Author.cpp
 *
 * An Author instance holds in its allocated memory alphanumeric data
 * derived from an \em author element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance may describe
 * an author of a complete dataset, or of a component of a dataset, or of a
 * modification to a dataset.  Author contact details may be expressed in
 * either \em address or \em contactInfo forms.  The \em contactInfo form is
 * newer, more flexible and generally preferred.  The class also provides the
 * functions that allow a calling Janus instance to access these data
 * elements.
 *
 * The Author class is only used within the janus namespace, and should
 * only be referenced indirectly through the FileHeader, Modification or
 * Provenance classes.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

#include <Ute/aMessageStream.h>

#include "DomFunctions.h"
#include "Author.h"

using namespace std;
using namespace dstoute;

namespace janus
{

  //------------------------------------------------------------------------//

  Author::Author() :
    XmlElementDefinition()
  {
  }

  Author::Author(
    const DomFunctions::XmlNode& authorElement) :
    XmlElementDefinition()
  {
    initialiseDefinition( authorElement);
  }

  //------------------------------------------------------------------------//

  void Author::initialiseDefinition(
    const DomFunctions::XmlNode& authorElement)
  {
    static const aString functionName( "Author::initialiseDefinition()");

    /*
     * Retrieve the element attributes
     */

    try {
      name_ = DomFunctions::getAttribute( authorElement, "name", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Function without \"dataTable\" attribute.\n - "
        << excep.what()
      );
    }

    try {
      org_ = DomFunctions::getAttribute( authorElement, "org", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Function without \"org\" attribute.\n - "
        << excep.what()
      );
    }

    xns_ = DomFunctions::getAttribute( authorElement, "xns");
    email_ = DomFunctions::getAttribute( authorElement, "email");

    /*
     * Retrieve ContactInfo elements if they exist.
     */
    if ( DomFunctions::isChildInNode( authorElement, "contactInfo")) {
      elementType_ = ELEMENT_CONTACTINFO;
      try {
        DomFunctions::initialiseChildren( this, authorElement, name_,
          "contactInfo", false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - Function without \"contactInfo\" element.\n - "
          << excep.what()
        );
      }
    }
    else {
      /*
       * Retrieve Address elements if they exist.
       */
      elementType_ = ELEMENT_ADDRESS;
      try {
        DomFunctions::initialiseChildren( this, authorElement, name_, "address",
          false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - Function without \"address\" element.\n - "
          << excep.what()
        );
      }
    }
  }

  //------------------------------------------------------------------------//

  void Author::exportDefinition(
    DomFunctions::XmlNode& documentElement,
    const aString& authorTag)
  {
    /*
     * Create a child node in the DOM for the Array element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild(
      documentElement, authorTag);

    /*
     * Add attributes to the Author child
     */
    DomFunctions::setAttribute( childElement, "name", name_);
    DomFunctions::setAttribute( childElement, "org", org_);

    if ( !xns_.empty()) {
      DomFunctions::setAttribute( childElement, "xns", xns_);
    }

    if ( !email_.empty()) {
      DomFunctions::setAttribute( childElement, "email", email_);
    }

    /*
     * Add address elements
     */
    for ( size_t i = 0; i < address_.size(); i++) {
      DomFunctions::setChild( childElement, "address", getAddress( i));
    }

    /*
     * Add contact information
     */
    DomFunctions::XmlNode contactElement;

    for ( size_t i = 0; i < contactInfoContent_.size(); i++) {
      contactElement = DomFunctions::setChild( childElement, "contactInfo",
        getContactInfo( i));
      DomFunctions::setAttribute( contactElement, "contactInfoType",
        getContactInfoType( i));
      DomFunctions::setAttribute( contactElement, "contactLocation",
        getContactLocation( i));
    }
  }

  void Author::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    switch ( elementType_) {
    case ELEMENT_CONTACTINFO:
      contactInfoType_.push_back(
        DomFunctions::getAttribute( xmlElement, "contactInfoType"));
      contactLocation_.push_back(
        DomFunctions::getAttribute( xmlElement, "contactLocation"));
      contactInfoContent_.push_back( DomFunctions::getCData( xmlElement));
      break;

    case ELEMENT_ADDRESS:
      address_.push_back( DomFunctions::getCData( xmlElement));
      break;

    default:
      break;
    }
  }

  //------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const Author &author)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display Author contents:" << endl
       << "-----------------------------------" << endl;

    os << "  name               : " << author.getName() << endl
       << "  organisation       : " << author.getOrg() << endl
       << "  xns                : " << author.getXns() << endl
       << "  email              : " << author.getEmail() << endl << endl;

    /*
     * Address information associated with Class
     */
    const aStringList& address = author.getAddress();
    for ( size_t i = 0; i < address.size(); i++) {
      os << "  address " << i << " = " << address[ i] << endl;
    }

    /*
     * Contact information associated with Class
     */
    for ( size_t i = 0; i < author.getContactInfoCount(); i++) {
      os << "  Contact " << i << endl;
      os << "    Type      : " << author.getContactInfoType( i) << endl;
      os << "    Location  : " << author.getContactLocation( i) << endl;
      os << "    Content   : " << author.getContactInfo( i) << endl;
    }

    return os;
  }

//------------------------------------------------------------------------//

} // namespace janus

