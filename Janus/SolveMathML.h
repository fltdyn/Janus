#ifndef _SOLVEMATHML_H_
#define _SOLVEMATHML_H_

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
// Title:      Janus/SolveMathML
// Class:      SolveMathML
// Module:     SolveMathML.h
// First Date: 2012
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file SolveMathML.h
 *
 * This class contains functions for solving mathematics procedures
 * defined using the MathML syntax. Data detailing each MathML operation
 * is stored in a MathMLDataClass structure. This includes the
 * sub-elements to which the operator is to be applied. Functions to
 * process both scalar and matrix data are included.
 */

/*
 * Author:  Geoff Brian
 *
 * Modified :  S. Hill
 *
 */

// C++ Includes

// Ute Includes
#include <Ute/aMap.h>
#include <Ute/aString.h>

namespace dstomathml {

  struct MathMLData;

  namespace solvemathml {

    typedef double (MathMLFunction) ( const MathMLData& t);

    typedef dstoute::aMap< dstoute::aString, MathMLFunction* > SolveMathMLMap;

    extern SolveMathMLMap solveMathMLMap;

  } // end namespace solvemathml

  namespace solvematrixmathml {

    typedef const MathMLData& (MathMLFunction) ( const MathMLData& t);

    typedef std::pair< dstoute::aString, MathMLFunction* > SolveMathMLPair;
    typedef dstoute::aMap< dstoute::aString, MathMLFunction* > SolveMathMLMap;

    extern SolveMathMLMap solveMathMLMap;

  } // end namespace solvematrixmathml

} // end namespace dstomathml

#endif // End _SOLVEMATHML_H_
