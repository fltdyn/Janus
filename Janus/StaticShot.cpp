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

//------------------------------------------------------------------------//
// Title:      Janus/StaticShot
// Class:      StaticShot
// Module:     StaticShot.cpp
// First Date: 2009-05-08
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file StaticShot.cpp
 *
 * A StaticShot instance holds in its allocated memory alphanumeric data
 * derived from a \em staticShot element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance describes
 * the inputs and outputs, and possibly internal values, of a DAVE-ML model
 * at a particular instant in time.  The class also provides the functions
 * that allow a calling Janus instance to access these data elements.
 *
 * The StaticShot class is only used within the janus namespace, and should
 * only be referenced indirectly through the CheckData class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes
#include <stdexcept>
#include <sstream>
#include <iostream>

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aUnits.h>
#include <Ute/aMatrix.h>
#include <Ute/aOptional.h>

//#include "Janus.h"
#include "DomFunctions.h"
#include "StaticShot.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;
using namespace dstomath;

namespace janus {

const aString OUT_OF_RANGE = "Error:: requested index is out of range";

//------------------------------------------------------------------------//

StaticShot::StaticShot( )
  :
  XmlElementDefinition(),
  janus_( 0),
  elementType_( ELEMENT_NOTSET),
  signalType_( SIGNAL_CHECKINPUTS),
  hasProvenance_( false )
{
}


StaticShot::StaticShot(
  const DomFunctions::XmlNode& elementDefinition,
  Janus* janus )
  :
  XmlElementDefinition(),
  janus_( 0),
  elementType_( ELEMENT_NOTSET),
  signalType_( SIGNAL_CHECKINPUTS),
  hasProvenance_( false )
{
  initialiseDefinition( elementDefinition, janus);
}


//------------------------------------------------------------------------//

void StaticShot::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition,
  Janus* janus )
{
  static const aString functionName( "StaticShot::initialiseDefinition()");
  janus_ = janus;

  /*
   * Retrieve attributes for the element's Definition
   */
  name_  = DomFunctions::getAttribute( elementDefinition, "name");
  refID_ = DomFunctions::getAttribute( elementDefinition, "refID");
  
  /*
   * Retrieve the description associated with the element
   */
  description_ = DomFunctions::getChildValue( elementDefinition, "description" );

  /*
   * Retrieve the optional Provenance associated with the element
   */
  elementType_ = ELEMENT_PROVENANCE;
  try {
    DomFunctions::initialiseChildOrRef( this,
                                        elementDefinition,
                                        EMPTY_STRING,
                                        "provenance",
                                        "provenanceRef",
                                        "provID",
                                        false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - for function \"" << name_ << "\"\n - "
      << excep.what()
    );
  }
  
  /*
   * Retrieve the optional CheckInputs associated with the element
   */
  elementType_ = ELEMENT_STATICSHOT;
  signalType_ = SIGNAL_CHECKINPUTS;
  try {
    DomFunctions::initialiseChild( this,
                                   elementDefinition,
                                   name_,
                                   "checkInputs",
                                   false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - for function \"" << name_ << "\"\n - "
      << excep.what()
    );
  }
  
  /*
   * Retrieve the optional InternalValues associated with the element
   */
  signalType_ = SIGNAL_INTERNALVALUES;
  try {
    DomFunctions::initialiseChild( this,
                                   elementDefinition,
                                   name_,
                                   "internalValues",
                                   false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - for function \"" << name_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Retrieve the CheckOutputs associated with the element
   */
  signalType_ = SIGNAL_CHECKOUTPUTS;
  try {
    DomFunctions::initialiseChild( this,
                                   elementDefinition,
                                   name_,
                                   "checkOutputs",
                                   true);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - for function \"" << name_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Need to check the consistency between signal types, particularly
   * if they are managed through a signal list.
   */
  try {
    checkSignalConsistency();
  }
  catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - for signal data consistency \n - "
        << excep.what()
      );
  }
}

//------------------------------------------------------------------------//
void StaticShot::checkSignalConsistency()
{
  /*
   * Need to check the consistency between signal types, particularly
   * if they are managed through a signal list.
   *
   * a) if hasSignals = true for one signal type (checkInputs, internalVales,
   *    checkOuptuts) then others also have hasSignals = true.
   * b) if hasSignalList = true for one signal type (checkInputs, internalVales,
   *    checkOuptuts) then others also have hasSignalList = true.
   * c) if hasSignalList and checkInputs signalDefs have multiply values then
   *    checkOutputs signalDefs are sized appropriately
   * d) if varID in signalDefs refers to a vector/matrix variableDef then
   *    signal data values managed appropriately
   */

  static const aString functionName( "StaticShot::checkSignalConsistency()");

  // The following check addresses both a) and b)
  bool XOR_A = ( checkInputs_.hasSignals() ^ internalValues_.hasSignals());
  bool XOR_B = ( checkInputs_.hasSignals() ^ checkOutputs_.hasSignals());
  if ( XOR_A | XOR_B) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - the signal types (checkInputs, internalVales, checkOuptuts)\n"
      << " use a mixture of legacy and new signal definitions\n - "
      << "\n This arrangement is not supported by Janus."
      );
  }

  // The following check addresses c)
  size_t inputValueCount  = 1;
  size_t outputValueCount = 0;
  SignalDef signalDef;

  if ( checkInputs_.hasSignalList()) {
    SignalList signalList = checkInputs_.getSignalList();
    for ( size_t i = 0; i < signalList.getSignalCount(); ++i) {
      signalDef =
        janus_->getSignalDef( signalList.getSignalRef()[i]);
      inputValueCount *= signalDef.getValueCount();
    }

    signalList = checkOutputs_.getSignalList();
    for ( size_t i = 0; i < signalList.getSignalCount(); ++i) {
      signalDef =
        janus_->getSignalDef( signalList.getSignalRef()[i]);

      outputValueCount = signalDef.getValueCount();
      if ( outputValueCount != inputValueCount) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - the number of values for the checkOutput signal: " << signalDef.getName()
          << "\n does not match the number of values for the checkInput signals.\n - "
          << "\n This arrangement is not supported by Janus."
          );
      }
    }
  }

  // The following check addresses d):
  try {
    if ( checkInputs_.hasSignalList()) {
      checkVectorMatrixConsistency( checkInputs_.getSignalList());
    }
  }
  catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - for checkInputs consistency \n - "
        << excep.what()
      );
  }

  try {
    if ( checkInputs_.hasSignalList()) {
      checkVectorMatrixConsistency( internalValues_.getSignalList());
    }
  }
  catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - for checkInputs consistency \n - "
        << excep.what()
      );
  }

  try {
    if ( checkInputs_.hasSignalList()) {
      checkVectorMatrixConsistency( checkOutputs_.getSignalList());
    }
  }
  catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - for checkInputs consistency \n - "
        << excep.what()
      );
  }
}

