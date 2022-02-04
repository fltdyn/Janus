#ifndef _SIGNALDEF_H_
#define _SIGNALDEF_H_

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
// Title:      Janus/Signal
// Class:      SignalDef
// Module:     SignalDef.h
// First Date: 2017-09-07
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file SignalDef.h
 *
 * A SignalDef instance holds in its allocated memory alphanumeric data
 * derived from a \em signalDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance may describe
 * inputs, internal values of a computation, or outputs.  The class also
 * provides the functions that allow a calling StaticShot instance to access
 * these data elements. It is used to document the name, ID, value, tolerance,
 * and units of measure for checkcases.
 *
 * A \em signalDef must have attributes of a \em name, an \em ID and \em units.
 * An optional \em symbol attribute may also be defined.
 *
 * Additionally, a reference to a variable definition must be provided, together
 * with a type tag (either \em sigInput, \em sigInternal, or \em sigOutput) to
 * identify the what check part of a \em staticShot the signal is associated with.
 * The value of the signal must also be provided, either as a scaler or an array
 * of data using the \em signalValue sub-element. An optional signal
 * \em description may be provided
 *
 * The SignalDef class is only used within the janus namespace, and should
 * only be referenced indirectly through the StaticShot, CheckInputs,
 * InternalValues and CheckOutputs classes.
 */

/*
 * Author:  G. Brian
 */

// C++ Includes 
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif

// Ute Includes
#include <Ute/aList.h>
#include <Ute/aString.h>

// Local Includes
#include "XmlElementDefinition.h"

namespace janus {

  class SignalDef : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #SignalDef class without
     * supplying the DOM \em signalDef element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em signalDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally for use within higher level
     * instances, where memory needs to be allocated before the data to fill it
     * is specified.
     */
    SignalDef();

    /**
     * The constructor, when called with an argument pointing to a \em signalDef
     * element within a DOM, instantiates the #SignalDef class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em signalDef component
     * within the DOM.
     * \param signalType is a enumeration identifying the signal as either an
     * input, and output, or an internal value.
     */
    SignalDef( const DomFunctions::XmlNode& elementDefinition,
               const SignalTypeEnum &signalType = SIGNAL_NOSET);

    /**
     * An uninitialised instance of #SignalDef is filled with data from a
     * particular \em signalDef element within a DOM by this function.  If another
     * \em signalDef element pointer is supplied to an instance that has already
     * been initialised, the instance will be re-initialised with the new data.
     * However, this is not a recommended procedure, since optional elements may
     * not be replaced.
     *
     * \param elementDefinition is an address of a \em signalDef component
     * within the DOM.
     * \param signalType is a enumeration identifying the signal as either an
     * input, and output, or an internal value.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
                               const SignalTypeEnum &signalType = SIGNAL_NOSET);

    /**
     * This function returns the content of the signal's \em signalName child
     * element.  If the instance has not been initialised from a DOM, an
     * empty string is returned.
     *
     * \return The \em signalName content string is returned by reference.
     */
    const dstoute::aString& getName( ) const { return name_; }

    /**
     * This function provides access to the \em sigID attribute
     * of the \em signalDef element represented by this #SignalDef instance.
     * A signal's \em sigID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * signal.  It is used for indexing signals within an XML dataset,
     * and provides underlying cross-references for most of the Janus library
     * functionality.  If the instance has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em sigID string is returned by reference.
     */
    const dstoute::aString& getSigID( ) const { return sigID_; }

    /**
     * This function returns the content of the signal's \em units attribute.
     * The \em units content is a string of arbitrary length, but normally short,
     * and complying with the format requirements chosen by \htmlonly AD
     * APS\endhtmlonly \latexonly \ac{AD} \ac{APS} \cite{brian1}\endlatexonly in
     * accordance with \htmlonly SI\endhtmlonly \latexonly\ac{SI} \endlatexonly
     * and other systems. If the #SignalDef has not been initialised from a DOM,
     * an empty string is returned.
     *
     * \return The \em units content string is returned by reference.
     */
    const dstoute::aString& getUnits( ) const { return units_; }

    /**
     * This function returns the content of the signal's \em symbol attribute.
     * A signalDef's \em symbol attribute contains a Unicode representation of
     * the symbol associated with a signal represented by the \em signalDef.
     * A typical example might be \latexonly $ \alpha $\endlatexonly
     * \htmlonly <var>&alpha;</var> \endhtmlonly associated with angle of attack.
     * If no symbol is specified in the XML dataset, or the #SignalDef has not
     * been initialised from the DOM, a blank Unicode character is returned.
     *
     * \return The \em symbol Unicode character is returned by reference.
     */
    const dstoute::aString& getSymbol( ) const { return symbol_; }

