#ifndef _SIGNALIST_H_
#define _SIGNALIST_H_

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
// Title:      Janus/SignalList
// Class:      SignalList
// Module:     SignalList.h
// First Date: 2011-11-04
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file SignalList.h
 *
 * A SignalList instance behaves as a container for a list of Signal definition
 * elements (either signalDef or signalRef), which provide the properties of
 * signal elements associated with checkInputs, internalValues, and checkOutputs
 * elements of StaticShot instances.
 *
 * The SignalList class is only used within the \em janus namespace, and is
 * inherited by the CheckInputs, InternalValues and CheckOutputs classes. It
 * should only be referenced indirectly through the StaticShot class.
 */

/*
 * Author:  G.J. Brian
 */

// C++ Includes 

// Ute Includes
#include <Ute/aOptional.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "SignalDef.h"

namespace janus {

  class Janus;

  class SignalList : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #SignalList class
     * without supplying the DOM \em signal elements from which the instance
     * is constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a list of
     * \em signalDef elements before any further use of the instanced class.
     *
     * This form of the constructor is principally for use within higher level
     * instances, where memory needs to be allocated before the data to fill it
     * is specified.
     *
     * \sa initialiseElement
     */
      SignalList( );

    /**
     * The constructor, when called with an argument pointing to \em signalList
     * elements within a DOM, instantiates the #SignalList class and fills it
     * with data from the DOM.
     *
     * \param elementDefinition is an address of the \em signalList element
     * within the DOM.
     * \param signalType is a enumeration identifying the check case type
     * associated with the signal/signalList as either an input, and output,
     * or an internal value.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
      SignalList( const DomFunctions::XmlNode& elementDefinition,
                  const SignalTypeEnum &signalType,
                  Janus* janus);

    /**
     * The \em initialiseDefinition function fills an uninitialised instance of
     * #SignalList with data from a \em signalList element defined within a DOM.
     * The \em signalList element will contain a list of Signal definition
     * elements, either defined as signalDef or signalRef elements.
     *
     * If another \em signalList element is supplied to an instance that
     * has already been initialised, the instance will be re-initialised with
     * the new data.
     *
     * \param elementDefinition is an address of the \em signalList element
     * within the DOM.
     * \param signalType is a enumeration identifying the check case type
     * associated with the signal/signalList as either an input, and output,
     * or an internal value.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
                               const SignalTypeEnum &signalType,
                               Janus* janus);

    /**
     * This function provides the number of signals (signalDef or signalRef)
     * making up the referenced #SignalList instance. If the instance has not
     * been populated from a DOM element, zero is returned.
     *
     * \return An integer number, one or more in a populated instance.
     */
    size_t getSignalCount( ) const { return signalRef_.size(); }

//    /**
//     * This function provides access to the signal definitions (\em signalDef )
//     * instances that have been defined for the singnalList instance.
//     * An empty vector will be returned if no \em signalDef instances have been
//     * been populated from a DOM. In all other cases, the vector will contain
//     * at least one \em signalDef instance.
//     *
//     * \return An vector of signal definitions.
//     *
//     * \sa SignalDef
//     */
//    const SignalDefList& getSignalDef() const { return signalDef_;}

    /**
     * This function provides access to the signal definition references
     * (\em signalRef ) instances that have been defined for the signalList
     * instance. An empty vector will be returned if no \em signalRef instances
     * have been been populated from a DOM. In all other cases, the vector will
     * contain at least one \em signalRef instance.
     *
     * \return An vector of signal definition references.
     *
     * \sa SignalRef
     */
    const dstoute::aStringList& getSignalRef() const { return signalRef_;}

    // @TODO :: Add set parameter functions
    void setSignalRef( const dstoute::aStringList& signalRef) {
      signalRef_ = signalRef;
    }

    /**
     * This function is used to export the \em SignalList data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML document type
     * definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const SignalList &signalList);
  
    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
      const dstoute::aString& elementID,
      const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

    void subList( const size_t& begin, const size_t& end, SignalList* signalList);

   private:
  
    /************************************************************************
     * These are the signalList parameters that are set up during
     * Class instantiation.
     */
    Janus* janus_;
    ElementDefinitionEnum elementType_;
    SignalTypeEnum        signalType_;

    dstoute::aStringList  signalRef_;
  };

}

#endif /* _SIGNALIST_H_ */