void StaticShot::checkVectorMatrixConsistency( const SignalList& signalList)
{
  static const aString functionName( "StaticShot::checkVectorMatrixConsistency()");

  size_t inputValueCount  = 1;
  size_t dimTotal = 0;
  double quot = 0;
  SignalDef signalDef;


  for ( size_t i = 0; i < signalList.getSignalCount(); ++i) {
    signalDef =
        janus_->getSignalDef( signalList.getSignalRef()[i]);

    inputValueCount = signalDef.getValueCount();

    VariableDef &variableDef = janus_->getVariableDef( signalDef.getVarIndex());

    if ( variableDef.isMatrix() || variableDef.isVector()) {
      dimTotal = variableDef.getDimension().getDimTotal();
      double rem = ::modf( double( inputValueCount / dimTotal), &quot);

      if ( fabs( rem) > dstomath::zero()) {
        throw_message( invalid_argument,
                       setFunctionName( functionName)
                       << "\n - This signal represents a vector or a matrix: " << signalDef.getName()
                       << "\n - the number of values for the signal: " << inputValueCount
                       << "\n is not a whole multiply of the vector/matrix dimension: " << dimTotal
                       << "\n This arrangement is not supported by Janus."
        );
      }
    }
  }
}

//------------------------------------------------------------------------//

//bool StaticShot::hasInternalValues( ) const
//{
//  if ( 0 < internalValuesValid_.size() ) {
//    return true;
//  }
//  return false;
//}

//------------------------------------------------------------------------//

