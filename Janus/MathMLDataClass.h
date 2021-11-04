#ifndef _MATHMLDATACLASS_H_
#define _MATHMLDATACLASS_H_

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
// Module:     MathMLDataClass.h
// First Date: 2012
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file MathMLDataClass.h
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

// Ute Includes
#include <Ute/aList.h>
#include <Ute/aMatrix.h>
#include <Ute/aOptional.h>

// Local Includes
#include "SolveMathML.h"

namespace janus
{

  class Janus;
  class VariableDef;
}

namespace dstomathml
{

  /**
   * A MathMLDataClass instance holds in its allocated memory alphanumeric data
   * derived from \em MathML elements of a DOM corresponding to a DAVE-ML
   * compliant XML dataset source file. The data may include tags defining
   * the \em MathML element and its attributes, a list of children associated
   * with the \em MathML element, and call-backs to functions to evaluate the
   * element.
   *
   * The MathMLDataClass class is only used within the Janus.
   */

  const double EXPONENTIALE = 2.71828182845905;
  const double EULERGAMMA = 0.57721566490153286;

  /**
   * The \em MathRetType enumeration is used to flag the type of argument
   * returned from a mathematical operation.  The type is based on the
   * W3C MathML recommendations document. At present only \em Real and
   * \em Boolean types are handled.
   */
  enum MathRetType
  {
    REAL, // A double precision real number is returned.
    BOOL // A Boolean number ( true | false ) is returned.
  };

  struct MathMLData;
  typedef dstoute::aList< MathMLData> MathMLDataVector;

  /**
   * A MathMLDataClass instance holds in its allocated memory alphanumeric data
   * derived from \em MathML elements of a DOM corresponding to a DAVE-ML
   * compliant XML dataset source file. The data may include tags defining
   * the \em MathML element and its attributes, a list of children associated
   * with the \em MathML element, and call-backs to functions to evaluate the
   * element.
   *
   * The MathMLData struct class is only used within the Janus.
   */
  struct MathMLData
  {
    MathMLData();
    MathMLData(
      janus::Janus *janus);

    // Function to reset the Janus pointer and VariableDef pointer in the lower level classes.
    static void crossReference_ci(
      MathMLData &t,
      janus::Janus* janus);

    /*
     * A pointer to the original janus instance. Only required during MathML parsing.
     */
    janus::Janus *janus_;

    /*
     * A tag for the math function. This is used to identify the function using
     * a string tag.
     */
    dstoute::aString functionTag_;

    /*
     * This enum defines the type of return argument expected
     * (see MathRetType)
     */
    mutable MathRetType mathRetType_;

    /*
     * An index to the variableDef element associated with a 'ci' element.
     */
    dstoute::aOptionalSizeT ciIndex_;

    /*
     * A pointer to the MathML function of a MathML entity
     */
    solvemathml::MathMLFunction *mathMLFunctionPtr_;
    solvematrixmathml::MathMLFunction *mathMLMatrixFunctionPtr_;

    /*
     * A pointer to the Janus VariableDef for this MathML operator.
     * Used for ci.
     */
    janus::VariableDef *variableDef_;

    /*
     * A vector of child MathML structures
     */
    MathMLDataVector mathChildren_;

    /*
     * The attribute associated with a math function. For example, 'csymbol'
     * has a 'definitionURL' attribute.
     */
    dstoute::aString attribute_;

    /*
     * The value of a variableDef that is associated with a <cn></cn> element
     */
    double cnValue_;

    /*
     * Return data cache and assignment functions.
     */
    mutable bool isMatrix_;
    mutable bool test_;
    mutable double value_;
    mutable dstomath::DMatrix matrix_;

    inline void operator=(
      const double& value) const
    {
      test_ = false;
      value_ = value;
      isMatrix_ = false;
    }
    inline void operator=(
      const dstomath::DMatrix& matrix) const
    {
      test_ = false;
      if ( matrix.size() > 1) {
        matrix_ = matrix;
        isMatrix_ = true;
      }
      else {
        value_ = matrix( 0, 0);
        isMatrix_ = false;
      }
    }
    inline void operator=(
      const MathMLData& t) const
    {
      test_ = t.test_;
      if ( t.isMatrix_) {
        if ( t.matrix_.size() > 1) {
          matrix_ = t.matrix_;
          isMatrix_ = true;
        }
        else {
          value_ = t.matrix_.matrixData()[ 0];
          isMatrix_ = false;
        }
      }
      else {
        value_ = t.value_;
        isMatrix_ = false;
      }
    }
  };

} // end namespace dstomathml

#endif // End _MATHMLDATACLASS_H_
