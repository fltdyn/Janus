#ifndef _FILEHEADER_H_
#define _FILEHEADER_H_

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
// Module:     FileHeader.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file FileHeader.h
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

// C++ Includes 
#include <vector>

// Local Includes
#include "XmlElementDefinition.h"
#include "Author.h"
#include "Provenance.h"
#include "Modification.h"
#include "Reference.h"

namespace janus
{

  /**
   * A #FileHeader instance holds in its allocated memory alphanumeric data
   * derived from the \em fileHeader element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  There is always one
   * FileHeader instance for each Janus instance.  It requires at least one
   * author, a creation date and a version indicator; optional content are
   * description, references and modification records.  The class also
   * provides the functions that allow a calling Janus instance to access
   * these data elements.
   *
   * The #FileHeader class is only used within the janus namespace, and should
   * only be referenced indirectly through the Janus class.
   *
   * A typical usage is:
    \code
      Janus test( xmlFileName );
      FileHeader header = test.getFileHeader();
      int nAuthors = header.getAuthorCount( );
      cout << "Number of authors : " << nAuthors << "\n\n";
      for ( int i = 0 ; i < nAuthors ; i++ ) {
        Author author = header.getAuthor( i );
        cout << " Author " << i << " : Name                  : "
             << author.getName( ) << "\n"
             << "          Organisation            : "
             << author.getOrg( ) << "\n"
      }
      cout << " File creation date               : "
           << header.getCreationDate() << "\n"
           << " File version                     : "
           << header.getFileVersion() << "\n"
           << " File description                 : "
           << header.getDescription() << "\n"
           << " Number of reference records      : "
           << header.getReferenceCount() << "\n"
           << " Number of modification records   : "
           << header.getModificationCount() << "\n";
    \endcode
   */
  class FileHeader: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #FileHeader class
     * without supplying the DOM \em fileHeader element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em fileHeader element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    FileHeader();

