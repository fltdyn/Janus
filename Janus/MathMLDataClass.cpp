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
// Title:      Janus/MathMLDataClass : DSTO Base Math-ML Data Class
// Class:      MathMLDataClass
// Module:     MathMLDataClass.cpp
// First Date: 2012
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file MathMLDataClass.cpp
 *
 * This file defines the data structure used for interpreting
 * MathML mathematics procedures.The data include a tag defining
 * the MathML element, a list of children associated with the
 * MathML element, and call-backs to functions to evaluate the
 * element.
 */

/*
 * Author: Geoff Brian
 *
 * Modified :  S. Hill
 *
 */

// C++ Includes
#include <iostream>

// Ute Includes
#include <Ute/aOptional.h>

#include "Janus.h"
#include "MathMLDataClass.h"
#include "VariableDef.h"

using namespace std;
using namespace dstoute;

namespace dstomathml {

  //------------------------------------------------------------------------//

  MathMLData::MathMLData()
    : janus_( 0),
      mathRetType_( dstomathml::REAL),
      ciIndex_( aOptionalSizeT::invalidValue()),
      mathMLFunctionPtr_(0),
      mathMLMatrixFunctionPtr_(0),
      variableDef_(0),
      cnValue_(0),
      isMatrix_( false),
      test_( false),
      value_(0)
  {
  }

  MathMLData::MathMLData( janus::Janus *janus)
    : janus_( janus),
      mathRetType_( dstomathml::REAL),
      ciIndex_( aOptionalSizeT::invalidValue()),
      mathMLFunctionPtr_(0),
      mathMLMatrixFunctionPtr_(0),
      variableDef_(0),
      cnValue_(0),
      isMatrix_( false),
      test_( false),
      value_(0)
  {
  }

  void MathMLData::crossReference_ci( MathMLData &t, janus::Janus* janus)
  {
    t.janus_ = janus;
    if ( t.ciIndex_.isValid()) {
      t.variableDef_ = &t.janus_->getVariableDef( t.ciIndex_);
    }
    for ( MathMLDataVector::iterator child = t.mathChildren_.begin();
          child != t.mathChildren_.end(); ++child) {
      MathMLData::crossReference_ci( *child, janus);
    }
  }

} // end namespace dstomathml


