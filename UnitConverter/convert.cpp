
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

// C++ Includes
//#include <cstdio>
//#include <iostream>
//#include <stdexcept>
//#include <cmath>
//#include <string>
//#include <vector>
//#include <sstream>

#include <mex.h>

#include "units.h"

using namespace std;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if ( nrhs != 2 && nrhs != 3) {
    mexErrMsgTxt( 
      "\n\n Usage:  \n"
      "  value = convert( value, from, to)\n"
      "  value = convert( value, from) % Converts to metric\n"
    );
  }

  //
  // Values
  //
  if ( mxIsDouble( prhs[0]) != 1) {
    mexErrMsgTxt( "Value must be a double.");
  }
  const size_t xCols = mxGetN( prhs[0]);
  const size_t xRows = mxGetM( prhs[0]);
  const double* x = (double *) mxGetPr( prhs[0]);

  plhs[0] = mxCreateDoubleMatrix( xRows, xCols, mxREAL);
  double* y = (double*) mxGetPr( plhs[0]);

  //
  // From units
  //
  if ( mxIsChar( prhs[1]) != 1) {
    mexErrMsgTxt( "From units must be a string.");
  }
  const size_t fromLen = mxGetM( prhs[1]) * mxGetN( prhs[1]) + 1;
  char* from = (char*) mxMalloc( fromLen * sizeof( char));
  int fromStatus = mxGetString( prhs[1], from, fromLen);
  if ( fromStatus != 0) {
    mexWarnMsgTxt( "Not enough space. From units are truncated.");
  }

  //
  // To units
  //
  char* to;

  if ( nrhs == 2) {
    const string metricTo = units::metric( from);
    to = (char*) mxMalloc( metricTo.size() * sizeof( char));
    strcpy( to, metricTo.c_str());
  }
  else {
    if ( mxIsChar( prhs[2]) != 1) {
      mexErrMsgTxt( "To units must be a string.");
    }
    const size_t toLen = mxGetM( prhs[2]) * mxGetN( prhs[2]) + 1;
    to = (char*) mxMalloc( toLen * sizeof( char));
    int toStatus = mxGetString( prhs[2], to, toLen);
    if ( toStatus != 0) {
      mexWarnMsgTxt( "Not enough space. To units are truncated.");
    }
  }

  //
  // Conversion
  //
  auto converter = units::get_converter( from, to);

  if ( !converter) {
    const string err = "Units \"" + string( from) + "\" and \"" + string( to) + "\" are not compatible.";
    mexErrMsgTxt( err.c_str());
  }

  for ( size_t j = 0; j < xRows; ++j) {
    for ( size_t i = 0; i < xCols; ++i) {
      y[i + j * xCols] = converter( x[i + j * xCols]);
    }
  }

  mxFree( from);
  mxFree( to);
}
