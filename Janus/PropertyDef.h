#ifndef _PROPERTYDEF_H_
#define _PROPERTYDEF_H_

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
// Title:      Janus/PropertyDef
// Class:      PropertyDef
// Module:     PropertyDef.h
// First Date: 2015-11-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file PropertyDef.h
 *
 * A PropertyDef instance holds in its allocated memory data
 * derived from a \em propertyDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
 * alphanumeric identification and cross-reference data.
 * This class sets up a structure that manages the \em propertyDef content.
 *
 * The PropertyDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  G Brian
 *
 */

// C++ Includes

// Ute Includes

// Local Includes
#include "XmlElementDefinition.h"
#include "Provenance.h"

namespace janus {

  class Janus;

  /**
   * A #PropertyDef instance holds in its allocated memory data
   * derived from a \em propertyDef element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
   * alphanumeric identification and cross-reference data.
   *
   * The #PropertyDef class is only used within the janus namespace, and should
   * only be referenced through the Janus class.
   *
   * To determine the characteristics of a dataset's variables, typical usage is:
  \code
    Janus test( xmlFileName );
    vector<PropertyDef> propertyDef = test.getPropertyDef();
    for ( size_t i = 0 ; i < propertyDef.size() ; i++ ) {
      cout << "  Property " << i << " : \n"
           << "   ID           : "
           << propertyDef.at( i ).getPtyID() << "\n"
           << "   Name         : "
           << propertyDef.at( i ).getName() << "\n"
           << "   Description  : "
           << propertyDef.at( i ).getDescription() << "\n"
           << "\n";
    }
  \endcode
   */
  
  class PropertyDef : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #PropertyDef class
     * without supplying the DOM \em propertyDef element from which the instance
     * is constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em propertyDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally for use within higher level
     * instances, where memory needs to be allocated before the data to fill
     * it is specified.
     *
     * \sa initialiseDefinition
     */
    PropertyDef( );

    /**
     * The constructor, when called with an argument pointing to a \em propertyDef
     * element within a DOM, instantiates the #PropertyDef class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em propertyDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    PropertyDef( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #PropertyDef is filled with data from a
     * particular \em propertyDef element within a DOM by this function.  If
     * another \em propertyDef element pointer is supplied to an
     * instance that has already been initialised, data corruption may occur.
     *
     * \param elementDefinition is an address of a \em propertyDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em name attribute of the
     * \em propertyDef element represented by this VariableDef instance.
     * A property's \em name attribute is a string of arbitrary
     * length, but normally short.  It is not used for indexing, and therefore
     * need not be unique (although uniqueness may aid both programmer and
     * user), but should comply with the ANSI/AIAA S-119-2011 standard\latexonly
     * \cite{aiaa_data}\endlatexonly .  If the instance has not been
     * initialised from a DOM, an empty string is returned.
     *
     * \return The \em name string is returned by reference.
     */
    const dstoute::aString& getName( ) const { return name_; }

    /**
     * This function provides access to the \em ptyID attribute
     * of the \em propertyDef element represented by this #PropertyDef instance.
     * A property's \em varID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * property.  It is used for indexing variables within an XML dataset,
     * and provides underlying cross-references for most of the Janus library
     * functionality.  If the instance has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em ptyID string is returned by reference.
     */
    const dstoute::aString& getPtyID( ) const { return ptyID_; }

    /**
     * The \em refID attribute is an optional document reference for a
     * \em propertyDef.  It may be used for cross-referencing a list
     * of references contained in the \em fileHeader.  This function returns the
     * \em refID of a \em propertyDef, if one has been supplied in the
     * XML dataset.  If not, it returns an empty string.
     *
     * \return The \em refID string is returned by reference.
     *
     * \sa Reference
     * \sa FileHeader
     */
    const dstoute::aString& getRefID( ) const { return refID_; }

    /**
     * This function provides access to the optional \em description of the
     * \em propertyDef element represented by this #PropertyDef instance.
     * A \em propertyDef's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting of the XML source
     * will also appear in the returned description.  If no description
     * is specified in the XML dataset, or the #PropertyDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function indicates whether a \em propertyDef element of a
     * DAVE-ML dataset includes either \em provenance or \em provenanceRef.
     *
     * \return A boolean variable, 'true' if the \em function includes a
     * provenance, defined either directly or by reference.
     *
     * \sa Provenance
     */
    const bool& hasProvenance( ) const { return hasProvenance_; }

