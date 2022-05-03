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

#define S_FUNCTION_NAME JanusSFunction
#define S_FUNCTION_LEVEL 2

#include <set>
#include <string>

#include <simstruc.h>
#include <Janus/Janus.h>

using namespace std;

// #define DEBUG_PRINT( ...) printf( __VA_ARGS__)
#define DEBUG_PRINT( ...)

template<typename _Kty, class _Pr = std::less<_Kty>, class _Alloc = std::allocator<_Kty>>
std::set<_Kty,_Pr,_Alloc> exclusive_to_first( std::set<_Kty,_Pr,_Alloc> s1, const std::set<_Kty,_Pr,_Alloc>& s2)
{
  for ( auto it = s1.begin(); it != s1.end(); ) {
    if ( s2.find( *it) == s2.end()) {
      ++it;
    }
    else {
      it = s1.erase( it);
    }
  }
  return s1;
}

enum PARAMS
{
  PARAM_XML_FILENAME,
  PARAM_INDVARS,
  PARAM_DEPVARS,
  PARAM_COUNT
};

enum POINTERS
{
  JANUS,
  INDVARIDS,
  DEPVARIDS,
  INDVARS,
  DEPVARS,
  POINTER_COUNT
};

static void mdlInitializeSizes( SimStruct* S)
{
  ssSetNumSFcnParams( S, PARAM_COUNT);
  if ( ssGetNumSFcnParams( S) != ssGetSFcnParamsCount( S)) return;

  ssSetSFcnParamTunable( S, PARAM_XML_FILENAME, SS_PRM_NOT_TUNABLE);
  ssSetSFcnParamTunable( S, PARAM_INDVARS, SS_PRM_NOT_TUNABLE);
  ssSetSFcnParamTunable( S, PARAM_DEPVARS, SS_PRM_NOT_TUNABLE);

  ssSetNumContStates( S, 0);
  ssSetNumDiscStates( S, 0);

  // Get number of independent variables
  const mxArray* indvarArray = ssGetSFcnParam( S, PARAM_INDVARS);
  if ( mxGetClassID( indvarArray) == mxCHAR_CLASS) {
    const int nIndVars = mxGetM( indvarArray);
    DEBUG_PRINT( "nIndVars: %d\n", nIndVars);
  
    if ( !ssSetNumInputPorts( S, 1)) return;
    ssSetInputPortWidth( S, 0, nIndVars);

    ssSetInputPortRequiredContiguous( S, 0, false); 
    ssSetInputPortDirectFeedThrough( S, 0, 1);
  }
  else {
    if ( !ssSetNumInputPorts( S, 0)) return;
  }

  // Get number of dependent variables
  const mxArray* depvarArray = ssGetSFcnParam( S, PARAM_DEPVARS);
  if ( mxGetClassID( depvarArray) != mxCHAR_CLASS) {
    // ssSetErrorStatus( S, "Dependent varIDs must be a string array.");
    return;
  }
  const int nDepVars = mxGetM( depvarArray);
  DEBUG_PRINT( "nDepVars: %d\n", nDepVars);

  if ( !ssSetNumOutputPorts( S, 1)) return;
  ssSetOutputPortWidth( S, 0, nDepVars);

  ssSetNumSampleTimes( S, 1);
  ssSetNumRWork( S, 0);
  ssSetNumIWork( S, 0);
  ssSetNumPWork( S, POINTER_COUNT);
  ssSetNumModes( S, 0);
  ssSetNumNonsampledZCs( S, 0);

  ssSetOptions( S, 0);
  /*ssSetOptions( S, SS_OPTION_USE_TLC_WITH_ACCELERATOR | SS_OPTION_WORKS_WITH_CODE_REUSE);
  ssSetSupportedForCodeReuseAcrossModels( S, 1);*/
}

static void mdlInitializeSampleTimes( SimStruct* S)
{
  ssSetSampleTime( S, 0, INHERITED_SAMPLE_TIME);
  ssSetOffsetTime( S, 0, 0.0);

  ssSetModelReferenceSampleTimeInheritanceRule( S, INHERITED_SAMPLE_TIME);
}