void StaticShot::verifyStaticShot()
{
  static const aString functionName( "StaticShot::verifyStaticShot()");

  /*
   * Initialise the valid flags for the internal and output
   * checkData variables.
   */
  for ( size_t i = 0 ; i < internalValues_.getSignalCount() ; i++ ) {
    internalValuesValid_.push_back( false );
  }
  for ( size_t i = 0 ; i < checkOutputs_.getSignalCount() ; i++ ) {
    checkOutputsValid_.push_back( false );
  }

  /*
   * Copy the variableDefs of the checkInputs so they can be reset
   * after the static shot has been evaluated.
   */
  copyCheckInputVariableDefs();

  /*
   * NOTE: It is expected when defining a static shot that the convention
   * for defining signals is consistent between the checkInputs, internalValues,
   * and the checkOutputs. That is, all signals for a static shot will be
   * defined using the legacy 'signals' definition [DAVE-ML V2.0], or alternatively
   * using the replacement 'signalsDef' definition [CheckData V2.0].
   *
   * The schemes WILL NOT be mixed
   */
  try {
    evaluateSignals();
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
                   setFunctionName( functionName)
                   << "\n - "
                   << excep.what()
    );
  }

  /*
   * Reset the input signal variables to their state prior to
   * evaluating check data.
   */
  resetCheckInputVariableDefs();

  return;
}

//------------------------------------------------------------------------//

size_t StaticShot::getInvalidVariableCount( ) const
{
  size_t nInvalid = 0;
  
  size_t numberOfInternalValuesValid = internalValuesValid_.size();
  for ( size_t i = 0 ; i < numberOfInternalValuesValid ; i++ ) {
    if ( !internalValuesValid_[ i ] ) {
      nInvalid++;
    }
  }

  size_t numberOfCheckOutputsValid = checkOutputsValid_.size();
  for ( size_t i = 0 ; i < numberOfCheckOutputsValid ; i++ ) {
    if ( !checkOutputsValid_[ i ] ) {
      nInvalid++;
    }
  }
  
  return nInvalid;
}

//------------------------------------------------------------------------//

aString StaticShot::getInvalidVariableMessage( int index) const
{
  static const aString functionName( "StaticShot::getInvalidVariableMessage()");

  aList<bool> validList;
  validList << internalValuesValid_;
  validList << checkOutputsValid_;

  aOptionalSizeT varIdx;
  for ( size_t i = 0; i < validList.size(); ++i) {
    if ( !validList[ i]) --index;
    if ( index < 0) {
      varIdx = i;
      break;
    }
  }

  if ( !varIdx.isValid()) {
    return OUT_OF_RANGE;
  }

  if ( internalValues_.hasSignals()) {
    if ( varIdx < internalValuesValid_.size()) { // Internal Value
      return invalidMessageSignals( internalValues_.getSignals().getSignal(),
                                    internalValues_.getSignalType(),
                                    varIdx);
    }
  }
  else { // SignalList of signalDefs/signalRefs
    if ( varIdx < internalValuesValid_.size()) { // Internal Value
      return invalidMessageSignalList( internalValues_.getSignalType(),
                                       varIdx);
    }
  }

  // Output Value
  varIdx -= internalValuesValid_.size();
  if ( checkOutputs_.hasSignals()) {
    return invalidMessageSignals( checkOutputs_.getSignals().getSignal(),
                                  checkOutputs_.getSignalType(),
                                  varIdx);
  }
  else { // SignalList of signalDefs/signalRefs
    return invalidMessageSignalList( checkOutputs_.getSignalType(),
                                     varIdx);
  }

  return EMPTY_STRING;
}

//------------------------------------------------------------------------//

void StaticShot::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the StaticShot element
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "staticShot");

  /*
   * Add attributes to the StaticShot child
   */
  DomFunctions::setAttribute( childElement, "name", name_);

  if ( !refID_.empty()) {
    DomFunctions::setAttribute( childElement, "refID", refID_);
  }

  /*
   * Add description element
   */
  if ( !description_.empty()) {
    DomFunctions::setChild( childElement, "description", description_);
  }

  /*
   * Add the optional provenance entry to the StaticShot child
   */
  if ( hasProvenance_) {
    provenance_.exportDefinition( childElement);
  }

  /*
   * Add signal entries to the StaticShot child
   */
  checkInputs_.exportDefinition( childElement);
  internalValues_.exportDefinition( childElement);
  checkOutputs_.exportDefinition( childElement);
}

