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
// Title:      Janus/FileHeader
// Class:      FileHeader
// Module:     FileHeader.cpp
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file FileHeader.cpp
 *
 * A FileHeader instance holds in its allocated memory alphanumeric data
 * derived from the \em fileHeader element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  There is always one
 * FileHeader instance for each Janus instance.  It requires at least one
 * author, a creation date and a version indicator; optional content are
 * description, references and modification records.  The class also
 * provides the functions that allow a calling Janus instance to access
 * these data elements.
 *
 * The FileHeader class is only used within the janus namespace, and should
 * only be referenced indirectly through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 */

#include <Ute/aMessageStream.h>

#include "DomFunctions.h"
#include "FileHeader.h"

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

  FileHeader::FileHeader() :
    XmlElementDefinition(),
    elementType_( ELEMENT_FILEHEADER)
  {
  }

  FileHeader::FileHeader(
    const DomFunctions::XmlNode& elementDefinition) :
    XmlElementDefinition(),
    elementType_( ELEMENT_FILEHEADER)
  {
    initialiseDefinition( elementDefinition);
  }

//------------------------------------------------------------------------//

  void FileHeader::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "FileHeader::initialiseDefinition()");

    /*
     * Retrieve the element attributes
     */
    name_ = DomFunctions::getAttribute( elementDefinition, "name");

    /*
     * Retrieve the classification associated with the element
     */
    classification_ = DomFunctions::getChildValue( elementDefinition, "classification");

    /*
     * Retrieve the data assumptions associated with the element
     */
    dataAssumptions_ = DomFunctions::getChildValue( elementDefinition, "dataAssumptions");

    /*
     * Retrieve the tag associated with the element
     */
    tag_ = DomFunctions::getChildValue( elementDefinition, "tag");

    /*
     * Retrieve the type associated with the element
     */
    type_ = DomFunctions::getChildValue( elementDefinition, "type");

    /*
     * Retrieve the description associated with the element
     */
    description_ = DomFunctions::getChildValue( elementDefinition, "description");

    /*
     * Retrieve the file creation date element
     */
    DomFunctions::XmlNode creationDateElement;
    if ( DomFunctions::isChildInNode( elementDefinition, "creationDate")) {
      creationDateElement = DomFunctions::getChild( elementDefinition, "creationDate");
    }
    else if ( DomFunctions::isChildInNode( elementDefinition, "fileCreationDate")) {
      creationDateElement = DomFunctions::getChild( elementDefinition, "fileCreationDate");
    }
    else {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - File Header element"
        << "\" does not have a valid file creation date."
      );
    }
    creationDate_ = DomFunctions::getAttribute( creationDateElement, "date");

    /*
     * Retrieve the file version element
     */
    fileVersion_ = DomFunctions::getChildValue( elementDefinition, "fileVersion");

    /*
     * Retrieve author elements
     */
    elementType_ = ELEMENT_AUTHOR;
    try {
      DomFunctions::initialiseChildren( this, elementDefinition, name_, "author", true);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - Function without \"author\" elements.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve reference elements
     */
    elementType_ = ELEMENT_REFERENCE;
    try {
      DomFunctions::initialiseChildren( this, elementDefinition, name_, "reference", false);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - Error instantiating \"reference\" elements.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve modification elements
     */
    elementType_ = ELEMENT_MODIFICATION;
    try {
      DomFunctions::initialiseChildren( this, elementDefinition, name_, "modificationRecord", false);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - Error instantiating \"modificationRecord\" elements.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve provenance elements
     */
    elementType_ = ELEMENT_PROVENANCE;
    try {
      DomFunctions::initialiseChildren( this, elementDefinition, name_, "provenance", false);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - Error instantiating \"provenance\" elements.\n - "
        << excep.what()
      );
    }
  }

//------------------------------------------------------------------------//
  void FileHeader::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the FileHeader element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild( documentElement, "fileHeader");

    /*
     * Add attributes to the FileHeader child
     */
    if ( !name_.empty()) {
      DomFunctions::setAttribute( childElement, "name", name_);
    }

    /*
     * Add author entries to the FileHeader child
     */
    if ( author_.size() > 0) {
      DomFunctions::setComment( childElement, " Authors ");
    }
    for ( size_t i = 0; i < getAuthorCount(); i++) {
      author_[ i].exportDefinition( childElement);
    }

    /*
     * Add the optional entries to the FileHeader child
     */
    if ( !creationDate_.empty()) {
      DomFunctions::XmlNode dateElement = DomFunctions::setChild( childElement, "creationDate");
      DomFunctions::setAttribute( dateElement, "date", creationDate_);
    }

    if ( !fileVersion_.empty()) {
      DomFunctions::setChild( childElement, "fileVersion", fileVersion_);
    }

    if ( !description_.empty()) {
      DomFunctions::setChild( childElement, "description", description_);
    }

    if ( !classification_.empty()) {
      DomFunctions::setChild( childElement, "classification", classification_);
    }

    if ( !dataAssumptions_.empty()) {
      DomFunctions::setChild( childElement, "dataAssumptions", dataAssumptions_);
    }

    if ( !tag_.empty()) {
      DomFunctions::setChild( childElement, "tag", tag_);
    }

    if ( !type_.empty()) {
      DomFunctions::setChild( childElement, "type", type_);
    }

    /*
     * Add Reference entries to the FileHeader Element
     */
    if ( reference_.size() > 0) {
      DomFunctions::setComment( childElement, " References ");
    }
    for ( size_t i = 0; i < getReferenceCount(); i++) {
      reference_[ i].exportDefinition( childElement);
    }

    /*
     * Add Modification entries to the FileHeader Element
     */
    if ( modification_.size() > 0) {
      DomFunctions::setComment( childElement, " Modifications ");
    }
    for ( size_t i = 0; i < getModificationCount(); i++) {
      modification_[ i].exportDefinition( childElement);
    }

    /*
     * Add Provenance entries to the FileHeader Element
     */
    if ( provenance_.size() > 0) {
      DomFunctions::setComment( childElement, " Provenance Entries ");
    }
    for ( size_t i = 0; i < getProvenanceCount(); i++) {
      provenance_[ i].exportDefinition( childElement);
    }
  }

  void FileHeader::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    switch ( elementType_) {
    case ELEMENT_AUTHOR:
      author_.push_back( Author( xmlElement));
      break;

    case ELEMENT_REFERENCE:
      reference_.push_back( Reference( xmlElement));
      break;

    case ELEMENT_MODIFICATION:
      modification_.push_back( Modification( xmlElement));
      break;

    case ELEMENT_PROVENANCE:
      provenance_.push_back( Provenance( xmlElement));
      break;

    default:
      break;
    }

    return;
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const FileHeader &fileHeader)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display FileHeader contents:" << endl << "-----------------------------------" << endl;

    os << "  name               : " << fileHeader.getName() << endl << "  creation date      : "
      << fileHeader.getCreationDate() << endl << "  file version       : " << fileHeader.getFileVersion() << endl
      << "  Classification     : " << fileHeader.getClassification() << endl << "  Data Assumptions   : "
      << fileHeader.getDataAssumptions() << endl << "  Tag                : " << fileHeader.getTag() << endl
      << "  Type               : " << fileHeader.getType() << endl << "  description        : "
      << fileHeader.getDescription() << endl << endl;

    /*
     * Data associated with Class
     */
    const vector< Author>& author = fileHeader.getAuthor();
    for ( size_t i = 0; i < author.size(); i++) {
      os << "  Author " << i << endl;
      os << author[ i] << endl;
    }

    const vector< Reference>& reference = fileHeader.getReference();
    for ( size_t i = 0; i < reference.size(); i++) {
      os << "  Reference " << i << endl;
      os << reference[ i] << endl;
    }

    const vector< Modification>& modification = fileHeader.getModification();
    for ( size_t i = 0; i < modification.size(); i++) {
      os << "  Modification " << i << endl;
      os << modification[ i] << endl;
    }

    const vector< Provenance>& provenance = fileHeader.getProvenance();
    for ( size_t i = 0; i < provenance.size(); i++) {
      os << "  Provenance " << i << endl;
      os << provenance[ i] << endl;
    }

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus
