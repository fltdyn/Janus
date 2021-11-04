#ifndef _AUTHOR_H_
#define _AUTHOR_H_

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
// Module:     Author.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file Author.h
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

// C++ Includes 
// Ute Includes
#include <Ute/aString.h>

// Local Includes
#include "XmlElementDefinition.h"

namespace janus
{
  /**
   * An #Author instance holds in its allocated memory alphanumeric data
   * derived from an \em author element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  The instance may describe
   * an author of a complete dataset, or of a component of a dataset, or of a
   * modification to a dataset.  Author contact details may be expressed in
   * either \em address or \em contactInfo forms.  The \em contactInfo form is
   * newer, more flexible and generally preferred.  The class also provides the
   * functions that allow a calling Janus instance to access these data
   * elements.
   *
   * The #Author class is only used within the janus namespace, and should
   * only be referenced indirectly through the FileHeader, Modification or
   * Provenance classes.
   *
   * Typical usage might be:
    \code
      Janus test( xmlFileName );
      int nAuthors = test.getFileHeader().getAuthorCount( );
      cout << "Number of authors : " << nAuthors << "\n\n";
      for ( size_t i = 0 ; i < nAuthors ; i++ ) {
        Author author = test.getFileHeader().getAuthor( i );
        cout << " Author " << i << " : Name                  : "
             << author.getName( ) << "\n"
             << "          Organisation            : "
             << author.getOrg( ) << "\n"
             << "          Email                   : "
             << author.getEmail( ) << "\n\n";
        for ( size_t j = 0 ; j < author.getAddressCount() ; j++ ) {
          cout << "          Address " << j << "               : "
               << author.getAddress( j ) << "\n\n";
        }
        for ( size_t j = 0 ; j < author.getContactInfoCount() ; j++ ) {
          cout << "          Contact " << j << " type          : "
               << author.getContactInfoType( j ) << "\n"
               << "                    location      : "
               << author.getContactLocation( j ) << "\n"
               << "                    content       : "
               << author.getContactInfo( j )
               << "\n\n";
        }
      }
    \endcode
   */
  class Author: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #Author class without
     * supplying the DOM \em author element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing an
     * \em author element before any further use of the instanced class.
     *
     * This form of the constructor is principally for use within higher level
     * instances, where memory needs to be allocated before the data to fill it
     * is specified.
     *
     * \sa initialiseDefinition
     */
    Author();

    /**
     * The constructor, when called with an argument pointing to an \em author
     * element within a DOM, instantiates the #Author class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param authorElement is an address of an \em author
     * component node within the DOM.
     */
    Author( const DomFunctions::XmlNode& authorElement);

    /**
     * An uninitialised instance of #Author is filled with data from a
     * particular \em author element within a DOM by this function.
     * If another \em author element pointer is supplied to an
     * instance that has already been initialised, the instance will be
     * re-initialised with the new data.  However, this is not a
     * recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address of an \em author
     * component node within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function returns the author's \em name from the referenced #Author
     * instance.  If the instance has not been initialised from a DOM, an
     * empty string is returned.
     *
     * \return The \em name string is passed by reference.
     */
    const dstoute::aString& getName() const
    {
      return name_;
    }

    /**
     * This function returns the author's \em org attribute from the
     * referenced #Author instance.  The \em org attribute is a descriptive
     * string identifying the author's employing organisation.  If the
     * instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The \em org string is passed by reference.
     */
    const dstoute::aString& getOrg() const
    {
      return org_;
    }

    /**
     * This function returns the author's \em xns attribute from the
     * referenced #Author instance.  The \em xns attribute is a descriptive
     * string containing the author's eXtensible Name Service identifier.
     * This is an optional attribute.
     *
     * \return The \em xns string is passed by reference.  If the #Author
     * instance has not been initialised or does not contain an \em xns
     * attribute, an empty string is returned.
     */
    const dstoute::aString& getXns() const
    {
      return xns_;
    }

    /**
     * This function returns the author's \em email attribute from the
     * referenced #Author instance.  The \em email attribute contains the
     * author's email address.  This is an optional attribute.
     *
     * \return The \em email string is passed by reference.  If the
     * #Author instance has not been initialised or does not contain an
     * \em email attribute, an empty string is returned.
     */
    const dstoute::aString& getEmail() const
    {
      return email_;
    }

    /**
     * This function returns the number of addresses listed in an
     * #Author instance.  An instance can have no, one or multiple \em address
     * components.  The \em address and \em contactInfo components are
     * mutually exclusive alternatives.  If the instance has not been populated
     * from a DOM element, zero is returned.
     *
     * \return An integer number, zero or more in a populated instance.
     * \sa getContactInfoCount
     */
    size_t getAddressCount() const
    {
      return address_.size();
    }

    /**
     * This function returns the \em address list from an #Author instance.
     *
     * \return The \em address list is passed as a reference to string list of
     * \em address entries.
     */
    const dstoute::aStringList& getAddress() const
    {
      return address_;
    }

    /**
     * This function returns a selected \em address component from an
     * #Author instance.
     *
     * \param index has a range from zero to ( #getAddressCount() - 1 ),
     * and selects the required \em address component.  An attempt to access
     * a non-existent \em address will throw a standard out_of_range exception.
     *
     * \return The selected \em address string is passed by reference.
     */
    const dstoute::aString& getAddress( const size_t &index) const
    {
      return address_.at( index);
    }