void StaticShot::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "StaticShot::readDefinitionFromDom()");

  try {
    switch ( elementType_) {
      case ELEMENT_PROVENANCE:
        provenance_.initialiseDefinition( xmlElement );
      break;

      default:
        break;
    }
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }

  try {
    switch ( signalType_) {
      case SIGNAL_CHECKINPUTS:
        checkInputs_.initialiseDefinition( xmlElement, signalType_, janus_);
      break;

      case SIGNAL_INTERNALVALUES:
        internalValues_.initialiseDefinition( xmlElement, signalType_, janus_);
      break;

      case SIGNAL_CHECKOUTPUTS:
        checkOutputs_.initialiseDefinition( xmlElement, signalType_, janus_);
      break;

      default:
        break;
    }
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }
}

bool StaticShot::compareElementID(
  const DomFunctions::XmlNode& xmlElement,
  const aString& elementID,
  const size_t& /*documentElementReferenceIndex*/)
{
  if ( DomFunctions::getAttribute( xmlElement, "provID") != elementID) {
    return false;
  }

  readDefinitionFromDom( xmlElement);  
  return true;
}

//------------------------------------------------------------------------//

void StaticShot::evaluateSignals()
{
  static const aString functionName( "StaticShot::evaluateSignals()");

  /*
   * NOTE: It is expected when defining a static shot that the convention
   * for defining signals is consistent between the checkInputs, internalValues,
   * and the checkOutputs. That is, all signals for a static shot will be
   * defined using the legacy 'signals' definition [DAVE-ML V2.0], or alternatively
   * using the replacement 'signalsDef' definition [CheckData V2.0].
   *
   * The schemes WILL NOT be mixed
   */

  if ( checkInputs_.hasSignalList()) {
    processSignalList( checkInputs_.getSignalList());
  }
  else {
    // Loop through the different signal types, starting with checkInputs,
    // then internalValues and finally checkOutputs.

    if ( !checkInputs_.hasSignals() || !checkOutputs_.hasSignals()) {
      throw_message( invalid_argument,
	    setFunctionName( functionName)
	    << "\n - the signal types (checkInputs, internalVales, checkOuptuts)\n"
	    << " use a mixture of legacy and new signal definitions\n - "
	    << "\n This arrangement is not supported by Janus."
	    );
    }

    // Check Inputs
    try {
      processSignals( checkInputs_.getSignalType(),
                      checkInputs_.getSignals().getSignal());
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - "
        << excep.what()
        );
    }

    // Internal Values - Optional
    if ( internalValues_.hasSignals()) {
      try {
        processSignals( internalValues_.getSignalType(),
                        internalValues_.getSignals().getSignal());
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
          );
      }
    }

    // Check Outputs
    try {
      processSignals( checkOutputs_.getSignalType(),
                      checkOutputs_.getSignals().getSignal());
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - "
        << excep.what()
      );
    }
  }
}

void StaticShot::processSignals( const SignalTypeEnum& signalType,
                                 aListSignals& signals)
{
  static const aString functionName( "StaticShot::processSignals()");
  
  aOptionalSizeT varIndex;
  double value;
  vector<VariableDef>& variableDefs = janus_->getVariableDef();

  size_t i = 0;
  aListSignals::iterator it;

  for ( it = signals.begin(); it != signals.end(); it++) {
    switch ( signalType) {
      case SIGNAL_CHECKINPUTS:
        varIndex = copyInputSignal_[i].first;
        break;

      case SIGNAL_INTERNALVALUES:
        varIndex = janus_->crossReferenceId( ELEMENT_VARIABLE, it->getVarID());
        break;

      case SIGNAL_CHECKOUTPUTS:
        varIndex = janus_->crossReferenceName( ELEMENT_VARIABLE_OUTPUT, it->getName());
        break;

      default:
        break;
    }

    aString variableUnits;
    if ( varIndex.isValid()) {
      switch ( signalType) {
        case SIGNAL_CHECKINPUTS:
          value = it->getValue();
          variableUnits = variableDefs[varIndex].getUnits();
          if ( variableUnits != it->getUnits() ) {
            aUnits fromUnits = findUnits( it->getUnits());
            aUnits toUnits   = findUnits( variableUnits);
            value = convert( fromUnits, toUnits, value);
          }
          variableDefs[varIndex].setValue( value);
          break;

        case SIGNAL_INTERNALVALUES:
          value = variableDefs[ varIndex].getValue();
          it->setActualValue( value);
          if ( fabs( value - it->getValue()) <= it->getTolerance()) {
            internalValuesValid_[ i ] = true;
          }
          break;

        case SIGNAL_CHECKOUTPUTS:
          value = variableDefs[ varIndex].getValue();
          variableUnits = variableDefs[varIndex].getUnits();
          if ( variableUnits != it->getUnits() ) {
            aUnits fromUnits = findUnits( it->getUnits());
            aUnits toUnits   = findUnits( variableUnits);
            value = convert( fromUnits, toUnits, value);
          }

          it->setActualValue( value);
          if ( fabs( value - it->getValue()) <= it->getTolerance()) {
            checkOutputsValid_[ i ] = true;
          }
          break;

        default:
          break;
      }
    }
    else {
      throw_message( runtime_error,
        setFunctionName( functionName)
        << "\n - Check signal \""
        << it->getName()
        << "\" not found in dataset."
      );
    }

    ++i;
  }
}