#define MDL_START
static void mdlStart( SimStruct* S)
{
  if ( ssGetNumOutputPorts( S) == 0) return;

  ssGetPWork(S)[JANUS] = new janus::Janus;
  janus::Janus* janus = static_cast<janus::Janus*>( ssGetPWork(S)[JANUS]);

  ssGetPWork(S)[INDVARS] = new vector<janus::VariableDef*>;
  vector<janus::VariableDef*>* indVars = static_cast<vector<janus::VariableDef*>*>( ssGetPWork(S)[INDVARS]);

  ssGetPWork(S)[DEPVARS] = new vector<janus::VariableDef*>;
  vector<janus::VariableDef*>* depVars = static_cast<vector<janus::VariableDef*>*>( ssGetPWork(S)[DEPVARS]);

  int status, n;

  //
  // Get filename and read into Janus
  //
  const mxArray* filenameArray = ssGetSFcnParam( S, PARAM_XML_FILENAME);
  const size_t filenameLength = mxGetM( filenameArray) * mxGetN( filenameArray) + 1;
  char* filename = static_cast<char*>( calloc( filenameLength, sizeof( char)));
  status = mxGetString( filenameArray, filename, filenameLength);
  DEBUG_PRINT( "filename: %s\n", filename);
  if ( status) {
    // ssSetErrorStatus( S, "XML filename could not be read");
    return;
  }

  try {
    janus->setXmlFileName( filename);
    free( filename);
  } 
  catch ( std::exception &excep) {
    ssSetErrorStatus( S, excep.what());
    free( filename);
    return;
  }

  //
  // Get independent variables
  //
  if ( ssGetNumInputPorts( S) != 0) {
    const mxArray* indvarArray = ssGetSFcnParam( S, PARAM_INDVARS);
    const int nIndVars = mxGetM( indvarArray);
    char** indVarIDs = static_cast<char**>( calloc( nIndVars, sizeof( char*)));
    const int indVarLength = mxGetM( indvarArray) * mxGetN( indvarArray) + 1;
    char* indVarBuf = static_cast<char*>( calloc( indVarLength, sizeof( char)));
    status = mxGetString( indvarArray, indVarBuf, indVarLength);
    if ( status) {
      ssWarning( S, "Independent varID strings are truncated.");
    }
    n = mxGetN( indvarArray);
    for ( int var = 0; var < nIndVars; ++var) {
      indVarIDs[var] = static_cast<char*>( calloc( n + 1, sizeof( char)));
      int i = 0;
      for ( ; i < n; ++i) {
        if ( indVarBuf[var + nIndVars * i] == ' ') break;
        indVarIDs[var][i] = indVarBuf[var + nIndVars * i];
      }
      DEBUG_PRINT( "%d\t", i);
      indVarIDs[var][i] = '\0';
      DEBUG_PRINT( "indVarIDs[%d]: %s\n", var, indVarIDs[var]);
    }
    ssSetPWorkValue( S, INDVARIDS, indVarIDs);
    free( indVarBuf);

    // Sanity check variables
    set<string> janusInputVars;
    const janus::VariableDefList& varDefList = janus->getVariableDef();
    for ( auto& v : varDefList) {
      if ( v.isInput()) janusInputVars.insert( v.getVarID());
    }

    set<string> thisInputVars;
    for ( int i = 0; i < nIndVars; ++i) {
      thisInputVars.insert( indVarIDs[i]);
    }

    const set<string> janusOnlyInputs = exclusive_to_first( janusInputVars, thisInputVars);
    if ( !janusOnlyInputs.empty()) {
      string errorMessage = "The following input variables are expected by the dataset:\n";
      for ( const string& s : janusOnlyInputs) errorMessage += s + "\n";
      ssWarning( S, errorMessage.c_str());
    }

    set<string> badInputs;
    for ( const string& s : thisInputVars) {
      auto v = janus->findVariableDef( s);
      if ( v && !v->isInput()) badInputs.insert( s);
    }
    if ( !badInputs.empty()) {
      string errorMessage = "The following input variables are not marked as inputs within the dataset:\n";
      for ( const string& s : badInputs) errorMessage += s + "\n";
      ssWarning( S, errorMessage.c_str());
    }

    // Store varids
    for ( int i = 0; i < nIndVars; ++i) {
      indVars->push_back( janus->findVariableDef( indVarIDs[i]));
    }
  }
  else {
    ssSetPWorkValue( S, INDVARIDS, nullptr);
  }

  //
  // Get dependent variables
  //
  const mxArray* depvarArray = ssGetSFcnParam( S, PARAM_DEPVARS);
  const int nDepVars = mxGetM( depvarArray);
  char** depVarIDs = static_cast<char**>( calloc( nDepVars, sizeof( char*)));
  const int depVarLength = mxGetM( depvarArray) * mxGetN( depvarArray) + 1;
  char* depVarBuf = static_cast<char*>( calloc( depVarLength, sizeof( char)));
  status = mxGetString( depvarArray, depVarBuf, depVarLength);
  if ( status) {
    ssWarning( S, "Dependent varID strings are truncated.");
  }
  n = mxGetN( depvarArray);
  for ( int var = 0; var < nDepVars; ++var) {
    depVarIDs[var] = static_cast<char*>( calloc( n + 1, sizeof( char)));
    int i = 0;
    for ( ; i < n; ++i) {
      if ( depVarBuf[var + nDepVars * i] == ' ') break;
      depVarIDs[var][i] = depVarBuf[var + nDepVars * i];
    }
    DEBUG_PRINT( "%d\t", i);
    depVarIDs[var][i] = '\0';
    DEBUG_PRINT( "depVarIDs[%d]: %s\n", var, depVarIDs[var]);
  }
  ssSetPWorkValue( S, DEPVARIDS, depVarIDs);
  free( depVarBuf);

  // Sanity check variables
  set<string> badOutputs;
  for ( int i = 0; i < nDepVars; ++i) {
    if ( !janus->findVariableDef( depVarIDs[i])) badOutputs.insert( depVarIDs[i]);
  }
  if ( !badOutputs.empty()) {
    string errorMessage = "The following output variables are not provided by the dataset:\n";
    for ( const string& s : badOutputs) errorMessage += s + "\n";
    ssWarning( S, errorMessage.c_str());
  }

  // Store varids
  for ( int i = 0; i < nDepVars; ++i) {
    depVars->push_back( janus->findVariableDef( depVarIDs[i]));
  }
}

