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
// Title:      Janus/PolyInterpolation
// Class:      PolyInterpolation
// Module:     PolyInterpolation.cpp
// First Date: 2008-07-17
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file PolyInterpolation.cpp
 *
 * This private function performs interpolations when \a not all the degrees 
 * of freedom for a function are specified as linear or first order 
 * polynomial.
 *
 * If the interpolation order in the \latexonly$i$th\endlatexonly
 * \htmlonly<var>i</var>th\endhtmlonly degree of freedom is 
 * \latexonly$k_i$,\endlatexonly\htmlonly<var>k</var><sub>i</sub>,\endhtmlonly 
 * then given \latexonly$\Pi_1^n (k_i + 1)$\endlatexonly 
 * \htmlonly<var>&Pi;</var><sub>1</sub><sup>n</sup>(<var>k</var><sub>i</sub> 
 * + 1)\endhtmlonly uniformly gridded values 
 * of a function of \latexonly$n$\endlatexonly
 * \htmlonly<var>n</var>\endhtmlonly variables, 
 * provided to the instance of the class by either setVariableByIndex() or
 * setVariableByID(), this private function is called by 
 * getOutputVariable() to perform a multi-dimensional polynomial 
 * interpolation between the values and returns the result.  At present the
 * maximum polynomial order is limited to 3.  The interpolation maintains 
 * continuity of function across the grid, but not of derivatives of the function.
 *
 * {\b Note}: this function can perform an extrapolation, which is controlled by the
 * \em extrapolate attribute, but polynomial extrapolation is notoriously
 * inaccurate and unstable and should not be relied on by users wanting to
 * maintain modelling fidelity.
 */

/*
 * Author: D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes 
#include <stdexcept>
#include <sstream>

// Ute Includes
#include <Ute/aMessageStream.h>

// Local Includes
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

double Janus::getPolyInterpolation(
  Function& function,
  const vector<double>& dataTable )
{
  const vector<InDependentVarDef>& inDependentVarDefs = function.getInDependentVarDef();
  const size_t n = inDependentVarDefs.size();

  /*
   * Because the newer DAVE-ML versions don't use interpolationOrder, we
   * construct a fictitious order in each degree of freedom based on the
   * newer interpolate attribute
   */
  for ( size_t i = 0 ; i < n ; ++i ) {
    switch ( inDependentVarDefs[i].getInterpolationMethod()) {
      case INTERPOLATE_LINEAR:
        nOrd_[ i ] = 1;
        break;

      case INTERPOLATE_DISCRETE:
        nOrd_[ i ] = 0;
        break;

      case INTERPOLATE_CEILING:
        nOrd_[ i ] = -2;
        break;

      case INTERPOLATE_FLOOR:
        nOrd_[ i ] = -1;
        break;

      case INTERPOLATE_QSPLINE: // Temporary association until the spline interpolation is supported
      case INTERPOLATE_POLY2:
        nOrd_[ i ] = 2;
        break;

      case INTERPOLATE_CSPLINE: // Temporary association until the spline interpolation is supported
      case INTERPOLATE_POLY3:
        nOrd_[ i ] = 3;
        break;

      default:
        nOrd_[ i ] = 3; // As written by Dan, default is 3.
        break;
    }
  }

  /*
   * The function iterates over each input degree of freedom in turn to find 
   * the breakpoints which straddle the input state, and the fraction in
   * each direction which the input value represents.
   */
  GriddedTableDef& griddedTableDef = griddedTableDef_[ function.getTableRef()];
  const vector<size_t>& breakpointRefs = griddedTableDef.getBreakpointRef();

  size_t nEvals = 1;
  for ( size_t i = 0 ; i < n ; ++i ) {
    const vector<double>& bpVals = breakpointDef_[ breakpointRefs[i]].getBpVals();
    nbp_[ i ] = static_cast<int>( bpVals.size());

    double x = variableDef_[ inDependentVarDefs[i].getVariableReference()].getValue();

    /*
     * An input variable is always constrained to its minimum -- maximum 
     * range, if these have been set 
     */
    x = dstomath::bound( x, inDependentVarDefs[ i].getMin(), inDependentVarDefs[ i].getMax());

    /*
     * Each variable is also checked against its breakpoint range, since
     * breakpoint ends do not necessarily match the minimum and maximum
     * attributes.
     */
    ExtrapolateMethod extrapolate = inDependentVarDefs[i].getExtrapolationMethod();
    if ( x < bpVals.front() ) {
      /*
       * less than minimum bp -> neg extrapolation may be required
       */
      bpa_[ i ] = 0;
      if ( EXTRAPOLATE_BOTH  != extrapolate &&
           EXTRAPOLATE_MINEX != extrapolate ) {
        x = bpVals.front();
      }
    } 
    else if ( x > bpVals.back() ) {
      /*
       * greater than maximum bp -> pos extrapolation may be required
       */
      bpa_[ i ] = static_cast<int>( bpVals.size()) - 2;
      if ( EXTRAPOLATE_BOTH  != extrapolate &&
           EXTRAPOLATE_MAXEX != extrapolate ) {
        x = bpVals[ nbp_[ i ] - 1 ];
      }
    } 
    else {
      /*
       * in range -> interpolation by bisection.  NB uses ge to ensure 
       * capture of both extreme end points 
       */
      bpa_[ i ] = static_cast<int>( lower_bound_index( bpVals.begin(), bpVals.end(), x));
    }

    /*
     * if polynomial order is greater than 1, offset first breakpoint in 
     * series towards min by half the number of extra points required.
     * For odd order, this keeps desired interval in centre of points
     * chosen.  For even order, desired interval is lower side of centre
     * point.  Where first or last breakpoint falls inside nominal list
     * of points, adjust list to begin or end at extreme breakpoint. 
     * For arrays with less breakpoints in any DoF than nOrd[i]+1, need to 
     * reduce the order. Should we tell the user ????
     */
    if ( nOrd_[ i ] + 1 > nbp_[ i ] ) {
      nOrd_[ i ] = nbp_[ i ] - 1;
    }
    nEvals = nEvals * ( dstomath::max( nOrd_[ i ], 0 ) + 1 );
    int itest = ( dstomath::max( nOrd_[ i ], 0 ) - 1 ) / 2;
    if ( bpa_[ i ] < itest ) {
      bpa_[ i ] = 0;
    } else {
      bpa_[ i ] -= itest;
    }
    itest = nbp_[ i ] - dstomath::max( nOrd_[ i ], 0 ) - 1;
    if ( bpa_[ i ] > itest ) {
      bpa_[ i ] = itest;
    }

    /*
     * set up fractions for the input value and all breakpoint locations
     * for this degree of freedom, allowing for irregularly spaced
     * breakpoints.  The 0 -> 1 range goes from first chosen point to last
     * chosen point.  
     */
    double bpRange;
    if ( nOrd_[ i ] > 0 ) {                     // non-discrete
      bpRange = bpVals[ bpa_[ i ] + nOrd_[ i ] ] - bpVals[ bpa_[ i ] ];
    }
    else if ( bpa_[ i ] + 1 < nbp_[ i ] ) {  // discrete, >= 1 bp above
      bpRange = bpVals[ bpa_[ i ] + 1 ] - bpVals[ bpa_[ i ] ];
    }
    else if ( bpa_[ i ] > 0 ) {             // discrete, 0 bp above
      bpRange = bpVals[ bpa_[ i ] - 1 ] - bpVals[ bpa_[ i ] ];
    }
    else {                                 //
      bpRange = 1.0;
    }
    frac_[ i ] = ( x - bpVals[ bpa_[ i ] ] ) / bpRange;
    for ( int j = 0 ; j < nOrd_[ i ] + 1 ; j++ ) {
      fracBp_( i, j ) = ( bpVals[ bpa_[ i ] + j ] - bpVals[ bpa_[ i ] ] ) /
          bpRange;
    }
    if ( 0 >= nOrd_[ i ] ) { // discrete,
      if ( nbp_[ i ] > 1 ) {
        if ( 0 == nOrd_[ i ] ) { // nearest neighbour
          frac_[ i ] = dstomath::nearbyint( frac_[ i ] );
          if ( 1 == frac_[ i ] ) { // don't ever want upper bound for discrete
            frac_[ i ] = 0.0;
            bpa_[ i ] += 1;
          }
        } else if ( -1 == nOrd_[ i ] ) { // floor
          frac_[ i ] = 0.0;
        } else if ( -2 == nOrd_[ i ] ) { // ceiling
          frac_[ i ] = 0.0;
          bpa_[ i ] += 1;
        }
      } else {  // single bp value in this DoF
        frac_[ i ] = 0.0;
        fracBp_( i, 0 ) = 0.0;
      }
    }
  }

  /*
   * The multi-dimensional polynomial interpolation is performed using a
   * weighted sum of pi(order+1) function values.  The offset into the
   * function data table, which was extracted from the DOM, is computed with 
   * the last breakpoint in the function definition's list
   * changing most rapidly.
   */
  double result = 0.0;
  for ( size_t k = 0 ; k < nEvals; ++k ) {
    /*
     * get pointers to the breakpoints - these 0, 1, ... (nOrd-1) for 
     * each leg of the lattice, and are equivalent to an array of the
     * bits making up the 'nOrd+1'-ary representation of the function value 
     * index.
     */

    int ibits = static_cast<int>( k);
    for ( int j = static_cast<int>( n) - 1 ; j >= 0 ; --j ) {
      bpi_[ j ] = ( ibits % ( dstomath::max( nOrd_[ j ], 0 ) + 1 ) );
      ibits /= ( dstomath::max( nOrd_[ j ], 0 ) + 1 );
    }

    /*
     * get the function value at this corner of the lattice
     */
    size_t offset = 0;
    for ( size_t j = 0 ; j < n ; ++j ) {
      offset *= nbp_[ j ];
      offset += bpa_[ j ] + bpi_[ j ];
    }
    double y = dataTable[ offset ];

    /*
     * compute the weighting factor, iterating over each degree of freedom
     * note range is 0 -> 1 in each DoF
     */
    double weight = 1.0;
    for ( size_t j = 0 ; j < n ; ++j ) {
      if ( 1 >= nOrd_[ j ] ) {
        weight *= ( bpi_[ j ] * frac_[ j ] + ( 1 - bpi_[ j ] ) * ( 1.0 - frac_[ j ] ) );

      }
      else if ( 2 == nOrd_[ j ] ) {
        double x12 = fracBp_( j, 1 ) * fracBp_( j, 1 );
        double xx2 = frac_[ j ] * frac_[ j ];
        double numerator = 0.0;
        double denominator = fracBp_( j, 1 ) - x12 ;
        if ( 0 == bpi_[ j ] ) {
          numerator = frac_[ j ] * ( x12 - 1.0 ) + xx2 * ( 1.0 - fracBp_( j, 1 ) )
                + denominator;
        }
        else if ( 1 == bpi_[ j ] ) {
          numerator = frac_[ j ] - xx2;
        }
        else {
          numerator = xx2 * fracBp_( j, 1 ) - frac_[ j ] * x12;
        }
        weight *= numerator / denominator;

      }
      else if ( 3 == nOrd_[ j ] ) {
        double x12 = fracBp_( j, 1 ) * fracBp_( j, 1 );
        double x22 = fracBp_( j, 2 ) * fracBp_( j, 2 );
        double x13 = fracBp_( j, 1 ) * x12;
        double x23 = fracBp_( j, 2 ) * x22;
        double xx2 = frac_[ j ] * frac_[ j ];
        double xx3 = frac_[ j ] * xx2;
        double numerator = 0.0;
        double denominator = fracBp_( j, 1 ) * ( x22 - x23 )
              - x12 * ( fracBp_( j, 2 ) - x23 ) + x13 * ( fracBp_( j, 2 ) - x22 );
        if ( 0 == bpi_[ j ] ) {
          numerator =
              frac_[ j ] * ( x23 + x12 * ( 1.0 - x23 ) - x22 - x13 * ( 1.0 - x22 ))
              + xx2 * (-x23 - fracBp_( j, 1 ) * ( 1.0 - x23 ) + fracBp_( j, 2 ) + x13 * ( 1.0 - fracBp_( j, 2 ) ))
              + xx3 * ( x22 + fracBp_( j, 1 ) * ( 1.0 - x22)  - fracBp_( j, 2 ) - x12 * ( 1.0 - fracBp_( j, 2 ) ))
              + denominator;
        }
        else if ( 1 == bpi_[ j ] ) {
          numerator = xx2 * ( x23 - fracBp_( j, 2 ) ) + frac_[ j ] * ( x22 - x23 )
                + xx3 * ( fracBp_( j, 2 ) - x22 );
        }
        else if ( 2 == bpi_[ j ] ) {
          numerator = frac_[ j ] * ( x13 - x12 ) + xx2 * ( fracBp_( j, 1 ) - x13 )
                + xx3 * ( x12 - fracBp_( j, 1 ) );
        }
        else {
          numerator = frac_[ j ] * ( x12 * x23 - x13 * x22 )
                + xx2 * ( x13 * fracBp_( j, 2 ) - fracBp_( j, 1 ) * x23 )
                + xx3 * ( fracBp_( j, 1 ) * x22 - x12 * fracBp_( j, 2 ) );
        }
        weight *= numerator / denominator;

      }
      else {
        throw_message( range_error,
          setFunctionName( "Janus::getPolyInterpolation()")
          << "\n - Polynomial order too high in degree of freedom "
          << j << " for function \"" << function.getName() << "\"."
        );
      }
    }

    /*
     * add the function value multiplied by its weighting to the result
     */
    result += y * weight;
  }

  return result;
}

} // End namespace janus.


