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
// Title:      Janus/SignalDef
// Class:      SignalDef
// Module:     SignalDef.cpp
// First Date: 2017-09-08
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file SignalDef.cpp
 *
 * A SignalDef instance holds in its allocated memory alphanumeric data
 * derived from a \em signalDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance may describe
 * inputs, internal values of a computation, or outputs.  The class also
 * provides the functions that allow a calling StaticShot instance to access
 * these data elements. It is used to document the name, ID, value, tolerance,
 * and units of measure for checkcases.
 *
 * A \em signalDef must have attributes of a \em name, an \em ID and \em units.
 * An optional \em symbol attribute may also be defined.
 *
 * Additionally, a reference to a variable definition must be provided, together
 * with a type tag (either \em sigInput, \em sigInternal, or \em sigOutput) to
 * identify the what check part of a \em staticShot the signal is associated with.
 * The value of the signal must also be provided, either as a scaler or an array
 * of data using the \em signalValue sub-element. An optional signal
 * \em description may be provided
 *
 * The SignalDef class is only used within the janus namespace, and should
 * only be referenced indirectly through the StaticShot, CheckInputs,
 * InternalValues and CheckOutputs classes.
 */

/*
 * Author:  G. Brian
 */

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aMath.h>

// Local Includes
#include "JanusConstants.h"
#include "DomFunctions.h"
#include "SignalDef.h"

using namespace std;
using namespace dstoute;
using namespace dstomath;

namespace janus
{

//------------------------------------------------------------------------//

  SignalDef::SignalDef() :
    XmlElementDefinition(),
    name_( EMPTY_STRING),
    units_( EMPTY_STRING),
    sigID_( EMPTY_STRING),
    symbol_( EMPTY_STRING),
    isSigInput_( false),
    isSigInternal_( false),
    isSigOutput_( false),
    varID_( EMPTY_STRING),
    varIndex_(),
    description_( EMPTY_STRING)
  {
  }

  SignalDef::SignalDef(
    const DomFunctions::XmlNode& elementDefinition,
    const SignalTypeEnum &signalType) :
    XmlElementDefinition(),
    name_( EMPTY_STRING),
    units_( EMPTY_STRING),
    sigID_( EMPTY_STRING),
    symbol_( EMPTY_STRING),
    isSigInput_( false),
    isSigInternal_( false),
    isSigOutput_( false),
    varID_( EMPTY_STRING),
    varIndex_(),
    description_( EMPTY_STRING)
  {
    initialiseDefinition( elementDefinition, signalType);
  }

//------------------------------------------------------------------------//

