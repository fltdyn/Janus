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

const aString XMLFile = "CombinedExample.xml";

void example00()
{
  Janus janus( XMLFile);

  VariableDef& angleOfAttack  = janus.getVariableDef( "angleOfAttack");
  VariableDef& reynoldsNumber = janus.getVariableDef( "reynoldsNumber");
  VariableDef& ambientDensity = janus.getVariableDef( "ambientDensity");
  VariableDef& trueAirspeed   = janus.getVariableDef( "trueAirspeed");
  VariableDef& referenceArea  = janus.getVariableDef( "referenceArea");
  VariableDef& drag           = janus.getVariableDef( "drag");

  reynoldsNumber.setValue( 0.36e6);
  angleOfAttack.setValue( 10.0);
  ambientDensity.setValue( 1.225);
  trueAirspeed.setValue( 100.0);
  referenceArea.setValue( 25.0);

  cout << "Output value: " << drag.getValue() << " (expected: 1531.25)" << endl;
}

int main( int, char **)
{
  example00();

  return 0;
}