    /**
     * This function provides access to the Provenance instance
     * associated with a #PropertyDef instance.  There may be zero or one
     * of these elements for each variable definition in a valid dataset,
     * either provided directly or cross-referenced through a
     * \em provenanceRef element.
     *
     * \return The Provenance instance is returned by reference.
     *
     * \sa Provenance
     */
    const Provenance& getProvenance( ) const { return provenance_; }

    /**
     * This function provides access to the \em property content of the
     * #PropertyDef element represented by this #PropertyDef instance.
     *
     * A \em propertyDef's \em property child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means pretty formatting of the XML source
     * will also appear in the returned description. If no property element
     * is specified in the XML dataset, or the #PropertyDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em property string is returned by reference.
     */
    const dstoute::aString& getProperty() const { return propertyList_.front();}

    /**
     * This function provides access to the \em property list content of the
     * #PropertyDef element represented by this #PropertyDef instance.
     *
     * A \em propertyDef's \em property child elements consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means pretty formatting of the XML source
     * will also appear in the returned description. If no property element
     * is specified in the XML dataset, or the #PropertyDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em property list is returned by reference.
     */
    const dstoute::aStringList& getPropertyList() const { return propertyList_;}

    /**
     * This function permits the \em name attribute
     * of the \em propertyDef element to be reset for this #PropertyDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param name a string representation of the \em name attribute.
     */
    void setName( const dstoute::aString& name) { name_ = name; }

    /**
     * This function permits the \em ptyID attribute
     * of the \em propertyDef element to be reset for this #PropertyDef instance.
     * A \em ptyID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * propertyDef.  It is used for indexing property elements within an XML dataset,
     * and provides underlying cross-references if required.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param ptyID a string representation of the \em ptyID identifier.
     */
    void setPtyID( const dstoute::aString& ptyID) { ptyID_ = ptyID; }

    /**
     * This function permits the \em refID attribute of the
     * of the \em propertyDef element to be reset for this #PropertyDef instance.
     *
     * The \em refID attribute is an optional document reference for a
     * \em propertyDef.  It may be used for cross-referencing a list
     * of references contained in the \em fileHeader.
     * A \em refID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * reference.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param refID a string representation of the \em refID identifier.
     */
    void setRefID( const dstoute::aString& refID) { refID_ = refID; }

    /**
     * This function permits the optional \em description of the
     * \em propertyDef element to be reset for this #PropertyDef instance.
     * A \em propertyDef's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means pretty formatting of the XML source
     * will also appear in the returned description.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param description a string representation of the description.
     */
    void setDescription( const dstoute::aString& description)
    { description_ = description; }

    /**
     * This function provides the means to set the content of the property
     * element within this #PropertyDef instance.
     *
     * \param property is a string containing the content to set for the property
     * element of this #PropertyDef instance.
     */
    void setProperty( const dstoute::aString& property)
    { propertyList_.clear(); propertyList_.push_back( property);}

    /**
     * This function provides the means to set the content of the property
     * elements within this #PropertyDef instance.
     *
     * \param propertyList is a string list containing the contents to set for the property
     * elements of this #PropertyDef instance.
     */
    void setPropertyList( const dstoute::aStringList& propertyList)
    { propertyList_ = propertyList;}

    /**
     * This function is used to export the \em propertyDef data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream& os,
                                      const PropertyDef& propertyDef);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

   protected:
    // ---- Internally reference functions. ----


  // private:
    // ---- Internally reference functions. ----

    /************************************************************************
     * These are the variableDef elements, set up during instantiation.
     */
    Janus* janus_;
    ElementDefinitionEnum elementType_;

    dstoute::aString name_;
    dstoute::aString ptyID_;
    dstoute::aString refID_;

    dstoute::aString     description_;
    dstoute::aStringList propertyList_;
    bool isProvenanceRef_;
    bool hasProvenance_;
    Provenance provenance_;
  };

  typedef std::vector<PropertyDef> PropertyDefList;

}

#endif /* _PROPERTYDEF_H_ */
