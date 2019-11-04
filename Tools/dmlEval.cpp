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

//
// Title: dmlEval (Dave-ML Evaluation Tool)
//
// Description:
//   Evaluates, tests and provides feedback on Dave-ML (dml) file.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 11/08/2017
// Based On: test2.cpp from DSTO SDHEngSoft library.
//

// C++ Includes
#include <iostream>

// Janus Includes
#include <Janus/Janus.h>

using namespace std;
using namespace dstoute;
using namespace janus;

void showVersion()
{
  cout << "dmlEval V-1.00" << endl
       << JANUS_VERSION_LONG << endl;
}

void showUsage()
{
  cout << "Usage: dmlEval [options] <dml_file> [output_var] [input_var...]"
       << "\n"
       << "\nOptions:"
       << "\n--help,       -h: This help display."
       << "\n--version     -V: Print out version information."
       << "\n--verbose     -v: Provide more detail about the variables."
       << "\n--checkdata   -c: Run checkdata cases and exit."
       << endl;
}

void showVarDefInfo( const VariableDef& varDef)
{
  cout << "\n" << (varDef.getType() == VariableDef::TYPE_INPUT ? "Input" : "Output") << " Variable"
       << "\n  ID           : " << varDef.getVarID()
       << "\n  Name         : " << varDef.getName()
       << "\n  Units        : " << varDef.getUnits()
       << "\n  Type         : " << varDef.getTypeString()
       << "\n  Method       : " << varDef.getMethodString()
       << "\n  Axis System  : " << varDef.getAxisSystem()
       << "\n  Symbol       : " << varDef.getSymbol()
       << "\n  Initial Value: " << varDef.getInitialValue()
       << "\n  Description  : " << varDef.getDescription()
       << "\n\n";
}

void checkData( Janus& dmlFile)
{
  size_t testCount = 0;
  size_t failCount = 0;

  const StaticShotList& staticShot = dmlFile.getCheckData().getStaticShot();
  for ( size_t i = 0; i < staticShot.size(); ++i) {
    testCount += staticShot[ i].getInternalValues().getSignalCount() +
      staticShot[ i].getCheckOutputs().getSignalCount();
    for ( size_t j = 0; j < staticShot[ i].getInvalidVariableCount(); ++j) {
      ++failCount;
      if ( !j) {
        cout << "FAIL: StaticShot \"" << staticShot[ i].getName() << "\"";
      }
      cout << "\n - " << staticShot[ i].getInvalidVariableMessage( j);
    }
  }

  if ( failCount) {
    cout << "\n\n" << failCount << " out of " << testCount << " tests failed." << endl;
  }
  else {
    if ( testCount) {
      cout << "All " << testCount << " tests passed." << endl;
    }
    else {
      cout << "No CheckData elements found." << endl;
    }
  }
}

