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
// Title:      Janus/Provenance
// Class:      Provenance
// Module:     Provenance.cpp
// First Date: 2008-06-24
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Provenance.cpp
 *
 * A Provenance instance holds in its allocated memory alphanumeric data
 * derived from a \em provenance element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  Provenances may apply to
 * a complete dataset or to individual components within a dataset.
 * Not all provenances will contain all possible \em provenance
 * components. The Provenance instance also provides the
 * functions that allow a calling Janus instance to access these data
 * elements.
 *
 * The Provenance class is only used within the janus namespace, and should
 * only be referenced indirectly through the FileHeader, VariableDef,
 * GriddedTableDef, UngriddedTableDef, Function or CheckData classes.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

#include <Ute/aMessageStream.h>

#include "DomFunctions.h"
#include "Provenance.h"

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

Provenance::Provenance( )
  :
  XmlElementDefinition(),
  elementType_( ELEMENT_NOTSET)
{
}

Provenance::Provenance(
  const DomFunctions::XmlNode& elementDefinition )
  :
  XmlElementDefinition(),
  elementType_( ELEMENT_NOTSET)
{
  initialiseDefinition( elementDefinition );
}


//------------------------------------------------------------------------//

void Provenance::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition )
{
  static const aString functionName( "Provenance::initialiseDefinition()");
  
  /*
   * set the provID from attribute, if provided in XML file
   */
  provID_ = DomFunctions::getAttribute( elementDefinition, "provID");

  /*
   * Retrieve the creationDate / functionCreationDate
   */
  DomFunctions::XmlNode creationDateElement;
  if ( DomFunctions::isChildInNode( elementDefinition, "creationDate")) {
    creationDateElement = DomFunctions::getChild( elementDefinition, "creationDate");
  }
  else if ( DomFunctions::isChildInNode( elementDefinition, "functionCreationDate")) {
    creationDateElement = DomFunctions::getChild( elementDefinition, "functionCreationDate");
  }
  else {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Provenance element"
      << "\" does not have a valid file creation date."
    );
  }
  creationDate_ = DomFunctions::getAttribute( creationDateElement, "date");
   
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
                                      provID_,
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
                                            provID_,
                                            "reference",
                                            EMPTY_STRING,
                                            "documentRef",
                                            "refID",
                                            false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Function without \"reference\" element,\n - "
      << excep.what()
    );
  }

  /*
   * Retrieve Modification References
   */
  elementType_ = ELEMENT_MODIFICATION;
  try {
    DomFunctions::initialiseChildrenOrRefs( this,
                                            elementDefinition,
                                            provID_,
                                            "modificationRecord",
                                            EMPTY_STRING,
                                            "modificationRef",
                                            "modID",
                                            false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Error initialising \"modificationRecord\" element.\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//
void Provenance::exportDefinition(
  DomFunctions::XmlNode& documentElement,
  const bool &isReference)
{
  /*
   * Create a child node in the DOM for the Provenance element
   */
  DomFunctions::XmlNode childElement;
  if ( isReference) {
    childElement =
      DomFunctions::setChild( documentElement, "provenanceRef");
  }
  else {
    childElement =
      DomFunctions::setChild( documentElement, "provenance");
  }


  /*
   * Add attributes to the Provenance child
   */
  if ( !provID_.empty()) {
    DomFunctions::setAttribute( childElement, "provID", provID_);
  }

  if ( !isReference) {
    /*
     * Add author entries to the Provenance child
     */
    for ( size_t i = 0; i < getAuthorCount(); i++) {
      author_[i].exportDefinition( childElement);
    }

    /*
     * Add creation date element
     */
    if ( !creationDate_.empty()) {
      DomFunctions::XmlNode dateElement = DomFunctions::setChild( childElement, "creationDate");
      DomFunctions::setAttribute( dateElement, "date", creationDate_);
    }

    /*
     * Add description element
     */
    if ( !description_.empty()) {
      DomFunctions::setChild( childElement, "description", description_);
    }

    /*
     * Add document reference entries to the Provenance child
     */
    DomFunctions::XmlNode docRefElement;
    for ( size_t i = 0; i < getDocumentRefCount(); i++) {
      docRefElement = DomFunctions::setChild( childElement, "documentRef");
      DomFunctions::setAttribute( docRefElement, "refID",
                                  getDocumentRefID( i));
    }

    /*
     * Add modification record reference entries to the Provenance child
     */
    DomFunctions::XmlNode modRefElement;
    for ( size_t i = 0; i < getModificationRefCount(); i++) {
      modRefElement = DomFunctions::setChild( childElement, "modificationRef");
      DomFunctions::setAttribute( modRefElement, "modID",
                                  getModificationModID( i));
    }
  }
}

void Provenance::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement )
{
  if ( elementType_ == ELEMENT_AUTHOR) {
    author_.push_back( Author( xmlElement));
  }
}

bool Provenance::compareElementID(
  const DomFunctions::XmlNode& xmlElement,
  const aString& elementID,
  const size_t& /*documentElementReferenceIndex*/)
{
  aString crossRefElementId;

  switch ( elementType_) {
    case ELEMENT_REFERENCE:
	  crossRefElementId = DomFunctions::getAttribute( xmlElement, "refID");
	  if ( crossRefElementId != elementID) {
	    return false;
      }
	  documentRefID_.push_back( elementID);
	  break;

    case ELEMENT_MODIFICATION:
	  crossRefElementId = DomFunctions::getAttribute( xmlElement, "modID");
	  if ( crossRefElementId != elementID) {
	    return false;
      }
	  modificationModID_.push_back( elementID);
	  break;

    default:
      return false;
      break;
  }

  return true;
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const Provenance &provenance)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display Provenance contents:" << endl
     << "-----------------------------------" << endl;

  os << "  provID             : " << provenance.getProvID() << endl
     << "  description        : " << provenance.getDescription() << endl
     << "  creationDate       : " << provenance.getCreationDate() << endl
     << endl;

  /*
   * Authors associated with Class
   */
  const vector<Author>& author = provenance.getAuthor();
  for ( size_t i = 0; i < author.size(); i++) {
    os << author[i] << endl;
  }

  /*
   * Document Reference IDs associated with Class
   */
  const aStringList& documentRefID = provenance.getDocumentRefID();
  for ( size_t i = 0; i < documentRefID.size(); i++) {
    os << "  documentRefID " << i << " = " << documentRefID[i] << endl;
  }

  /*
   * Modification IDs associated with Class
   */
  const aStringList& modificationModID = provenance.getModificationModID();
  for ( size_t i = 0; i < modificationModID.size(); i++) {
    os << "  modificationModID " << i << " = " << modificationModID[i] << endl;
  }

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus



