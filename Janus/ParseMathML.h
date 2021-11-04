#ifndef _PARSEMATHML_H_
#define _PARSEMATHML_H_

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
// Title:      Janus/ParseMathML : DSTO Math-ML functions Class
// Class:      ParseMathML
// Module:     ParseMathML.h
// First Date: 2012
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file ParseMathML.h
 *
 * This class contains functions for parsing mathematics procedures
 * defined using the MathML syntax. Data detailing each MathML operation
 * is stored in a MathMLDataClass structure. This includes the
 * sub-elements to which the operator is to be applied. Functions to
 * process both scalar and matrix data are included.
 */

/*
 * Author: Geoff Brian
 *
 * Modified :  S. Hill
 *
 */

// C++ Includes

// Ute Includes
#include <Ute/aMap.h>
#include <Ute/aString.h>

// Local Includes
#include "DomTypes.h"

namespace dstomathml {

  struct MathMLData;

  namespace parsemathml {

    typedef void (ParseMathMLFunction) ( DomFunctions::XmlNode& xmlElement, MathMLData& t);

    typedef dstoute::aMap< dstoute::aString, ParseMathMLFunction* > ParseMathMLMap;

    extern ParseMathMLMap parseMathMLMap;

    void parse( DomFunctions::XmlNode& xmlElement, dstomathml::MathMLData& t);
    void crossReference_ci( dstomathml::MathMLData& t);

  } // end namespace parsemathml

  namespace selectormethod {
    extern const dstoute::aMap< int, dstoute::aString> selectorMethodMap;
  } // end namespace selectormethod

} // end namespace dstomathml

#endif // End _PARSEMATHML_H_
