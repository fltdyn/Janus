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

const aString XMLFile = "SetVarDefExample.xml";

void example00()
{
  Janus janus( XMLFile);

  VariableDef& input00        = janus.getVariableDef( "input00");
  const VariableDef& output00 = janus.getVariableDef( "output00");

  input00.setValue( 15.0);
  cout << "Output value: " << output00.getValue() << " (expected: 19)" << endl;
}

void example01()
{
  Janus janus( XMLFile);

  const VariableDef& incrementer = janus.getVariableDef( "incrementer");
  const VariableDef& output01    = janus.getVariableDef( "output01");

  incrementer.getValue(); // Compiles expression

  cout << "Output value: " << output01.getValue() << " (expected: 6)" << endl;

  incrementer.getValue();
  cout << "Output value: " << output01.getValue() << " (expected: 9)"  << endl;

  incrementer.getValue();
  cout << "Output value: " << output01.getValue() << " (expected: 12)"  << endl;
}

void example02()
{
  Janus janus( XMLFile);

  VariableDef& input02          = janus.getVariableDef( "input02");
  const VariableDef& output02         = janus.getVariableDef( "output02");
  const VariableDef& function02       = janus.getVariableDef( "function02");

  input02.setValue( 42.0);
  function02.getValue();
  cout << "Output value: " << output02.getValue() << " (expected: 42)" << endl;
}

void example03()
{
  Janus janus( XMLFile);

  VariableDef& input03           = janus.getVariableDef( "input03");
  const VariableDef& output03_00 = janus.getVariableDef( "output03_00");
  const VariableDef& function03  = janus.getVariableDef( "function03");

  input03.setValue( 123.456);
  function03.getValue();
  cout << "Output value: " << output03_00.getValue() << " (expected: 123.456)" << endl;
}

void example04()
{
  Janus janus( XMLFile);

  const VariableDef& function04_00 = janus.getVariableDef( "function04_00");
  const VariableDef& function04_01 = janus.getVariableDef( "function04_01");

  cout << "Output value: " << function04_00.getValue() << " (expected: 56.78)" << endl;
  cout << "Output value: " << function04_01.getValue() << " (expected: 56.78)" << endl;
}

void example05()
{
  Janus janus( XMLFile);

  const VariableDef& output05_00 = janus.getVariableDef( "output05_00");
  const VariableDef& output05_01 = janus.getVariableDef( "output05_01");

  cout << "Output value: " << output05_00.getValue() << " (expected: 0.0)" << endl;
  cout << "Output value: " << output05_01.getValue() << " (expected: 40.0)" << endl;
}

void example06()
{
  Janus janus( XMLFile);

  const VariableDef& output06_00   = janus.getVariableDef( "output06_00");
  const VariableDef& output06_01   = janus.getVariableDef( "output06_01");
  const VariableDef& output06_02   = janus.getVariableDef( "output06_02");
  const VariableDef& output06_03   = janus.getVariableDef( "output06_03");
  const VariableDef& internal06_00 = janus.getVariableDef( "internal06_00");
  const VariableDef& internal06_01 = janus.getVariableDef( "internal06_01");
  const VariableDef& internal06_02 = janus.getVariableDef( "internal06_02");
  const VariableDef& internal06_21 = janus.getVariableDef( "internal06_21");

  cout << "Output value: " << output06_00.getValue()   << " (expected: 2704.0)" << endl;
  cout << "Output value: " << internal06_00.getValue() << " (expected: 0.0)" << endl;
  cout << "Output value: " << output06_01.getValue()   << " (expected: 52.0)" << endl;
  cout << "Output value: " << internal06_01.getValue() << " (expected: 0.0)" << endl;
  cout << "Output value: " << output06_02.getValue()   << " (expected: 52.0)" << endl;
  cout << "Output value: " << internal06_02.getValue() << " (expected: 0.0)" << endl;
  cout << "Output value: " << output06_03.getValue()   << " (expected: 231.0)" << endl;
  cout << "Output value: " << internal06_21.getValue() << " (expected: 0.0)" << endl;
}

int main( int, char **)
{
  example00();
  example01();
  example02();
  example03();
  example04();
  example05();
  example06();

  return 0;
}
