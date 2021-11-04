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
// Title:      Janus/CheckData
// Class:      CheckData
// Module:     CheckData.cpp
// First Date: 2011-12-05
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file CheckData.cpp
 *
 * Check data is used for XML dataset content verification.
 * A CheckData instance holds in its allocated memory alphanumeric data
 * derived from a \em checkData element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It will include static
 * check cases, trim shots, and dynamic check case information.  At present
 * only static check cases are implemented, using \em staticShot children of the
 * top-level \em checkData element.  The functions within this class provide
 * access to the raw check data, as well as actually performing whatever
 * checks may be done on the dataset using the \em checkData.
 *
 * The CheckData class is only used within the janus namespace, and should
 * normally only be referenced through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

#include <Ute/aMessageStream.h>

#include "DomFunctions.h"
#include "CheckData.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

  CheckData::CheckData() :
    XmlElementDefinition(),
    janus_( 0),
    elementType_( ELEMENT_NOTSET),
    hasProvenance_( false)
  {
  }

  CheckData::CheckData(
    const DomFunctions::XmlNode& elementDefinition,
    Janus* janus) :
    XmlElementDefinition(),
    janus_( 0),
    elementType_( ELEMENT_NOTSET),
    hasProvenance_( false)
  {
    initialiseDefinition( elementDefinition, janus);
  }

//------------------------------------------------------------------------//

  void CheckData::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition,
    Janus* janus)
  {
    static const aString functionName( "CheckData::initialiseDefinition()");
    janus_ = janus;

    /*
     * Retrieve the optional Provenance associated with the element
     */
    elementType_ = ELEMENT_PROVENANCE;
    try {
      DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING,
        "provenance", "provenanceRef", "provID", false);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Function without \"provenanceRef\" element.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve one or more Static Shot elements
     */
    elementType_ = ELEMENT_STATICSHOT;
    try {
      DomFunctions::initialiseChildren( this, elementDefinition, EMPTY_STRING,
        "staticShot", true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Function without \"staticShot\" elements.\n - "
        << excep.what()
      );
    }
  }

//------------------------------------------------------------------------//
  void CheckData::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the CheckData element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild(
      documentElement, "checkData");

    if ( hasProvenance_) {
      provenance_.exportDefinition( childElement);
    }

    for ( size_t i = 0; i < staticShot_.size(); i++) {
      staticShot_[ i].exportDefinition( childElement);
    }
  }

  void CheckData::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    switch ( elementType_) {
    case ELEMENT_STATICSHOT:
      staticShot_.push_back( StaticShot( xmlElement, janus_));
      break;

    case ELEMENT_PROVENANCE:
      provenance_.initialiseDefinition( xmlElement);
      hasProvenance_ = true;
      break;

    default:
      break;
    }

    return;
  }

  bool CheckData::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& /*documentElementReferenceIndex*/)
  {
    aString provId = DomFunctions::getAttribute( xmlElement, "provID");

    if ( provId != elementID) {
      return false;
    }

    readDefinitionFromDom( xmlElement);
    return true;
  }

//------------------------------------------------------------------------//

  void CheckData::resetJanus( Janus* janus)
  {
    janus_ = janus;

    for ( size_t i = 0; i < staticShot_.size(); ++i) {
      staticShot_.at(i).resetJanus( janus_);
    }
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const CheckData &checkData)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display CheckData contents:" << endl
      << "-----------------------------------" << endl;

    os << "  hasProvenance      : " << checkData.hasProvenance() << endl
      << endl;

    /*
     * Data associated with Class
     */
    if ( checkData.hasProvenance()) {
      os << checkData.getProvenance() << endl;
    }

//  vector<StaticShot>& staticShot = checkData.getStaticShot();
    const vector< StaticShot>& staticShot = checkData.staticShot_;
    for ( size_t i = 0; i < staticShot.size(); i++) {
      os << "  Static Shot " << i << endl;
      os << staticShot[ i] << endl;
    }

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus
