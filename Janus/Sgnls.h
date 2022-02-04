#ifndef _SIGNALS_H_
#define _SIGNALS_H_

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
// Title:      Janus/Signals
// Class:      Signals
// Module:     Signals.h
// First Date: 2011-11-04
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Sgnls.h
 *
 * A Signals instance functions as a container for the Signal class, and
 * provides the functions that allow a calling StaticShot instance to access
 * the \em signal elements that define either the input or output values for
 * a check case. A \em signal element contains a list parameters including:
   * \em signalName, \em signalUnits, and \em signalValue elements. An
   * optional \em tol element may be included.
 *
 * The Signals class is only used within the janus namespace, and is inherited
 * by the CheckInputs and CheckOutputs classes. It should only be referenced
 * indirectly through the StaticShot class.
 */

/*
 * Author:  G.J. Brian
 *
 * Modified :  S. Hill
 *
 */

// C++ Includes 
#include <vector>

// Local Includes
#include "XmlElementDefinition.h"
#include "Sgnl.h"

namespace janus {

  /**
   * A #Signals instance functions as a container for the Signal class, and
   * provides the functions that allow a calling StaticShot instance to access
   * the \em signal elements that define either the input or output values for
   * a check case. A \em signal element contains a list parameters including:
   * \em signalName, \em signalUnits, and \em signalValue elements. An
   * optional \em tol element may be included.
   *
   * The #Signals class is only used within the janus namespace, and is inherited
   * by the CheckInputs and CheckOutputs classes. It should only be referenced
   * indirectly through the StaticShot class.
   *
   * Typical usage:
  \code
    Janus test( xmlFileName );
    CheckData checkData = test.getCheckData();
    size_t nss = checkData.getStaticShotCount( );
    for ( size_t j = 0 ; j < nss ; j++ ) {
      StaticShot staticShot = checkData.getStaticShot( j );
      CheckOutputs checkOutputs = staticShot.getCheckOutputs();
      size_t ncout = checkOutputs.getSignalCount();
      cout << " staticShot[" << j << "] : " << endl
           << "      Name                      = "
           << staticShot.getName( ) << endl
           << "      Number of check outputs   = " << ncout << endl;
      for ( size_t k = 0 ; k < ncout ; k++ ) {
        cout << "  checkOutputs[" << k << "] : " << endl
             << "    signalName                = "
             << checkOutputs.getName( k ) << endl
             << "    signalUnits               = "
             << checkOutputs.getUnits( k ) << endl
             << "    signalValue               = "
             << checkOutputs.getValue( k ) << endl
             << "          tol                 = "
             << checkOutputs.getTolerance( k ) << endl
             << endl;
      }
    }
  \endcode
   */
  class Signals : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #Signals class
     * without supplying the DOM \em signal elements from which the instance
     * is constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em signal elements list before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseElement
     */
    Signals();

    /**
     * The constructor, when called with an argument pointing to
     * \em signal elements within a DOM, instantiates the #Signals class
     * and fills it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a list of \em signal components
     * within the DOM.
     * \param signalType is a enumeration identifying the signal as either an
     * input, and output, or an internal value.
     */
    Signals( const DomFunctions::XmlNode& elementDefinition,
             const SignalTypeEnum &signalType);

    /**
     * An uninitialised instance of #Signals is filled with data from a
     * particular list of \em signal elements within a DOM by this function.  If
     * another list of \em signal elements pointer is supplied to an instance that
     * has already been initialised, the instance will be re-initialised with
     * the new data.
     *
     * \param elementDefinition is an address of a list of \em signal components
     * within the DOM.
     * \param signalType is a enumeration identifying the signal as either an
     * input, and output, or an internal value.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
                               const SignalTypeEnum &signalType);

    /**
     * This function provides the number of signals making up the referenced
     * #Signals instance.  If the instance has not been populated from a
     * DOM element, zero is returned.  For a full check case, this function
     * will return the number of output variables, explicit or implicit, in the
     * XML dataset.
     *
     * \return An integer number, one or more in a populated instance.
     * \sa Signal
     */
    size_t getSignalCount( ) const { return signal_.size(); }

