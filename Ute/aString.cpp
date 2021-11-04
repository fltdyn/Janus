//
// DST Ute Library (Utilities Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2018 Commonwealth of Australia
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

//
// Title:      aString Extended String Class
// Class:      aString
//
// Description:
//   Extension of the std::string class.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 23/03/2011
//

// C++ Includes
#include <cmath>

// Local Includes
#include "aMath.h"
#include "aString.h"

// ExprTk Script Includes
#if defined(WIN32) || defined(WIN64)
  #if defined(__GNUC__)
    // Disabled under MinGW due to long, long, long compile times.
    #define exprtk_disable_enhanced_features
//    #define exprtk_disable_string_capabilities
  #endif
#endif
#define exprtk_disable_return_statement
#define exprtk_disable_caseinsensitivity
#include "exprtk_matrix.hpp"

namespace dstoute {

  // Exprtk math expression evaluation
  double aString::evaluate() const
  {
    exprtk::symbol_table<double> constants;
    exprtk::expression<double> expression;
    exprtk::parser<double> parser;

    expression.register_symbol_table( constants);
    constants.add_constants();
    parser.compile( *this, expression);

    const double retval( expression.value());

    // Retun nan if inf to make error handling easier
    if ( dstomath::isinf( retval)) return dstomath::nan();

    return retval;
  }

} // namespace dstoute
