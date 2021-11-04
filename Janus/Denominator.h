#ifndef _DENOMINATOR_H_
#define _DENOMINATOR_H_

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
// Title:      Janus/Denominator
// Class:      Denominator
// Module:     Denominator.h
// First Date: 2015-11-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file Denominator.h
 *
 */

// C++ Includes 
// Local Includes
#include "Array.h"

namespace janus
{

  /**
   *
   */
  class Denominator: public Array
  {
  public:

    /**
     * The empty constructor can be used to instance the #Denominator class
     * without supplying the dynamic systems model transfer function
     * \em denominator element from which the instance
     * is constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from the dynamic systems model transfer
     * function DOM \em denominator element before any further use of the instanced
     * class.
     *
     * This form of the constructor is principally for use within higher level
     * instances, where memory needs to be allocated before the data to fill
     * it is specified.
     * \sa setDenominatorFromDom
     */
    Denominator() :
      Array()
    {
    }


    /**
     * The constructor, when called with an argument pointing to a
     * dynamic system model transfer function \em denominator element within
     * a DOM, instantiates the #Denominator class and fills it with alphanumeric
     * data from the DOM.
     *
     * \param denominatorElement is an address to a \em denominator component
     * node within the DOM.
     */
    Denominator( const DomFunctions::XmlNode& denominatorElement) :
      Array( denominatorElement)
    {
    }

  };

}

#endif /* _DENOMINATOR_H_ */