    /**
     * This function provides access to the signal definitions
     * instances that have been defined for the singnalList instance.
     * An empty vector will be returned if the Signal instance has not
     * been populated from a DOM. In all other cases, the vector will contain
     * at least one signal instance.
     *
     * \return An vector of signal definitions instances.
     *
     * \sa Signal
     */
    aListSignals& getSignal() { return signal_;}
    const aListSignals& getSignal() const { return signal_;}

    /**
     * This function returns the \em signalName from a Signal associated with
     * the referenced #Signals instance.  If the Signal has not been
     * initialised from a DOM, an empty string is returned.
     * \param index has a range from zero to ( #getSignalCount() - 1 ),
     * and selects the required Signal component.  Attempting to access a
     * Signal outside the available range will throw a standard out_of_range
     * exception.
     *
     * \return The selected \em signalName string is returned by reference.
     *
     * \sa Signal
     */
    const dstoute::aString& getName( const size_t &index ) const
    { return signal_.at( index ).getName(); }

    /**
     * This function returns the \em signalUnits from a Signal associated with
     * the referenced #Signals instance.  If the Signal has not been
     * initialised from a DOM, an empty string is returned.
     * \param index has a range from zero to ( #getSignalCount() - 1 ),
     * and selects the required Signal component.  Attempting to access a
     * Signal outside the available range will throw a standard out_of_range
     * exception.
     *
     * \return The selected \em signalUnits string is returned by reference.
     *
     * \sa Signal
     */
    const dstoute::aString& getUnits( const size_t &index ) const
    { return signal_.at( index ).getUnits(); }

    /**
     * This function returns the \em varID from a Signal associated with
     * the referenced #Signals instance.  If the Signal has not been
     * initialised from a DOM, an empty string is returned.
     * \param index has a range from zero to ( #getSignalCount() - 1 ),
     * and selects the required Signal component.  Attempting to access a
     * Signal outside the available range will throw a standard out_of_range
     * exception.
     *
     * \return The selected \em varID string is returned by reference.
     *
     * \sa Signal
     */
    const dstoute::aString& getVarID( const size_t &index ) const
    { return signal_.at( index ).getVarID(); }

    /**
     * This function returns the \em signalValue from a Signal associated with
     * the referenced #Signals instance.  If the Signal has not been
     * populated from a DOM element, NaN is returned.
     * \param index has a range from zero to ( #getSignalCount() - 1 ),
     * and selects the required Signal component.  Attempting to access a
     * Signal outside the available range will throw a standard out_of_range
     * exception.
     *
     * \return A double precision variable containing the requested signal
     * value is returned.
     *
     * \sa Signal
     */
    const double& getValue( const size_t &index ) const
    { return signal_.at( index ).getValue(); }

    /**
     * This function returns the \em tol component from a Signal associated
     * with the referenced #Signals instance.  If the Signal has not been
     * populated from a DOM, NaN will be returned.  If a tolerance is not
     * specified for the \em signal within the XML dataset, this function will
     * return zero.
     * \param index has a range from zero to ( #getSignalCount() - 1 ),
     * and selects the required Signal component.  Attempting to access a
     * Signal outside the available range will throw a standard out_of_range
     * exception.
     *
     * \return A double precision variable containing the tolerance on the
     * requested signal value is returned.
     *
     * \sa Signal
     */
    const double& getTolerance( const size_t &index ) const
    { return signal_.at( index ).getTolerance(); }

    /**
     * This function returns the index number within the #Signals instance
     * that corresponds with a specified signal \em name.
     *
     * \param name is a string containing \em name of the signal of interest.
     *
     * \return An integer value for the index corresponding to the specified
     * \em name is returned.  If the specified name does not appear in any
     * signal within the #Signals instance, -1 is returned.
     *
     * \sa Signal
     */
    int getIndex( const dstoute::aString &name ) const;

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em SignalList data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, Signals &sgnls);
  
    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );

    void setActualValue( const size_t &index, const double& value) const
      { signal_.at( index).setActualValue( value);}

    const double& getActualValue( const size_t &index) const
      { return signal_.at( index).getActualValue();}

   private:
  
    /************************************************************************
     * These are the checkOutputs' signal element, set up during
     * instantiation.
     */
    ElementDefinitionEnum elementType_;
    SignalTypeEnum        signalType_;

    aListSignals signal_;

  };
  
}

#endif /* _SIGNALS_H_ */
