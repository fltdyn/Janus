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

/*
 * Title:      Janus Data Element Class
 * Class:      JanusVariable
 * Module:     JanusVariable.cpp
 * Reference:
 *
 * Description:
 *  This file contains the class prototypes for data elements that
 *  interact with Janus XML files.
 *  The associated header file for the class is: JanusVariable.cpp
 *
 * Written by: Geoff Brian (Geoff.Brian@dsto.defence.gov.au)
 *             Shane Hill  (Shane.Hill@dsto.defence.gov.au)
 *
 * Derived from: XmlDataElement class by Geoff Brian.
 *
 * First Date: 19/01/2010
 *
 */

// C++ Includes
#include <iostream>
#include <sstream>
#include <cassert>

// Local Includes
#include "JanusVariable.h"

// Ute Includes
#include <Ute/aMath.h>
#include <Ute/aMessageStream.h>
#include <Ute/aOptional.h>

using namespace dstoute;
using namespace dstomath;
using namespace std;

JanusVariable EMPTY_JANUSVARIABLE;
const aString EMPTY_STRING;

JanusVariable::JanusVariable()
 : janusFile_( 0),
   variableDef_( 0),
   variableName_( ""),
   variableType_( janusOutputVariable),
   userData_( 0),
   value_( 0),
   isInitialised_( false),
   isMandatory_( false),
   doUnitConversion_( false),
   doDeltaConversion_( false)
{
}

JanusVariable::JanusVariable( int /* dummy */)
 : janusFile_( 0),
   variableDef_( 0),
   variableName_( ""),
   variableType_( janusOutputVariable),
   userData_( 0),
   value_( 0),
   isInitialised_( false),
   isMandatory_( false),
   doUnitConversion_( false),
   doDeltaConversion_( false)
{
}

JanusVariable::JanusVariable( janus::Janus &janusFile, const aString &variableName,
                              JanusVariableType variableType, bool isMand, const double &val)
 : janusFile_( &janusFile),
   variableDef_( 0),
   variableName_( variableName),
   variableType_( variableType),
   userData_(0),
   value_( val),
   isInitialised_( false),
   isMandatory_( isMand),
   doUnitConversion_( false),
   doDeltaConversion_( false)
{
  initialiseVariable();
}

JanusVariable::JanusVariable( janus::Janus &janusFile, const aString &variableName, JanusVariableType variableType,
                              bool isMand, const aString &specificUnits, const double &val)
 : janusFile_( &janusFile),
   variableDef_( 0),
   variableName_( variableName),
   variableType_( variableType),
   userData_(0),
   value_( val),
   isInitialised_( false),
   isMandatory_( isMand),
   doUnitConversion_( false),
   doDeltaConversion_( false)
{
  requiredValue_ = findUnits( specificUnits);
  initialiseVariable();
}

JanusVariable::JanusVariable( const aString &variableName, JanusVariableType variableType,
                              bool isMand, const double &val)
 : janusFile_( 0),
   variableDef_( 0),
   variableName_( variableName),
   variableType_( variableType),
   userData_(0),
   value_( val),
   isInitialised_( false),
   isMandatory_( isMand),
   doUnitConversion_( false)
{
}

JanusVariable::JanusVariable( const aString &variableName, JanusVariableType variableType,
                              bool isMand, const aString &specificUnits, const double &val)
 : janusFile_( 0),
   variableDef_( 0),
   variableName_( variableName),
   variableType_( variableType),
   userData_(0),
   value_( val),
   isInitialised_( false),
   isMandatory_( isMand),
   doUnitConversion_( false)
{
  requiredValue_ = findUnits( specificUnits);
}

void JanusVariable::setJanusFile( janus::Janus *janusFile)
{
  if ( janusFile_ != janusFile) {
    janusFile_ = janusFile;
    aOptionalSizeT variableDefIndex = janusFile_->getVariableIndex( variableName_.c_str());
    if ( variableDefIndex.isValid()) {
      variableDef_ = &janusFile_->getVariableDef()[ variableDefIndex];
    }
  }
}