void StaticShot::processSignalList( const SignalList& signalList)
{
  static const aString functionName( "StaticShot::processSignalList()");

  SignalDef signalDef;
  size_t nSignals = signalList.getSignalCount();

  // populate the values of the checkInput variable

  // @TODO: Need to Resolve - DIES HERE if nSignals = 0, Why is nSignals = 0 ?????
  signalDef =
    janus_->getSignalDef( signalList.getSignalRef()[0]);

  if ( !signalDef.getVarIndex().isValid()) {
    throw_message( runtime_error,
      setFunctionName( functionName)
      << "\n - The signal definition \""
      << signalDef.getName()
      << "\" has an invalid variable definition index."
    );
  }

  VariableDef &variableDef = janus_->getVariableDef( signalDef.getVarIndex());

  // If vector/Matrix then nLoops = nValues / Dimensions
  // If nLoops < Dimensions then repeat for nLoops with all V/M elements
  // set identical
  aDoubleList values = signalDef.getValue();
  size_t nValues = values.size();
  size_t nLoops  = nValues;

  DVector tVector;
  DMatrix tMatrix;
  double quot;
  size_t dimTotal = 0;
  if ( variableDef.isMatrix() || variableDef.isVector()) {
    dimTotal = variableDef.getDimension().getDimTotal();
    double rem = ::modf( double( nValues / dimTotal), &quot);
    nLoops = size_t( quot) + size_t( ceil( rem * dimTotal));

    if ( variableDef.isVector()) {
      tVector.resize( dimTotal);
    }

    if ( variableDef.isMatrix()) {
      tMatrix.resize( variableDef.getDimension().getDim(0),
                      variableDef.getDimension().getDim(1));
    }
  }

  for ( size_t j = 0; j < internalValues_.getSignalList().getSignalCount(); ++j) {
    janus_->getSignalDef( internalValues_.getSignalList().getSignalRef()[j]).clearActualValue();
  }
  for ( size_t j = 0; j < checkOutputs_.getSignalList().getSignalCount(); ++j) {
    janus_->getSignalDef( checkOutputs_.getSignalList().getSignalRef()[j]).clearActualValue();
  }

  int upLmt = -1;
  int lwLmt = 0;
  for ( size_t i = 0; i < nLoops; ++i) {
    if ( variableDef.isValue()) {
      variableDef.setValue( values[i], true);
    }
    else if ( variableDef.isMatrix()) { // isMatrix == true
      lwLmt = upLmt + 1;
      if ( i < quot) {
        upLmt = int( dimTotal * ( i + 1)) - 1;
        size_t nCols = tMatrix.cols();
        for ( size_t j = 0; j < tMatrix.rows(); ++j) {
          for ( size_t k = 0; k < nCols; ++k) {
            tMatrix( j, k) = values[ lwLmt + (j* nCols) + k];
          }
        }
      }
      else {
        tMatrix = values[ lwLmt];
        ++upLmt;
      }
      variableDef.setValue( tMatrix, true);
    }
    else { // or isVector == true
      lwLmt = upLmt + 1;
      if ( i < quot) {
        upLmt = int( dimTotal * ( i + 1)) - 1;
        for ( size_t k = 0; k < dimTotal; ++k) {
          tVector[k] = values[ lwLmt + k];
        }
      }
      else {
        tVector = values[ lwLmt];
        ++upLmt;
      }
      variableDef.setValue( tVector, true);
    }

    if ( nSignals > 1) {
      SignalList signalListCopy = signalList;
      SignalList signalSubList;
      signalListCopy.subList( 1, nSignals, &signalSubList);
      processSignalList( signalSubList);
    }
    else {
      SignalDef *evalSignalDef = 0;
      // evaluate internal signals
      for ( size_t j = 0; j < internalValues_.getSignalList().getSignalCount(); ++j) {
        try {
          evalSignalDef =
              &janus_->getSignalDef( internalValues_.getSignalList().getSignalRef()[j]);
        }
        catch ( exception &excep) {
          throw_message( invalid_argument,
                         setFunctionName( functionName)
                         << "\n - "
                         << excep.what()
          );
        }

        VariableDef &variableDefIntV = janus_->getVariableDef( evalSignalDef->getVarIndex());

        if ( variableDefIntV.isValue()) {
          evalSignalDef->setActualValue( variableDefIntV.getValue());
        }
        else if ( variableDefIntV.isMatrix()) { // isMatrix == true
          DVector matrixData = variableDefIntV.getMatrix().matrixData();
          for ( size_t k = 0; k < matrixData.size(); ++k) {
            evalSignalDef->setActualValue( matrixData[k]);
          }
        }
        else { // or isVector == true
          DVector matrixData = variableDefIntV.getVector();
          for ( size_t k = 0; k < matrixData.size(); ++k) {
            evalSignalDef->setActualValue( matrixData[k]);
          }
        }
      }

      // evaluate output signals
      for ( size_t j = 0; j < checkOutputs_.getSignalList().getSignalCount(); ++j) {
        try {
          evalSignalDef =
              &janus_->getSignalDef( checkOutputs_.getSignalList().getSignalRef()[j]);
        }
        catch ( exception &excep) {
          throw_message( invalid_argument,
                         setFunctionName( functionName)
                         << "\n - "
                         << excep.what()
          );
        }

        VariableDef &variableDefOutC = janus_->getVariableDef( evalSignalDef->getVarIndex());
        if ( variableDefOutC.isValue()) {
          evalSignalDef->setActualValue( variableDefOutC.getValue());
        }
        else if ( variableDefOutC.isMatrix()) { // isMatrix == true
          DVector matrixData = variableDefOutC.getMatrix().matrixData();
          for ( size_t k = 0; k < matrixData.size(); ++k) {
            evalSignalDef->setActualValue( matrixData[k]);
          }
        }
        else { // or isVector == true
          DVector matrixData = variableDefOutC.getVector();
          for ( size_t k = 0; k < matrixData.size(); ++k) {
            evalSignalDef->setActualValue( matrixData[k]);
          }
        }
      }
    }
  }

  // Check if the internal values and the outputs are valid.
  // check internal value signals
  SignalDef *evalSignalDef = 0;

  for ( size_t i = 0; i < internalValues_.getSignalList().getSignalCount(); ++i) {
    try {
      evalSignalDef =
          &janus_->getSignalDef( internalValues_.getSignalList().getSignalRef()[i]);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
                     setFunctionName( functionName)
                     << "\n - "
                     << excep.what()
      );
    }

    internalValuesValid_[ i] = evalSignalDef->isCheckValid();
  }

  // check output signals
  for ( size_t i = 0; i < checkOutputs_.getSignalList().getSignalCount(); ++i) {
    try {
      evalSignalDef =
          &janus_->getSignalDef( checkOutputs_.getSignalList().getSignalRef()[i]);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
                     setFunctionName( functionName)
                     << "\n - "
                     << excep.what()
      );
    }

    checkOutputsValid_[ i] = evalSignalDef->isCheckValid();
  }
}