int main( int argc, char *argv[] )
{
  bool flagVerbose   = false;
  bool flagCheckdata = false;
  aString fileName;
  aString outputVar;
  aStringList inputVar;

  // Process arguments.
  while ( --argc) {
    aString command = *++argv;
    // --help
    if ( command == "--help" || command == "-h") {
      showUsage();
      return 0;
    }
    else if ( command == "--version" || command == "-V") {
      showVersion();
      return 0;
    }
    else if ( command == "--verbose" || command == "-v") {
      flagVerbose = true;
    }
    else if ( command == "--checkdata" || command == "-c") {
      flagCheckdata = true;
    }
    else if ( fileName.empty()) {
      fileName = command;
    }
    else if ( outputVar.empty()) {
      outputVar = command;
    }
    else {
      inputVar << command;
    }
  }

  if ( fileName.empty()) {
    cout << "Error: Too few arguments...\n" << endl;
    showUsage();
    return 1;
  }

  // Load the dml file.
  Janus dmlFile;
  try {
    dmlFile.setXmlFileName( fileName );
  } catch ( exception & excep ) {
    cerr << excep.what() << endl;
    return 1;
  }

  if ( flagCheckdata) {
    checkData( dmlFile);
    return 0;
  }

  const VariableDefList& varDef = dmlFile.getVariableDef();

  // Output verbose information.
  if ( flagVerbose) {
    cout << "Total number of variables = " << varDef.size()
         << "\n";

    //
    // Find all potential input variables (including internal),
    // then display their characteristics and allow user to select one.
    //
    for ( size_t i = 0 ; i < varDef.size(); ++i) {
      if ( varDef[ i].getType() == VariableDef::TYPE_INPUT) {
        showVarDefInfo( varDef[ i]);
      }
    }

    //
    // Find all potential output variables (including internal),
    // then display their characteristics and allow user to select one.
    //
    for ( size_t i = 0 ; i < varDef.size(); ++i) {
      if ( varDef[ i].getType() != VariableDef::TYPE_INPUT) {
        showVarDefInfo( varDef[ i]);
      }
    }
  }

  if ( outputVar.empty()) {
    // Collect all output variables
    aList<size_t> outputRef;
    for ( size_t i = 0 ; i < varDef.size(); ++i) {
      if ( varDef[ i].getType() != VariableDef::TYPE_INPUT) {
        outputRef << i;
      }
    }

    cout << "Select output variable to evaluate:" << "\n";
    for ( size_t i = 0; i < outputRef.size(); ++i) {
      cout << "\n  " << setw(3) << i << " - "<< varDef[ outputRef[i]].getVarID();
      if ( flagVerbose &&
           varDef[ outputRef[i]].getDescription().size()) {
        cout << ": " << varDef[ outputRef[i]].getDescription();
      }
    }
    cout << "\n\nEnter index/name: ";

    aString userResponse;
    while ( userResponse.empty()) {
      getline( cin, userResponse);
    }
    cout << "\nEnter input values for..."
         << "\n(Ctrl-C to break)\n";

    const VariableDef& evalDef = ( userResponse.isDecimal() ?
      dmlFile.getVariableDef( outputRef[ userResponse.toSize_T()]) : dmlFile.getVariableDef( userResponse));

    if ( flagVerbose) {
      cout << "\nInitial value of " << evalDef.getVarID() << " = " << evalDef.getValue()
           << "\n";
    }

    // Loop evaluation until Ctrl-C.
    cout << "\n";
    while ( true) {
      const vector<size_t>& inputVarRef = evalDef.getAncestorsRef();
      for ( size_t i = 0 ; i < inputVarRef.size(); ++i) {
        VariableDef& inputVariableDef = dmlFile.getVariableDef( inputVarRef[ i]);
        if ( inputVariableDef.isInput()) {
          cout << inputVariableDef.getVarID() << " (" << inputVariableDef.getValue()
               << (inputVariableDef.getUnits().empty() ? "" : " " + inputVariableDef.getUnits()) << "): ";
          getline( cin, userResponse);
          if ( userResponse.isNumeric()) {
            inputVariableDef.setValue( userResponse.toDouble());
          }
        }
      }

      double y = 0.0;
      try {
        y = evalDef.getValue();
      }
      catch ( exception &testErr) {
        cerr << testErr.what();
        exit( 1);
      }

      cout << "\n  " << evalDef.getVarID() << " = " << y
           << ( evalDef.getUnits().empty() ? "" : " " + evalDef.getUnits())
           << "\n\n";

      // Exit if there are no inputs for this variable, i.e. a constant.
      if ( inputVarRef.size() == 0) return 0;
    }
  }

  // Single evaluation from command-line.
  else {
    aList<VariableDef*> inputDef;
    const VariableDef& evalDef = dmlFile.getVariableDef( outputVar);
    const vector<size_t>& inputVarRef = evalDef.getAncestorsRef();
    for ( size_t i = 0 ; i < inputVarRef.size(); ++i) {
      if ( dmlFile.getVariableDef( inputVarRef[ i]).isInput()) {
        inputDef << &dmlFile.getVariableDef( inputVarRef[ i]);
      }
    }

    if ( inputDef.size() != inputVar.size()) {
      cerr << "\nError: Missing all input data for variable \"" << evalDef.getVarID() << "\""
           << "\n       Required inputs are:";
      for ( size_t i = 0; i < inputDef.size(); ++i) {
        cerr << "\n       - \"" << inputDef[ i]->getVarID() << "\" (" << inputDef[ i]->getUnits() << ")";
      }
      return 1;
    }

    for ( size_t i = 0; i < inputDef.size(); ++i) {
      inputDef[ i]->setValue( inputVar[ i].toDouble());
    }
    cout << evalDef.getVarID() << " = " << evalDef.getValue()
         << ( evalDef.getUnits().empty() ? "" : " " + evalDef.getUnits());
  }

  return 0;
}
