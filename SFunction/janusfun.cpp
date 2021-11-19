//
// DST Janus Library (Janus DAVE-ML Interpreter Library)
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

/**
 * \file janusfun.cpp
 *
 * Title:     DAVE-ML to Matlab gateway function
 * Class:     DaveLab
 * Module:    janusfun.cpp
 * Reference: 
 *
 * Description:
 * A loadable mex-file or dynamic link library provides a low-level interface 
 * that allows the Octave or Matlab workspace to use the Janus 
 * function-evaluating code.  The dll is called from the workspace in the 
 * form:
 *
 * depVar = janusfun( filename, depVarID, indepVarIDs, indepVars )
 * janusfun( command)
 *
 * First Date:  20/04/2006
 * 
 * Written by: Dan Newman (dmnewman@pobox.com)
 *             Quantitative Aeronautics Pty Ltd,
 *             under contract to DSTO.
 *
 */

// C++ Includes
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>

#include <mex.h>

#include <Janus/Janus.h>

using namespace janus;
using namespace std;

class JanusElement
{
public: 
  JanusElement() :
    janus_(0)
  {
  }
  
  JanusElement(Janus *janus, const string &fileName)
  {
    janus_ = janus;
    fileName_= fileName;
  }
  
  JanusElement(const JanusElement &element)
  {
    janus_ = element.janus_;
    fileName_ = element.fileName_;
  }
  ;
  
  string fileName_;
  Janus *janus_;
};

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  //
  // vector containing instances of Janus.
  //
  static vector<JanusElement> janusList;

  int iLen, iInp, iCols;
  char* filename;
  char* depVarID;
  char** indepVarID;
  char errmsg[1024];

  if ( ( nrhs != 1 && nrhs != 4 && nrhs != 5 ) || nlhs > 3 ) {
    mexErrMsgTxt( "\n\n Usage:  \n"
      "  [x, axis, unit] = janusfun( XMLfilename, depVarID, indepVarIDs, indepVars, <uncertainty> )\n"
      "  janusfun('@reset')\n\n"
      "         Evaluate a variable or its uncertainty as defined within a \n"
      "         DAVE-ML compliant XML dataset, based on values of variables \n"
      "         within the Matlab workspace.\n\n"
      " Inputs: XMLfilename    = file defining variable to evaluate\n"
      "         depvarID       = varID of variable for which to solve\n"
      "         indepVarIDs(n) = column vector of varIDs for which values are supplied\n"
      "         indepVars(n)   = column vector of variable values for indepVarIDs\n"
      "         uncertainty    = optional argument\n"
      "                          integer = no of standard deviations of Gaussian pdf\n"
      "                          boolean true/false = upper/lower bound of uniform pdf\n\n"
      " Examples:\n"
      "   x = janusfun( 'example19.xml', 'Cm_u', char('Alpha_deg'), [8.5]);\n"
      "   x = janusfun( 'example19.xml', 'Cm_u', char('Alpha_deg'), [8.5], numSigmas);\n"
      "   x = janusfun( 'MachCoeff.xml', 'MachCoeff2D', char('Alpha', 'Mach'), [-20.0; 0.8]);\n"
      "   [x, axis] = janusfun( 'MachCoeff.xml', 'MachCoeff2D', char('Alpha', 'Mach'), [-20.0; 0.8]);\n"
      "   [x, axis, unit] = janusfun( 'MachCoeff.xml', 'MachCoeff2D', char('Alpha', 'Mach'), [-20.0; 0.8]);\n"
      );
  }

  if (mxIsChar(prhs[0]) != 1) {
    mexErrMsgTxt("Dataset name must be a string.");
  }
  iLen = ( mxGetM(prhs[0]) * mxGetN(prhs[0]) ) + 1;
  filename = (char*)mxMalloc( iLen * sizeof(char));
  int status = mxGetString( prhs[0], filename, iLen );
  if ( status != 0) {
    mexWarnMsgTxt("Not enough space.  Dataset name is truncated.");
  }

  //
  // JanusList commands:
  //   @reset:   Delete all Janus instances.
  //
  if ( nrhs == 1) {
    string janusCommand = filename;
    if ( janusCommand == "@reset") {
      for ( unsigned int i = 0; i < janusList.size(); ++i) {
        delete janusList[i].janus_;
      }
      janusList.clear();
      return;
    }
    return;
  }

  if ( mxIsChar(prhs[1]) != 1) {
    mexErrMsgTxt( "Dependent variable ID must be a string.");
  }
  iLen = ( mxGetM(prhs[1]) * mxGetN(prhs[1]) ) + 1;
  depVarID = (char*)mxMalloc( iLen * sizeof(char));
  status = mxGetString( prhs[1], depVarID, iLen );
  if ( status != 0) {
    mexWarnMsgTxt( "Not enough space.  Dependent varID is truncated.");
  }

  if (mxIsChar(prhs[2]) != 1) {
    mexErrMsgTxt("Independent varIDs must be a string array.");
  }
  iInp = mxGetM(prhs[2]);
  indepVarID = (char**)mxCalloc(iInp, sizeof(char*));
  iLen = (mxGetM(prhs[2]) * mxGetN(prhs[2]) ) + 1;
  char* input_buf = (char*)mxCalloc(iLen, sizeof(char));
  status = mxGetString(prhs[2], input_buf, iLen );
  if ( 0 != status ) {
    mexWarnMsgTxt("Independent varID strings are truncated.");
  }
  iLen = mxGetN(prhs[2]);
  int iof, j2;
  for ( int i = 0; i < iInp ; i++) {
    for ( int j = 0; j < iLen ; j++) {
      iof = i + iInp * j;
      if ( ' ' == input_buf[iof]|| j == iLen - 1) {
        j2 = j + 1;
        if ( ' ' != input_buf[iof]) {
          j2 = j2 + 1;
        }
        indepVarID[i] = (char*)mxCalloc(j2, sizeof(char));
        indepVarID[i][j2 - 1] = '\0';
        for ( int k = 0; k < j2 - 1; k++) {
          indepVarID[i][k] = input_buf[i + iInp * k ];
        }
        break;
      }
    }
  }
  mxFree(input_buf );
  
  if ( int( mxGetM(prhs[3])) != iInp ) {
    sprintf( errmsg, "%s\n\t VarIDs = %d, Vars = %d\n",
      "Input varID and variable rows mismatch", iInp, int( mxGetM(prhs[3])));
    mexErrMsgTxt( errmsg);
  }
  iCols = mxGetN(prhs[3]);
  double* x = (double *)mxGetPr(prhs[3]);

  plhs[0] = mxCreateDoubleMatrix( 1, iCols, mxREAL );
  double* y = (double *)mxGetPr(plhs[0]);

  /*
   * Initialise Janus instance, not encrypted
   */

  //
  // Check for existing instance of this XML file within JanusList.
  //
  Janus *janus = 0;
  for ( unsigned int i = 0; janus == 0&& i < janusList.size(); ++i) {
    if ( janusList[i].fileName_ == filename) {
      janus = janusList[i].janus_;
    }
  }

  //
  // If instance does not exist in JanusList, create a new instance.
  //
  if ( janus == 0) {
    janus = new Janus;
    try {
      janus->setXmlFileName( filename);
    }
    catch ( exception &excep ) {
      mexErrMsgTxt( excep.what() );
    }
    janusList.push_back(JanusElement(janus, filename));
  }

  VariableDef& outputVarDef = janus->getVariableDef( depVarID );
  int numSigmas = -1;
  bool isUpper = false;
  if ( 5 == nrhs ) {
    const Uncertainty::UncertaintyPdf& pdf = 
      outputVarDef.getUncertainty().getPdf();
    if ( mxIsNumeric( prhs[4] ) && Uncertainty::NORMAL_PDF == pdf ) {
      numSigmas = int( mxGetScalar( prhs[4]));
    }
    else if ( mxIsLogicalScalarTrue( prhs[4] ) &&
              Uncertainty::UNIFORM_PDF == pdf ) {
      isUpper = true; 
    }
    else if ( Uncertainty::UNIFORM_PDF == pdf ) {
      stringstream errStr;
      errStr << "Variable \"" << depVarID
             << "\" has uniform pdf ...";
      mexErrMsgTxt( errStr.str().c_str() );
    }
    else if ( Uncertainty::NORMAL_PDF == pdf ) {
      stringstream errStr;
      errStr << "Variable \"" << depVarID
             << "\" has Gaussian pdf ...";
      mexErrMsgTxt( errStr.str().c_str() );
    }
  }

  string axisString = string();
  string unitString = string();

  for ( int i = 0; i < iCols ; i++) {
    for ( int j = 0; j < iInp ; j++) {
      iof = j + i * iInp;
      try {
        janus->getVariableDef( dstoute::aString( indepVarID[j]) ).setValue( x[iof]);
      }
      catch (...) {
//        char errmsg[80];
        sprintf( errmsg, "Variable \"%s\" not set ...", indepVarID[j]);
        mexErrMsgTxt( errmsg );
      }
    }
    if ( 4 == nrhs ) {
      y[i] = outputVarDef.getValue();
      axisString = outputVarDef.getAxisSystem();
      unitString = outputVarDef.getUnits();
    }
    else {
      if ( 0 < numSigmas ) {
        y[i] = outputVarDef.getUncertaintyValue( size_t( numSigmas) );
      }
      else {
        y[i] = outputVarDef.getUncertaintyValue( isUpper );
      }
    }
  }

  // Create Axis mxArray entry
  if ( nlhs > 1) {
    const char **axisCharArray = (const char**)mxCalloc(1, sizeof(const char*));
    *axisCharArray = axisString.c_str();
    plhs[1] = mxCreateCharMatrixFromStrings( 1, axisCharArray);
    mxFree( axisCharArray );
  }

  // Create Unit mxArray entry
  if ( nlhs > 2) {
    const char **unitCharArray = (const char**)mxCalloc(1, sizeof(const char*));
    *unitCharArray = unitString.c_str();
    plhs[2] = mxCreateCharMatrixFromStrings( 1, unitCharArray);
    mxFree( unitCharArray );
  }

  mxFree( filename );
  mxFree( depVarID );
  for ( int i = 0; i < iInp ; i++) {
    mxFree ( indepVarID[i]);
  }
  mxFree( indepVarID );

  return;

}