void JanusVariable::initialiseVariable()
{
  static const aString functionName( "JanusVariable::initialiseVariable()");

  if ( (janusFile_ == 0) || variableName_.empty()) {
    return;
  }

  bool   ignoreUnits  = false;
  double initialValue = value_;
  isInitialised_      = true;
  doUnitConversion_   = false;
  doDeltaConversion_  = false;
  variableDef_        = 0;
  errorMessage_.clear();

  switch ( variableType_) {
  // Delta variables.
  case janusDeltaOutputVariable:
    variableType_      = janusOutputVariable;
    doDeltaConversion_ = true;
    break;

  case janusDeltaInputVariable:
    variableType_      = janusInputVariable;
    doDeltaConversion_ = true;
    break;

  case janusDeltaInputOutputVariable:
    variableType_      = janusInputOutputVariable;
    doDeltaConversion_ = true;
    break;

  // Ignore units variables.
  case janusIgnoreUnitsOutputVariable:
    variableType_      = janusOutputVariable;
    ignoreUnits        = true;
    break;

  case janusIgnoreUnitsInputVariable:
    variableType_      = janusInputVariable;
    ignoreUnits        = true;
    break;

  case janusIgnoreUnitsInputOutputVariable:
    variableType_      = janusInputOutputVariable;
    ignoreUnits        = true;
    break;

  default:
    break;
  }

  aString variableList( variableName_);
  aString preferredName = aString( variableName_).getStringToken("|");
  aOptionalSizeT variableDefIndex;

  while ( variableList.hasStringTokens()) {

    variableName_ = variableList.getStringToken("|");
    variableDefIndex = janusFile_->getVariableIndex( variableName_.c_str());

    if ( variableDefIndex.isValid()) {
      variableDef_ = &janusFile_->getVariableDef()[ variableDefIndex];
      variableDef_->setReferencedExternally( true);

      if ( variableType_ == janusString) return;

      value_ = variableDef_->getValue();

      // Check for missing initialValue attributes on numeric variableDefs.
      if ( dstomath::isnan( value_) &&
           variableDef_->getMethod() == janus::VariableDef::METHOD_PLAIN_VARIABLE) {
        ErrorStream eout;
        eout << setFunctionName( functionName)
             << "\n - Initial value for variable \"" << variableName_ << "\" is missing or invalid."
             << "\n - See DML file \"" << janusFile_->getXmlFileName() << "\".";
        throw runtime_error( eout.throwStr());
      }

      xmlValue_.clear();
      xmlValue_ = findUnits( variableDef_->getUnits());
      xmlValue_.setValue( value_);

      if ( ignoreUnits) return;

      // For performance reasons, if requiredValue is metric (and compatible) leave unit conversion to xmlValue itself
      if ( !requiredValue_.isEmpty()
        && isCompatible( requiredValue_, xmlValue_)
        && isZero( requiredValue_.scale() * requiredValue_.scaleToMetric() - 1.0) 
        && isZero( requiredValue_.offset())) {
        requiredValue_ = aUnits();
      }

      if ( doDeltaConversion_) {
        // This is mainly used for temperature deviations.
        xmlValue_.setOffset( 0);
        requiredValue_.setOffset( 0);
      }
      if ( requiredValue_.isEmpty()) {
        doUnitConversion_ = ( xmlValue_.scale() * xmlValue_.scaleToMetric() != 1 || xmlValue_.offset() != 0);
      }
      else if ( !requiredValue_.isCompatible( xmlValue_, false)) {
        throw_message( runtime_error,
          setFunctionName( functionName)
          << "\n - Units incompatible for variable \"" << variableName_ << "\"."
          << "\n - Required units compatible with \"" << requiredValue_.units() << "\" but got \"" << xmlValue_.units() << "\"."
          << "\n - See DML file \"" << janusFile_->getXmlFileName() << "\"."
        );
      }
      else {
        doUnitConversion_ = ( xmlValue_.units() != requiredValue_.units());
      }
      if ( doUnitConversion_) {
        if ( requiredValue_.isEmpty()) {
          value_ = xmlValue_.valueMetric();
        }
        else {
          value_ = convert( xmlValue_, requiredValue_);
        }
      }

      if ( dstomath::isnan( value_) &&
           ( variableType_ == janusInputVariable || variableType_ == janusInputOutputVariable)) {
        if ( !dstomath::isnan( initialValue)) {
          setValue( initialValue);
          errorMessage_  = variableName_ + ": Input seems to be nan in\n";
          errorMessage_ += janusFile_->getXmlFileName();
        }
        else {
          throw_message( runtime_error,
            setFunctionName( functionName)
            << "\n - Initial value for variable \"" << variableName_ << "\" is not valid."
            << "\n - See DML file \"" << janusFile_->getXmlFileName() << "\"."
          );
        }
      }

      return; // Found the variable.
    }
  }

  if ( isMandatory_ &&
       variableType_ != janusInputVariable) { // All input variables aren't mandatory.
    throw_message( runtime_error,
      setFunctionName( functionName)
      << "\"" << preferredName << "\" - Mandatory variable does not exist in \""
      << janusFile_->getXmlFileName() << "\""
    );
  }
  else {
    errorMessage_ = "\"" + preferredName + "\" - Variable does not exist in\n";
    errorMessage_ += janusFile_->getXmlFileName();
  }
}

const double& JanusVariable::value() const
{
  if ( isNotAvailable()) {
    return value_;
  }

  value_ = variableDef_->getValue();
  xmlValue_.setValue( value_);
  if ( doUnitConversion_) {
    if ( requiredValue_.isEmpty()) {
      value_ = xmlValue_.valueMetric();
    }
    else {
      value_ = convert( xmlValue_, requiredValue_);
    }
  }

  return value_;
}