  void SignalDef::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition,
    const SignalTypeEnum &signalType)
  {
    static const aString functionName( "SignalDef::initialiseDefinition()");

    /*
     * Retrieve attributes for the element's Definition
     */
    name_  = DomFunctions::getAttribute( elementDefinition, "name", true);
    units_ = DomFunctions::getAttribute( elementDefinition, "units", true);
    try {
      sigID_ = DomFunctions::getAttribute( elementDefinition, "sigID", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - the \"sigID\" attribute has not be defined for SignalDef.\n - "
        << excep.what()
      );
    }
    symbol_ = DomFunctions::getAttribute( elementDefinition, "symbol");

    /*
     * Retrieve the description associated with the element
     */
    description_ = DomFunctions::getChildValue( elementDefinition,
      "description");

    /*
     * Retrieve the reference to the associated variableDef
     */
    elementType_ = ELEMENT_VARIABLE;
    try {
      DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING,
        "variableDef", "variableRef", "varID", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Error initialising the \"variableRef\" element for the signalDef.\n - "
        << excep.what()
      );
    }

    /*
     * Check that !varID_.empty() or varIndex_ != INVALID_INDEX
     * if they do then throw error that variableRef not initialised correctly.
     */
    if ( varID_.empty()) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Error \"variableRef\" element - " << varID_
        << " for the \"signalDef\""
        << " does NOT match any defined \"variableDef\" elements"
      );
    }

    /*
     * Retrieve and check the signal type
     */
    isSigInput_    = DomFunctions::isChildInNode( elementDefinition, "sigInput");
    isSigInternal_ = DomFunctions::isChildInNode( elementDefinition, "sigInternal");
    isSigOutput_   = DomFunctions::isChildInNode( elementDefinition, "sigOutput");

    bool isSigCheck = (isSigInput_ ^ isSigInternal_);
    if (( isSigCheck & !isSigOutput_) || ( !isSigCheck & isSigOutput_)) {
      if (( isSigInput_) && (signalType != SIGNAL_CHECKINPUTS) &&
          (signalType != SIGNAL_NOSET)) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - Error Input signal type mismatch for the signalDef -"
          << name_ << ".\n - "
        );
      }

      elementType_ = ELEMENT_SIGNAL;
      if (( isSigInternal_) && (signalType != SIGNAL_INTERNALVALUES) &&
          (signalType != SIGNAL_NOSET)) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - Error Internal value signal type mismatch for the signalDef -"
          << name_ << ".\n - "
        );
      }
      else {
        try {
          DomFunctions::initialiseChild(
            this, elementDefinition, EMPTY_STRING, "sigInternal", false);
        }
        catch ( exception &excep) {
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - Error initialising the \"sigInternal\" element for the signalDef - "
            << name_ << ".\n - "
            << excep.what()
          );
        }
      }

      if (( isSigOutput_) && (signalType != SIGNAL_CHECKOUTPUTS) &&
          (signalType != SIGNAL_NOSET)) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - Error Output signal type mismatch for the signalDef -"
          << name_ << ".\n - "
        );
      }
      else {
        try {
          DomFunctions::initialiseChild(
            this, elementDefinition, EMPTY_STRING, "sigOutput", false);
        }
        catch ( exception &excep) {
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - Error initialising the \"sigOutput\" element for the signalDef - "
            << name_ << ".\n - "
            << excep.what()
          );
        }
      }
    }
    else {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Error Multiple signal types specified for the signalDef -"
        << name_ << ".\n - "
      );
    }

    /*
     * Retrieve and convert the signal data -
     *
     * ==========
     * Cross checking the data with the variableRef to check if the
     * associated variableDef is either a scalar, vector or matrix, and then
     * verify that sufficient entries in the values_ table are present depending
     * on the variableDef type is done in StaticShot.cpp.
     * ==========
     *
     */
    elementType_ = ELEMENT_DATATABLE;
    try {
      DomFunctions::initialiseChild(
        this, elementDefinition, EMPTY_STRING, "signalValue", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Error initialising the \"signalValue\" element for the signalDef - "
        << name_ << ".\n - "
        << excep.what()
      );
    }

    // Set the size of the actual value data list. This is the same as 'value_'
    actualValue_.resize( value_.size());
  }

//------------------------------------------------------------------------//
  void SignalDef::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the SignalDef element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild(
      documentElement, "signalDef");

    /*
     * Add attributes to the BreakpointDef child
     */
    if ( !name_.empty()) {
      DomFunctions::setAttribute( childElement, "name", name_);
    }

    DomFunctions::setAttribute( childElement, "sigID", sigID_);

    if ( !units_.empty()) {
      DomFunctions::setAttribute( childElement, "units", units_);
    }

    if ( !symbol_.empty()) {
      DomFunctions::setAttribute( childElement, "symbol", symbol_);
    }

    /*
     * Add description element
     */
    if ( !description_.empty()) {
      DomFunctions::setChild( childElement, "description", description_);
    }

    /*
     * Add signal type element
     */
    if ( isSigInput_) {
      DomFunctions::setChild( childElement, "sigInput");
    }
    else if ( isSigInternal_) {
      DomFunctions::XmlNode subChildElement =
        DomFunctions::setChild( childElement, "sigInternal");
      aString tolValuesStr = dataToString( tol_);
      if ( !tolValuesStr.empty()) {
        DomFunctions::setAttribute( subChildElement, "tol", tolValuesStr);
      }
    }
    else if ( isSigOutput_) {
      DomFunctions::XmlNode subChildElement =
        DomFunctions::setChild( childElement, "sigOutput");
      aString tolValuesStr = dataToString( tol_);
      if ( !tolValuesStr.empty()) {
        DomFunctions::setAttribute( subChildElement, "tol", tolValuesStr);
      }
    }

    /*
     * Add signal data
     */
    aString signalValuesStr = dataToString( value_);
    DomFunctions::setChild( childElement, "signalValue", signalValuesStr);
  }

  void SignalDef::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    static const aString functionName(
      "SignalDef::readDefinitionFromDom()");

    aStringList dataTableStr;
    switch ( elementType_) {
      case ELEMENT_SIGNAL:
        dataTableStr =
          DomFunctions::getAttribute( xmlElement, "tol").toStringList( JANUS_DELIMITERS);
        tol_ = stringToData( dataTableStr);
        break;

      case ELEMENT_DATATABLE:
        dataTableStr = DomFunctions::getCData( xmlElement).toStringList( JANUS_DELIMITERS);
        value_ = stringToData( dataTableStr);
        break;

      default:
        break;
    }

    return;
  }

  bool SignalDef::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& documentElementReferenceIndex)
  {
    aString crossRefElementId;

    switch ( elementType_) {
    case ELEMENT_VARIABLE:
      if ( DomFunctions::getAttribute( xmlElement, "varID") != elementID) {
        return false;
      }
      varID_    = elementID;
      varIndex_ = documentElementReferenceIndex;
      break;

    default:
      return false;
      break;
    }

//    readDefinitionFromDom( xmlElement);
    return true;
  }

//------------------------------------------------------------------------//

  bool SignalDef::isCheckValid( void)
  {
    bool valid = true;

    size_t tolSize   = tol_.size();
    size_t valueSize = value_.size();

    double tolCheck  = dstomath::zero();
    if ( tolSize > 0) {
      tolCheck = tol_[0];
    }

    for ( size_t i = 0; i < valueSize; ++i) {
      if ( tolSize == valueSize) {
        tolCheck = tol_[i];
      }

      if ( std::fabs( value_[i] - actualValue_[i]) > tolCheck) {
        valid = false;
        break;
      }
    }

    return valid;
  }

//------------------------------------------------------------------------//

  aDoubleList SignalDef::stringToData( const aStringList &dataStr)
  {
    aDoubleList dataValues;
    aString dataEntry;
    for ( size_t i = 0; i < dataStr.size(); ++i) {
      dataEntry = dataStr.at(i);

      if ( dataEntry.isNumeric()) {
        dataValues.push_back( dataEntry.toDouble());
      }
    }

    return dataValues;
  }

  aString SignalDef::dataToString( const aDoubleList &dataTable)
  {
    aString dataTableStr;
    for ( size_t i = 0; i < dataTable.size(); ++i) {
      dataTableStr += aString( "%").arg( dataTable[ i]);
      dataTableStr += ",";
    }

    return dataTableStr;
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const SignalDef &signalDef)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl
       << "Display SignalDef contents:" << endl
       << "-----------------------------------" << endl;

    os << "  name               : " << signalDef.getName() << endl
       << "  sigID              : " << signalDef.getSigID() << endl
       << "  units              : " << signalDef.getUnits() << endl
       << "  symbol             : " << signalDef.getSymbol() << endl
       << "  description        : " << signalDef.getDescription() << endl
       << endl;

    os << "  sigInput           : " << signalDef.isSigInput() << endl
       << "  sigInternal        : " << signalDef.isSigInternal() << endl
       << "  sigOutput          : " << signalDef.isSigOutput() << endl;


   /*
    * Tolerance Data associated with sigInternal or sigOutput sub-elements
    */
    if ( signalDef.isSigInternal() || signalDef.isSigOutput()) {
      os << "  tol_               : " << endl;

      aDoubleList dataTable = signalDef.getTolerance();
      for (size_t i = 0; i < dataTable.size(); ++i) {
        os << dataTable.at(i) << " , "
           << endl;
      }
    }

    /*
     * Data associated with Class
     */
    os << "  signalValue        : " << endl;

    aDoubleList dataTable = signalDef.getValue();
    for (size_t i = 0; i < dataTable.size(); ++i) {
      os << dataTable.at(i) << " , "
         << endl;
    }

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus
