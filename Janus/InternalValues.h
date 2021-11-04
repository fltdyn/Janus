#ifndef _INTERNALVALUES_H_
#define _INTERNALVALUES_H_

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
// Title:      Janus/InternalValues
// Class:      InternalValues
// Module:     InternalValues.h
// First Date: 2011-11-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file InternalValues.h
 *
 * An InternalValues instance functions as a container for the Signal class,
 * and provides the functions that allow a calling StaticShot instance to
 * access the \em signal elements that define the internal values for a check
 * case.
 *
 * The InternalValues class is only used within the \em janus namespace, and should
 * only be referenced indirectly through the StaticShot class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes 
// Local Includes
#include "CheckSignal.h"

namespace janus
{
  class Janus;

  /**
   * An #InternalValues instance functions as a container for the Signal class,
   * and provides the functions that allow a calling StaticShot instance to
   * access the \em signal elements that define the internal values for a check
   * case.
   *
   * The #InternalValues class is only used within the \em janus namespace, and should
   * only be referenced indirectly through the StaticShot class.
   */
  class InternalValues: public CheckSignal
  {
  public:

    /**
     * The empty constructor can be used to instance the #InternalValues class
     * without supplying the DOM \em internalValues element from which the
     * instance is constructed, but in this state is not useful for any class
     * functions.
     * It is necessary to populate the class from a DOM containing a
     * \em internalValues element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     */
    InternalValues() :
        CheckSignal()
    {
    }
    ;

    /**
     * The constructor, when called with an argument pointing to a
     * \em internalValues element within a DOM, instantiates the #InternalValues
     * class and fills it with alphanumeric data from the DOM.
     *
     * \param internalValuesElement is an address of an \em internalValues
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    InternalValues(
      const DomFunctions::XmlNode& internalValuesElement,
      Janus* janus) :
        CheckSignal( internalValuesElement, SIGNAL_INTERNALVALUES, janus)
    {
    }
    ;

  };

}

#endif /* INTERNALVALUES_H */

