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

Janus* getJanusInstance( int nrhs, const mxArray *prhs[]);
string getDepVarId( int nrhs, const mxArray *prhs[]);
vector<string> getIndepVarIds( int nrhs, const mxArray *prhs[]);
void writeValuesOutput( Janus* janus, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void writeAxisOutput( Janus* janus, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void writeUnitsOutput( Janus* janus, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if ( ( nrhs != 1 && nrhs != 2 && nrhs != 4 && nrhs != 5) || nlhs > 3) {
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
      "   x = janusfun( 'example19.xml', 'aerodynamicReferenceArea');\n"
      "   x = janusfun( 'example19.xml', 'Cm_u', char('Alpha_deg'), [8.5]);\n"
      "   x = janusfun( 'example19.xml', 'Cm_u', char('Alpha_deg'), [8.5], numSigmas);\n"
      "   x = janusfun( 'MachCoeff.xml', 'MachCoeff2D', char('Alpha', 'Mach'), [-20.0; 0.8]);\n"
      "   [x, axis] = janusfun( 'MachCoeff.xml', 'MachCoeff2D', char('Alpha', 'Mach'), [-20.0; 0.8]);\n"
      "   [x, axis, unit] = janusfun( 'MachCoeff.xml', 'MachCoeff2D', char('Alpha', 'Mach'), [-20.0; 0.8]);\n"
      );
  }

  //
  // Get Janus Instance - also handles reset
  //
  Janus* janus = getJanusInstance( nrhs, prhs);
  if ( !janus) return;

  writeValuesOutput( janus, nlhs, plhs, nrhs, prhs);
  writeAxisOutput( janus, nlhs, plhs, nrhs, prhs);
  writeUnitsOutput( janus, nlhs, plhs, nrhs, prhs);
}

Janus* getJanusInstance( int nrhs, const mxArray *prhs[])
{
  static vector<JanusElement> janusList;

  //
  // Read filename
  //
  if ( nrhs < 1) return nullptr;

  if ( mxIsChar(prhs[0]) != 1) {
    mexErrMsgTxt("Dataset name must be a string.");
  }
  size_t iLen = ( mxGetM( prhs[0]) * mxGetN( prhs[0])) + 1;
  char* filename = (char*)mxMalloc( iLen * sizeof(char));
  int status = mxGetString( prhs[0], filename, iLen );
  if ( status != 0) {
    mexWarnMsgTxt("Not enough space.  Dataset name is truncated.");
  }


  if ( nrhs == 1) {
    string janusCommand = filename;
    mxFree( filename);
    if ( janusCommand == "@reset") {
      for ( unsigned int i = 0; i < janusList.size(); ++i) {
        delete janusList[i].janus_;
      }
      janusList.clear();
    }
    else {
      mexErrMsgTxt( "Unrecognised janus command");
    }
    return nullptr;
  }

  //
  // Check for existing instance of this XML file within JanusList.
  //
  Janus* janus = nullptr;
  for ( unsigned int i = 0; janus == 0&& i < janusList.size(); ++i) {
    if ( janusList[i].fileName_ == filename) {
      janus = janusList[i].janus_;
    }
  }

  //
  // If instance does not exist in JanusList, create a new instance.
  //
  if ( !janus) {
    janus = new Janus;
    try {
      janus->setXmlFileName( filename);
    }
    catch ( exception &excep ) {
      mexErrMsgTxt( excep.what() );
    }
    janusList.push_back( JanusElement( janus, filename));
  }

  mxFree( filename);
  return janus;
}

string getDepVarId( int nrhs, const mxArray *prhs[])
{
  if ( nrhs < 2) return "";

  if ( mxIsChar(prhs[1]) != 1) {
    mexErrMsgTxt( "Dependent variable ID must be a string.");
  }
  size_t iLen = ( mxGetM( prhs[1]) * mxGetN( prhs[1])) + 1;
  char* depVarID = (char*)mxMalloc( iLen * sizeof(char));
  int status = mxGetString( prhs[1], depVarID, iLen);
  if ( status != 0) {
    mexWarnMsgTxt( "Not enough space.  Dependent varID is truncated.");
  }
  string ret( depVarID);
  mxFree( depVarID);
  return ret;
}

vector<string> getIndepVarIds( int nrhs, const mxArray *prhs[])
{
  if ( nrhs < 3) return {};

  if (mxIsChar(prhs[2]) != 1) {
    mexErrMsgTxt("Independent varIDs must be a string array.");
  }
  size_t iInp = mxGetM( prhs[2]);
  char** indepVarID = (char**)mxCalloc(iInp, sizeof(char*));
  size_t iLen = ( mxGetM(prhs[2]) * mxGetN( prhs[2])) + 1;
  char* input_buf = (char*)mxCalloc(iLen, sizeof(char));
  int status = mxGetString(prhs[2], input_buf, iLen );
  if ( 0 != status ) {
    mexWarnMsgTxt("Independent varID strings are truncated.");
  }
  iLen = mxGetN( prhs[2]);
  size_t iof, j2;
  for ( size_t i = 0; i < iInp ; i++) {
    for ( size_t j = 0; j < iLen ; j++) {
      iof = i + iInp * j;
      if ( ' ' == input_buf[iof] || j + 1 == iLen) {
        j2 = j + 1;
        if ( ' ' != input_buf[iof]) {
          j2 = j2 + 1;
        }
        indepVarID[i] = (char*)mxCalloc(j2, sizeof(char));
        indepVarID[i][j2 - 1] = '\0';
        for ( size_t k = 0; k + 1 < j2; k++) {
          indepVarID[i][k] = input_buf[i + iInp * k ];
        }
        break;
      }
    }
  }
  mxFree(input_buf);
  vector<string> ret( iInp);
  for ( int i = 0; i < iInp; ++i) {
    ret[i] = string( indepVarID[i]);
    mxFree( indepVarID[i]);
  }
  mxFree( indepVarID );
  return ret;
}

void writeValuesOutput( Janus* janus, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  const string depVarId = getDepVarId( nrhs, prhs);
  const vector<string> indepVarId = getIndepVarIds( nrhs, prhs);

  const size_t nInp  = nrhs > 3 ? mxGetM( prhs[3]) : 0;
  const size_t nCols = std::max( nrhs > 3 ? int( mxGetN( prhs[3])) : 1, 1); // Getting a constant

  if ( nInp != int( indepVarId.size())) {
    stringstream errStr;
    errStr << "Input varID and variable rows mismatch\n\t"
           << " VarIDs = " << indepVarId.size()
           << ", Vars = " << nInp << "\n";
    mexErrMsgTxt( errStr.str().c_str() );
  }

  double* x = nrhs > 3 ? (double *)mxGetPr( prhs[3]) : nullptr;

  plhs[0] = mxCreateDoubleMatrix( 1, nCols, mxREAL);
  double* y = (double *)mxGetPr( plhs[0]);

  VariableDef& outputVarDef = janus->getVariableDef( depVarId);
  int numSigmas = -1;
  bool isUpper = false;
  if ( 5 == nrhs) {
    const Uncertainty::UncertaintyPdf& pdf = outputVarDef.getUncertainty().getPdf();
    if ( mxIsNumeric( prhs[4]) && Uncertainty::NORMAL_PDF == pdf) {
      numSigmas = int( mxGetScalar( prhs[4]));
    }
    else if ( mxIsLogicalScalarTrue( prhs[4]) && Uncertainty::UNIFORM_PDF == pdf) {
      isUpper = true; 
    }
    else if ( Uncertainty::UNIFORM_PDF == pdf) {
      stringstream errStr;
      errStr << "Variable \"" << depVarId << "\" has uniform pdf ...";
      mexErrMsgTxt( errStr.str().c_str() );
    }
    else if ( Uncertainty::NORMAL_PDF == pdf ) {
      stringstream errStr;
      errStr << "Variable \"" << depVarId << "\" has Gaussian pdf ...";
      mexErrMsgTxt( errStr.str().c_str() );
    }
  }

  for ( size_t i = 0; i < nCols ; i++) {
    for ( size_t j = 0; j < nInp ; j++) {
      size_t iof = j + i * nInp;
      try {
#ifdef JANUSFUN_SI
        janus->getVariableDef( indepVarId[j]).setValueSI( x[iof]);
#else
        janus->getVariableDef( indepVarId[j]).setValue( x[iof]);
#endif
      }
      catch (...) {
        stringstream errStr;
        errStr << "Variable \"" << indepVarId[j] << "\" not set ...";
        mexErrMsgTxt( errStr.str().c_str() );
      }
    }
    if ( nrhs != 5) {
#ifdef JANUSFUN_SI
      y[i] = outputVarDef.getValueSI();
#else
      y[i] = outputVarDef.getValue();
#endif
    }
    else if ( 0 < numSigmas) {
      y[i] = outputVarDef.getUncertaintyValue( size_t( numSigmas));
    }
    else {
      y[i] = outputVarDef.getUncertaintyValue( isUpper);
    }
  }
}

void writeAxisOutput( Janus* janus, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if ( nlhs < 2) return;

  const string depVarId = getDepVarId( nrhs, prhs);
  VariableDef& outputVarDef = janus->getVariableDef( depVarId);
  const string axisString = outputVarDef.getAxisSystem();
  const char **axisCharArray = (const char**)mxCalloc(1, sizeof(const char*));
  *axisCharArray = axisString.c_str();
  plhs[1] = mxCreateCharMatrixFromStrings( 1, axisCharArray);
  mxFree( axisCharArray );
}

void writeUnitsOutput( Janus* janus, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if ( nlhs < 3) return;

  const string depVarId = getDepVarId( nrhs, prhs);
  VariableDef& outputVarDef = janus->getVariableDef( depVarId);
#ifdef JANUSFUN_SI
  const string sourceUnitsString = outputVarDef.getUnits();
  const dstoute::aUnits units( sourceUnitsString);
  const string unitsString = units.unitsSI();
#else
  const string unitsString = outputVarDef.getUnits();
#endif
  const char **unitCharArray = (const char**)mxCalloc(1, sizeof(const char*));
  *unitCharArray = unitsString.c_str();
  plhs[2] = mxCreateCharMatrixFromStrings( 1, unitCharArray);
  mxFree( unitCharArray );
}
