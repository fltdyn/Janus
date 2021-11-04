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
// Title:      Janus/UngriddedInterpolation
// Class:      UngriddedInterpolation
// Module:     UngriddedInterpolation.cpp
// First Date: 2009-06-05
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file UngriddedInterpolation.cpp
 *
 * This file contains private functions to perform linear interpolations
 * on ungridded datasets.  It is called by \p getOutputVariable to perform
 * a multi-linear interpolation between the values and returns the result.
 * It maintains continuity of function across the dataset, but not of
 * derivatives of the function.
 */

/*
 * Author: D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

#include <utility>
#include <algorithm>

#include <Ute/aMath.h>
#include <Ute/aMatrix.h>

#include "InDependentVarDef.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;
using namespace dstomath;

namespace janus {

/*********************************************************************
 * Class for sorting of pairs
 */
class comPair {
 public:
  bool operator()( pair<double, size_t> x, 
                   pair<double, size_t> y) const
  {
    return ( x.first < y.first );
  }
};

//------------------------------------------------------------------------//

double Janus::getUngriddedInterpolation(
  Function& function,
  const vector<double>& dataColumn )
{
  double result = dstomath::nan();

  UngriddedTableDef& ungriddedTableDef = ungriddedTableDef_[ function.getTableRef()];
  const vector<InDependentVarDef>& inDependentVarDefs = function.getInDependentVarDef();
  size_t nCols = inDependentVarDefs.size();

  /*
   * find the simplex to be used for interpolation by computing the
   * barycentric coordinates of x with respect to each simplex in
   * turn.  The simplex with all barycentric coordinates (equivalent
   * to vertex weights) greater than zero is the one which contains
   * point x.
   */
  size_t nVer = nCols + 1;

  const dstomath::DMatrix& independentData     = ungriddedTableDef.getIndependentData();
  const vector< vector<size_t> >& thisDelaunay = ungriddedTableDef.getDelaunay();
  const vector< vector<size_t> >& connect      = ungriddedTableDef.getConnect();
  size_t nDel = thisDelaunay.size();

  dstomath::DVector& xival  = ungriddedTableDef.getXIVal();
  dstomath::DVector& weight = ungriddedTableDef.getWeight();
  dstomath::DMatrix& A      = ungriddedTableDef.getAMatrix();

  size_t ip;
  size_t nMin = nDel;

  InterpolateMethod interpolate;
  bool discreteFlag = true;
  /*
   * set up a vector of the input variable values.  An input variable is
   * always constrained to its minimum -- maximum range, if these have
   * been set
   */
  xival[ 0 ] = 1.0;
  for ( size_t i = 0 ; i < nCols ; i++ ) {
    ip = i + 1;
    xival[ ip ] =
      variableDef_[ inDependentVarDefs[i].getVariableReference()].getValue();

    /*
     * An input variable is always constrained to its minimum -- maximum
     * range, if these have been set
     */
    xival[ ip ] = dstomath::bound( xival[ ip ], inDependentVarDefs[ i].getMin(), inDependentVarDefs[ i].getMax());

    /*
     * Check for DISCRETE setting -- in all directions
     * If not set the discreteFlag to false. This flag is used to determine
     * if the weights are modified such that a discrete evaluation is applied
     * to the ungridded data table instead of linear, or higher order, interpolation.
     */
    interpolate = inDependentVarDefs[i].getInterpolationMethod();
    if ( interpolate != INTERPOLATE_DISCRETE) {
      discreteFlag = false;
    }
  }

  // Check if the last interpolation of the ungridded table used the
  // the stored simplex, and if so then reuse it.
  if ( ungriddedTableDef.lastUsedSimplex() < nDel ) { // last used simplex != number Delaunay
    /*
     * Test the last simplex which was used for interpolation of this
     * ungridded table.  It may be the same if we're working on a
     * different output column with the same inputs, or if the state
     * of the input variables is changing slowly.  If so, this will
     * give a very fast result.
     */
    A[ 0 ] = 1.0;
    vector<size_t>& delaunay =
      (vector<size_t>&)thisDelaunay[ ungriddedTableDef.lastUsedSimplex() ];

    for ( size_t j = 0 ; j < nVer ; j++ ) {
      size_t& ij = delaunay[ j ];
      for ( size_t k = 0 ; k < nCols ; k++ ) {
        A( k + 1, j ) = independentData[ ij ][ k ];
      }
    }
    weight = A.gSolve( xival );
    if ( weight.min() >= -dstomath::zero()) {
      nMin = ungriddedTableDef.lastUsedSimplex();
    }
  }

  if ( nMin == nDel) { // nMin == nDel || last used simplex == number Delaunay
    /*
     * The input point was not in the simplex used for the previous
     * interpolation, so need to go through the process of finding
     * a new simplex.
     *
     * To speed up the calculation, find closest data point to required location
     * and check all the simplices connected to that point.  If the required
     * location is inside the convex hull, it will be inside one of these
     * simplices.  If the required location is actually at a data point, just
     * return the value at that point.
     *
     * n.b. the following code is only searching through the independent variables
     *      of the data table.
     */
    size_t nRows = independentData.rows();
    size_t iMin  = 0;
    double xMin  = 1.0e208;
    double xDist, yDist;

    for ( size_t i = 0 ; i < nRows ; i++ ) {
      xDist = 0.0;
      for ( size_t j = 0 ; j < nCols ; j++ ) {
        yDist = xival[ 1 + j ] - independentData( i, j);
        xDist += (yDist * yDist);
      }
      xDist = sqrt( xDist );

      if ( dstomath::isZero( xDist)) {  // coincides with a dataPoint exactly
        return dataColumn[ i ];
      }
      else if ( xDist < xMin ) {
        xMin = xDist;
        iMin = i;
      }
    }

    /*
     * now compute barycentric coordinates (= weights) over connected
     * simplices.
     *
     * n.b. the following code is only operating on generated from the
     *      independent variables of the data table.
     */
    const vector<size_t>& iCon = connect[ iMin];

    A[ 0 ] = 1.0;  // first row is always ones to apply constraint
    for ( size_t i = 0 ; i < iCon.size() ; i++ ) {
      vector<size_t>& delaunay = (vector<size_t>&)thisDelaunay[ iCon[ i ] ];
      for ( size_t j = 0 ; j < nVer ; j++ ) {
        size_t& ij = delaunay[ j ];
        for ( size_t k = 0 ; k < nCols ; k++ ) {
          A( k + 1, j ) = independentData( ij, k);
        }
      }

      /*
       * decompose and back-substitute using ValMatrix.
       * Use L-U decomposition, which will work unless the matrix is rank
       * deficient, means the simplex is degenerate.  Should not happen,
       * since initialisation used "Qt" option.
       */
      weight = A.gSolve( xival );
      if ( nVer > A.gInfo().rank ) {
        continue;
      }

      /*
       * check for positive values of all coordinates, which is the test
       * for the point being in the simplex.
       */
      if ( weight.min() >= -dstomath::zero() ) {
        nMin = iCon[ i ];
        ungriddedTableDef.lastUsedSimplex( nMin);
        break;
      }
    }

    if ( nMin == nDel ) {
      /*
       * The simplices connected to the nearest point did not contain the
       * required location (must be very ugly geometry), so go slowly and
       * painfully through all simplices.  Also save least worst location,
       * in case we need to extrapolate.
       */
      double wTest, wMin = 1.0e208;
      size_t nMinOut = 0;
      for ( size_t i = 0 ; i < nDel ; i++ ) {
        vector<size_t>& delaunay = (vector<size_t>&)thisDelaunay[ i ];
        for ( size_t j = 0 ; j < nVer ; j++ ) {
          size_t& ij = delaunay[ j ];
          for ( size_t k = 0 ; k < nCols ; k++ ) {
            A( k + 1, j ) = independentData( ij, k);
          }
        }

        weight = A.gSolve( xival );
        if ( nVer > A.gInfo().rank ) {
          continue;
        }

        wTest = weight.min();
        if ( wTest >= -dstomath::zero() ) {
          nMin = i;
          ungriddedTableDef.lastUsedSimplex( nMin);
          break;
        }
        else {
          wTest = fabs( wTest );
          if ( wTest < wMin ) {
            wMin = wTest;
            nMinOut = i;
          }
        }
      }

      if ( nMin == nDel ) {
        /*
         * could not find a simplex enclosing the data point, so use SVD
         * to extrapolate outside the convex hull, from "nearest" simplex.
         * Can't directly relate Cartesian extrapolation flag to barycentric
         * directions, so only extrapolate if \emph ALL Cartesian directions
         * allow it.
         */
        ExtrapolateMethod extrapolate;
        for ( size_t i = 0 ; i < nCols ; i++ ) {
          extrapolate = inDependentVarDefs[i].getExtrapolationMethod();
          if (( EXTRAPOLATE_BOTH != extrapolate) && !discreteFlag) {
            return result; // = dstomath::nan()
          }
        }

        nMin = nMinOut;
        ungriddedTableDef.lastUsedSimplex( nMin);
        vector<size_t>& delaunay = (vector<size_t>&)thisDelaunay[ nMin ];
        A[ 0 ] = 1.0;
        for ( size_t j = 0 ; j < nVer ; j++ ) {
          size_t& ij = delaunay[ j ];
          for ( size_t k = 0 ; k < nCols ; k++ ) {
            A( k + 1, j ) = independentData( ij, k);
          }
        }
        weight = A.svSolve( xival );
      }
    }
  }

  /*
   * Check for DISCRETE setting -- in all directions
   * If the discrete flag has been set then he weightings will be modified such that
   * a discrete evaluation is applied to the ungridded data table instead of linear,
   * or higher order, interpolation.
   *
   * The weighting entry closest to 1 is set to 1 and all others 0. If more than one
   * entry with a value of 1 is present then the first entry is set to 1 and others
   * set to 0.
   */
  if ( discreteFlag) {
    weight -= 1.0;
    weight  = abs( weight);
    size_t entry = findMin( weight);
    weight = 0.0;
    weight[ entry] = 1.0;
  }
  
  /*
   * apply the weights to the vertices of the simplex and sum the results.
   */
  result = 0.0;
  vector<size_t>& delaunay = (vector<size_t>&)thisDelaunay[ nMin ];
  for ( size_t i = 0 ; i < nVer ; i++ ) {
    result = result + dataColumn[ delaunay[ i ] ] * weight[ i ];
  }

  return result;
}

} // namespace janus