//------------------------------------------------------------------------//

aString StaticShot::invalidMessageSignals( const aListSignals& signals,
                                           const SignalTypeEnum& signalType,
                                           const size_t sIndex) const
{
  aString unitsText = signals[ sIndex].getUnits();
  aString varID_nameText;

  switch ( signalType) {
    case SIGNAL_INTERNALVALUES:
      varID_nameText = signals[ sIndex].getVarID();
      break;

    case SIGNAL_CHECKOUTPUTS:
      varID_nameText = signals[ sIndex].getName();
      break;

    default:
      break;
  }

  if ( !unitsText.empty()) {
    unitsText = aString( " (%)").arg( unitsText);
  }
  return aString( "%: expected % %, but evaluated as % %\n")
    .arg( varID_nameText)
    .arg( signals[ sIndex].getValue())
    .arg( unitsText)
    .arg( signals[ sIndex].getActualValue())
    .arg( unitsText);
}

aString StaticShot::invalidMessageSignalList( const SignalTypeEnum& signalType,
                                              const size_t sIndex) const
{
  aString   unitsText;
  aString   varIDText;
  SignalDef signalDef;

  switch ( signalType) {
    case SIGNAL_INTERNALVALUES:
      signalDef =
        janus_->getSignalDef( internalValues_.getSignalList().getSignalRef()[sIndex]);
      break;

    case SIGNAL_CHECKOUTPUTS:
      signalDef =
        janus_->getSignalDef( checkOutputs_.getSignalList().getSignalRef()[sIndex]);
      break;

    default:
      break;
  }

  unitsText = signalDef.getUnits();
  varIDText = signalDef.getVarID();

  if ( !unitsText.empty()) {
    unitsText = aString( " (%)").arg( unitsText);
  }

  aString message;
  for ( size_t i = 0; i < signalDef.getValue().size(); ++i) {
    message += aString( ": expected % %, but evaluated as % %\n")
      .arg( signalDef.getValue()[i])
      .arg( unitsText)
      .arg( signalDef.getActualValue()[i])
      .arg( unitsText);
  }

  return aString( "%: invalid check signal \n%")
    .arg( varIDText)
    .arg( message);
}

