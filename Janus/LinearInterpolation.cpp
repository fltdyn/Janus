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
// Title:      Janus/LinearInterpolation
// Class:      LinearInterpolation
// Module:     LinearInterpolation.cpp
// First Date: 2008-07-17
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file LinearInterpolation.cpp
 *
 * This private function performs interpolations when all the degrees of
 * freedom for a function are specified as linear or first order polynomial, 
 * or for the default condition when interpolationType is not specified.
 *
 * Given \a 2^n  uniformly gridded  values of a function of  \a n  variables, 
 * provided to the instance of the class by either \p setVariableByIndex or
 * \p setVariableByID, this private function is called by 
 * \p getOutputVariable to perform a multi-linear interpolation between 
 * the values and returns the result.  It maintains continuity of function 
 * across the grid, but not of derivatives of the function.  NB if the 
 * fractions based on the grid direction variables are outside the range 
 * 0.0 -> 1.0 this function can perform an extrapolation, controlled by
 * the `extrapolate' attribute, with possibly dubious results depending on 
 * the shape of the represented function.
 */

/*
 * Author: D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

#include <Ute/aMath.h>

#include "InDependentVarDef.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace janus {

template <typename _ForwardIterator, typename _Tp> inline size_t lower_bound_index(
  _ForwardIterator __first, _ForwardIterator __last, const _Tp& __val)
{
  size_t __maxIndex = __last - __first - 2; // This can wrap. All okay.
  size_t __index = std::lower_bound( __first, __last, __val) - __first;
  if ( __index > 0) --__index;
  if ( __index > __maxIndex) --__index;
  return __index;
}

double Janus::getLinearInterpolation(
  Function& function,
  const vector<double>& dataTable )
{
  const vector<size_t>& breakpointRefs = griddedTableDef_[ function.getTableRef()].getBreakpointRef();
  const vector<InDependentVarDef>& inDependentVarDefs = function.getInDependentVarDef();

  /*
   * The function iterates over each input degree of freedom in turn to find 
   * the breakpoints which straddle the input state, and the fraction in
   * each direction which the input value represents.
   */
  const size_t n = inDependentVarDefs.size();
  for ( size_t i = 0 ; i < n; ++i) {
    const vector<double>& bpVals = breakpointDef_[ breakpointRefs[i]].getBpVals();
    nbp_[ i ] = static_cast<int>( bpVals.size());

    const InDependentVarDef &inDependentVarDef = inDependentVarDefs[ i];
    double x = variableDef_[ inDependentVarDef.getVariableReference()].getValue();

    /*
     * An input variable is always constrained to its minimum -- maximum 
     * range, if these have been set 
     */
    x = dstomath::bound( x, inDependentVarDef.getMin(), inDependentVarDef.getMax());

    /*
     * Each variable is also checked against its breakpoint range, since
     * breakpoint ends do not necessarily match the minimum and maximum
     * attributes.
     */
    ExtrapolateMethod extrapolateMethod = inDependentVarDef.getExtrapolationMethod();
    if ( x < bpVals.front() ) {
      /*
       * less than minimum bp -> neg extrapolation may be required
       */
      bpa_[ i ] = 0;
      if ( EXTRAPOLATE_BOTH  != extrapolateMethod &&
           EXTRAPOLATE_MINEX != extrapolateMethod) {
        x = bpVals.front();
      }
    } 
    else if ( x > bpVals.back() ) {
      /*
       * greater than maximum bp -> pos extrapolation may be required
       */
      bpa_[ i ] = static_cast<int>( bpVals.size()) - 2;
      if ( EXTRAPOLATE_BOTH  != extrapolateMethod &&
           EXTRAPOLATE_MAXEX != extrapolateMethod) {
        x = bpVals.back();
      }
    } 
    else {
      /*
       * in range -> interpolation by bisection.
       */
      bpa_[ i ] = static_cast<int>( lower_bound_index( bpVals.begin(), bpVals.end(), x));
    }

    frac_[ i ] = ( x - bpVals[ bpa_[ i ] ] ) / ( bpVals[ bpa_[ i ] + 1 ] - bpVals[ bpa_[ i ] ] );

    /*
     * allow for order 0 linear interpolation, i.e. take nearest discrete
     * value on the grid.
     */
    switch ( inDependentVarDef.getInterpolationMethod()) {
      case INTERPOLATE_DISCRETE:
        if ( 0.5 >= frac_[ i ] ) {
          frac_[ i ] = 0.0;
        }
        else {
          frac_[ i ] = 1.0;
        }
        break;

      case INTERPOLATE_FLOOR:
        frac_[ i ] = 0.0;
        break;

      case INTERPOLATE_CEILING:
        frac_[ i ] = 1.0;
        break;

      default:
        break;
    }
  }
  
  /*
   * The multi-dimensional linear interpolation is performed using a
   * weighted sum of 2^n function values.  The offset into the
   * function data table, which was extracted from the DOM, is computed with 
   * the last breakpoint in the function definition's list
   * changing most rapidly.
   */
  double result = 0.0;
  size_t nEvals = 2 << ( n - 1 );

  for ( size_t i = 0 ; i < nEvals; ++i ) {
    /*
     * get pointers to the breakpoints - these 0 or 1 for the two ends of
     * each leg of the lattice, and are equivalent to an array of the
     * bits making up the binary representation of the function value index.
     */

    int ibits = static_cast<int>( i);
    for ( int j = static_cast<int>( n) - 1 ; j >= 0 ; --j ) {
      bpi_[ j ] = ( ibits & 1 );
      ibits >>= 1;
    }

    /*
     * get the function value at this corner of the lattice
     */
    size_t offset = 0;
    {
      for ( size_t j = 0 ; j < n ; ++j ) {
        offset *= nbp_[ j ];
        offset += bpa_[ j ] + bpi_[ j ];
      }
    }

    /*
     * compute the weighting factor, iterating over each degree of freedom
     */
    double weight = 1.0;
    {
      for ( size_t j = 0 ; j < n ; ++j ) {
        weight *= ( ( bpi_[ j ] * frac_[ j ] ) + ( 1 - bpi_[ j ] ) * ( 1.0 - frac_[ j ] ) );
      }
    }

    /*
     * add the function value multiplied by its weighting to the result
     */
    result += dataTable[ offset ] * weight;
  }

  return result;
}
} // End namespace janus.