    /**
     * This function returns the number of \em contactInfo components listed
     * in the referenced #Author instance.  An instance can have no, one or
     * multiple \em contactInfo components.  The \em contactInfo and \em address
     * components are mutually exclusive alternatives.  If the instance has not
     * been populated from a DOM element, zero is returned.
     *
     * \return An integer number, zero or more in a populated instance.
     * \sa getAddressCount
     */
    size_t getContactInfoCount() const
    {
      return contactInfoContent_.size();
    }

    /**
     * This function returns the \em contactInfoType of a selected
     * \em contactInfo component from an #Author instance.
     *
     * \param index has a range from zero to ( #getContactInfoCount() - 1 ),
     * and selects the required \em contactInfo component.  An attempt to access
     * a non-existent \em contactInfo will throw a standard out_of_range
     * exception.
     *
     * \return The selected \em contactInfoType string is passed by reference.
     */
    const dstoute::aString& getContactInfoType( const size_t &index) const
    {
      return contactInfoType_.at( index);
    }

    /**
     * This function returns the \em contactLocation of a selected
     * \em contactInfo component from an #Author instance.
     *
     * \param index has a range from zero to ( #getContactInfoCount() - 1 ),
     * and selects the required \em contactInfo component.  An attempt to access
     * a non-existent \em contactInfo will throw a standard out_of_range
     * exception.
     *
     * \return The selected \em contactLocation string is passed by reference.
     */
    const dstoute::aString& getContactLocation( const size_t &index) const
    {
      return contactLocation_.at( index);
    }

    /**
     * This function returns the content of a selected
     * \em contactInfo component from an #Author instance.
     *
     * \param index has a range from zero to ( #getContactInfoCount() - 1 ),
     * and selects the required \em contactInfo component.  An attempt to access
     * a non-existent \em contactInfo will throw a standard out_of_range
     * exception.
     *
     * \return The selected \em contactInfo content string is passed by reference.
     */
    const dstoute::aString& getContactInfo( const size_t &index) const
    {
      return contactInfoContent_.at( index);
    }

    /**
     * This function is used to export the \em author data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     * \param authorTag a string specifying the tag to use when exporting
     * the \em Author element. The default tag is \em author, with other
     * an alternative being \em pilot when used by higher-level applications
     * such as \em Thames.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement,
                           const dstoute::aString& authorTag = "author");

    /**
     * This function permits the \em name attribute of the
     * \em author element to be reset for this Author instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param name a string detailing the author's name.
     */
    void setName( const dstoute::aString &name)
    {
      name_ = name;
    }

    /**
     * This function permits the \em org attribute of the
     * \em author element to be reset for this Author instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param org a string detailing the name of the author's organisation.
     */
    void setOrg( const dstoute::aString &org)
    {
      org_ = org;
    }

    /**
     * This function permits the \em xns attribute of the
     * \em author element to be reset for this Author instance.
     * The \em xns attribute is a descriptive string containing the
     * author's eXtensible Name Service identifier.
     * This is an optional attribute.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param xns a string detailing the author's xns attribute.
     */
    void setXns( const dstoute::aString &xns)
    {
      xns_ = xns;
    }

    /**
     * This function permits the author's \em email attribute of the
     * \em author element to be reset for this Author instance.
     * The \em email attribute contains the author's email address.
     * This is an optional attribute.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param email a string detailing the address to be stored in the author's email attribute.
     */
    void setEmail( const dstoute::aString &email)
    {
      email_ = email;
    }

    /**
     * This function permits the \em address vector of the
     * \em author element to be reset for this Author instance.
     * An alternative is to populate the \em contactInfo entries of the
     * Author instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param address a string list containing address entries for the named author.
     */
    void setAddress( const dstoute::aStringList& address)
    {
      address_ = address;
    }

    /**
     * This function permits the vector of \em contactInfo of the
     * \em author element to be reset for this Author instance. The element content is set
     * through this function, with the type and location attributes populated using the
     * \em setContactInfoType() and \em setContactLocation() functions.
     * An alternative is to populate the \em address entries of the Author instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param contactInfo a string list containing contact information for the named author.
     */
    void setContactInfo( const dstoute::aStringList& contactInfo)
    {
      contactInfoContent_ = contactInfo;
    }

    /**
     * This function permits the vector of \em contactInfoType data of the
     * \em author element to be reset for this Author instance. These data are an
     * attribute of the \em contactInfo element.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param contactInfoType a string list containing contact type
     * information (as defined in the DAVE-ML dtd) for the named author.
     */
    void setContactInfoType( const dstoute::aStringList& contactInfoType)
    {
      contactInfoType_ = contactInfoType;
    }

    /**
     * This function permits the vector of \em contactLocation data of the
     * \em author element to be reset for this Author instance. These data are an
     * attribute of the \em contactInfo element.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param contactLocation a string list containing contact location
     * information (as defined in the DAVE-ML dtd) for the named author.
     */
    void setContactLocation( const dstoute::aStringList& contactLocation)
    {
      contactLocation_ = contactLocation;
    }

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os, const Author &author);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);

  private:
    /************************************************************************
     * These are the author elements, set up during instantiation.
     */
    dstoute::aString name_;
    dstoute::aString org_;
    dstoute::aString xns_;
    dstoute::aString email_;
    dstoute::aStringList address_;
    dstoute::aStringList contactInfoType_;
    dstoute::aStringList contactLocation_;
    dstoute::aStringList contactInfoContent_;

  };

  typedef dstoute::aList< Author> AuthorList;

}

#endif /* _AUTHOR_H_ */
