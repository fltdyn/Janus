#ifndef _PROVENANCE_H_
#define _PROVENANCE_H_

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
// Module:     Provenance.h
// First Date: 2009-05-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Provenance.h
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

// Ute Includes
#include <Ute/aList.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "Author.h"

namespace janus {

  /**
   * A #Provenance instance holds in its allocated memory alphanumeric data
   * derived from a \em provenance element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  Provenances may apply to
   * a complete dataset or to individual components within a dataset.
   * Not all provenances will contain all possible \em provenance
   * components. The #Provenance instance also provides the
   * functions that allow a calling Janus instance to access these data
   * elements.
   *
   * The #Provenance class is only used within the janus namespace, and should
   * only be referenced indirectly through the FileHeader, VariableDef,
   * GriddedTableDef, UngriddedTableDef, Function or CheckData classes.
    *
   * A typical usage is:
  \code
    Janus test( xmlFileName );
    FileHeader header = test.getFileHeader();
    int nProv = header.getProvenanceCount();
    cout << " Number of header provenance elements   : " << nProv << "\n\n";

    for ( int i = 0 ; i < nProv ; i++ ) {
      Provenance provenance = header.getProvenance( i );
      cout << " Header Provenance " << i << "   : \n"
           << "    provID                : "
           << provenance.getProvID() << "\n"
           << "    creationDate          : "
           << provenance.getCreationDate() << "\n";
      for ( int j = 0 ; j < provenance.getAuthorCount() ; j++ ) {
        cout << "     author " << j << "  : "
             << provenance.getAuthor( j ).getName() << "\n";
      }
      cout << "    description           : \n"
           << provenance.getDescription() << "\n\n";
    }
  
    for ( int i = 0 ; i < test.getNumberOfFunctions() ; i++ ) {
      cout << " Function \" "
           << test.getFunctionName( i ) << "\" \n";
      if ( true == test.getFunction().at( i ).hasProvenance() ) {
        Provenance provenance = test.getFunction().at( i ).getProvenance();
        cout << " Provenance \n"
             << "     provID            : "
             << provenance.getProvID() << "\n"
             << "     creation date     : "
             << provenance.getCreationDate() << "\n";
        for ( int j = 0 ; j < provenance.getAuthorCount() ; j++ ) {
          cout << "     author " << j << "  : "
               << provenance.getAuthor( j ).getName() << "\n";
        }
        cout << "    description           : \n"
             << provenance.getDescription() << "\n\n";
      }
    }

  \endcode
  */
  class Provenance : public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #Provenance class without
     * supplying the DOM \em provenence element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em provenance element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Provenance( );

    /**
     * The constructor, when called with an argument pointing to a \em provenance
     * element within a DOM, instantiates the #Provenance class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em provenance component within
     * the DOM.
     */
    Provenance( const DomFunctions::XmlNode& elementDefinition );

    /**
     * An uninitialised instance of #Provenance is filled with data from a
     * particular \em provenance element within a DOM by this function.  If
     * another \em provenance element pointer is supplied to an
     * instance that has already been initialised, the instance will be
     * re-initialised with the new data.  However, this is not a
     * recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address of a \em provenance component within
     * the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition );

    /**
     * The \em provID allows \em provenance elements to be referenced by
     * elements other than their immediate parent.  It is an optional attribute.
     * This function returns the \em provID of the referenced \em provenance
     * element, if one has been supplied in the XML dataset.  If not, it
     * returns an empty string.
     *
     * \return The \em provID string is returned by reference.
     */
    const dstoute::aString& getProvID( ) const
    { return provID_; }

    /**
     * This function returns the number of authors listed in a
     * #Provenance.  If the instance has not been populated from a DOM element,
     * zero is returned.
     *
     * \return An integer number, one or more in a populated instance.
     *
     * \sa Author
     */
    size_t getAuthorCount( ) const
    { return author_.size(); }

    /**
     * This function returns a reference to the ist of Author instances within
     * a #Provenance instance.
     *
     * \return The list of Author instances is returned by reference.
     *
     * \sa Author
     */
    const AuthorList& getAuthor() const
    { return author_; }

    /**
     * This function returns a reference to the selected Author instance within
     * a #Provenance instance.
     *
     * \param index has a range from zero to ( #getAuthorCount() - 1 ),
     * and selects the required Author instance.  An attempt to access
     * a non-existent \em author will throw a standard out_of_range exception.
     *
     * \return The requested Author instance is returned by reference.
     *
     * \sa Author
     */
    const Author& getAuthor( const size_t &index ) const
    { return author_.at( index ); }

    /**
     * This function returns the \em creationDate attribute of a \em provenance
     * element (\em fileCreationDate is a deprecated alternative).
     * The format of the dataset string is determined by the XML dataset builder,
     * but DAVE-ML recommends the ISO 8601 form ("2004-01-02" to refer to
     * 2 January 2004).  If the #Provenance has not been populated from
     * a DOM element , the function returns an empty string.
     *
     * \return The \em creationDate string is returned by reference.
     */
    const dstoute::aString& getCreationDate( ) const
    { return creationDate_; }

    /**
     * This function returns the number of document references listed in a
     * a \em provenance element.  A \em provenance can include no, one or
     * multiple \em documentRef components.  If the #Provenance has not been
     * populated from a DOM element, zero is returned.
     *
     * \return An integer number, zero or more in a populated instance.
     */
    size_t getDocumentRefCount( ) const
    { return documentRefID_.size(); }

    /**
     * This function returns the list of \em refID's from the \em documentRef
     * child element of a \em provenance element (\em docID is a deprecated
     * alternative).
     * The \em refID's allows \em reference elements elsewhere in the DOM to be
     * referenced by elements other than their immediate parent.
     *
     * \return The list of \em refID strings from the
     * selected \em documentRef is returned by reference.
     *
     * \sa Reference
     */
    const dstoute::aStringList& getDocumentRefID() const
    { return documentRefID_; }

    /**
     * This function returns the selected \em refID from the \em documentRef
     * child element of a \em provenance element (\em docID is a deprecated
     * alternative).
     * The \em refID allows \em reference elements elsewhere in the DOM to be
     * referenced by elements other than their immediate parent.
     *
     * \param index has a range from zero to ( #getDocumentRefCount() - 1 ),
     * and selects the required \em documentRef from which the \em refID string
     * is to be returned.  An attempt to access a non-existent \em documentRef
     * will throw a standard out_of_range exception.
     *
     * \return The \em refID string from the selected \em documentRef is
     * returned by reference.
     *
     * \sa Reference
     */
    const dstoute::aString& getDocumentRefID( const size_t &index ) const
    { return documentRefID_.at( index ); }

    /**
     * This function returns the number of modification references listed in
     * a \em provenance element.   A \em provenance can include no, one or
     * multiple \em modificationRef components.  If the #Provenance has not been
     * populated from a DOM element, zero is returned.
     *
     * \return An integer number, zero or more in a populated instance.
     */
    size_t getModificationRefCount( ) const
    { return modificationModID_.size(); }

    /**
     * This function returns the list of \em modID's from the \em modificationRef
     * child element of a \em provenance element.  The \em modID allows
     * \em modificationRecord elements elsewhere in the DOM to be referenced by
     * elements other than their immediate parent.
     *
     * \return The list of \em modID strings from the selected
     * \em modificationRef is returned by reference.
     *
     * \sa Modification
     */
    const dstoute::aStringList& getModificationModID() const
    { return modificationModID_; }

    /**
     * This function returns the selected \em modID from the \em modificationRef
     * child element of a \em provenance element.  The \em modID allows
     * \em modificationRecord elements elsewhere in the DOM to be referenced by
     * elements other than their immediate parent.
     *
     * \param index has a range from zero to ( #getModificationRefCount() - 1 ),
     * and selects the required \em modificationRef from which the \em modID
     * string is to be returned.  An attempt to access a non-existent
     * \em modificationRef will throw a standard out_of_range exception.
     *
     * \return The \em modID string from the selected \em modificationRef is
     * returned by reference.
     *
     * \sa Modification
     */
    const dstoute::aString& getModificationModID( const size_t &index ) const
    { return modificationModID_.at( index ); }

    /**
     * This function returns the \em description from a \em provenance
     * element, if one has been supplied in the XML dataset.
      * The description consists of a string of arbitrary length, which can
     * include tabs and new lines as well as alphanumeric data.  This means
     * text formatting embedded in the XML source will also appear in the returned
     * description string.  If the \em provenance contains no
     * \em description, the returned string is blank.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em provenance data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     * \param isReference a boolean flag indicating the provenance element
     *         should be treated as a reference.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement,
                           const bool &isReference = false);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const Provenance &provenance);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

  private:

    /************************************************************************
     * These are pointers to the provenance elements, set up during
     * instantiation.  Note the `provID' is optional.
     */
    ElementDefinitionEnum elementType_;

    dstoute::aString      provID_;
    AuthorList            author_;
    dstoute::aString      creationDate_;
    dstoute::aString      description_;
    dstoute::aStringList  documentRefID_;
    dstoute::aStringList  modificationModID_;

  };

  typedef dstoute::aList<Provenance> ProvenanceList;
  
}

#endif /* _PROVENANCE_H_ */
