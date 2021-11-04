#ifndef _MODIFICATION_H_
#define _MODIFICATION_H_

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
// Module:     Modification.h
// First Date: 2009-05-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Modification.h
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

// Ute Includes
#include <Ute/aList.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "Author.h"

namespace janus {

  /**
   * A #Modification instance holds in its allocated memory alphanumeric data
   * derived from a \em modificationRecord element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  The instance describes
   * the author and content of a modification to a dataset.  The class also
   * provides the functions that allow a calling Janus instance to access
   * these data elements.
   *
   * The #Modification class is only used within the janus namespace, and should
   * only be referenced indirectly through the FileHeader class.
   *
   * A typical usage is:
  \code
    Janus test( xmlFileName );
    FileHeader header = test.getFileHeader();
    int nMod = header.getModificationCount();
    cout << " Number of modification records   : " << nMod << "\n\n";

    for ( int i = 0 ; i < nMod ; i++ ) {
      Modification modification = header.getModification( i );
      cout << " Modification Record " << i << " : \n"
           << "   modID               : "
           << modification.getModID( ) << "\n"
           << "   date                : "
           << modification.getDate( ) << "\n"
           << "   refID               : "
           << modification.getRefID( ) << "\n\n";
      int nModAuthors = modification.getAuthorCount( );
      for ( int j = 0 ; j < nModAuthors ; j ++ ) {
        Author author = modification.getAuthor( j );
        cout << " Author " << j << " : Name                  : "
             << author.getName( ) << "\n"
             << "          Organisation            : "
             << author.getOrg( ) << "\n";
      }
      cout << "   description         : "
           << modification.getDescription( )
           << "\n" << "\n";

      int nExdoc = modification.getExtraDocCount( );
      cout << " Number of extra documents related to modification : "
           << nExdoc << "\n\n";

      for ( int j = 0 ; j < nExdoc ; j++ ) {
        cout << "  Extra document " << j << " refID : "
             << modification.getExtraDocRefID( j ) << "\n";
      }
      cout << "\n";
    }
  \endcode
   */
  class Modification : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #Modification class
     * without supplying the DOM \em modificationRecord element from which the
     * instance is constructed, but in this state is not useful for any class
     * functions.
     * It is necessary to populate the class from a DOM containing a
     * \em modificationRecord element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Modification( );

    /**
     * The constructor, when called with an argument pointing to a
     * \em modificationRecord element within a DOM, instantiates the
     * #Modification class and fills it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em modificationRecord
     * component within the DOM.
     */
    Modification( const DomFunctions::XmlNode& elementDefinition );

    /**
     * An uninitialised instance of #Modification is filled with data from a
     * particular \em modificationRecord element within a DOM by this function.
     * If another \em modificationRecord element pointer is supplied to an
     * instance that has already been initialised, the instance will be
     * re-initialised with the new data.  However, this is not a
     * recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address of a \em modificationRecord
     * component within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition );

    /**
     * A \em modID is a single letter used to identify all modified data
     * associated with a modification record.  It allows \em modificationRecord
     * elements to be referenced by elements other than their immediate parent.
     * This function returns the \em modID of the #Modification
     * instance.
     *
     * \return The \em modID string is returned by reference.
     */
    const dstoute::aString& getModID( ) const { return modID_; }

    /**
     * This function returns the \em date attribute of a
     * \em modificationRecord.  The format of the dataset string is determined
     * by the XML dataset builder, but DAVE-ML recommends the ISO 8601 form
     * ("2004-01-02" to refer to 2 January 2004). If the #Modification has not
     * been populated from the DOM element, the function returns an empty string.
     *
     * \return The \em date string is returned by reference.
     */
    const dstoute::aString& getDate( ) const { return date_; }

    /**
     * The \em refID attribute is an optional document reference for a
     * \em modificationRecord.  It may be used for cross-referencing a list
     * of references contained in the \em fileHeader.  This function returns the
     * \em refID of a \em modificationRecord, if one has been supplied in the
     * XML dataset.  If not, it returns an empty string.
     *
     * \return The \em refID string is returned by reference.
     *
     * \sa Reference
     * \sa FileHeader
     */
    const dstoute::aString& getRefID( ) const { return refID_; }

    /**
     * This function returns the number of authors listed in a #Modification.
     * If the instance has not been populated from a DOM element, zero is
     * returned.
     *
     * \return An integer number, one or more in a populated instance.
     *
     * \sa Author
     */
    size_t getAuthorCount( ) const { return author_.size(); }

    /**
     * This function returns a reference to the is of Author instances within
     * a #Modification instance.
     *
     * \return The list of Author instances is returned by reference.
     *
     * \sa Author
     */
    const AuthorList& getAuthor( ) const
    { return author_; }

    /**
     * This function returns a reference to the selected Author instance within
     * a #Modification instance.
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
     * This function returns the \em description from a
     * \em modificationRecord, if one has been supplied in the XML dataset.
     * The description consists of a string of arbitrary length, which can
     * include tabs and new lines as well as alphanumeric data.  This means
     * text formatting embedded in the XML source will also appear in the returned
     * description string. If the \em modificationRecord contains no
     * \em description, the returned string is blank.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function returns the number of \em extraDocRef elements listed in
     * a \em modificationRecord.  A #Modification can have no,
     * one or multiple \em extraDocRef components.  If the
     * instance has not been populated from a DOM element, zero is returned.
     *
     * \return An integer number, zero or more in a populated instance.
     */
    size_t getExtraDocCount( ) const { return extraDocRefID_.size(); }

    /**
     * This function returns the \em refID of a selected \em extraDocRef
     * component from a \em modificationRecord.  A #Modification
     * can have no, one or multiple \em extraDocRef components.
     * The \em refID provides cross-referencing to \em reference definitions
     * elsewhere in the FileHeader.
     *
     * \param index has a range from zero to ( #getExtraDocCount() - 1 ), and
     * selects the required \em extraDocRef record.  An attempt to access
     * a non-existent \em extraDocRef will throw a standard out_of_range
     * exception.
     *
     * \return The requested \em refID string is returned by reference.
     *
     * \sa Reference
     */
    const dstoute::aString& getExtraDocRefID( const size_t &index ) const
    { return extraDocRefID_.at( index ); }

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em Function data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const Modification &function);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

   private:

    /************************************************************************
     * These are pointers to the modification elements, set up during
     * instantiation. */
    ElementDefinitionEnum elementType_;

    dstoute::aString     modID_;
    dstoute::aString     date_;
    dstoute::aString     refID_;
    AuthorList           author_;
    dstoute::aString     description_;
    dstoute::aStringList extraDocRefID_;

  };
  
  typedef dstoute::aList<Modification> ModificationList;
  
}

#endif /* _MODIFICATION_H_ */
