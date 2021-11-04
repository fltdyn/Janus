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
// Title:      Janus/Signals
// Class:      Signals
// Module:     Signals.cpp
// First Date: 2011-11-04
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Signals.cpp
 *
 * A Signals instance functions as a container for the Signal class, and
 * provides the functions that allow a calling StaticShot instance to access
 * the \em signal elements that define either the input or output values for
 * a check case. A \em signal element contains a list parameters including:
   * \em signalName, \em signalUnits, and \em signalValue elements. An
   * optional \em tol element may be included.
   *
   *
 *  A \em signals element contains a list of \em signal elements,
 * that include \em signalName, \em signalUnits, \em signalValue elements. An
 * optional \em tol element may be included.
 *
 * The Signals class is only used within the janus namespace, and is inherited
 * by the CheckInputs and CheckOutputs classes. It should only be referenced
 * indirectly through the StaticShot class.
 */

/*
 * Author:  G.J. Brian
 *
 * Modified :  S. Hill
 *
 */

// Ute Includes
#include <Janus/Signals.h>
#include <Ute/aMessageStream.h>

#include "DomFunctions.h"

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

Signals::Signals()
  :
  XmlElementDefinition(),
  elementType_( ELEMENT_SIGNAL),
  signalType_( SIGNAL_CHECKINPUTS)
{
}

Signals::Signals(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType )
  :
  XmlElementDefinition(),
  elementType_( ELEMENT_SIGNAL),
  signalType_( SIGNAL_CHECKINPUTS)
{
  initialiseDefinition( elementDefinition, signalType);
}

//------------------------------------------------------------------------//

void Signals::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType)
{
  static const aString functionName( "Signals::initialiseDefinition()");

  // One or more Signal elements
  signalType_  = signalType;
  elementType_ = ELEMENT_SIGNAL;
  try {
    DomFunctions::initialiseChildren( this,
                                      elementDefinition,
                                      EMPTY_STRING,
                                      "signal",
                                      true);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//

int Signals::getIndex( const aString &name ) const
{
  int iRet = -1;
  for ( int i = 0 ; i < (int)signal_.size() ; i++ ) {
    if ( name == signal_[ i ].getName() ) {
      iRet = i;
      break;
    }
  }
  return iRet;
}

//------------------------------------------------------------------------//

void Signals::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
//  /*
//   * Create a child node in the DOM for the SignalList elements
//   * (checkInputs, internalValues, checkOutputs)
//   */
//
//  DomFunctions::XmlNode childElement;
//
//  switch ( signalType_) {
//    case SIGNAL_CHECKINPUTS:
//      childElement = DomFunctions::setChild( documentElement, "checkInputs");
//      break;
//
//    case SIGNAL_CHECKOUTPUTS:
//      childElement = DomFunctions::setChild( documentElement, "checkOutputs");
//      break;
//
//    case SIGNAL_INTERNALVALUES:
//      childElement = DomFunctions::setChild( documentElement, "internalValues");
//      break;
//  }

  /*
   * Add signals to the signal list child
   */
  for ( size_t i = 0; i < getSignalCount(); i++) {
    signal_.at(i).exportDefinition( documentElement);
  }
}

void Signals::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  signal_.push_back( Signal( xmlElement, signalType_));
  return;
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  Signals &signals)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display Signals contents:" << endl
     << "-----------------------------------" << endl;

  /*
   * Data associated with Class
   */
  vector<Signal>& signal = signals.getSignal();
  for ( size_t i = 0; i < signal.size(); i++) {
    os << "  signal " << i << endl;
    os << signal.at(i) << endl;
  }

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus


