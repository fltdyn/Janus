#ifndef _JANUSVARIABLE_H_
#define _JANUSVARIABLE_H_

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
 * Title:      Janus Variable Class
 * Class:      JanusVariable
 * Module:     JanusVariable.h
 * Reference:
 *
 * Description:
 *  This file contains the class prototypes for data elements that
 *  interact with Janus XML files.
 *  The associated code file for the class is: JanusVariable.cpp
 *
 * Written by: Geoff Brian (Geoff.Brian@dsto.defence.gov.au)
 *             Shane Hill  (Shane.Hill@dsto.defence.gov.au)
 *
 * Derived from: XmlDataElement class by Geoff Brian.
 *
 * First Date: 08/02/2010
 *
 */

// C++ Includes

// Janus Includes
#include "Janus.h"

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aMath.h>
#include <Ute/aUnits.h>
#include <Ute/aOptional.h>

// ----------------
// Class Definition
// ----------------

const double      JV_ZERO( 0);
const dstoute::aString JV_EMPTY_STRING;
const bool        janusMandatory( true);
const bool        janusRequired( true);

class JanusVariableManager;

enum JanusVariableType {
  janusOutputVariable,
  janusInputVariable,
  janusInputOutputVariable,
  janusDeltaOutputVariable,
  janusDeltaInputVariable,
  janusDeltaInputOutputVariable,
  janusIgnoreUnitsOutputVariable,
  janusIgnoreUnitsInputVariable,
  janusIgnoreUnitsInputOutputVariable,
  janusString
};

class JanusVariable
{
 public:
  JanusVariable();
  JanusVariable( int /* dummy */); //Needed for ValMatrix use.

  JanusVariable( janus::Janus &janusFile, const dstoute::aString &variableName, JanusVariableType variableType,
                 bool isMand, const double &val = 0);
  JanusVariable( janus::Janus &janusFile, const dstoute::aString &variableName, JanusVariableType variableType,
                 bool isMand, const dstoute::aString &specificUnits, const double &val = 0);

  JanusVariable( const dstoute::aString &variableName, JanusVariableType variableType,
                 bool isMand, const double &val = 0);
  JanusVariable( const dstoute::aString &variableName, JanusVariableType variableType,
                 bool isMand, const dstoute::aString &specificUnits, const double &val = 0);

  virtual ~JanusVariable() {}

  void setUserData( int uData)   { userData_ = uData;}
  int  userData() const          { return userData_;}

  bool isInitialised() const     { return  isInitialised_;}
  bool isNotInitialised() const  { return !isInitialised_;}
  bool isAvailable() const       { return ( variableDef_ != 0);}
  bool isNotAvailable() const    { return ( variableDef_ == 0);}
  bool isMissing() const         { return ( variableDef_ == 0);}
  bool isMandatory() const       { return  isMandatory_;}

  virtual const double& value() const;
  virtual const double& valueOr( const double& defaultVal) const;
  dstoute::aOptionalDouble optionalValue() const;
  const dstoute::aString& stringValue() const;

  virtual bool setValue( const double &val);

  int          toInt() const;
  unsigned int toUnsignedInt() const;
  std::size_t  toSize_t() const;
  float        toFloat() const;
  bool         toBool() const;
  
  int          toIntOr( const int defaultValue) const;
  unsigned int toUnsignedIntOr( const unsigned int defaultValue) const;
  std::size_t  toSize_tOr( const size_t defaultValue) const;
  float        toFloatOr( const float& defaultValue) const;
  bool         toBoolOr( const bool defaultValue) const;

  dstoute::aString toString() const;
  dstoute::aString getRevisionString() const;

  const dstoute::aString& errorMessage() const { return errorMessage_;}
  friend std::ostream& operator<< ( std::ostream &os, const JanusVariable &janusVariable);

  // Janus VariableDef functions.
  const dstoute::aString& getName()         const { return isAvailable() ? variableDef_->getName()         : JV_EMPTY_STRING;}
  const dstoute::aString& getVarID()        const { return isAvailable() ? variableDef_->getVarID()        : JV_EMPTY_STRING;}
  const dstoute::aString& getUnits()        const { return isAvailable() ? variableDef_->getUnits()        : JV_EMPTY_STRING;}
  const dstoute::aString& getAxisSystem()   const { return isAvailable() ? variableDef_->getAxisSystem()   : JV_EMPTY_STRING;}
  const dstoute::aString& getSign()         const { return isAvailable() ? variableDef_->getSign()         : JV_EMPTY_STRING;}
  const dstoute::aString& getAlias()        const { return isAvailable() ? variableDef_->getAlias()        : JV_EMPTY_STRING;}
  const dstoute::aString& getStringValue()  const { return isAvailable() ? variableDef_->getStringValue()  : JV_EMPTY_STRING;}
  const dstoute::aString& getSymbol()       const { return isAvailable() ? variableDef_->getSymbol()       : JV_EMPTY_STRING;}
  const double&           getInitialValue() const { return isAvailable() ? variableDef_->getInitialValue() : JV_ZERO;}
  const dstoute::aString& getDescription()  const { return isAvailable() ? variableDef_->getDescription()  : JV_EMPTY_STRING;}

  bool isOutput()        const { return isAvailable() ? variableDef_->isOutput()        : false;}
  bool isState()         const { return isAvailable() ? variableDef_->isState()         : false;}
  bool isStateDeriv()    const { return isAvailable() ? variableDef_->isStateDeriv()    : false;}
  bool isStdAIAA()       const { return isAvailable() ? variableDef_->isStdAIAA()       : false;}

  friend class JanusVariableManager;

 protected:
  void setJanusFile( janus::Janus *janusFile);
  void initialiseVariable();

  janus::Janus       *janusFile_;
  janus::VariableDef *variableDef_;
  dstoute::aString         variableName_;
  JanusVariableType   variableType_;
  int                 userData_;

  mutable double value_;
  mutable dstoute::aUnits xmlValue_;
  dstoute::aUnits         requiredValue_;

  bool isInitialised_;
  bool isMandatory_;
  bool doUnitConversion_;
  bool doDeltaConversion_;

 private:
  dstoute::aString errorMessage_;
};

extern JanusVariable EMPTY_JANUSVARIABLE;

class JanusVariableList : public std::vector<JanusVariable>
{
public:
  JanusVariableList( const dstoute::aStringList &variableNames, JanusVariableType variableType, bool isMand, const double &val = 0);
  JanusVariableList( const dstoute::aStringList &variableNames, JanusVariableType variableType, bool isMand, 
                     const dstoute::aString &specificUnits, const double &val = 0);
};

#endif /* _JANUSVARIABLE_H_ */
