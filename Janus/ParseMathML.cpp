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
// Module:     ParseMathML.cpp
// First Date: 2012
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file ParseMathML.cpp
 *
 * This class contains functions for parsing mathematics procedures
 * defined using the MathML syntax. Data detailing each MathML operation
 * and is stored in a MathMLDataClass structure. This includes the
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
#include <iostream>

// Ute Includes
#include <Ute/aMath.h>
#include <Ute/aMessageStream.h>
#include <Ute/aString.h>

// Local Includes
#include "ElementDefinitionEnum.h"
#include "ParseMathML.h"
#include "VariableDef.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace dstomathml {

  namespace parsemathml {

    //------------------------------------------------------------------------//

    /*
     * Functions for populating the mathChildren vector,
     */
    void parseTagName( DomFunctions::XmlNode& xmlElement,
                       dstomathml::MathMLData& t,
                       const aString& tagName)
    {
      /*
       * Check that the tagName represents a function in the MathML map.
       */
      if ( parseMathMLMap.find( tagName) != parseMathMLMap.end()) {
        t.functionTag_             = tagName;
        t.mathMLFunctionPtr_       = solvemathml::solveMathMLMap[ t.functionTag_];
        t.mathMLMatrixFunctionPtr_ = solvematrixmathml::solveMathMLMap[ t.functionTag_];
        parseMathMLMap[ tagName]( xmlElement, t);
      }
      else {
        throw_message( range_error,
          setFunctionName( "parseTagName()")
          << "\n - Support for tag \"" << tagName << "\" not provide."
        );
      }
    }

    //------------------------------------------------------------------------//

    void populateMathChildren( DomFunctions::XmlNode& xmlElement,
                               MathMLData& t,
                               const aString &functionName,
                               const bool &isSibling = true,
                               const size_t &nElements = 0);
    void populateMathChildren( DomFunctions::XmlNode& xmlElement,
                               MathMLData& t,
                               const aString &functionName,
                               const bool &isSibling,
                               const size_t &nElements)
    {
      t.functionTag_ = functionName;

      DomFunctions::XmlNodeList children;
      if ( isSibling) {
        children =
            DomFunctions::getSiblings( xmlElement, janus::EMPTY_STRING);
      }
      else {
        children =
            DomFunctions::getChildren( xmlElement, janus::EMPTY_STRING);
      }
      size_t childrenSize = children.size();

      if ( nElements == 0) {
        if ( childrenSize == 0) {
          throw_message( invalid_argument,
            setFunctionName( "populateMathChildren()")
            << "\n - Function \"" << functionName << "\" has no associate elements."
          );
        }
      }
      else if ( childrenSize != nElements) {
        throw_message( invalid_argument,
          setFunctionName( "populateMathChildren()")
          << "\n - Function \"" << functionName << "\" has an incorrect number of associate elements."
        );
      }

      t.mathRetType_ = dstomathml::REAL;
      for ( size_t i = 0; i < childrenSize; i++) {
        t.mathChildren_.push_back( MathMLData( t.janus_));
        parse( children[i], t.mathChildren_.back());
      }
    }

    //------------------------------------------------------------------------//

    /*
     * Functions to parse mathematics procedures defined using the MathML syntax.
     *
     * parse    : A function to parse the MathML syntax.
     * parse_ci : A function to parse the 'ci' MathML elements.
     * crossReference_ci : A function to set up call-back function
     *                     associations for 'ci' MathML elements.
     */

    void parse( DomFunctions::XmlNode& xmlElement, dstomathml::MathMLData& t)
    {
      parseTagName( xmlElement, t, DomFunctions::getChildName( xmlElement));
    }

    //------------------------------------------------------------------------//

    void csymbol( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      t.attribute_ = DomFunctions::getAttribute( xmlElement, "cd");
      if ( t.attribute_.empty()) {
        t.attribute_ = DomFunctions::getAttribute( xmlElement, "definitionURL");

        if ( t.attribute_.empty()) {
          t.attribute_ = DomFunctions::getAttribute( xmlElement, "type");
        }
      }

      parseTagName( xmlElement, t, DomFunctions::getCData( xmlElement).trim());
    }

    //------------------------------------------------------------------------//

    /*
     * Functions to parse mathematics procedures defined using the MathML syntax.
     *
     * parse    : A function to parse the MathML syntax.
     * parse_ci : A function to parse the 'ci' MathML elements.
     * crossReference_ci : A function to set up call-back function
     *                     associations for 'ci' MathML elements.
     */
    void parse( DomFunctions::XmlNode& xmlElement, MathMLData& t);

    /*
     * Numbers and Identifiers
     *
     * cn
     * ci
     * apply
     * csymbol
     */

    void cn( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      aString cdata = DomFunctions::getCData( xmlElement);
      if ( !cdata.isNumeric()) {
        throw_message( range_error,
          setFunctionName( "cn()")
          << "\n - \"" << cdata << "\" must be a floating point value within <cn> tag.\n"
          << "\n - Did you mean to use a <ci> instead?"
        );
      }
      t.cnValue_ = cdata.toDouble();
    }

    void ci( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      aString ciElementName = DomFunctions::getCData( xmlElement).trim();
      t.ciIndex_ = t.janus_->crossReferenceId( janus::ELEMENT_VARIABLE, ciElementName);

      if ( !t.ciIndex_.isValid()) {
        throw_message( range_error,
          setFunctionName( "ci()")
          << "\n - \"" << ciElementName << "\" is not in VariableDef list."
        );
      }
    }

    void apply( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      DomFunctions::XmlNodeList children =
          DomFunctions::getChildren( xmlElement, janus::EMPTY_STRING);
      if ( children.size() > 0) {
        t.mathChildren_.push_back( MathMLData( t.janus_));
        parse( children.front(), t.mathChildren_.front());
      }
    }

    void csymbol( DomFunctions::XmlNode& xmlElement, MathMLData& t);

    /*
     * Boolean relational operators
     *
     * The boolean relational operators evaluate the equality of
     * two expressions. When evaluated, the operators assign a
     * boolean true/false value to a flag variable. This variable
     * is converted to a double representation prior to returning
     * from the function call, thereby permitting 'solve' function
     * to be generalised.
     *
     * If a function expects to evaluate one of the relational operators
     * it should check the 'mathReturnType' flag of the associated
     * 'MathMLDataClass' to determine if real or boolean data is stored or
     * being evaluated. An example of such a function is <piecewise>,
     * which often determines its course of action based on relational
     * assessment.
     *
     * eq
     * neq
     * gt
     * geq
     * lt
     * leq
     */
    void eq( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "eq", true, 2);
      t.mathRetType_ = dstomathml::BOOL;
    }

    void neq( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "neq", true, 2);
      t.mathRetType_ = dstomathml::BOOL;
    }

    void gt( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "gt", true, 2);
      t.mathRetType_ = dstomathml::BOOL;
    }

    void geq( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "geq", true, 2);
      t.mathRetType_ = dstomathml::BOOL;
    }

    void lt( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "lt", true, 2);
      t.mathRetType_ = dstomathml::BOOL;
    }

    void leq( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "leq", true, 2);
      t.mathRetType_ = dstomathml::BOOL;
    }

    /*
     * Arithmetic, Algebra and Logic Operators
     *
     * The boolean 'and', 'or', 'xor' and 'not' operators evaluate
     * the validity of one or more expressions. When evaluated, the
     * operators assign a boolean true/false value to a flag variable.
     * This variable is converted to a double representation prior to
     * returning from the function call, thereby permitting 'solve'
     * function to be generalised.
     *
     * If a function expects to evaluate one of the 'and', 'or', 'xor'
     * or 'not' operators it should check the 'mathReturnType' flag of
     * the associated 'MathMLDataClass' to determine if real or boolean data
     * is stored or being evaluated. An example of such a function is
     * <piecewise>, which often determines its course of action based
     * on such assessments.
     *
     * quotient
     * factorial
     * divide
     * max
     * min
     * minus
     * plus
     * power
     * rem
     * times
     * root, degree
     * and
     * or
     * xor
     * not
     * abs
     * floor
     * ceiling
     */
    void quotient( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "quotient", true, 2);
    }

    void factorial( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "factorial", true, 1);
    }

    void divide( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "divide", true, 2);
    }

    void max( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "max");
    }

    void min( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "min");
    }

    void minus( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "minus");
    }

    void plus( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "plus");
    }

    void power( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "power", true, 2);
    }

    void rem( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "rem", true, 2);
    }

    void times( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "times");
    }

    void root( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "root");
      if ( t.mathChildren_.size() == 0 || t.mathChildren_.size() > 2) {
        throw_message( invalid_argument,
          setFunctionName( "root()")
          << "\n - \"root\" has an incorrect number of associate elements."
        );
      }
    }

    void degree( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "degree", false, 1);
    }

    void and_( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "and");
      t.mathRetType_ = dstomathml::BOOL;
    }

    void or_( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "or");
      t.mathRetType_ = dstomathml::BOOL;
    }

    void xor_( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "xor");
      t.mathRetType_ = dstomathml::BOOL;
    }

    void not_( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "not", true, 1);
      t.mathRetType_ = dstomathml::BOOL;
    }

    void abs( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "abs", true, 1);
    }

    void floor( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "floor", true, 1);
    }

    void ceiling( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "ceiling", true, 1);
    }

    /*
     * Functions and Inverses
     *
     * (<piecewise>, <piece>, <otherwise>)
     * fmod
     * sign
     * bound
     * nearbyint
     */
    void piecewise( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "piecewise", false);
    }

    void piece( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "piece", false, 2);
    }

    void otherwise( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "otherwise", false, 1);
    }

    void fmod( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "fmod", true, 2);
    }

    void sign( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "sign", true, 2);
    }

    void bound( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "bound", true, 3);
    }

    void nearbyint( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "nearbyint", true, 1);
    }

    /*
     * Elementary Classical Functions
     *
     * a) Trigonometric functions
     *
     * sin
     * cos
     * tan
     * sec
     * csc
     * cot
     * sind
     * cosd
     * tand
     * secd
     * cscd
     * cotd
     * arcsin
     * arccos
     * arctan
     * arcsec
     * arccsc
     * arccot
     * arcsind
     * arccosd
     * arctand
     * arcsecd
     * arccscd
     * arccotd
     * atan2
     * atan2d
     *
     * b) others
     * exp
     * ln
     * log, logbase
     */
    void sin( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "sin", true, 1);
    }

    void cos( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "cos", true, 1);
    }

    void tan( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "tan", true, 1);
    }

    void sec( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "sec", true, 1);
    }

    void csc( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "csc", true, 1);
    }

    void cot( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "cot", true, 1);
    }

    void sind( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "sind", true, 1);
    }

    void cosd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "cosd", true, 1);
    }

    void tand( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "tand", true, 1);
    }

    void secd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "secd", true, 1);
    }

    void cscd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "cscd", true, 1);
    }

    void cotd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "cotd", true, 1);
    }

    void arcsin( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arcsin", true, 1);
    }

    void arccos( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arccos", true, 1);
    }

    void arctan( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arctan", true, 1);
    }

    void arcsec( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arcsec", true, 1);
    }

    void arccsc( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arccsc", true, 1);
    }

    void arccot( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arccot", true, 1);
    }

    void arcsind( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arcsind", true, 1);
    }

    void arccosd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arccosd", true, 1);
    }

    void arctand( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arctand", true, 1);
    }

    void arcsecd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arcsecd", true, 1);
    }

    void arccscd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arccscd", true, 1);
    }

    void arccotd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "arccotd", true, 1);
    }

    void atan2( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "atan2", true, 2);
    }

    void atan2d( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "atan2d", true, 2);
    }

    void exp( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "exp", true, 1);
    }

    void ln( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "ln", true, 1);
    }

    void log( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "log");
      if ( t.mathChildren_.size() == 0 || t.mathChildren_.size() > 2) {
        throw_message( invalid_argument,
          setFunctionName( "log()")
          << "\n - \"log\" has an incorrect number of associate elements."
        );
      }
    }

    void logbase( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "logbase", false, 1);
    }

    /*
     * Linear Algebra
     *
     * determinant
     * transpose
     * inverse
     * selector
     * vectorproduct
     * scalarproduct
     * outerproduct
     * unitmatrix
     * eulertransform
     * eulertransformd
     * cross
     */
    void determinant( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "determinant", true, 1);
    }

    void transpose( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "transpose", true, 1);
    }

    void inverse( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "inverse", true, 1);
    }

    void selector( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      t.attribute_ = DomFunctions::getAttribute( xmlElement, "other");
      if ( t.attribute_.empty()) {
        t.attribute_ = "element";
      }
      parseTagName( xmlElement, t, aString( "selector_%").arg( t.attribute_));
    }

    void selector_element( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "selector_element");

      if ( t.mathChildren_.size() < 2 || t.mathChildren_.size() > 3) {
        throw_message( invalid_argument,
          setFunctionName( "selector_element()")
          << "\n - \"selector_element\" has an incorrect number of associate elements."
        );
      }
    }

    void selector_row( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "selector_row");

      if ( t.mathChildren_.size() % 2 == 1) {
        throw_message( invalid_argument,
          setFunctionName( "selector_row()")
          << "\n - \"selector_row\" has an incorrect number of associate elements."
        );
      }
    }

    void selector_column( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "selector_column");

      if ( t.mathChildren_.size() % 2 == 1) {
        throw_message( invalid_argument,
          setFunctionName( "selector_column()")
          << "\n - \"selector_column\" has an incorrect number of associate elements."
        );
      }
    }

    void selector_diag( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "selector_diag");

      if ( t.mathChildren_.size() % 3 != 0) {
        throw_message( invalid_argument,
          setFunctionName( "selector_diag()")
          << "\n - \"selector_diag\" the number of children must be divisible by 3."
        );
      }
    }

    void selector_mslice( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "selector_mslice", true, 5);
    }

    void vectorproduct( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "vectorproduct", true, 2);
    }

    void scalarproduct( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "scalarproduct", true, 2);
    }

    void outerproduct( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "outerproduct", true, 2);
    }

    void unitmatrix( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "unitmatrix", true, 1);
    }

    void eulertransform( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "eulertransform", true, 3);
    }

    void eulertransformd( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "eulertransformd", true, 3);
    }

    void cross( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "cross", true, 1);
    }

    void mask( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      if ( t.attribute_.empty()) {
        t.attribute_ = "times";
      }

      parseTagName( xmlElement, t, aString( "mask_%").arg( t.attribute_));
    }

    void mask_plus( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "mask_plus", true, 2);
    }

    void mask_minus( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "mask_minus", true, 2);
    }

    void mask_times( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "mask_times", true, 2);
    }

    void mask_divide( DomFunctions::XmlNode& xmlElement, MathMLData& t)
    {
      populateMathChildren( xmlElement, t, "mask_divide", true, 2);
    }
    /*
     * Constant and Symbols
     *
     * exponentiale
     * notanumber
     * pi
     * eulergamma
     * infinity
     * noop
     *
     */
    void exponentiale( DomFunctions::XmlNode& /*xmlElement*/, MathMLData& /*t*/) {}

    void notanumber( DomFunctions::XmlNode& /*xmlElement*/, MathMLData& /*t*/) {}

    void pi( DomFunctions::XmlNode& /*xmlElement*/, MathMLData& /*t*/) {}

    void eulergamma( DomFunctions::XmlNode& /*xmlElement*/, MathMLData& /*t*/) {}

    void infinity( DomFunctions::XmlNode& /*xmlElement*/, MathMLData& /*t*/) {}

    void noop( DomFunctions::XmlNode& /*xmlElement*/, MathMLData& /*t*/) {}


    //------------------------------------------------------------------------//

    ParseMathMLMap parseMathMLMap = ( ParseMathMLMap()
        << ParseMathMLMap::pair( "cn",            cn)
        << ParseMathMLMap::pair( "ci",            ci)
        << ParseMathMLMap::pair( "apply",         apply)
        << ParseMathMLMap::pair( "csymbol",       csymbol)
        << ParseMathMLMap::pair( "eq",            eq)
        << ParseMathMLMap::pair( "neq",           neq)
        << ParseMathMLMap::pair( "gt",            gt)
        << ParseMathMLMap::pair( "geq",           geq)
        << ParseMathMLMap::pair( "lt",            lt)
        << ParseMathMLMap::pair( "leq",           leq)
        << ParseMathMLMap::pair( "quotient",      quotient)
        << ParseMathMLMap::pair( "factorial",     factorial)
        << ParseMathMLMap::pair( "divide",        divide)
        << ParseMathMLMap::pair( "max",           max)
        << ParseMathMLMap::pair( "min",           min)
        << ParseMathMLMap::pair( "minus",         minus)
        << ParseMathMLMap::pair( "plus",          plus)
        << ParseMathMLMap::pair( "power",         power)
        << ParseMathMLMap::pair( "rem",           rem)
        << ParseMathMLMap::pair( "times",         times)
        << ParseMathMLMap::pair( "root",          root)
        << ParseMathMLMap::pair( "degree",        degree)
        << ParseMathMLMap::pair( "and",           and_)
        << ParseMathMLMap::pair( "or",            or_)
        << ParseMathMLMap::pair( "xor",           xor_)
        << ParseMathMLMap::pair( "not",           not_)
        << ParseMathMLMap::pair( "abs",           abs)
        << ParseMathMLMap::pair( "floor",         floor)
        << ParseMathMLMap::pair( "ceiling",       ceiling)
        << ParseMathMLMap::pair( "piecewise",     piecewise)
        << ParseMathMLMap::pair( "piece",         piece)
        << ParseMathMLMap::pair( "otherwise",     otherwise)
        << ParseMathMLMap::pair( "fmod",          fmod)
        << ParseMathMLMap::pair( "sign",          sign)
        << ParseMathMLMap::pair( "bound",         bound)
        << ParseMathMLMap::pair( "nearbyint",     nearbyint)
        << ParseMathMLMap::pair( "sin",           sin)
        << ParseMathMLMap::pair( "cos",           cos)
        << ParseMathMLMap::pair( "tan",           tan)
        << ParseMathMLMap::pair( "sec",           sec)
        << ParseMathMLMap::pair( "csc",           csc)
        << ParseMathMLMap::pair( "cot",           cot)
        << ParseMathMLMap::pair( "arcsin",        arcsin)
        << ParseMathMLMap::pair( "arccos",        arccos)
        << ParseMathMLMap::pair( "arctan",        arctan)
        << ParseMathMLMap::pair( "arcsec",        arcsec)
        << ParseMathMLMap::pair( "arccsc",        arccsc)
        << ParseMathMLMap::pair( "arccot",        arccot)
        << ParseMathMLMap::pair( "exp",           exp)
        << ParseMathMLMap::pair( "ln",            ln)
        << ParseMathMLMap::pair( "log",           log)
        << ParseMathMLMap::pair( "logbase",       logbase)

        // Constants.
        << ParseMathMLMap::pair( "exponentiale",  exponentiale)
        << ParseMathMLMap::pair( "notanumber",    notanumber)
        << ParseMathMLMap::pair( "pi",            pi)
        << ParseMathMLMap::pair( "eulergamma",    eulergamma)
        << ParseMathMLMap::pair( "infinity",      infinity)
        << ParseMathMLMap::pair( "noop",          noop)

        // Matrix Ops.
        << ParseMathMLMap::pair( "determinant",   determinant)
        << ParseMathMLMap::pair( "transpose",     transpose)
        << ParseMathMLMap::pair( "inverse",       inverse)
        << ParseMathMLMap::pair( "vectorproduct", vectorproduct)
        << ParseMathMLMap::pair( "scalarproduct", scalarproduct)
        << ParseMathMLMap::pair( "outerproduct",  outerproduct)
        << ParseMathMLMap::pair( "selector",         selector)
        << ParseMathMLMap::pair( "selector_element", selector_element)
        << ParseMathMLMap::pair( "selector_row",     selector_row)
        << ParseMathMLMap::pair( "selector_column",  selector_column)
        << ParseMathMLMap::pair( "selector_diag",    selector_diag)
        << ParseMathMLMap::pair( "selector_mslice",  selector_mslice)

        // csymbol definitions (technically these are not defined in MathML)
        << ParseMathMLMap::pair( "sind",          sind)
        << ParseMathMLMap::pair( "cosd",          cosd)
        << ParseMathMLMap::pair( "tand",          tand)
        << ParseMathMLMap::pair( "secd",          secd)
        << ParseMathMLMap::pair( "cscd",          cscd)
        << ParseMathMLMap::pair( "cotd",          cotd)
        << ParseMathMLMap::pair( "arcsind",       arcsind)
        << ParseMathMLMap::pair( "arccosd",       arccosd)
        << ParseMathMLMap::pair( "arctand",       arctand)
        << ParseMathMLMap::pair( "arcsecd",       arcsecd)
        << ParseMathMLMap::pair( "arccscd",       arccscd)
        << ParseMathMLMap::pair( "arccotd",       arccotd)
        << ParseMathMLMap::pair( "atan2",         atan2)
        << ParseMathMLMap::pair( "atan2d",        atan2d)

        << ParseMathMLMap::pair( "unitmatrix",    unitmatrix)
        << ParseMathMLMap::pair( "eulertransform",eulertransform)
        << ParseMathMLMap::pair( "eulertransformd",eulertransformd)
        << ParseMathMLMap::pair( "cross",         cross)
        << ParseMathMLMap::pair( "mask",          mask)
        << ParseMathMLMap::pair( "mask_plus",     mask_plus)
        << ParseMathMLMap::pair( "mask_minus",    mask_minus)
        << ParseMathMLMap::pair( "mask_times",    mask_times)
        << ParseMathMLMap::pair( "mask_divide",   mask_divide)

        // Possible csymbol matrix ops.
//        << ParseMathMLMap::pair( "eulerTransformMatrixPsi",  eulerTransformMatrixPsi)
//        << ParseMathMLMap::pair( "eulerTransformMatrixTheta",  eulerTransformMatrixTheta)
//        << ParseMathMLMap::pair( "eulerTransformMatrixPhi",  eulerTransformMatrixPhi)
//        << ParseMathMLMap::pair( "eulerTransformMatrixPsiTheta",  eulerTransformMatrixPsiTheta)
//        << ParseMathMLMap::pair( "eulerTransformMatrixPsiPhi",  eulerTransformMatrixPsiPhi)
//        << ParseMathMLMap::pair( "eulerTransformMatrixThetaPhi",  eulerTransformMatrixThetaPhi)
    );

  } // end namespace parsemathml

} // end namespace dstomathml