    /**
     * This function provides access to the optional \em description of the
     * \em signalDef element represented by this #SignalDef instance.
     * A \em signalDef's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #SignalDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function returns the content of the signal's \em variableRef child
     * sub-element, instantiated as a \em varID. The \em varID is a unique
     * (per list of check case elements), short string not including whitespace
     * that indicates the VariableDef corresponding to the signal definition.
     * It is used for signal indexing.
     *
     * If the #Signal has not been initialised from a DOM, an empty string
     * is returned.
     *
     * \return The \em varID content string is returned by reference.
     */
    const dstoute::aString& getVarID( ) const { return varID_; }

    /**
     * This function returns the content of the signal's \em variableRef child
     * sub-element, instantiated as an \em index to the \em variableDef in the
     * global list. It is used for signal indexing.
     *
     * If the #Signal has not been initialised from a DOM, an INVALID_INDEX
     * is returned.
     *
     * \return The \em varIndex is returned by reference.
     */
    const dstoute::aOptionalSizeT& getVarIndex( ) const { return varIndex_; }

    /**
     * This function indicates whether a \em signalDef element of a
     * DAVE-ML dataset has been formally designated as an input signal using
     * the \em sigInput child sub-element.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as an input signal.
     */
    const bool& isSigInput( ) const { return isSigInput_; }

    /**
     * This function indicates whether a \em signalDef element of a
     * DAVE-ML dataset has been formally designated as an internal signal using
     * the \em sigInternal child sub-element.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as an internal signal.
     */
    const bool& isSigInternal( ) const { return isSigInternal_; }

    /**
     * This function indicates whether a \em signalDef element of a
     * DAVE-ML dataset has been formally designated as an output signal using
     * the \em sigOutput child sub-element.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as an output signal.
     */
    const bool& isSigOutput( ) const { return isSigOutput_; }

    /**
     * This function returns the content of the signal's \em signalValue child
     * sub-element. It represent the numeric values that particulars variable(s)
     * from the XML dataset should return for the check case that forms the
     * parent of this signal.
     *
     * If the #SignalDef has not been populated from a DOM element, an empty
     * list is returned.
     *
     * \return A list of double precision values are returned.
     */
    const dstoute::aDoubleList& getValue( ) const { return value_; }

    /**
     * This function returns the number of numeric values contained within the
     * signal's \em signalValue child sub-element.
     *
     * If the #SignalDef has not been populated from a DOM element, zero
     * is returned.
     *
     * \return A count of the number of values is returned.
     */
    size_t getValueCount() const { return value_.size();}

    /**
     * This function returns the content of the signal's \em tol[erance] child
     * sub-element. It represent the numeric values that are used to check the
     * accuracy of calculations for particulars variable(s) from the XML dataset.
     *
     * If the #SignalDef has not been populated from a DOM element, an empty
     * list is returned.
     *
     * \return A list of double precision values are returned.
     */
    const dstoute::aDoubleList& getTolerance( ) const { return tol_; }

    /**
     * This function returns the number of numeric tolerances contained within the
     * signal's \em tol[erance] child sub-element.
     *
     * If the #SignalDef has not been populated from a DOM element, zero
     * is returned.
     *
     * \return A count of the number of tolerances is returned.
     */
    size_t getToleranceCount() const { return tol_.size();}

    // @TODO :: Add set parameter functions

    /**
     * This function is used to set the signal definition reference identifier.
     * This function is used internally within Janus while instantiating a
     * DAVE-ML compliant XML dataset source file.
     *
     * \param sigID is the reference identifier of the signal definition.
     */
    void setSigID( const dstoute::aString& sigID ) { sigID_ = sigID; }


    /**
     * This function is used to export the \em Signal data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const SignalDef &signal);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
      const dstoute::aString& elementID,
      const size_t &documentElementReferenceIndex = 0);

    void setActualValue( const double& value) const
    { actualValue_.push_back( value);}
    const dstoute::aDoubleList& getActualValue() const { return actualValue_;}
    void clearActualValue() { actualValue_.clear();}

    bool isCheckValid( void);

   private:
    dstoute::aDoubleList stringToData( const dstoute::aStringList &dataStr);
    dstoute::aString dataToString( const dstoute::aDoubleList &dataTable);

    /************************************************************************
     * These are the signal elements, set up during instantiation.
     */

    // SignalDef attributes
    dstoute::aString name_;
    dstoute::aString units_;
    dstoute::aString sigID_;
    dstoute::aString symbol_;

    // SignalDef sub-elements
    bool                    isSigInput_;
    bool                    isSigInternal_;
    bool                    isSigOutput_;
    dstoute::aString        varID_;
    dstoute::aOptionalSizeT varIndex_;
    dstoute::aString        description_;

    // Value could be a single value or an array of values
    dstoute::aDoubleList value_;

    // Tolerance could be a single value or an array of values
    dstoute::aDoubleList tol_;

    mutable dstoute::aDoubleList actualValue_;
  };

  typedef dstoute::aList<SignalDef> SignalDefList;

}

#endif /* _SIGNALDEF_H_ */