    /**
     * The constructor, when called with an argument pointing to a \em fileHeader
     * element within a DOM, instantiates the #FileHeader class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address to the Level 1 element within a
     * DOM that is tagged as a \em fileHeader.  There should always be one
     * such element.
     */
    FileHeader( const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #FileHeader is filled with data from
     * the \em fileHeader element within a DOM by this function.  If a
     * \em fileHeader element pointer is supplied to an
     * instance that has already been initialised, the instance will be
     * re-initialised with the new data.  However, this is not a
     * recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address to the Level 1 element within a
     * DOM that is tagged as a \em fileHeader.  There should always be one
     * such element.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function returns the optional \em name attribute of the #FileHeader
     * instance, if one has been supplied in the XML dataset.  If not, or if the
     * instance has not been initialised from a DOM, it returns an empty string.
     *
     * \return The \em name string is passed by reference.
     */
    const dstoute::aString& getName() const
    {
      return name_;
    }

    /**
     * This function returns the number of primary authors listed in a
     * #FileHeader.  If the instance has not been populated from a DOM
     * element, zero is returned.
     *
     * \return An integer number, one or more in a populated instance.
     *
     * \sa Author
     */
    size_t getAuthorCount() const
    {
      return author_.size();
    }

    /**
     * This function returns a reference to the list of authors defined within
     * the #FileHeader instance.
     *
     * \return The list authors is returned by reference.
     *
     * \sa Author
     */
    const AuthorList& getAuthor() const
    {
      return author_;
    }

    /**
     * This function returns a reference to the selected Author instance within
     * the #FileHeader instance.
     *
     * \param index has a range from zero to ( #getAuthorCount() - 1 ),
     * and selects the required Author instance.  An attempt to access
     * a non-existent \em author will throw a standard out_of_range exception.
     *
     * \return The requested Author instance is returned by reference.
     *
     * \sa Author
     */
    const Author& getAuthor(
      const size_t& index) const
    {
      return author_.at( index);
    }

    /**
     * This function returns the \em creationDate element of the
     * \em fileHeader element (\em fileCreationDate is a deprecated alternative).
     * The format of the dataset string is determined by the XML dataset builder,
     * but DAVE-ML recommends the ISO 8601 form ("2004-01-02" to refer to
     * 2 January 2004).  If the #FileHeader has not been populated from a DOM
     * element , the function returns an empty string.
     *
     * \return The \em creationDate string is returned by reference.
     */
    const dstoute::aString& getCreationDate() const
    {
      return creationDate_;
    }

    /**
     * The \em fileVersion element is an optional document identifier for a
     * \em fileHeader.  The format of the version string is determined by the XML
     * dataset builder.  This function returns the \em fileVersion element of
     * the referenced file header, if one has been supplied in the XML
     * dataset.  If not, it returns an empty string.
     *
     * \return The \em fileVersion string is returned by reference.
     */
    const dstoute::aString& getFileVersion() const
    {
      return fileVersion_;
    }

    /**
     * The \em classification element is an optional document identifier
     * defining the security classification for the information stored with
     * the XML dataset. This function returns the \em classification element of
     * the referenced file header, if one has been supplied in the XML
     * dataset.  If not, it returns an empty string.
     *
     * \return The \em classification string is returned by reference.
     */
    const dstoute::aString getClassification() const
    {
      return classification_;
    }

    /**
     * The \em dataAssumptions element is an optional identifier
     * documenting assumptions associated with the information stored with
     * the XML dataset. This function returns the \em dataAssumptions element of
     * the referenced file header, if one has been supplied in the XML
     * dataset.  If not, it returns an empty string.
     *
     * \return The \em dataAssumptions string is returned by reference.
     */
    const dstoute::aString getDataAssumptions() const
    {
      return dataAssumptions_;
    }

    /**
     * The \em tag element is an optional identifier that is used to
     * identify the several DAVE-ML compliant XML dataset source files as
     * being part of the same version of an aircraft model. This is similar
     * to a \em tag used in source code version control.
     *
     * This function returns the \em tag element of the referenced file header,
     * if one has been supplied in the XML dataset.  If not, it returns an
     * empty string.
     *
     * \return The \em tag string is returned by reference.
     */
    const dstoute::aString getTag() const
    {
      return tag_;
    }

    /**
     * The \em type element is an optional parameter that is used to
     * identify different types of DAVE-ML compliant XML dataset source files
     * by aircraft type; for example, fixed wing verses rotary wing.
     *
     * This function returns the \em type element of the referenced file
     * header, if one has been supplied in the XML dataset.  If not, it
     * returns an empty string.
     *
     * \return The \em type string is returned by reference.
     */
    const dstoute::aString getType() const
    {
      return type_;
    }

    /**
     * This function returns the \em description from a \em fileHeader
     * element, if one has been supplied in the XML dataset.  The description
     * consists of a string of arbitrary length, which can include tabs and new
     * lines as well as alphanumeric data.  This means text formatting embedded
     * in the XML source will also appear in the returned description string.
     * Since description of a file is optional, the returned string may
     * be blank.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription() const
    {
      return description_;
    }

    /**
     * This function returns the number of \em reference elements listed in
     * a \em fileHeader element.  A \em fileHeader can include no, one or
     * multiple \em reference components.  If the #FileHeader has not been
     * populated from a DOM element, zero is returned.
     *
     * \return An integer number, zero or more in a populated instance.
     *
     * \sa Reference
     */
    size_t getReferenceCount() const
    {
      return reference_.size();
    }

    /**
     * This function provides access to the \em reference records
     * contained in the XML dataset file header, through the Reference class
     * structure.
     *
     * \return The list of references is returned by reference.
     *
     * \sa Reference
     */
    const ReferenceList& getReference() const
    {
      return reference_;
    }

    /**
     * This function provides access to the \em reference records
     * contained in the XML dataset file header, through the Reference class
     * structure.
     *
     * \param index has a range from 0 to ( #getReferenceCount() - 1 ),
     * and selects the required \em reference record.  An attempt to access
     * a non-existent \em reference will throw a standard out_of_range exception.
     *
     * \return The requested Reference instance is returned by reference.
     *
     * \sa Reference
     */
    const Reference& getReference(
      const size_t& index) const
    {
      return reference_.at( index);
    }

    /**
     * This function returns the number of \em modificationRecord records at
     * the top level of the \em fileHeader component of the XML dataset.  A
     * \em fileHeader can include no, one or multiple \em modificationRecord
     * components.  If the #FileHeader has not been populated from a DOM element,
     * zero is returned.
     *
     * \return An integer number, zero or more in a populated instance.
     *
     * \sa Modification
     */
    size_t getModificationCount() const
    {
      return modification_.size();
    }

    /**
     * This function provides access to the \em modificationRecord elements
     * contained in a DAVE-ML \em fileHeader element, through the Modification
     * class structure.
     *
     * \return The list of modification records is returned by reference.
     *
     * \sa Modification
     */
    const ModificationList& getModification() const
    {
      return modification_;
    }

    /**
     * This function provides access to the \em modificationRecord elements
     * contained in a DAVE-ML \em fileHeader element, through the Modification
     * class structure.
     *
     * \param index has a range from 0 to ( #getModificationCount() - 1 ),
     * and selects the required \em modificationRecord.  An attempt to access
     * a non-existent \em modificationRecord will throw a standard out_of_range
     * exception.
     *
     * \return The requested Modification instance is returned by reference.
     *
     * \sa Modification
     */
    const Modification& getModification(
      const size_t& index) const
    {
      return modification_.at( index);
    }

    /**
     * This function returns the number of \em provenance elements
     * contained in a DAVE-ML \em fileHeader element.  It does NOT include
     * provenance elements contained in other elements of the dataset.  There
     * may be zero or more of these elements in a valid file header.
     * \return The integer count of provenance elements contained in a
     * \em fileHeader element is returned.  Possible values are zero or more.
     * \sa Provenance
     */
    size_t getProvenanceCount() const
    {
      return provenance_.size();
    }

    /**
     * This function provides access to the \em provenance elements
     * contained in a DAVE-ML \em fileHeader element, through the Provenance
     * class structure.
     *
     * \return The list of provenence records is returned by reference.
     *
     * \sa Provenance
     */
    const ProvenanceList& getProvenance() const
    {
      return provenance_;
    }

    /**
     * This function provides access to the \em provenance elements
     * contained in a DAVE-ML \em fileHeader element, through the Provenance
     * class structure.
     *
     * \param index has a range from 0 to ( #getProvenanceCount() - 1 ),
     * and selects the required \em provenance record.  An attempt to access
     * a non-existent \em provenance will throw a standard out_of_range
     * exception.
     *
     * \return The requested Provenence instance is returned by reference.
     *
     * \sa Provenance
     */
    const Provenance& getProvenance(
      const size_t& index) const
    {
      return provenance_.at( index);
    }

    /**
     * This function is used to export the \em FileHeader data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address pointer to the parent DOM node/element.
     */
    void exportDefinition(
      DomFunctions::XmlNode& documentElement);

    // @TODO :: Add set parameter functions

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<(
      std::ostream &os,
      const FileHeader &fileHeader);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom(
      const DomFunctions::XmlNode& elementDefinition);

  private:

    /************************************************************************
     * These are pointers to the fileHeader elements, set up during
     * instantiation. */

    ElementDefinitionEnum elementType_;

    dstoute::aString name_;
    AuthorList author_;
    dstoute::aString creationDate_;
    dstoute::aString fileVersion_;
    dstoute::aString classification_;
    dstoute::aString dataAssumptions_;
    dstoute::aString tag_;
    dstoute::aString type_;
    dstoute::aString description_;
    ReferenceList reference_;
    ModificationList modification_;
    ProvenanceList provenance_;

  };

}

#endif /* _FILEHEADER_H_ */
