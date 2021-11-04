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
// Module:     SolveMathML.cpp
// First Date: 2012
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file SolveMathML.cpp
 *
 * This class contains functions for solving mathematics procedures
 * defined using the MathML syntax. Data detailing each MathML operation
 * and is stored in a MathMLData structure. This includes the
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
#include <iostream>

// Local Includes
#include "MathMLDataClass.h"
#include "SolveMathML.h"
#include "VariableDef.h"

// Ute Includes
#include <Ute/aMath.h>
#include <Ute/aMatrix.h>
#include <Ute/aMessageStream.h>

using namespace std;
using namespace dstomath;

namespace dstomathml {

  //----------------------------------- Value Only Lookups --------------------------------------------//
  //------------------ See below half way for combined Value and Matrix Lookups -----------------------//

  namespace solvemathml {

    /*
     * Function to solve mathematics procedures defined using the MathML syntax.
     */

    double solve( const MathMLData& t)
    {
      return t.mathMLFunctionPtr_( t);
    }

    /*
     * Numbers and Identifiers
     *
     * cn
     * ci
     * apply
     * csymbol
     */
    double cn( const MathMLData& t)
    {
      return t.cnValue_;
    }

    double ci( const MathMLData& t)
    {
      return t.variableDef_->getValue();
    }

    double apply( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "apply: incorrect number of associated elements.");
        }
      );

      t.value_ = solve( t.mathChildren_.front());
      t.test_ = t.mathChildren_.front().test_;
      t.mathRetType_ = t.mathChildren_.front().mathRetType_;

      return t.value_;
    }

    double csymbol( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "csymbol: incorrect number of associated elements.");
        }
      );

      return solve( t.mathChildren_.front());
    }

    /*
     * Equivalent IF-THEN-ELSE
     *
     * piecewise
     * piece
     * otherwise
     */

    double piecewise( const MathMLData& t)
    {
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end();
            ++child) {
        t.value_ = solve( *child);
        if ( child->test_) {
          return t.value_;
        }
      }
      t.value_ = dstomath::nan();
      return t.value_;
    }

    double piece( const MathMLData& t)
    {
     math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "piece: incorrect number of associated elements.");
        }
      );

      t.test_ = false;
      solve( t.mathChildren_.back());
      if ( t.mathChildren_.back().test_) {
        t.test_ = true;
        return solve( t.mathChildren_.front());
      }
      return dstomath::nan();
    }

    double otherwise( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "otherwise: incorrect number of associated elements.");
        }
      );

      t.test_ = true;
      return solve( t.mathChildren_.front());
    }

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
     * 'MathMLData' to determine if real or boolean data is stored or
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
    double eq( const MathMLData& t)
    {
      t.test_ = ( solve( t.mathChildren_.front()) == solve( t.mathChildren_.back()));
      return double( t.test_);
    }

    double neq( const MathMLData& t)
    {
      t.test_ = ( solve( t.mathChildren_.front()) != solve( t.mathChildren_.back()));
      return double( t.test_);
    }

    double gt( const MathMLData& t)
    {
      t.test_ = ( solve( t.mathChildren_.front()) > solve( t.mathChildren_.back()));
      return double( t.test_);
    }

    double geq( const MathMLData& t)
    {
      t.test_ = ( solve( t.mathChildren_.front()) >= solve( t.mathChildren_.back()));
      return double( t.test_);
    }

    double lt( const MathMLData& t)
    {
      t.test_ = ( solve( t.mathChildren_.front()) < solve( t.mathChildren_.back()));
      return double( t.test_);
    }

    double leq( const MathMLData& t)
    {
      t.test_ = ( solve( t.mathChildren_.front()) <= solve( t.mathChildren_.back()));
      return double( t.test_);
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
     * the associated 'MathMLData' to determine if real or boolean data
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

    double quotient( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "quotient: incorrect number of associated elements.");
        }
      )

      double quot;
      ::modf(( solve( t.mathChildren_.front()) / solve( t.mathChildren_.back())), &quot);
      return quot;
    }

    double factorial( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "factorial: incorrect number of associated elements.");
        }
      );

      return ( dstomath::fact( solve( t.mathChildren_.front())));
    }

    double divide( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "divide: incorrect number of associated elements.");
        }
      );

      return solve( t.mathChildren_.front()) / solve( t.mathChildren_.back());
    }

    double max( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      double result = solve( *child);
      while ( ++child != t.mathChildren_.end()) {
        result = dstomath::max( result, solve( *child));
      }
      return result;
    }

    double min( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      double result = solve( *child);
      while ( ++child != t.mathChildren_.end()) {
        result = dstomath::min( result, solve( *child));
      }
      return result;
    }

    double minus( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      double result = solve( *child);
      if ( t.mathChildren_.size() > 1) {
        while ( ++child != t.mathChildren_.end()) {
          result -= solve( *child);
        }
        return result;
      }
      return -result;
    }

    double plus( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      double result = solve( *child);
      while ( ++child != t.mathChildren_.end()) {
        result += solve( *child);
      }
      return result;
    }

    double power( const MathMLData& t)
    {
      return ::pow( solve( t.mathChildren_.front()),
                    solve( t.mathChildren_.back()));
    }

    double rem( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "rem: incorrect number of associated elements.");
        }
      );

      double quot;
      return ::modf(( solve( t.mathChildren_.front()) / solve( t.mathChildren_.back())), &quot);
    }

    double times( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      double result = solve( *child);
      while ( ++child != t.mathChildren_.end()) {
        result *= solve( *child);
      }
      return result;
    }

    double root( const MathMLData& t)
    {
      if ( t.mathChildren_.size() == 1) {
        return ::sqrt( solve( t.mathChildren_.front()));
      }
      return ::pow( solve( t.mathChildren_.back()), ( 1.0 / solve( t.mathChildren_.front())));
    }

    double degree( const MathMLData& t)
    {
      return solve( t.mathChildren_.front());
    }

    double and_( const MathMLData& t)
    {
      t.test_ = true;
      double value( 0.0);
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end();
            ++child) {
        value = solve( *child);
        if ( child->mathRetType_ == dstomathml::BOOL) {
          if ( !child->test_) {
            t.test_ = false;
            break;
          }
        }
        else {
          math_range_check( throw_message( invalid_argument, "and: arguments must be of type BOOL."));
          if ( dstomath::isZero( value)) {
            t.test_ = false;
            break;
          }
        }
      }

      return double( t.test_);
    }

    double or_( const MathMLData& t)
    {
      t.test_ = false;
      double value( 0.0);
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end();
            ++child) {
        value = solve( *child);
        if ( child->mathRetType_ == dstomathml::BOOL) {
          if ( child->test_) {
            t.test_ = true;
            break;
          }
        }
        else {
          math_range_check( throw_message( invalid_argument, "or: arguments must be of type BOOL."));
          if ( dstomath::isNotZero( value)) {
            t.test_ = true;
            break;
          }
        }
      }

      return double( t.test_);
    }

    double xor_( const MathMLData& t)
    {
      t.test_ = false;
      double value( 0.0);
      size_t nTrueArg( 0);
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end();
            ++child) {
        value = solve( *child);
        if ( child->mathRetType_ == dstomathml::BOOL) {
          if ( child->test_) {
            ++nTrueArg;
          }
        }
        else {
          math_range_check( throw_message( invalid_argument, "xor: arguments must be of type BOOL."));
          if ( dstomath::isNotZero( value)) {
            ++nTrueArg;
          }
        }
      }

      if ( nTrueArg == 1) {
       t.test_ = true;
      }

      return double( t.test_);
    }

    double not_( const MathMLData& t)
    {
      t.test_ = false;
      double value = solve( t.mathChildren_.front());
      if ( t.mathChildren_.front().mathRetType_ == dstomathml::BOOL) {
        t.test_ = !t.mathChildren_.front().test_;
      }
      else {
        math_range_check( throw_message( invalid_argument, "not: argument must be of type BOOL."));
        t.test_ = dstomath::isZero( value);
      }

      return double( t.test_);
    }

    double abs( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "abs: incorrect number of associated elements.");
        }
      );

      return ( dstomath::abs( solve( t.mathChildren_.front())));
    }

    double floor( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "floor: incorrect number of associated elements.");
        }
      );

      return ( std::floor( solve( t.mathChildren_.front())));
    }

    double ceiling( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "ceiling: incorrect number of associated elements.");
        }
      );

      return ( std::ceil( solve( t.mathChildren_.front())));
    }

    /*
     * Functions and Inverses
     *
     * fmod
     * sign
     * bound
     * nearbyint
     */

    double fmod( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "fmod: incorrect number of associated elements.");
        }
      );

      return ::fmod( solve( t.mathChildren_.front()), solve( t.mathChildren_.back()));
    }

    double sign( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "sign: incorrect number of associated elements.");
        }
      );

      return dstomath::copysign( solve( t.mathChildren_.front()),
                                 solve( t.mathChildren_.back()));
    }

    double bound( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 3) {
          throw_message( invalid_argument, "bound: incorrect number of associated elements.");
        }
      );

      return dstomath::bound( solve( t.mathChildren_[ 0]),
                              solve( t.mathChildren_[ 1]),
                              solve( t.mathChildren_[ 2]));
    }

    double nearbyint( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "nearbyint: incorrect number of associated elements.");
        }
      );

      return dstomath::nearbyint( solve( t.mathChildren_.front()));
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
     * arccsc
     * arccot
     * arcsec
     * arcsind
     * arccosd
     * arctand
     * arccscd
     * arccotd
     * arcsecd
     * atan2
     * atan2d
     *
     * b) others
     * exp
     * ln
     * log, logbase
     */
    double sin( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "sin: incorrect number of associated elements.");
        }
      );

      return ::sin( solve( t.mathChildren_.front()));
    }

    double cos( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cos: incorrect number of associated elements.");
        }
      );

      return ::cos( solve( t.mathChildren_.front()));
    }

    double tan( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "tan: incorrect number of associated elements.");
        }
      );

      return ::tan( solve( t.mathChildren_.front()));
    }

    double sec( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "sec: incorrect number of associated elements.");
        }
      );

      return 1.0 / ::cos( solve( t.mathChildren_.front()));
    }

    double csc( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "csc: incorrect number of associated elements.");
        }
      );

      return 1.0 / ::sin( solve( t.mathChildren_.front()));
    }

    double cot( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cot: incorrect number of associated elements.");
        }
      );

      return 1.0 / ::tan( solve( t.mathChildren_.front()));
    }

    double sind( const MathMLData& t)
    {

      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "sind: incorrect number of associated elements.");
        }
      );

      return ::sin( solve( t.mathChildren_.front()) * dstomath::pi_180);
    }

    double cosd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cosd: incorrect number of associated elements.");
        }
      );

      return ::cos( solve( t.mathChildren_.front()) * dstomath::pi_180);
    }

    double tand( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "tand: incorrect number of associated elements.");
        }
      );

      return ::tan( solve( t.mathChildren_.front()) * dstomath::pi_180);
    }

    double secd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "secd: incorrect number of associated elements.");
        }
      );

      return 1.0 / ::cos( solve( t.mathChildren_.front()) * dstomath::pi_180);
    }

    double cscd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cscd: incorrect number of associated elements.");
        }
      );

      return 1.0 / ::sin( solve( t.mathChildren_.front()) * dstomath::pi_180);
    }

    double cotd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cotd: incorrect number of associated elements.");
        }
      );

      return 1.0 / ::tan( solve( t.mathChildren_.front()) * dstomath::pi_180);
    }

    double arcsin( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsin: incorrect number of associated elements.");
        }
      );

      return asin( solve( t.mathChildren_.front()));
    }

    double arccos( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccos: incorrect number of associated elements.");
        }
      );

      return acos( solve( t.mathChildren_.front()));
    }

    double arctan( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arctan: incorrect number of associated elements.");
        }
      );

      return atan( solve( t.mathChildren_.front()));
    }

    double arccsc( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccsc: incorrect number of associated elements.");
        }
      );

      return asin( 1.0 / solve( t.mathChildren_.front()));
    }

    double arccot( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccot: incorrect number of associated elements.");
        }
      );

      return atan( 1.0 / solve( t.mathChildren_.front()));
    }

    double arcsec( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsec: incorrect number of associated elements.");
        }
      );

      return acos( 1.0 / solve( t.mathChildren_.front()));
    }

    double arcsind( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsind: incorrect number of associated elements.");
        }
      );

      return asin( solve( t.mathChildren_.front())) * dstomath::d180_pi;
    }

    double arccosd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccosd: incorrect number of associated elements.");
        }
      );

      return acos( solve( t.mathChildren_.front())) * dstomath::d180_pi;
    }

    double arctand( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arctand: incorrect number of associated elements.");
        }
      );

      return atan( solve( t.mathChildren_.front())) * dstomath::d180_pi;
    }

    double arcsecd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsecd: incorrect number of associated elements.");
        }
      );

      return acos( 1.0 / solve( t.mathChildren_.front())) * dstomath::d180_pi;
    }

    double arccscd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccscd: incorrect number of associated elements.");
        }
      );

      return asin( 1.0 / solve( t.mathChildren_.front())) * dstomath::d180_pi;
    }

    double arccotd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccotd: incorrect number of associated elements.");
        }
      );

      return atan( 1.0 / solve( t.mathChildren_.front())) * dstomath::d180_pi;
    }

    double atan2( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "atan2: incorrect number of associated elements.");
        }
      );

      return ::atan2( solve( t.mathChildren_.front()),
                      solve( t.mathChildren_.back()));
    }

    double atan2d( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "atan2: incorrect number of associated elements.");
        }
      );

      return ::atan2( solve( t.mathChildren_.front()),
                      solve( t.mathChildren_.back())) * dstomath::d180_pi;
    }

    double exp( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "exp: incorrect number of associated elements.");
        }
      );

      return ::exp( solve( t.mathChildren_.front()));
    }

    double ln( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "ln: incorrect number of associated elements.");
        }
      );

      return ::log( solve( t.mathChildren_.front()));
    }

    double log( const MathMLData& t)
    {
      if ( t.mathChildren_.size() == 1) {
        return ::log10( solve( t.mathChildren_.front()));
      }

      return ::log( solve( t.mathChildren_.back())) / ::log( solve( t.mathChildren_.front()));
    }

    double logbase( const MathMLData& t)
    {
      return solve( t.mathChildren_.front());
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
    double exponentiale( const MathMLData&)
    {
      return dstomathml::EXPONENTIALE;
    }

    double notanumber( const MathMLData&)
    {
      return dstomath::nan();
    }

    double pi( const MathMLData&)
    {
      return dstomath::pi;
    }

    double eulergamma( const MathMLData&)
    {
      return dstomathml::EULERGAMMA;
    }

    double infinity( const MathMLData&)
    {
      return dstomath::infinity();
    }

    double noop( const MathMLData&)
    {
      // What to return for no operation? (0, 1, nan, ... ?)
      return 0.0;
    }

    SolveMathMLMap solveMathMLMap = ( SolveMathMLMap()
        << SolveMathMLMap::pair( "cn",            cn)
        << SolveMathMLMap::pair( "ci",            ci)
        << SolveMathMLMap::pair( "apply",         apply)
        << SolveMathMLMap::pair( "csymbol",       csymbol)
        << SolveMathMLMap::pair( "eq",            eq)
        << SolveMathMLMap::pair( "neq",           neq)
        << SolveMathMLMap::pair( "gt",            gt)
        << SolveMathMLMap::pair( "geq",           geq)
        << SolveMathMLMap::pair( "lt",            lt)
        << SolveMathMLMap::pair( "leq",           leq)
        << SolveMathMLMap::pair( "quotient",      quotient)
        << SolveMathMLMap::pair( "factorial",     factorial)
        << SolveMathMLMap::pair( "divide",        divide)
        << SolveMathMLMap::pair( "max",           max)
        << SolveMathMLMap::pair( "min",           min)
        << SolveMathMLMap::pair( "minus",         minus)
        << SolveMathMLMap::pair( "plus",          plus)
        << SolveMathMLMap::pair( "power",         power)
        << SolveMathMLMap::pair( "rem",           rem)
        << SolveMathMLMap::pair( "times",         times)
        << SolveMathMLMap::pair( "root",          root)
        << SolveMathMLMap::pair( "degree",        degree)
        << SolveMathMLMap::pair( "and",           and_)
        << SolveMathMLMap::pair( "or",            or_)
        << SolveMathMLMap::pair( "xor",           xor_)
        << SolveMathMLMap::pair( "not",           not_)
        << SolveMathMLMap::pair( "abs",           abs)
        << SolveMathMLMap::pair( "floor",         floor)
        << SolveMathMLMap::pair( "ceiling",       ceiling)
        << SolveMathMLMap::pair( "piecewise",     piecewise)
        << SolveMathMLMap::pair( "piece",         piece)
        << SolveMathMLMap::pair( "otherwise",     otherwise)
        << SolveMathMLMap::pair( "fmod",          fmod)
        << SolveMathMLMap::pair( "sign",          sign)
        << SolveMathMLMap::pair( "bound",         bound)
        << SolveMathMLMap::pair( "nearbyint",     nearbyint)
        << SolveMathMLMap::pair( "sin",           sin)
        << SolveMathMLMap::pair( "cos",           cos)
        << SolveMathMLMap::pair( "tan",           tan)
        << SolveMathMLMap::pair( "sec",           sec)
        << SolveMathMLMap::pair( "csc",           csc)
        << SolveMathMLMap::pair( "cot",           cot)
        << SolveMathMLMap::pair( "arcsin",        arcsin)
        << SolveMathMLMap::pair( "arccos",        arccos)
        << SolveMathMLMap::pair( "arctan",        arctan)
        << SolveMathMLMap::pair( "arccsc",        arccsc)
        << SolveMathMLMap::pair( "arccot",        arccot)
        << SolveMathMLMap::pair( "arcsec",        arcsec)
        << SolveMathMLMap::pair( "exp",           exp)
        << SolveMathMLMap::pair( "ln",            ln)
        << SolveMathMLMap::pair( "log",           log)
        << SolveMathMLMap::pair( "logbase",       logbase)
        << SolveMathMLMap::pair( "exponentiale",  exponentiale)
        << SolveMathMLMap::pair( "notanumber",    notanumber)
        << SolveMathMLMap::pair( "pi",            pi)
        << SolveMathMLMap::pair( "eulergamma",    eulergamma)
        << SolveMathMLMap::pair( "infinity",      infinity)
        << SolveMathMLMap::pair( "noop",          noop)

        // csymbol definitions (technically these are not defined in MathML)
        << SolveMathMLMap::pair( "sind",          sind)
        << SolveMathMLMap::pair( "cosd",          cosd)
        << SolveMathMLMap::pair( "tand",          tand)
        << SolveMathMLMap::pair( "secd",          secd)
        << SolveMathMLMap::pair( "cscd",          cscd)
        << SolveMathMLMap::pair( "cotd",          cotd)
        << SolveMathMLMap::pair( "arcsind",       arcsind)
        << SolveMathMLMap::pair( "arccosd",       arccosd)
        << SolveMathMLMap::pair( "arctand",       arctand)
        << SolveMathMLMap::pair( "arcsecd",       arcsecd)
        << SolveMathMLMap::pair( "arccscd",       arccscd)
        << SolveMathMLMap::pair( "arccotd",       arccotd)
        << SolveMathMLMap::pair( "atan2",         atan2)
        << SolveMathMLMap::pair( "atan2d",        atan2d)
    );

  } // end namespace solvemathml


  //----------------------------------- Value and Matrix Lookups --------------------------------------------//


  namespace solvematrixmathml {

    /*
     * Function to solve mathematics procedures defined using the MathML syntax.
     */

    const MathMLData& solve( const MathMLData& t)
    {
      return t.mathMLMatrixFunctionPtr_( t);
    }

    /*
     * Numbers and Identifiers
     *
     * cn
     * ci
     * apply
     * csymbol
     */
    const MathMLData& cn( const MathMLData& t)
    {
      t = t.cnValue_;
      return t;
    }

    const MathMLData& ci( const MathMLData& t)
    {
      if ( t.variableDef_->isMatrix()) {
        t = t.variableDef_->getMatrix();
      }
      else {
        t = t.variableDef_->getValue();
      }
      return t;
    }

    const MathMLData& apply( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "apply: incorrect number of associated elements.");
        }
      );

      t = solve( t.mathChildren_.front());
      t.mathRetType_ = t.mathChildren_.front().mathRetType_;

      return t;
    }

    const MathMLData& csymbol( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "csymbol: incorrect number of associated elements.");
        }
      );

      t = solve( t.mathChildren_.front());
      return t;
    }

    /*
     * Equivalent IF-THEN-ELSE
     *
     * piecewise
     * piece
     * otherwise
     */

    const MathMLData& piecewise( const MathMLData& t)
    {
      t = dstomath::nan();
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end(); ++child) {
        if ( solve( *child).test_) {
          t = *child;
          break;
        }
      }
      return t;
    }

    const MathMLData& piece( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "piece: incorrect number of associated elements.");
        }
      );

      t.test_ = false;
      if ( solve( t.mathChildren_.back()).test_) {
        t = solve( t.mathChildren_.front());
        t.test_ = true; // This piecewise piece is true;
      }
      return t;
    }

    const MathMLData& otherwise( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "otherwise: incorrect number of associated elements.");
        }
      );

      t = solve( t.mathChildren_.front());
      t.test_ = true; // Last test of piecewise is always true.
      return t;
    }

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
     * 'MathMLData' to determine if real or boolean data is stored or
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

    const MathMLData& eq( const MathMLData& t)
    {
      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( !v1.isMatrix_ && !v2.isMatrix_) {
        t.test_ = ( v1.value_ == v2.value_);
      }
      else if ( v1.isMatrix_ && v2.isMatrix_) {
        t.test_ = ( v1.matrix_ == v2.matrix_);
      }
      else {
        t.test_ = false;
      }
      return t;
    }

    const MathMLData& neq( const MathMLData& t)
    {
      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( !v1.isMatrix_ && !v2.isMatrix_) {
        t.test_ = ( v1.value_ != v2.value_);
      }
      else if ( v1.isMatrix_ && v2.isMatrix_) {
        t.test_ = ( v1.matrix_ != v2.matrix_);
      }
      else {
        t.test_ = true;
      }
      return t;
    }

    const MathMLData& gt( const MathMLData& t)
    {
      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( v1.isMatrix_ || v2.isMatrix_) {
        t.test_ = false;
      }
      else {
        t.test_ = ( v1.value_ > v2.value_);
      }
      return t;
    }

    const MathMLData& geq( const MathMLData& t)
    {
      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( v1.isMatrix_ || v2.isMatrix_) {
        t.test_ = false;
      }
      else {
        t.test_ = ( v1.value_ >= v2.value_);
      }
      return t;
    }

    const MathMLData& lt( const MathMLData& t)
    {
      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( v1.isMatrix_ || v2.isMatrix_) {
        t.test_ = false;
      }
      else {
        t.test_ = ( v1.value_ < v2.value_);
      }
      return t;
    }

    const MathMLData& leq( const MathMLData& t)
    {
      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( v1.isMatrix_ || v2.isMatrix_) {
        t.test_ = false;
      }
      else {
        t.test_ = ( v1.value_ <= v2.value_);
      }
      return t;
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
     * the associated 'MathMLData' to determine if real or boolean data
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

    const MathMLData& quotient( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "quotient: incorrect number of associated elements.");
        }
      );

      t = 0.0;
      ::modf( solve( t.mathChildren_.front()).value_ / solve( t.mathChildren_.back()).value_, &t.value_);

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_ || t.mathChildren_.back().isMatrix_) {
          throw_message( invalid_argument, "quotient: operation not permitted on a matrix.");
        }
      );

      return t;
    }

    const MathMLData& factorial( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "factorial: incorrect number of associated elements.");
        }
      );

      t = dstomath::fact( solve( t.mathChildren_.front()).value_);

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_ || t.mathChildren_.back().isMatrix_) {
          throw_message( invalid_argument, "factorial: operation not permitted on a matrix.");
        }
      );

      return t;
    }

    const MathMLData& divide( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "divide: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      if ( !v1.isMatrix_ && !v2.isMatrix_) {
        t = v1.value_ / v2.value_;
      }
      else if ( v1.isMatrix_ && v2.isMatrix_) {
        t = v1.matrix_ / v2.matrix_;
      }
      else if ( v1.isMatrix_){
        t = v1.matrix_ / v2.value_;
      }
      else {
        t = v1.value_ / v2.matrix_;
      }
      return t;
    }

    const MathMLData& max( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      t = solve( *child).value_;

      math_range_check(
        if ( child->isMatrix_) {
          throw_message( invalid_argument, "max: operation not permitted on a matrix.");
        }
      );

      while ( ++child != t.mathChildren_.end()) {
        t.value_ = dstomath::max( t.value_, solve( *child).value_);

        math_range_check(
          if ( child->isMatrix_) {
            throw_message( invalid_argument, "max: operation not permitted on a matrix.");
          }
        );
      }
      return t;
    }

    const MathMLData& min( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      t = solve( *child).value_;

      math_range_check(
        if ( child->isMatrix_) {
          throw_message( invalid_argument, "min: operation not permitted on a matrix.");
        }
      );

      while ( ++child != t.mathChildren_.end()) {
        t.value_ = dstomath::min( t.value_, solve( *child).value_);

        math_range_check(
          if ( child->isMatrix_) {
            throw_message( invalid_argument, "min: operation not permitted on a matrix.");
          }
        );
      }
      return t;
    }

    const MathMLData& minus( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      t = solve( *child);

      // Unary operator.
      if ( t.mathChildren_.size() == 1) {
        if ( !t.isMatrix_) {
          t.value_ = -t.value_;
        }
        else {
          t.matrix_ = -t.matrix_;
        }
      }

      // Multiple arguments.
      while ( ++child != t.mathChildren_.end()) {
        const MathMLData& v1 = solve( *child);
        if ( !t.isMatrix_) {
          if ( !v1.isMatrix_) {
            t.value_ -= v1.value_; // t & v are values.
          }
          else {
            t = t.value_ - v1.matrix_; // t is now a matrix based on v.
          }
        }
        else {
          if ( v1.isMatrix_) {
            t.matrix_ -= v1.matrix_; // t remains a matrix.
          }
          else {
            t.matrix_ -= v1.value_; // t remains a matrix.
          }
        }
      }
      return t;
    }

    const MathMLData& plus( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      t = solve( *child);
      while ( ++child != t.mathChildren_.end()) {
        const MathMLData& v1 = solve( *child);
        if ( !t.isMatrix_) {
          if ( !v1.isMatrix_) {
            t.value_ += v1.value_; // t & v are values.
          }
          else {
            t = t.value_ + v1.matrix_; // t is now a matrix based on v.
          }
        }
        else {
          if ( v1.isMatrix_) {
            t.matrix_ += v1.matrix_; // t remains a matrix.
          }
          else {
            t.matrix_ += v1.value_; // t remains a matrix.
          }
        }
      }
      return t;
    }

    const MathMLData& power( const MathMLData& t)
    {
      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      if ( !v1.isMatrix_ && !v2.isMatrix_) {
        t = ::pow( v1.value_, v2.value_);
      }
      else {
        math_range_check(
          if ( v2.isMatrix_) {
            throw_message( invalid_argument, "power: argument 2 can't be a matrix.");
          }
          if ( int( v2.value_) < 0) {
            throw_message( invalid_argument, "power: argument 2 can't be negative.");
          }
        );

        t = pow( v1.matrix_, size_t( v2.value_));
      }
      return t;
    }

    const MathMLData& rem( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "rem: incorrect number of associated elements.");
        }
      );

      double unused;
      t = ::modf( solve( t.mathChildren_.front()).value_ / solve( t.mathChildren_.back()).value_, &unused);

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_ || t.mathChildren_.back().isMatrix_) {
          throw_message( invalid_argument, "rem: operation not permitted on a matrix.");
        }
      );

      return t;
    }

    const MathMLData& times( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      t = solve( *child);
      while ( ++child != t.mathChildren_.end()) {
        const MathMLData& v1 = solve( *child);
        if ( !t.isMatrix_) {
          if ( !v1.isMatrix_) {
            t.value_ *= v1.value_; // t & v are values.
          }
          else {
            t = t.value_ * v1.matrix_; // t is now a matrix based on v.
          }
        }
        else {
          if ( v1.isMatrix_) {
            t.matrix_ *= v1.matrix_; // t remains a matrix.
          }
          else {
            t.matrix_ *= v1.value_; // t remains a matrix.
          }
        }
      }
      return t;
    }

    const MathMLData& root( const MathMLData& t)
    {
      if ( t.mathChildren_.size() == 1) {
        t = ::sqrt( solve( t.mathChildren_.front()).value_);

        math_range_check(
          if ( t.mathChildren_.front().isMatrix_) {
            throw_message( invalid_argument, "root: operation not permitted on a matrix.");
          }
        );
      }
      else {
        t = ::pow( solve( t.mathChildren_.back()).value_, 1.0 / solve( t.mathChildren_.front()).value_);

        math_range_check(
          if ( t.mathChildren_.front().isMatrix_ || t.mathChildren_.back().isMatrix_) {
            throw_message( invalid_argument, "root: operation not permitted on a matrix.");
          }
        );
      }
      return t;
    }

    const MathMLData& degree( const MathMLData& t)
    {
      t = solve( t.mathChildren_.front()).value_;

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_) {
          throw_message( invalid_argument, "degree: value can't be a matrix.");
        }
      );

      return t;
    }

    const MathMLData& and_( const MathMLData& t)
    {
      t = 1.0;
      t.test_ = true; //must come after t=(double), which sets test_ to false
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end(); ++child) {
        math_range_check(
          if ( child->mathRetType_ != dstomathml::BOOL) {
            throw_message( invalid_argument, "and_: arguments must be of type BOOL.");
          }
        );
        if ( dstomath::isZero( solve( *child).value_)) {
          t.value_ = 0.0;
          t.test_ = false;
          break;
        }
      }
      return t;
    }

    const MathMLData& or_( const MathMLData& t)
    {
      t = 0.0;
      //t.test_ = false; //must come after t=(double), which sets test_ to false
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end();
            ++child) {
        math_range_check(
          if ( child->mathRetType_ != dstomathml::BOOL) {
            throw_message( invalid_argument, "or_: arguments must be of type BOOL.");
          }
        );
        if ( dstomath::isNotZero( solve( *child).value_)) {
          t.value_= 1.0;
          t.test_ = true; //must come after t=(double), which sets test_ to false
          break;
        }
      }
      return t;
    }

    const MathMLData& xor_( const MathMLData& t)
    {
      size_t nTrueArg = 0;
      for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
            child != t.mathChildren_.end(); ++child) {
        math_range_check(
          if ( child->mathRetType_ != dstomathml::BOOL) {
            throw_message( invalid_argument, "xor_: arguments must be of type BOOL.");
          }
        );
        if ( dstomath::isNotZero( solve( *child).value_)) {
          ++nTrueArg;
        }
      }
      if ( nTrueArg == 1) {
        t = 1.0;
        t.test_ = true; //must come after t=(double), which sets test_ to false
      }
      else {
        t = 0.0;
        //t.test_ = false; //must come after t=(double), which sets test_ to false
      }
      return t;
    }

    const MathMLData& not_( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "not_: too many associated elements.");
        }
        if ( t.mathChildren_.front().mathRetType_ != dstomathml::BOOL) {
          throw_message( invalid_argument, "not_: argument must be of type BOOL.");
        }
      );

      t = solve( t.mathChildren_.front());
      t.test_ = !t.mathChildren_.front().test_;
      t.value_ = double( t.test_);

      return t;
    }

    const MathMLData& abs( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "abs: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = dstomath::abs( v1.value_);
      }
      else {
        t = abs( v1.matrix_);
      }
      return t;
    }

    const MathMLData& floor( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "floor: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = std::floor( v1.value_);
      }
      else {
        t = floor( v1.matrix_);
      }
      return t;
    }

    const MathMLData& ceiling( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "ceiling: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = std::ceil( v1.value_);
      }
      else {
        t = ceil( v1.matrix_);
      }
      return t;
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

    const MathMLData& fmod( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "fmod: incorrect number of associated elements.");
        }
      );

      t = ::fmod( solve( t.mathChildren_.front()).value_, solve( t.mathChildren_.back()).value_);

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_ || t.mathChildren_.back().isMatrix_) {
          throw_message( invalid_argument, "fmod: operation not permitted on a matrix.");
        }
      );

      return t;
    }

    const MathMLData& sign( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "sign: incorrect number of associated elements.");
        }
      );

      t = dstomath::copysign( solve( t.mathChildren_.front()).value_,
                              solve( t.mathChildren_.back()).value_);

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_ || t.mathChildren_.back().isMatrix_) {
          throw_message( invalid_argument, "sign: operation not permitted on a matrix.");
        }
      );

      return t;
    }

    const MathMLData& bound( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 3) {
          throw_message( invalid_argument, "bound: incorrect number of associated elements.");
        }
      );

      t = dstomath::bound( solve( t.mathChildren_[ 0]).value_,
                           solve( t.mathChildren_[ 1]).value_,
                           solve( t.mathChildren_[ 2]).value_);

      math_range_check(
        if ( t.mathChildren_[ 0].isMatrix_ || t.mathChildren_[ 1].isMatrix_ || t.mathChildren_[ 2].isMatrix_) {
          throw_message( invalid_argument, "bound: operation not permitted on a matrix.");
        }
      );

      return t;
    }

    const MathMLData& nearbyint( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "nearbyint: incorrect number of associated elements.");
        }
      );

      t = dstomath::nearbyint( solve( t.mathChildren_.front()).value_);

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_) {
          throw_message( invalid_argument, "nearbyint: operation not permitted on a matrix.");
        }
      );

      return t;
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
    const MathMLData& sin( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "sin: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::sin( v1.value_);
      }
      else {
        t = sin( v1.matrix_);
      }
      return t;
    }

    const MathMLData& cos( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cos: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::cos( v1.value_);
      }
      else {
        t = cos( v1.matrix_);
      }
      return t;
    }

    const MathMLData& tan( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "tan: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::tan( v1.value_);
      }
      else {
        t = tan( v1.matrix_);
      }
      return t;
    }

    const MathMLData& sec( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "sec: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = 1.0 / ::cos( v1.value_);
      }
      else {
        t = 1.0 / cos( v1.matrix_);
      }
      return t;
    }

    const MathMLData& csc( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "csc: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = 1.0 / ::sin( v1.value_);
      }
      else {
        t = 1.0 / sin( v1.matrix_);
      }
      return t;
    }

    const MathMLData& cot( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cot: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = 1.0 / ::tan( v1.value_);
      }
      else {
        t = 1.0 / tan( v1.matrix_);
      }
      return t;
    }

    const MathMLData& sind( const MathMLData& t)
    {

      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "sind: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::sin( v1.value_ * dstomath::pi_180);
      }
      else {
        t = sin( v1.matrix_ * dstomath::pi_180);
      }
      return t;
    }

    const MathMLData& cosd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cosd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::cos( v1.value_ * dstomath::pi_180);
      }
      else {
        t = cos( v1.matrix_ * dstomath::pi_180);
      }
      return t;
    }

    const MathMLData& tand( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "tand: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::tan( v1.value_ * dstomath::pi_180);
      }
      else {
        t = tan( v1.matrix_ * dstomath::pi_180);
      }
      return t;
    }

    const MathMLData& secd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "secd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = 1.0 / ::cos( v1.value_ * dstomath::pi_180);
      }
      else {
        t = 1.0 / cos( v1.matrix_ * dstomath::pi_180);
      }
      return t;
    }

    const MathMLData& cscd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cscd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = 1.0 / ::sin( v1.value_ * dstomath::pi_180);
      }
      else {
        t = 1.0 / sin( v1.matrix_ * dstomath::pi_180);
      }
      return t;
    }

    const MathMLData& cotd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cotd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = 1.0 / ::tan( v1.value_ * dstomath::pi_180);
      }
      else {
        t = 1.0 / tan( v1.matrix_ * dstomath::pi_180);
      }
      return t;
    }

    const MathMLData& arcsin( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsin: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::asin( v1.value_);
      }
      else {
        t = asin( v1.matrix_);
      }
      return t;
    }

    const MathMLData& arccos( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccos: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::acos( v1.value_);
      }
      else {
        t = acos( v1.matrix_);
      }
      return t;
    }

    const MathMLData& arctan( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arctan: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::atan( v1.value_);
      }
      else {
        t = atan( v1.matrix_);
      }
      return t;
    }

    const MathMLData& arcsec( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsec: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::acos( 1.0 / v1.value_);
      }
      else {
        t = acos( 1.0 / v1.matrix_);
      }
      return t;
    }

    const MathMLData& arccsc( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccsc: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::asin( 1.0 / v1.value_);
      }
      else {
        t = asin( 1.0 / v1.matrix_);
      }
      return t;
    }

    const MathMLData& arccot( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccot: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::atan( 1.0 / v1.value_);
      }
      else {
        t = atan( 1.0 / v1.matrix_);
      }
      return t;
    }

    const MathMLData& arcsind( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsind: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::asin( v1.value_) * dstomath::d180_pi;
      }
      else {
        t = asin( v1.matrix_) * dstomath::d180_pi;
      }
      return t;
    }

    const MathMLData& arccosd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccosd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::acos( v1.value_) * dstomath::d180_pi;
      }
      else {
        t = acos( v1.matrix_) * dstomath::d180_pi;
      }
      return t;
    }

    const MathMLData& arctand( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arctand: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::atan( v1.value_) * dstomath::d180_pi;
      }
      else {
        t = atan( v1.matrix_) * dstomath::d180_pi;
      }
      return t;
    }

    const MathMLData& arcsecd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arcsecd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::acos( 1.0 / v1.value_) * dstomath::d180_pi;
      }
      else {
        t = acos( 1.0 / v1.matrix_) * dstomath::d180_pi;
      }
      return t;
    }

    const MathMLData& arccscd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccscd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::asin( 1.0 / v1.value_) * dstomath::d180_pi;
      }
      else {
        t = asin( 1.0 / v1.matrix_) * dstomath::d180_pi;
      }
      return t;
    }

    const MathMLData& arccotd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "arccotd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::atan( 1.0 / v1.value_) * dstomath::d180_pi;
      }
      else {
        t = atan( 1.0 / v1.matrix_) * dstomath::d180_pi;
      }
      return t;
    }

    const MathMLData& atan2( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "atan2: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( !v1.isMatrix_ && !v2.isMatrix_) {
        t = ::atan2( v1.value_, v2.value_);
      }
      else {
        t = atan2( v1.matrix_, v2.matrix_);
      }

      math_range_check(
        if ( v1.isMatrix_ != v2.isMatrix_) {
          throw_message( invalid_argument, "atan2: can't mix value with matrix arguments.");
        }
      );

      return t;
    }

    const MathMLData& atan2d( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "atan2: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());
      if ( !v1.isMatrix_ && !v2.isMatrix_) {
        t = ::atan2( v1.value_, v2.value_) * dstomath::d180_pi;
      }
      else {
        t = atan2( v1.matrix_, v2.matrix_) * dstomath::d180_pi;
      }

      math_range_check(
        if ( v1.isMatrix_ != v2.isMatrix_) {
          throw_message( invalid_argument, "atan2: can't mix value with matrix arguments.");
        }
      );

      return t;
    }

    const MathMLData& exp( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "exp: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::exp( v1.value_);
      }
      else {
        t = exp( v1.matrix_);
      }
      return t;
    }

    const MathMLData& ln( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "ln: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      if ( !v1.isMatrix_) {
        t = ::log( v1.value_);
      }
      else {
        t = log( v1.matrix_);
      }
      return t;
    }

    const MathMLData& log( const MathMLData& t)
    {
      if ( t.mathChildren_.size() == 1) {
        const MathMLData& v1 = solve( t.mathChildren_.front());
        if ( !v1.isMatrix_) {
          t = ::log10( v1.value_);
        }
        else {
          t = log10( v1.matrix_);
        }
        return t;
      }

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( v1.isMatrix_) {
          throw_message( invalid_argument, "log: logbase value can't be a matrix.");
        }
      );

      if ( !v2.isMatrix_) {
        t = ::log( v2.value_) / ::log( v1.value_);
      }
      else {
        t = log( v2.matrix_) / ::log( v1.value_);
      }

      return t;
    }

    const MathMLData& logbase( const MathMLData& t)
    {
      t = solve( t.mathChildren_.front());

      math_range_check(
        if ( t.mathChildren_.front().isMatrix_) {
          throw_message( invalid_argument, "logbase: value can't be a matrix.");
        }
      );

      return t;
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
    const MathMLData& exponentiale( const MathMLData& t)
    {
      t = dstomathml::EXPONENTIALE;
      return t;
    }

    const MathMLData& notanumber( const MathMLData& t)
    {
      t = dstomath::nan();
      return t;
    }

    const MathMLData& pi( const MathMLData& t)
    {
      t = dstomath::pi;
      return t;
    }

    const MathMLData& eulergamma( const MathMLData& t)
    {
      t = dstomathml::EULERGAMMA;
      return t;
    }

    const MathMLData& infinity( const MathMLData& t)
    {
      t = dstomath::infinity();
      return t;
    }

    const MathMLData& noop( const MathMLData& t)
    {
      t = 0.0;
      return t;
    }

    /*
     * Linear Algebra
     *
     * determinant
     * transpose
     * inverse
     * element selector
     * row selector
     * column selector
     * diagonal selector
     * matrix slice selector
     * vectorproduct
     * scalarproduct
     * outerproduct
     * unitmatrix
     * eulertransform
     * cross
     */

    const MathMLData& determinant( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "determinant: incorrect number of associated elements.");
        }
      );

      t = solve( t.mathChildren_.front()).matrix_.gDeterminant();

      math_range_check(
        if ( !t.mathChildren_.front().isMatrix_) {
          throw_message( invalid_argument, "determinant: argument not a matrix.");
        }
      );

      return t;
    }

    const MathMLData& transpose( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "transpose: incorrect number of associated elements.");
        }
      );

      t = ~solve( t.mathChildren_.front()).matrix_;

      math_range_check(
        if ( !t.mathChildren_.front().isMatrix_) {
          throw_message( invalid_argument, "transpose: argument not a matrix.");
        }
      );

      return t;
    }

    const MathMLData& inverse( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "transpose: incorrect number of associated elements.");
        }
      );

      t = !solve( t.mathChildren_.front()).matrix_;

      math_range_check(
        if ( !t.mathChildren_.front().isMatrix_) {
          throw_message( invalid_argument, "transpose: argument not a matrix.");
        }
      );

      return t;
    }

    const MathMLData& selector_element( const MathMLData& t)
    {
      const DMatrix* m = 0;
      dstoute::aList<size_t> idx;
      for ( size_t i = 0; i < t.mathChildren_.size(); ++i) {
        const MathMLData& v = solve( t.mathChildren_[ i]);
        if ( i == 0) {
          m = &v.matrix_;
        }
        else {
          idx << size_t( v.value_) - 1;
        }

        math_range_check(
          if ( i == 0) {
            if ( !t.mathChildren_[ i].isMatrix_) {
              throw_message( invalid_argument, "selector_element: first argument not a matrix.");
            }
          }
          else if ( t.mathChildren_[ i].isMatrix_) {
            throw_message( invalid_argument, "selector_element: argument after matrix is not a value.");
          }
        );

      }

      switch ( t.mathChildren_.size()) {
      case 3: // M(i,j)
        t = (*m)( idx[ 0], idx[ 1]);
        break;

      case 2: // V(i)
        math_range_check(
          // Must have one row or column.
          if (( m->rows() != 1) == ( m->cols() != 1)) {
            throw_message( invalid_argument, "selector_element: expecting a row or column vector.");
          }
        );
        t = m->matrixData()[ idx[ 0]];
        break;

      case 1:
        throw_message( invalid_argument, "selector_element: no element index arguments.");
        break;

      case 0:
        throw_message( invalid_argument, "selector_element: no arguments.");
        break;

      default:
        throw_message( invalid_argument, "selector_element: too many arguments after matrix.");
        break;
      }

      return t;
    }

    const MathMLData& selector_row( const MathMLData& t)
    {
      size_t nRows = t.mathChildren_.size() / 2;
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      for ( size_t i = 0; i < nRows; ++i) {
        const MathMLData& t1 = solve( *child); ++child;
        const MathMLData& t2 = solve( *child); ++child;

        math_range_check(
          if ( !t1.isMatrix_) {
            throw_message( invalid_argument, "selector_row: First argument of argument pair must be a matrix.");
          }
          if ( t2.isMatrix_) {
            throw_message( invalid_argument, "selector_row: Second argument of argument pair must be a value.");
          }
        );

        if ( i == 0) {
          t.matrix_.resize( nRows, t1.matrix_.cols());
          t.isMatrix_ = true;
        }
        t.matrix_[ i] = t1.matrix_[ size_t( t2.value_) - 1];  // Range check done by aMatrix for this line.
      }

      return t;
    }

    const MathMLData& selector_column( const MathMLData& t)
    {
      size_t nCols = t.mathChildren_.size() / 2;
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      for ( size_t i = 0; i < nCols; ++i) {
        const MathMLData& t1 = solve( *child); ++child;
        const MathMLData& t2 = solve( *child); ++child;

        math_range_check(
          if ( !t1.isMatrix_) {
            throw_message( invalid_argument, "selector_column: First argument of argument pair must be a matrix.");
          }
          if ( t2.isMatrix_) {
            throw_message( invalid_argument, "selector_column: Second argument of argument pair must be a value.");
          }
        );

        if ( i == 0) {
          t.matrix_.resize( t1.matrix_.rows(), nCols);
          t.isMatrix_ = true;
        }
        t.matrix_( i) = t1.matrix_( size_t( t2.value_) - 1);  // Range check done by aMatrix for this line.
      }

      return t;
    }

    const MathMLData& selector_diag( const MathMLData& t)
    {
      size_t nCols = t.mathChildren_.size() / 3;
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      for ( size_t i = 0; i < nCols; ++i) {
        const MathMLData& t1 = solve( *child); ++child;
        const MathMLData& t2 = solve( *child); ++child;
        const MathMLData& t3 = solve( *child); ++child;

        math_range_check(
          if ( !t1.isMatrix_) {
            throw_message( invalid_argument, "selector_diag: First argument of argument pair must be a matrix.");
          }
          if ( t2.isMatrix_) {
            throw_message( invalid_argument, "selector_diag: Second argument of argument pair must be a value.");
          }
          if ( t3.isMatrix_) {
            throw_message( invalid_argument, "selector_diag: Third argument of argument pair must be a value.");
          }
        );

        DVector diag( t1.matrix_.diag( static_cast<int>( t2.value_) - 1, static_cast<int>( t3.value_) - 1));
        if ( i == 0) {
          t.matrix_.resize( diag.size(), nCols);
          t.isMatrix_ = true;
        }
        t.matrix_( i) = diag; // Range check done by aMatrix for this line.
      }

      return t;
    }

    const MathMLData& selector_mslice( const MathMLData& t)
    {
      MathMLDataVector::const_iterator child = t.mathChildren_.begin();
      const MathMLData& t1 = solve( *child); ++child;
      const MathMLData& t2 = solve( *child); ++child;
      const MathMLData& t3 = solve( *child); ++child;
      const MathMLData& t4 = solve( *child); ++child;
      const MathMLData& t5 = solve( *child);

      math_range_check(
        if ( !t1.isMatrix_) {
          throw_message( invalid_argument, "selector_mslice: First argument must be a matrix.");
        }
        if ( t2.isMatrix_) {
          throw_message( invalid_argument, "selector_mslice: Second argument must be a value.");
        }
        if ( t3.isMatrix_) {
          throw_message( invalid_argument, "selector_mslice: Third argument must be a value.");
        }
        if ( t4.isMatrix_) {
          throw_message( invalid_argument, "selector_mslice: Forth argument must be a value.");
        }
        if ( t5.isMatrix_) {
          throw_message( invalid_argument, "selector_mslice: Fifth argument be a value.");
        }
      );

      t = t1.matrix_[ dstomath::mslice( size_t( t2.value_) - 1, size_t( t3.value_) - 1,
                                        size_t( t4.value_), size_t( t5.value_))]; // Range check done by aMatrix for this line.

      return t;
    }

    const MathMLData& vectorproduct( const MathMLData& t)
    {
      // Cross Product: uXv
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "vectorproduct: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( !v1.isMatrix_ || !v2.isMatrix_) {
          throw_message( invalid_argument, "vectorproduct: arguments not a matrix.");
        }
        if (( v1.matrix_.rows() != v2.matrix_.rows()) &&
            ( v1.matrix_.cols() != 1)) {
          throw_message( invalid_argument, "vectorproduct: element dimensions not compatible.");
        }
      );

      t = DMatrix( v1.matrix_.rows(), 1, cross( v1.matrix_.matrixData(), v2.matrix_.matrixData()));
      return t;
    }

    const MathMLData& scalarproduct( const MathMLData& t)
    {
      // Dot product: u.v
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "scalarproduct: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( !v1.isMatrix_ || !v2.isMatrix_) {
          throw_message( invalid_argument, "scalarproduct: arguments not a matrix.");
        }
        if (( v1.matrix_.rows() != v2.matrix_.rows()) &&
            ( v1.matrix_.cols() != 1)) {
          throw_message( invalid_argument, "scalarproduct: element dimensions not compatible.");
        }
      );

      t = dot( v1.matrix_.matrixData(), v2.matrix_.matrixData());
      return t;
    }

    const MathMLData& outerproduct( const MathMLData& t)
    {
      // uvT
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "outerproduct: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( !v1.isMatrix_ || !v2.isMatrix_) {
          throw_message( invalid_argument, "outerproduct: arguments not a matrix.");
        }
        if (( v1.matrix_.cols() != v2.matrix_.cols()) &&
            ( v1.matrix_.cols() != 1)) {
          throw_message( invalid_argument, "outerproduct: element dimensions not compatible.");
        }
      );

      t = v1.matrix_ * ~v2.matrix_;
      return t;
    }

    const MathMLData& unitmatrix( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "unitmatrix: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());

      math_range_check(
        if ( v1.isMatrix_) {
          throw_message( invalid_argument, "unitmatrix: argument must be a scalar.");
        }
      );

      t = DMatrix( size_t( v1.value_), size_t( v1.value_));
      t.matrix_.unit();
      return t;
    }

    const MathMLData& eulertransform( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 3) {
          throw_message( invalid_argument, "eulertransform: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_[ 0]);
      const MathMLData& v2 = solve( t.mathChildren_[ 1]);
      const MathMLData& v3 = solve( t.mathChildren_[ 2]);

      math_range_check(
        if ( v1.isMatrix_ || v2.isMatrix_ || v3.isMatrix_) {
          throw_message( invalid_argument, "eulertransform: arguments must be a scalar.");
        }
      );

      t = DMatrix( 3, 3);
      t.matrix_.eulerTransformMatrix( v1.value_, v2.value_, v3.value_);
      return t;
    }

    const MathMLData& eulertransformd( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 3) {
          throw_message( invalid_argument, "eulertransformd: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_[ 0]);
      const MathMLData& v2 = solve( t.mathChildren_[ 1]);
      const MathMLData& v3 = solve( t.mathChildren_[ 2]);

      math_range_check(
        if ( v1.isMatrix_ || v2.isMatrix_ || v3.isMatrix_) {
          throw_message( invalid_argument, "eulertransformd: arguments must be a scalar.");
        }
      );

      t = DMatrix( 3, 3);
      t.matrix_.eulerTransformMatrix( v1.value_ * dstomath::pi_180,
                                      v2.value_ * dstomath::pi_180,
                                      v3.value_ * dstomath::pi_180);
      return t;
    }

    const MathMLData& cross( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 1) {
          throw_message( invalid_argument, "cross: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_[ 0]);

      math_range_check(
        if ( !v1.isMatrix_ || v1.matrix_.size() != 3) {
          throw_message( invalid_argument, "cross: arguments must be a vector of size 3.");
        }
      );

      t = DMatrix( 3, 3);
      t.matrix_.cross( v1.matrix_.matrixData());
      return t;
    }

    const MathMLData& mask_plus( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "mask_plus: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( !v1.isMatrix_ || !v2.isMatrix_) {
          throw_message( invalid_argument, "mask_plus: arguments are not matrices.");
        }
        if (( v1.matrix_.rows() != v2.matrix_.rows()) &&
            ( v1.matrix_.cols() != v2.matrix_.cols())) {
          throw_message( invalid_argument, "mask_plus: element dimensions not compatible.");
        }
      );

      t = DMatrix( v1.matrix_.rows(), v1.matrix_.cols());
      t.matrix_.matrixData() = v1.matrix_.matrixData() + v2.matrix_.matrixData();
      return t;
    }

    const MathMLData& mask_minus( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "mask_minus: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( !v1.isMatrix_ || !v2.isMatrix_) {
          throw_message( invalid_argument, "mask_minus: arguments are not matrices.");
        }
        if (( v1.matrix_.rows() != v2.matrix_.rows()) &&
            ( v1.matrix_.cols() != v2.matrix_.cols())) {
          throw_message( invalid_argument, "mask_minus: element dimensions not compatible.");
        }
      );

      t = DMatrix( v1.matrix_.rows(), v1.matrix_.cols());
      t.matrix_.matrixData() = v1.matrix_.matrixData() - v2.matrix_.matrixData();
      return t;
    }

    const MathMLData& mask_times( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "mask_times: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( !v1.isMatrix_ || !v2.isMatrix_) {
          throw_message( invalid_argument, "mask_times: arguments are not matrices.");
        }
        if (( v1.matrix_.rows() != v2.matrix_.rows()) &&
            ( v1.matrix_.cols() != v2.matrix_.cols())) {
          throw_message( invalid_argument, "mask_times: element dimensions not compatible.");
        }
      );

      t = DMatrix( v1.matrix_.rows(), v1.matrix_.cols());
      t.matrix_.matrixData() = v1.matrix_.matrixData() * v2.matrix_.matrixData();
      return t;
    }

    const MathMLData& mask_divide( const MathMLData& t)
    {
      math_range_check(
        if ( t.mathChildren_.size() != 2) {
          throw_message( invalid_argument, "mask_divide: incorrect number of associated elements.");
        }
      );

      const MathMLData& v1 = solve( t.mathChildren_.front());
      const MathMLData& v2 = solve( t.mathChildren_.back());

      math_range_check(
        if ( !v1.isMatrix_ || !v2.isMatrix_) {
          throw_message( invalid_argument, "mask_divide: arguments are not matrices.");
        }
        if (( v1.matrix_.rows() != v2.matrix_.rows()) &&
            ( v1.matrix_.cols() != v2.matrix_.cols())) {
          throw_message( invalid_argument, "mask_divide: element dimensions not compatible.");
        }
      );

      t = DMatrix( v1.matrix_.rows(), v1.matrix_.cols());
      t.matrix_.matrixData() = v1.matrix_.matrixData() / v2.matrix_.matrixData();
      return t;
    }

    SolveMathMLMap solveMathMLMap = ( SolveMathMLMap()
        << SolveMathMLMap::pair( "cn",               cn)
        << SolveMathMLMap::pair( "ci",               ci)
        << SolveMathMLMap::pair( "apply",            apply)
        << SolveMathMLMap::pair( "csymbol",          csymbol)
        << SolveMathMLMap::pair( "eq",               eq)
        << SolveMathMLMap::pair( "neq",              neq)
        << SolveMathMLMap::pair( "gt",               gt)
        << SolveMathMLMap::pair( "geq",              geq)
        << SolveMathMLMap::pair( "lt",               lt)
        << SolveMathMLMap::pair( "leq",              leq)
        << SolveMathMLMap::pair( "quotient",         quotient)
        << SolveMathMLMap::pair( "factorial",        factorial)
        << SolveMathMLMap::pair( "divide",           divide)
        << SolveMathMLMap::pair( "max",              max)
        << SolveMathMLMap::pair( "min",              min)
        << SolveMathMLMap::pair( "minus",            minus)
        << SolveMathMLMap::pair( "plus",             plus)
        << SolveMathMLMap::pair( "power",            power)
        << SolveMathMLMap::pair( "rem",              rem)
        << SolveMathMLMap::pair( "times",            times)
        << SolveMathMLMap::pair( "root",             root)
        << SolveMathMLMap::pair( "degree",           degree)
        << SolveMathMLMap::pair( "and",              and_)
        << SolveMathMLMap::pair( "or",               or_)
        << SolveMathMLMap::pair( "xor",              xor_)
        << SolveMathMLMap::pair( "not",              not_)
        << SolveMathMLMap::pair( "abs",              abs)
        << SolveMathMLMap::pair( "floor",            floor)
        << SolveMathMLMap::pair( "ceiling",          ceiling)
        << SolveMathMLMap::pair( "piecewise",        piecewise)
        << SolveMathMLMap::pair( "piece",            piece)
        << SolveMathMLMap::pair( "otherwise",        otherwise)
        << SolveMathMLMap::pair( "sin",              sin)
        << SolveMathMLMap::pair( "cos",              cos)
        << SolveMathMLMap::pair( "tan",              tan)
        << SolveMathMLMap::pair( "sec",              sec)
        << SolveMathMLMap::pair( "csc",              csc)
        << SolveMathMLMap::pair( "cot",              cot)
        << SolveMathMLMap::pair( "arcsin",           arcsin)
        << SolveMathMLMap::pair( "arccos",           arccos)
        << SolveMathMLMap::pair( "arctan",           arctan)
        << SolveMathMLMap::pair( "arcsec",           arcsec)
        << SolveMathMLMap::pair( "arccsc",           arccsc)
        << SolveMathMLMap::pair( "arccot",           arccot)
        << SolveMathMLMap::pair( "exp",              exp)
        << SolveMathMLMap::pair( "ln",               ln)
        << SolveMathMLMap::pair( "log",              log)
        << SolveMathMLMap::pair( "logbase",          logbase)
        << SolveMathMLMap::pair( "exponentiale",     exponentiale)
        << SolveMathMLMap::pair( "notanumber",       notanumber)
        << SolveMathMLMap::pair( "pi",               pi)
        << SolveMathMLMap::pair( "eulergamma",       eulergamma)
        << SolveMathMLMap::pair( "infinity",         infinity)
        << SolveMathMLMap::pair( "noop",             noop)

        // Matrix specific ops.
        << SolveMathMLMap::pair( "determinant",      determinant)
        << SolveMathMLMap::pair( "transpose",        transpose)
        << SolveMathMLMap::pair( "inverse",          inverse)
        << SolveMathMLMap::pair( "selector_element", selector_element) // Selector operation
        << SolveMathMLMap::pair( "selector_row",     selector_row)     // Selector operation
        << SolveMathMLMap::pair( "selector_column",  selector_column)  // Selector operation
        << SolveMathMLMap::pair( "selector_diag",    selector_diag)    // Selector operation
        << SolveMathMLMap::pair( "selector_mslice",  selector_mslice)  // Selector operation
        << SolveMathMLMap::pair( "vectorproduct",    vectorproduct)
        << SolveMathMLMap::pair( "scalarproduct",    scalarproduct)
        << SolveMathMLMap::pair( "outerproduct",     outerproduct)

        // csymbol definitions (technically these are not defined in MathML)
        << SolveMathMLMap::pair( "fmod",             fmod)
        << SolveMathMLMap::pair( "sign",             sign)
        << SolveMathMLMap::pair( "bound",            bound)
        << SolveMathMLMap::pair( "nearbyint",        nearbyint)
        << SolveMathMLMap::pair( "sind",             sind)
        << SolveMathMLMap::pair( "cosd",             cosd)
        << SolveMathMLMap::pair( "tand",             tand)
        << SolveMathMLMap::pair( "secd",             secd)
        << SolveMathMLMap::pair( "cscd",             cscd)
        << SolveMathMLMap::pair( "cotd",             cotd)
        << SolveMathMLMap::pair( "arcsind",          arcsind)
        << SolveMathMLMap::pair( "arccosd",          arccosd)
        << SolveMathMLMap::pair( "arctand",          arctand)
        << SolveMathMLMap::pair( "arcsecd",          arcsecd)
        << SolveMathMLMap::pair( "arccscd",          arccscd)
        << SolveMathMLMap::pair( "arccotd",          arccotd)
        << SolveMathMLMap::pair( "atan2",            atan2)
        << SolveMathMLMap::pair( "atan2d",           atan2d)

        << SolveMathMLMap::pair( "unitmatrix",       unitmatrix)
        << SolveMathMLMap::pair( "eulertransform",   eulertransform)
        << SolveMathMLMap::pair( "eulertransformd",  eulertransformd)
        << SolveMathMLMap::pair( "cross",            cross)
        << SolveMathMLMap::pair( "mask_plus",        mask_plus)
        << SolveMathMLMap::pair( "mask_minus",       mask_minus)
        << SolveMathMLMap::pair( "mask_times",       mask_times)
        << SolveMathMLMap::pair( "mask_divide",      mask_divide)

        // Possible csymbol matrix ops.
//        << SolveMathMLMap::pair( "eulerTransformMatrixPsi",  eulerTransformMatrixPsi)
//        << SolveMathMLMap::pair( "eulerTransformMatrixTheta",  eulerTransformMatrixTheta)
//        << SolveMathMLMap::pair( "eulerTransformMatrixPhi",  eulerTransformMatrixPhi)
//        << SolveMathMLMap::pair( "eulerTransformMatrixPsiTheta",  eulerTransformMatrixPsiTheta)
//        << SolveMathMLMap::pair( "eulerTransformMatrixPsiPhi",  eulerTransformMatrixPsiPhi)
//        << SolveMathMLMap::pair( "eulerTransformMatrixThetaPhi",  eulerTransformMatrixThetaPhi)
    );

  } // end namespace solvemathml
} // end namespace dstomathml
