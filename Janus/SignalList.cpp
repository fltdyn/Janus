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
// Title:      Janus/SignalList
// Class:      SignalList
// Module:     SignalList.cpp
// First Date: 2017-09-06
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file SignalList.cpp
 *
 * A SignalList instance behaves as a container for a list of Signal definition
 * elements (either signalDef or signalRef), which provide the properties of
 * signal elements associated with checkInputs, internalValues, and checkOutputs
 * elements of StaticShot instances.
 *
 * The SignalList class is only used within the \em janus namespace, and is
 * inherited by the CheckInputs, InternalValues and CheckOutputs classes. It
 * should only be referenced indirectly through the StaticShot class.
 */

/*
 * Author:  G.J. Brian
 */

// Ute Includes
#include <Janus/SignalList.h>
#include <Ute/aMessageStream.h>
#include <Ute/aOptional.h>

// local includes
#include "DomFunctions.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

SignalList::SignalList()
  :
  XmlElementDefinition(),
  janus_(0),
  elementType_( ELEMENT_SIGNAL),
  signalType_( SIGNAL_CHECKINPUTS)
{
}

SignalList::SignalList(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType,
  Janus* janus)
  :
  XmlElementDefinition(),
  janus_(0),
  elementType_( ELEMENT_SIGNAL),
  signalType_( SIGNAL_CHECKINPUTS)
{
  initialiseDefinition( elementDefinition, signalType, janus);
}

//------------------------------------------------------------------------//

void SignalList::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType,
  Janus* janus)
{
  static const aString functionName( "SignalList::initialiseDefinition()");
  janus_       = janus;
  signalType_  = signalType;

  /*
   * Retrieve to signalDefs and signalRefs associated with the signalList
   */
  elementType_ = ELEMENT_SIGNAL;
  try {
    DomFunctions::initialiseChildrenOrRefs( this,
                                            elementDefinition,
                                            EMPTY_STRING,
                                            "signalDef",
                                            EMPTY_STRING,
                                            "signalRef",
                                            "sigID",
                                            true);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Error initialising \"signalList\" elements.\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//

void SignalList::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the SignalList element
   */

  DomFunctions::XmlNode childElement;
  childElement = DomFunctions::setChild( documentElement, "signalList");

  /*
   * Add signalRefs to the signalList child
   */
  DomFunctions::XmlNode signalRefElement;
  for ( size_t i = 0; i < signalRef_.size(); i++) {
    if ( !signalRef_[i].empty()) {
      signalRefElement = DomFunctions::setChild( childElement, "signalRef");
      DomFunctions::setAttribute( signalRefElement, "sigID",
                                  signalRef_[i]);
    }
  }
}

void SignalList::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName(
    "SignalList::readDefinitionFromDom()");

  janus_->getSignalDef().push_back( SignalDef( xmlElement, signalType_));

  aString sigIDStr = janus_->getSignalDef().back().getSigID();
  size_t n_sigID   = janus_->isUniqueID( elementType_, sigIDStr) - 1;

  if ( n_sigID) {
    sigIDStr = aString( "%_%").arg( sigIDStr).arg(n_sigID);
    janus_->getSignalDef().back().setSigID( sigIDStr);
  }
  signalRef_.push_back( sigIDStr);
  return;
}

bool SignalList::compareElementID(
  const DomFunctions::XmlNode& xmlElement,
  const aString& elementID,
  const size_t& /* documentElementReferenceIndex */)
{
  static const aString functionName(
    "SignalList::compareElementID()");

  switch ( elementType_) {
  case ELEMENT_SIGNAL:
    if ( DomFunctions::getAttribute( xmlElement, "sigID") != elementID) {
      return false;
    }
    signalRef_.push_back( elementID);
    break;

  default:
    return false;
    break;
  }

  return true;
}

//------------------------------------------------------------------------//

void SignalList::resetJanus( Janus* janus)
{
  janus_ = janus;
}

//------------------------------------------------------------------------//

void SignalList::subList( const size_t& begin, const size_t& end,
                          SignalList* signalList)
{
  aStringList signalRef;

  signalList->resetJanus( janus_);

//  std::copy( signalRef_.begin(), signalRef_.end(), signalRef);
  for ( size_t i = begin; i < end; ++i) {
    signalRef.push_back( signalRef_.at(i));
  }
  signalList->setSignalRef( signalRef);
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const SignalList &signalList)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display SignalList_v2 contents:" << endl
     << "-----------------------------------" << endl;

  /*
   * SignalRef data associated with Class
   */
  const aStringList& signalRef = signalList.getSignalRef();
  for ( size_t i = 0; i < signalRef.size(); ++i) {
    os << "  signalRef " << i << endl;
    os << "  sigID :" << signalRef[i] << endl;
  }

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus


