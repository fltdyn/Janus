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
#include <Janus/JanusVariableManager.h>

using namespace std;
using namespace dstomath;
using namespace dstoute;

const aString XMLFile = "JanusVariableManagerExample.xml";

void example00()
{
  JanusVariableManager jvm;
  jvm.setXmlFileName( XMLFile);
  
  JanusIndex jInput00  = jvm.push_back( JanusVariable( "input00",  janusInputVariable,  janusMandatory, "m s-1", 0.0));
  JanusIndex jOutput00 = jvm.push_back( JanusVariable( "output00", janusOutputVariable, janusMandatory, "kn",    0.0));

  if ( jvm[ jInput00].isAvailable() && jvm[ jOutput00].isAvailable()) {
    // Note the conversion from m s-1 to knots
    jvm[ jInput00].setValue( 1.0);
    cout << "Output value: " << jvm[ jOutput00].value() << " (expected: 1.94384)" << endl;
  }
}

void example01()
{
  JanusVariableManager jvm;
  jvm.setXmlFileName( XMLFile);

  JanusIndex jInput00  = jvm.push_back( JanusVariable( "input00",  janusInputVariable,  janusMandatory, "m s-1", 0.0));
  JanusIndex jOutput00 = jvm.push_back( JanusVariable( "output00", janusOutputVariable, janusMandatory, "kn",    0.0));

  if ( jvm[ jInput00].setValue( 1.0)) {
    cout << "Output value: " << jvm[ jOutput00].value() << " (expected: 1.94384)" << endl;
  }

  if ( jvm[ jInput00].setValue( 1.0)) {
    // Not reached, as input00 had already been set to 1.0
    cout << "Output value: " << jvm[ jOutput00].value() << " (expected: 1.94384)" << endl;
  }

  if ( jvm[ jInput00].setValue( 2.0)) {
    cout << "Output value: " << jvm[ jOutput00].value() << " (expected: 3.88769)" << endl;
  }
}

int main( int, char **)
{
  example00();
  example01();

  return 0;
}
