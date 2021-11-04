#ifndef _REFERENCE_H_
#define _REFERENCE_H_

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
// Module:     Reference.h
// First Date: 2009-05-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Reference.h
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

// Ute Includes
#include <Ute/aList.h>

// Local Includes
#include "XmlElementDefinition.h"

namespace janus {

  /**
   * A #Reference instance holds in its allocated memory alphanumeric data
   * derived from a \em reference element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  The instance describes an
   * external document relevant to the dataset.  The class also provides the
   * functions that allow a calling Janus instance to access these data
   * elements.
   *
   * The #Reference class is only used within the janus namespace, and should
   * only be referenced indirectly through the FileHeader class.
   * A typical usage is:
  \code
    Janus test( xmlFileName );
    FileHeader header = test.getFileHeader();
    int nRef = header.getReferenceCount();
    cout << " Number of reference records      : " << nRef << "\n\n";

    for ( int i = 0 ; i < nRef ; i++ ) {
      Reference reference = header.getReference( i );
      cout << " Reference " << i << "    : \n"
           << "   xmlns:xlink    : "
           << reference.getXLink( ) << "\n"
           << "   xlink:type     : "
           << reference.XLinkType( ) << "\n"
           << "   refID          : "
           << reference.getRefID( ) << "\n"
           << "   author         : "
           << reference.getAuthor( ) << "\n"
           << "   title          : "
           << reference.getTitle( ) << "\n"
           << "   date           : "
           << reference.getDate( ) << "\n"
           << "   classification : "
           << reference.getClassification( ) << "\n"
           << "   accession   : "
           << reference.getAccession( ) << "\n"
           << "   xlink:href     : "
           << reference.getHref( ) << "\n"
           << "   description    : "
           << reference.getDescription( ) << "\n"
           << "\n";
    }
  \endcode
   */
  class Reference : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #Reference class without
     * supplying the DOM \em reference element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em reference element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Reference( );

    /**
     * The constructor, when called with an argument pointing to a \em reference
     * element within a DOM, instantiates the #Reference class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em reference component
     * within the DOM.
     */
    Reference( const DomFunctions::XmlNode& elementDefinition );

    /**
     * An uninitialised instance of #Reference is filled with data from a
     * particular \em reference element within a DOM by this function.  If
     * another \em reference element pointer is supplied to an instance that
     * has already been initialised, the instance will be
     * re-initialised with the new data.  However, this is not a
     * recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address of a \em reference component
     * within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition );

    /**
     * This function returns the \em xmlns:xlink associated with a #Reference
     * instance.
     * If the instance has not been initialised from a DOM, the string is
     * is set to "http://www.w3.org/19999/xlink", and returned.
     *
     * \return The \em xmlns:xlink string is returned by reference.
     */
    const dstoute::aString& getXLink( ) const { return xlink_; }

    /**
     * This function returns the \em xlink:type associated with a #Reference
     * instance.
     * If the instance has not been initialised from a DOM, the string is
     * is set to "simple", and returned.
     *
     * \return The \em xlink:type string is returned by reference.
     */
    const dstoute::aString& getXLinkType( ) const { return type_; }

    /**
     * This function returns the \em refID associated with a #Reference
     * instance.  The \em refID allows \em reference elements to be
     * cited by elements throughout the DOM, by elements other than their
     * immediate parent, \em fileHeader.
     * If the instance has not been initialised from a DOM, an empty string is
     * returned.
     *
     * \return The \em refID string is returned by reference.
     */
    const dstoute::aString& getRefID( ) const { return refID_; }

    /**
     * This function returns the \em author attribute of a \em reference
     * element.  The \em author attribute is a string containing the name
     * of the author of the referenced document.  If the instance has not
     * been initialised from a DOM, an empty string is returned.
     *
     * \return The \em author string is returned by reference.
     */
    const dstoute::aString& getAuthor( ) const { return author_; }

    /**
     * This function returns the \em title attribute of a \em reference
     * element.  The \em title attribute is a string containing the title
     * of the referenced document.  If the instance has not been
     * initialised from a DOM, an empty string is returned.
     *
     * \return The \em title string is returned by reference.
     */
    const dstoute::aString& getTitle( ) const { return title_; }

    /**
     * This function returns the \em date attribute of a \em reference
     * element.  The \em date attribute is a string containing the
     * publication date of the referenced document.  If the instance
     * has not been initialised from a DOM, an empty string
     * is returned.
     *
     * \return The \em date string is returned by reference.
     */
    const dstoute::aString& getDate( ) const { return date_; }

    /**
     * This function returns the \em classification attribute of a
     * \em reference element.  The \em classification attribute is
     * a string containing the security classification of the referenced
     * document.  This is an optional attribute.
     * If the #Reference instance does not contain a \em classification
     * attribute, or has not been initialised from a DOM, an empty string is
     * returned.
     *
     * \return The \em classification string is returned by reference.
     */
    const dstoute::aString& getClassification( ) const { return classification_; }

    /**
     * This function returns the \em accession attribute of a
     * \em reference element.  The \em accession attribute is
     * a string containing the accession number (ISBN or organisation
     * report number) of the referenced document.  This is an optional attribute.
     * If the #Reference instance does not contain a \em accession
     * attribute, or has not been initialised from a DOM, an empty string is
     * returned.
     *
     * \return The \em accession string is returned by reference.
     */
    const dstoute::aString& getAccession( ) const { return accession_; }

    /**
     * This function returns the \em xlink:href attribute of a
     * \em reference element.  The \em xlink:href attribute is
     * a string containing a URL of an on-line copy of the referenced
     * document.  This is an optional attribute.
     * If the #Reference instance does not contain a \em xlink:href
     * attribute, or has not been initialised from a DOM, an empty string is
     * returned.
     *
     * \return The \em xlink:href string is returned by reference.
     */
    const dstoute::aString& getHref( ) const { return href_; }

    /**
     * This function returns the \em description child element of a
     * \em reference instance.  The \em description child element is
     * a (possibly lengthy) string containing information regarding the
     * referenced document, whose format within the XML dataset will be
     * preserved by this function.  It is an optional attribute.
     * If the #Reference instance does not contain a \em description
     * attribute, or has not been initialised from a DOM, an empty string is
     * returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em author data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const Reference &reference);

    // ---- Internally reference functions. ----

   private:

    /************************************************************************
     * These are pointers to the reference elements, set up during
     * instantiation. */

    dstoute::aString xlink_;
    dstoute::aString type_;
    dstoute::aString refID_;
    dstoute::aString author_;
    dstoute::aString title_;
    dstoute::aString date_;
    dstoute::aString classification_;
    dstoute::aString accession_;
    dstoute::aString href_;
    dstoute::aString description_;

  };

  typedef dstoute::aList<Reference> ReferenceList;
  
}

#endif /* _REFERENCE_H_ */
