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
// Title:      Janus/CheckSignal
// Class:      CheckSignal
// Module:     CheckSignal.cpp
// First Date: 2017-09-07
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file CheckSignal.h
 *
 * The CheckSignal class is only used within the janus namespace, and should
 * only be referenced indirectly through the CheckInputs, InternalValues and
 * CheckOutputs classes.
 */

/*
 * Author:  G. Brian
 */

// Ute Includes
#include <Ute/aMessageStream.h>

// Local Includes
#include "DomFunctions.h"
#include "CheckSignal.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace janus
{
//------------------------------------------------------------------------//

CheckSignal::CheckSignal()
  :
  XmlElementDefinition(),
  janus_ (0),
  signalType_( SIGNAL_CHECKINPUTS),
  hasSignalList_(false),
  hasSignals_(false)
{
}

CheckSignal::CheckSignal(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType,
  Janus* janus )
  :
  XmlElementDefinition(),
  janus_ (0),
  signalType_( SIGNAL_CHECKINPUTS),
  hasSignalList_(false),
  hasSignals_(false)
{
  initialiseDefinition( elementDefinition, signalType, janus);
}

//------------------------------------------------------------------------//

void CheckSignal::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType,
  Janus* janus )
{
  static const aString functionName( "CheckSignal::initialiseDefinition()");
  janus_       = janus;
  signalType_  = signalType;

  bool hasSignalList = DomFunctions::isChildInNode( elementDefinition, "signalList");
  bool hasSignals    = DomFunctions::isChildInNode( elementDefinition, "signal");

  if (!hasSignalList && !hasSignals) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Check Signal Type \"" << signalType << "\" "
      << "requires at least 1 \"" << "SignalList or Signal"
      << "\" element."
    );
  }

  if ( hasSignalList) {
    try {
      DomFunctions::initialiseChild(
          this,
          elementDefinition,
          EMPTY_STRING,
          "signalList",
          false);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
                     setFunctionName( functionName)
                     << "\n - "
                     << excep.what()
      );
    }
    hasSignalList_ = true;
  }
  else {
    signals_.initialiseDefinition( elementDefinition, signalType);
    hasSignals_ = true;
  }
}

void CheckSignal::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the SignalList/Signal elements
   * (checkInputs, internalValues, checkOutputs)
   */

  DomFunctions::XmlNode childElement;

  switch ( signalType_) {
    case SIGNAL_CHECKINPUTS:
      childElement = DomFunctions::setChild( documentElement, "checkInputs");
      break;

    case SIGNAL_CHECKOUTPUTS:
      childElement = DomFunctions::setChild( documentElement, "checkOutputs");
      break;

    case SIGNAL_INTERNALVALUES:
      childElement = DomFunctions::setChild( documentElement, "internalValues");
      break;

    default:
      break;
  }

  if ( hasSignalList_) {
    signalList_.exportDefinition( childElement);
  }

  if ( hasSignals_) {
    signals_.exportDefinition( childElement);
  }
}

void CheckSignal::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName(
    "CheckSignal::readDefinitionFromDom()");

  signalList_.initialiseDefinition( xmlElement, signalType_, janus_);
  return;
}


//------------------------------------------------------------------------//

void CheckSignal::resetJanus( Janus* janus)
{
  janus_ = janus;
  signalList_.resetJanus( janus);
}
//------------------------------------------------------------------------//

ostream& operator<< (
    ostream &os,
    CheckSignal &checkSignal)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display CheckSignal contents:" << endl
     << "-----------------------------------" << endl;
  os << "  Check Signal Type  : " << checkSignal.getSignalType() << endl
     << "  (Input, Output, Internal)" << endl
     << "  hasSignalList      : " << checkSignal.hasSignalList() << endl
     << "  hasSignals         : " << checkSignal.hasSignals() << endl;

  if ( checkSignal.hasSignalList()) {
    os << endl
       << "   Signal List" << endl
       << "   -----------" << endl
       << checkSignal.getSignalList() << endl;
  }

  if ( checkSignal.hasSignals()) {
    os << endl
       << "   Signals" << endl
       << "   -----------" << endl
       << checkSignal.getSignals() << endl;
  }

  return os;
}
//------------------------------------------------------------------------//

} // namespace janus