static void mdlOutputs( SimStruct *S, int_T tid)
{
  vector<janus::VariableDef*>* indVars = static_cast<vector<janus::VariableDef*>*>( ssGetPWork(S)[INDVARS]);
  vector<janus::VariableDef*>* depVars = static_cast<vector<janus::VariableDef*>*>( ssGetPWork(S)[DEPVARS]);

  if ( ssGetNumInputPorts( S) != 0) {
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs( S,0);

    for ( size_t i = 0; i < indVars->size(); ++i) {
      if ( indVars->at( i)) indVars->at( i)->setValueMetric( *uPtrs[i]);
    }
  }

  char** depVarIDs = static_cast<char**>( ssGetPWork(S)[DEPVARIDS]);
  const mxArray* depvarArray = ssGetSFcnParam( S, PARAM_DEPVARS);
  const int nDepVars = mxGetM( depvarArray);

  real_T* y = ssGetOutputPortRealSignal( S, 0);

  for ( int i = 0; i < depVars->size(); ++i) {
    if ( depVars->at( i)) y[i] = depVars->at( i)->getValueMetric();
  }
}

static void mdlTerminate( SimStruct *S)
{
  janus::Janus* janus = static_cast<janus::Janus*>( ssGetPWork(S)[JANUS]);
  delete janus;

  vector<janus::VariableDef*>* indVars = static_cast<vector<janus::VariableDef*>*>( ssGetPWork(S)[INDVARS]);
  delete indVars;

  vector<janus::VariableDef*>* depVars = static_cast<vector<janus::VariableDef*>*>( ssGetPWork(S)[DEPVARS]);
  delete depVars;

  void* pIndVarIDs = ssGetPWork(S)[INDVARIDS];
  if ( pIndVarIDs) {
    char** indVarIDs = static_cast<char**>( pIndVarIDs);
    const mxArray* indvarArray = ssGetSFcnParam( S, PARAM_INDVARS);
    const int nIndVars = mxGetM( indvarArray);
    for ( int i = 0; i < nIndVars; ++i) free( indVarIDs[i]);
    free( indVarIDs);
  }

  char** depVarIDs = static_cast<char**>( ssGetPWork(S)[DEPVARIDS]);
  const mxArray* depvarArray = ssGetSFcnParam( S, PARAM_DEPVARS);
  const int nDepVars = mxGetM( depvarArray);
  for ( int i = 0; i < nDepVars; ++i) free( depVarIDs[i]);
  free( depVarIDs);
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include <simulink.c>      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