//------------------------------------------------------------------------//

void StaticShot::copyCheckInputVariableDefs()
{

  vector<VariableDef>& variableDefs = janus_->getVariableDef();
  size_t nInputSigs = 0;
  aOptionalSizeT varIndex;

  copyInputSignal_.clear();
  if ( checkInputs_.hasSignals()) {
    nInputSigs = checkInputs_.getSignals().getSignal().size();
    aListSignals signals = checkInputs_.getSignals().getSignal();

    for ( size_t i = 0; i < nInputSigs; ++i) {
      varIndex = janus_->crossReferenceName( ELEMENT_VARIABLE,
                                             signals[i].getName());
      copyInputSignal_.push_back( InputPair( varIndex,
                                             variableDefs[varIndex]));
    }
  }
  else {
    nInputSigs = checkInputs_.getSignalList().getSignalRef().size();
    aStringList signalRefs = checkInputs_.getSignalList().getSignalRef();
    SignalDef signalDef;

    for ( size_t i = 0; i < nInputSigs; ++i) {
      signalDef =
        janus_->getSignalDef( signalRefs[i]);
      varIndex = janus_->crossReferenceId( ELEMENT_VARIABLE,
                                           signalDef.getVarID());
      copyInputSignal_.push_back( InputPair( varIndex,
                                             variableDefs[varIndex]));
    }
  }
}

void StaticShot::resetCheckInputVariableDefs() const
{
  /*
   * Reset the input signal variables to their state prior to
   * evaluating check data.
   */
  vector<VariableDef>& variableDefs = janus_->getVariableDef();

  for ( size_t i = 0; i < copyInputSignal_.size(); i++) {
    if ( copyInputSignal_[i].first.isValid()) {
      variableDefs[ copyInputSignal_[i].first] = copyInputSignal_[i].second;
    }
  }

}

//------------------------------------------------------------------------//

void StaticShot::resetJanus( Janus* janus)
{
  janus_ = janus;
  checkInputs_.resetJanus( janus);
  internalValues_.resetJanus( janus);
  checkOutputs_.resetJanus( janus);
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const StaticShot &staticShot)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display StaticShot contents:" << endl
     << "-----------------------------------" << endl;

  os << "  name               : " << staticShot.getName() << endl
     << "  refID              : " << staticShot.getRefID() << endl
     << "  description        : " << staticShot.getDescription() << endl
     << endl;

  os << "  hasProvenance      : " << staticShot.hasProvenance() << endl
     << endl;

  /*
   * Data associated with Class
   */
  if ( staticShot.hasProvenance()) {
    os << staticShot.getProvenance() << endl;
  }

  // @TODO : add more here

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus
