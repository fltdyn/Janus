#ifndef _CHECKSIGNAL_H_
#define _CHECKSIGNAL_H_

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
// Title:      Janus/CheckSignal
// Class:      CheckSignal
// Module:     CheckSignal.h
// First Date: 2017-09-07
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file CheckSignal.h
 *
 * The CheckSignal class is only used within the janus namespace, and should
 * only be referenced indirectly through the CheckInputs, InternalValues and
 * CheckOutputs classes.
 */

/*
 * Author:  G. Brian
 */

// Local Includes
#include <Janus/SignalList.h>
#include <Janus/Signals.h>
#include "XmlElementDefinition.h"

namespace janus
{
  class Janus;

  class CheckSignal : public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #CheckSignal class.
     * In this state is not useful for any class functions. It is necessary to
     * populate the class from a DOM containing the elements * \em checkInputs,
     * \em internalValues, and \em checkOutputs before any further use of the
     * instanced class.
     *
     * This form of the constructor is principally for use within higher level
     * instances, where memory needs to be allocated before the data to fill
     * it is specified.
     */
    CheckSignal();


    /**
     * The constructor, when called with an argument pointing to a
     * \em 'check*' element within a DOM, instantiates the #CheckSignal class.
     *
     * \param checkElement is an address to a \em 'check*' component
     * node within the DOM.
     * \param signalType is a enumeration identifying the check case type
     * associated with the signal/signalList as either an input, and output,
     * or an internal value.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    CheckSignal( const DomFunctions::XmlNode& checkElement,
                 const SignalTypeEnum &signalType,
                 Janus* janus);

    /**
     * The \em initialiseDefinition function fills an uninitialised instance of
     * #CheckSignal with data within a DOM.
     *
     * \param elementDefinition is an address of the \em 'check*' element
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
     * This function indicates whether the \em 'check*' element is
     * constructed from a list of \em signal elements or from a \em signalList
     * element.
     *
     * \return A boolean indicating the presence of a \em signalList.
     */
    const bool& hasSignalList() const
    {
      return hasSignalList_;
    }

    /**
     * This function indicates whether the \em 'check*' element is
     * constructed from a list of \em signal elements or from a \em signalList
     * element.
     *
     * \return A boolean indicating the presence of a list of \em signals.
     */
    const bool& hasSignals() const
    {
      return hasSignals_;
    }

    /**
     * This function provides the number of signals making up the referenced
     * signalList/signal instance.
     *
     * \return An integer number, one or more in a populated instance.
     * \sa Signal
     */
    size_t getSignalCount( ) const {
      if ( hasSignalList_) {
        return signalList_.getSignalCount();
      }
      else if ( hasSignals_) {
        return signals_.getSignalCount();
      }
      return 0;
    }

    /**
     * This function provides access to the list of \em signalType attribute.
     *
     * \return An enumeration specifying the check signal type of signalList/signal.
     */
    const SignalTypeEnum& getSignalType() const
    {
      return signalType_;
    }

    /**
     * This function provides access to the \em signalList.
     *
     * \return A reference to the signalList parameter.
     */
    const SignalList& getSignalList() const
    {
      return signalList_;
    }

    /**
     * This function provides access to the list of \em signals.
     *
     * \return A reference to the list of signals parameter.
     */
    Signals& getSignals()
    {
      return signals_;
    }

    const Signals& getSignals() const
    {
      return signals_;
    }

    /**
     * This function is used to export the data references in the \em checkSignal
     * class
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os,
      const CheckSignal &checkSignal);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

  private:
    Janus* janus_;
    SignalTypeEnum signalType_;

    bool           hasSignalList_;
    bool           hasSignals_;

    Signals        signals_;
    SignalList     signalList_;

  };

}

#endif /* _CHECKSIGNAL_H_ */
