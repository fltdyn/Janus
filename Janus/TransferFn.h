#ifndef _TRANSFERFN_H_
#define _TRANSFERFN_H_

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
// Title:      Janus/TransferFn
// Class:      TransferFn
// Module:     TransferFn.h
// First Date: 2015-11-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file TransferFn.h
 *
 * A TransferFn instance holds in its allocated memory alphanumeric data
 * derived from a dynamic systems model \em transferFn element of a DOM
 * corresponding to a DAVE-ML compliant XML dataset source file.
 *
 * It includes entries arranged as follows:
 *  Entries for the numerator and denominator of the transfer function, which
 *  respectively contains coefficient data for each parameter.
 *
 * The TransferFn class is only used within the janus namespace, and should only
 * be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 */

// C++ Includes

// Local Includes
#include "XmlElementDefinition.h"
#include "Numerator.h"
#include "Denominator.h"
#include "Provenance.h"

namespace janus
{

  class Janus;

  /**
   * An #TransferFn instance holds in its allocated memory alphanumeric data
   * derived from a dynamic systems model \em transferFn element of a DOM
   * corresponding to a DAVE-ML compliant XML dataset source file.
   *
   * It includes entries arranged as follows:
   *  Entries for the numerator and denominator of the transfer function, which
   *  respectively contains coefficient data for each parameter.
   *
   * The #TransferFn class is only used within the janus namespace, and should only
   * be referenced through the Janus class.
   */
  class TransferFn: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #TransferFn class
     * without supplying the dynamic system model DOM \em transferFn element
     * from which the instance is constructed, but in this state it is not useful for
     * any class functions.  It is necessary to populated the class from
     * a DOM containing an \em transferFn element before any further use of
     * the instanced class.
     *
     * This form of the constructor is principally for use within
     * higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    TransferFn();

    /**
     * The constructor, when called with an argument pointing to a dynamic system
     * model \em transferFn element within a DOM, instantiates the
     * #TransferFn class and fills it with alphanumeric data from the DOM.
     * String-based numeric data are converted to double-precision linear
     * vectors.
     *
     * \param elementDefinition is an address of an \em transferFn
     * component node within the DOM.
     */
    TransferFn( const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #TransferFn is filled with data from a
     * particular dynamic systems model \em transferFn element within a DOM
     * by this function.  If another \em transferFn element pointer is supplied
     * to an instance that has already been initialised, data corruption will
     * occur and the entire Janus instance will become unusable.
     *
     * \param elementDefinition is an address of an \em transferFn
     * component node within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em name attribute of the
     * \em transferFn element represented by this #TransferFn instance.
     * A transferFn's \em name attribute is a string of arbitrary
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
     * This function provides access to the \em tfID attribute
     * of the \em transferFn element represented by this #TransferFn instance.
     * A transferFn's \em tfID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * property.  It is used for indexing variables within an XML dataset,
     * and provides underlying cross-references for most of the Janus library
     * functionality.  If the instance has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em tfID string is returned by reference.
     */
    const dstoute::aString& getTFID( ) const { return tfID_; }

    /**
     * This function provides access to the \em order attribute of the
     * \em transferFn element represented by this #TransferFn instance.
     * If the instance has not been initialised from a DOM, zero is returned.
     *
     * \return The \em order is returned as a unsigned integer.
     */
    const size_t& getOrder( ) const { return order_; }

    /**
     * This function provides access to the optional \em description of the
     * \em transferFn element represented by this #TransferFn instance.
     * A \em transferFn's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #TransferFn has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function provides access to the \em numerator element of the
     * transfer function. The numerators numeric coefficient data can be
     * accessed through the numerator class.
     *
     * \return The address of the numerator element.
     */
    const Numerator& getNumerator() const { return numerator_;}

    /**
     * This function provides access to the \em denominator element of the
     * transfer function. The \em denominator numeric coefficient data can be
     * accessed through the \em denominator class.
     *
     * \return The address of the numerator element.
     */
    const Denominator& getDenominator() const { return denominator_;}

    /**
     * This function indicates whether a \em transferFn element of a
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
     * associated with a #TransferFn instance.  There may be zero or one
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
     * This function permits the \em name attribute
     * of the \em transferFn element to be reset for this #TransferFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param name a string representation of the \em name attribute.
     */
    void setName( const dstoute::aString& name) { name_ = name; }

    /**
     * This function permits the \em tfID attribute
     * of the \em transferFn element to be reset for this #TransferFn instance.
     * A \em transFnID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * trransferFn.  It is used for indexing transfer functions within an XML dataset,
     * and provides underlying cross-references if required.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param tfID a string representation of the \em tfID identifier.
     */
    void setTFID( const dstoute::aString& tfID) { tfID_ = tfID; }

    /**
     * This function permits the \em order attribute
     * of the \em transferFn element to be reset for this #TransferFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param order an unsigned integer representation of the \em order attribute.
     */
    void setOrder ( const size_t& order) { order_ = order; }

    /**
     * This function permits the optional \em description of the
     * \em transferFn element to be reset for this #TransferFn instance.
     * A \em transferFn's \em description child element consists
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
     * This function is used to export the \em transferFn data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     * \param isReference a boolean flag indicating the transferFn element
     *         should be treated as a reference.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement,
    	                   const bool &isReference);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os, const TransferFn &transferFn);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

  private:
    /************************************************************************
     * These are the transferFn elements, set up during instantiation.
     */
    ElementDefinitionEnum elementType_;

    size_t           order_;
    dstoute::aString name_;
    dstoute::aString tfID_;
    dstoute::aString description_;

    Numerator        numerator_;
    Denominator      denominator_;

    bool             isProvenanceRef_;
    bool             hasProvenance_;
    Provenance       provenance_;

  };
}

#endif /* _TRANSFERFN_H_ */
