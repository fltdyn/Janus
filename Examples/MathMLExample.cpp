//
// DST Janus Library (Janus DAVE-ML Interpreter Library)
// 
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2019 Commonwealth of Australia
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

#include <iostream>
#include <Ute/aString.h>
#include <Janus/Janus.h>

using namespace std;
using namespace janus;
using namespace dstomath;
using namespace dstoute;

const aString XMLFile = "MathMLExample.xml";

void example00()
{
  Janus janus( XMLFile);

  VariableDef& x       = janus.getVariableDef( "x");
  VariableDef& y       = janus.getVariableDef( "y");
  VariableDef& result  = janus.getVariableDef( "result");
  VariableDef& sign    = janus.getVariableDef( "sign");
  VariableDef& cosd    = janus.getVariableDef( "cosd");
  VariableDef& sind    = janus.getVariableDef( "sind");
  VariableDef& tand    = janus.getVariableDef( "tand");
  VariableDef& cscd    = janus.getVariableDef( "cscd");
  VariableDef& secd    = janus.getVariableDef( "secd");
  VariableDef& cotd    = janus.getVariableDef( "cotd");
  VariableDef& arccosd = janus.getVariableDef( "arccosd");
  VariableDef& arcsind = janus.getVariableDef( "arcsind");
  VariableDef& arctand = janus.getVariableDef( "arctand");
  VariableDef& bound   = janus.getVariableDef( "bound");

  x.setValue( 0.1);
  y.setValue( 0.2);

  cout << "Output value: " << result.getValue() << " (expected: 0.463648)" << endl;
  cout << "sign( 0.1, 0.2): " << sign.getValue() << " (expected: 0.1)" << endl;
  cout << "cosd( 0.1): " << cosd.getValue() << " (expected: 0.999998)" << endl;
  cout << "sind( 0.1): " << sind.getValue() << " (expected: 0.00174533)" << endl;
  cout << "tand( 0.1): " << tand.getValue() << " (expected: 0.00174533)" << endl;
  cout << "cscd( 0.1): " << cscd.getValue() << " (expected: 572.958)" << endl;
  cout << "secd( 0.1): " << secd.getValue() << " (expected: 1.0)" << endl;
  cout << "cotd( 0.1): " << cotd.getValue() << " (expected: 572.957)" << endl;
  cout << "arccosd( 0.1): " << arccosd.getValue() << " (expected: 84.2608)" << endl;
  cout << "arcsind( 0.1): " << arcsind.getValue() << " (expected: 5.73917)" << endl;
  cout << "arctand( 0.1): " << arctand.getValue() << " (expected: 5.71059)" << endl;
  cout << "bound( 0.1, 0.5, 1.0): " << bound.getValue() << " (expected: 0.5)" << endl;
}

int main( int, char **)
{
  example00();

  return 0;
}