const double& JanusVariable::valueOr( const double& defaultVal) const
{
  if ( isAvailable()) {
    return value();
  }
  return defaultVal;
}

dstoute::aOptionalDouble JanusVariable::optionalValue() const
{
  if ( isAvailable()) {
    return value();
  }
  return aOptionalDouble::invalidValue();
}

const aString& JanusVariable::stringValue() const
{
  if ( isNotAvailable() || (variableType_ != janusString)) {
    return EMPTY_STRING;
  }

  return variableDef_->getStringValue();
}

// Return true if value has changed.
bool JanusVariable::setValue( const double &val)
{
  if ( variableType_ == janusInputVariable || variableType_ == janusInputOutputVariable)  {
    bool hasChanged = ( val != value_);
    value_ = val;

    if ( isAvailable() && hasChanged) {
      xmlValue_.setValue( value_);
      if ( doUnitConversion_) {
        if ( requiredValue_.isEmpty()) {
          xmlValue_.setValueMetric( value_);
        }
        else {
          xmlValue_.setValue( convert( requiredValue_, xmlValue_, value_));
        }
      }
      variableDef_->setValue( xmlValue_.value());
    }

    return hasChanged;
  }

  return false;
}

int JanusVariable::toInt() const
{
  return int( dstomath::nearbyint( value()));
}

unsigned int JanusVariable::toUnsignedInt() const
{
  return (unsigned int)( dstomath::nearbyint( value()));
}

size_t JanusVariable::toSize_t() const
{
  return size_t( dstomath::nearbyint( value()));
}

float JanusVariable::toFloat() const
{
  return float( value());
}

bool JanusVariable::toBool() const
{
  return dstomath::isNotZero( value());
}

int JanusVariable::toIntOr( const int defaultValue) const
{
  if ( isAvailable()) {
    return toInt();
  }
  return defaultValue;
}

unsigned int JanusVariable::toUnsignedIntOr( const unsigned int defaultValue) const
{
  if ( isAvailable()) {
    return toUnsignedInt();
  }
  return defaultValue;
}

std::size_t JanusVariable::toSize_tOr( const size_t defaultValue) const
{
  if ( isAvailable()) {
    return toSize_t();
  }
  return defaultValue;
}

float JanusVariable::toFloatOr( const float& defaultValue) const
{
  if ( isAvailable()) {
    return toFloat();
  }
  return defaultValue;
}

bool JanusVariable::toBoolOr( const bool defaultValue) const
{
  if ( isAvailable()) {
    return toBool();
  }
  return defaultValue;
}

aString JanusVariable::toString() const
{
  ostringstream ostr;
  ostr << value();
  return ostr.str();
}

aString JanusVariable::getRevisionString() const
{
  aString revisionString;
  if ( isAvailable()) {
    revisionString = getDescription();
    //TODO: Do we still want to do something different to getDescription()?
    //size_t loc = revisionString.find_first_of( "0123456789.");
    //if ( loc != aString::npos) {
    //  revisionString = revisionString.substr( loc);
    //  loc = revisionString.find_last_of( "0123456789.");
    //  if ( loc != aString::npos) {
    //    revisionString = revisionString.substr( 0, loc + 1);
    //  }
    //}
  }

  return revisionString;
}
ostream& operator<< ( ostream &os, const JanusVariable &janusVariable)
{
  JanusVariable &jv = const_cast<JanusVariable&>( janusVariable);
  os << "---- JanusVariable ------------------" << endl;
  os << "Janus File              :" << jv.janusFile_ << endl;
  os << "Variable Name           :" << jv.variableName_ << endl;
  os << "Variable Def            :" << jv.variableDef_ << endl;
  os << "Variable Type           :" << jv.variableType_ << endl;
  os << "XML Units               :" << jv.xmlValue_.units() << endl;
  if ( jv.doUnitConversion_) {
    os << "Specific Output Units   :" << jv.requiredValue_.units() << endl;
  }
  os << "User Data               :" << jv.userData_ << endl;
  os << "isAvailable             :" << jv.isAvailable() << endl;
  os << "Value                   :" << jv.value() << endl;
  os << "ValueToMetric           :" << jv.xmlValue_.valueMetric() << endl;
  os << "ValueSI                 :" << jv.xmlValue_.valueSI() << endl;

  return os;
}

JanusVariableList::JanusVariableList( const dstoute::aStringList &variableNames, JanusVariableType variableType, bool isMand, const double &val)
{
  for ( auto& s : variableNames) {
    push_back( JanusVariable( s, variableType, isMand, val));
  }
}

JanusVariableList::JanusVariableList( const dstoute::aStringList &variableNames, JanusVariableType variableType, bool isMand, 
                                      const dstoute::aString &specificUnits, const double &val)
{
  for ( auto& s : variableNames) {
    push_back( JanusVariable( s, variableType, isMand, specificUnits, val));
  }
}

