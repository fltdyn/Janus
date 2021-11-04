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
// Title:      Janus/Uncertainty
// Class:      Uncertainty
// Module:     Uncertainty.cpp
// First Date: 2008-07-07
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Uncertainty.cpp
 *
 * A Uncertainty instance holds in its allocated memory alphanumeric data
 * derived from a \em uncertainty element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The element is used in
 * function and parameter definitions to describe statistical variance in the
 * possible value of that function or parameter value. Only Gaussian
 * (normal) or uniform distributions of continuous random variable
 * distribution functions are supported.  The class also provides the functions
 * that allow a calling Janus instance to access these data elements.
 *
 * The Uncertainty class is only used within the janus namespace, and should
 * only be referenced indirectly through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Ute Includes
#include <Ute/aBiMap.h>
#include <Ute/aMessageStream.h>
#include <Ute/aMath.h>
#include <Ute/aOptional.h>

// Local Includes
#include "Janus.h"
#include "DomFunctions.h"
#include "Uncertainty.h"

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

const aBiMap< aString, int > Uncertainty::uncertaintyAttributesMap = {
  { "additive",       ADDITIVE_UNCERTAINTY       },
  { "multiplicative", MULTIPLICATIVE_UNCERTAINTY },
  { "percentage",     PERCENTAGE_UNCERTAINTY     },
  { "absolute",       ABSOLUTE_UNCERTAINTY       },
  { "unknown",        UNKNOWN_UNCERTAINTY        }
};

//------------------------------------------------------------------------//

Uncertainty::Uncertainty( )
  :
  XmlElementDefinition(),
  janus_( 0),
  elementType_(ELEMENT_UNIFORMPDF),
  effect_( UNKNOWN_UNCERTAINTY ),
  pdf_( UNKNOWN_PDF ),
  isSet_( false ),
  numSigmas_( 0 )
{
}


Uncertainty::Uncertainty(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
  :
  XmlElementDefinition(),
  janus_( janus),
  elementType_(ELEMENT_UNIFORMPDF),
  effect_( UNKNOWN_UNCERTAINTY ),
  pdf_( UNKNOWN_PDF ),
  isSet_( false ),
  numSigmas_( 0 )
{
  initialiseDefinition( janus, elementDefinition);
}

//------------------------------------------------------------------------//

void Uncertainty::initialiseDefinition(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
{
  static const aString functionName( "Uncertainty::initialiseDefinition()");

  janus_ = janus;

  /*
   * Retrieve the Uncertainty effect attribute
   */
  aString attributeStr = DomFunctions::getAttribute( elementDefinition, "effect");
  if ( attributeStr.empty()) {
    attributeStr = uncertaintyAttributesMap[ UNKNOWN_UNCERTAINTY];
  }
  if ( uncertaintyAttributesMap.find( attributeStr) == uncertaintyAttributesMap.end()) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - invalid \"uncertainty effect\" attribute \"" << attributeStr << "\"."
    );
  }
  effect_ = UncertaintyEffect( uncertaintyAttributesMap[ attributeStr]);


  /*
   * Retrieve the element attributes
   */
//  aString effectAttribute = DomFunctions::getAttributeFromDom( uncertaintyElement, "effect");
//  if ( effectAttribute == "additive" ) {
//    effect_ = ADDITIVE_UNCERTAINTY;
//  }
//  else if ( effectAttribute == "multiplicative" ) {
//    effect_ = MULTIPLICATIVE_UNCERTAINTY;
//  }
//  else if ( effectAttribute == "percentage" ) {
//    effect_ = PERCENTAGE_UNCERTAINTY;
//  }
//  else if ( effectAttribute == "absolute" ) {
//    effect_ = ABSOLUTE_UNCERTAINTY;
//  }

  /*
   * Retrieve the normalPDF | uniformPDF
   */
  bool isNormalPdf  = DomFunctions::isChildInNode( elementDefinition, "normalPDF");
  bool isUniformPdf = DomFunctions::isChildInNode( elementDefinition, "uniformPDF");
  
  if ( !isNormalPdf && !isUniformPdf) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - Uncertainty element"
      << "\" does not have either a normalPDF or uniformPDF child."
    );
  }
  
  isSet_ = true;
  if ( isNormalPdf) { // if normalPDF
    pdf_ = NORMAL_PDF;
    elementType_ = ELEMENT_NORMALPDF;
    try {
      DomFunctions::initialiseChild( this,
                                     elementDefinition,
                                     EMPTY_STRING,
                                     "normalPDF",
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
  else { // else uniformPDF
    pdf_ = UNIFORM_PDF;
    elementType_ = ELEMENT_UNIFORMPDF;
    try {
      DomFunctions::initialiseChild( this,
                                     elementDefinition,
                                     EMPTY_STRING,
                                     "uniformPDF",
                                     true);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - "
        << excep.what()
      );
    }
	
    // symmetric or asymmetric bounds check
    if ( 0 == bounds_.size() || bounds_.size() > 2) {
      throw_message( range_error,
        setFunctionName( functionName)
        << "\n - Uncertainty element"
        << "\" has uniform PDF with incorrect bounds."
      );
    }
  }
}

//------------------------------------------------------------------------//

void Uncertainty::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the Uncertainty element
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "uncertainty");

  /*
   * Add attributes to the child
   */
  DomFunctions::setAttribute( childElement, "effect",
                              uncertaintyAttributesMap[ effect_]);

  /*
   * Add PDF element to the Uncertainty child
   */
  if (pdf_ == NORMAL_PDF) {
    DomFunctions::XmlNode normalPDFElement =
      DomFunctions::setChild( childElement, "normalPDF");

    DomFunctions::setAttribute( normalPDFElement, "numSigmas",
                                aString("%").arg( numSigmas_));

    for ( size_t i = 0; i < bounds_.size(); i++) {
      bounds_[i].exportDefinition( normalPDFElement);
    }

    DomFunctions::XmlNode correlatesWithElement;
    for ( size_t i = 0; i < correlationVarIdList_.size(); i++) {
      correlatesWithElement = DomFunctions::setChild( childElement, "correlatesWith");
      DomFunctions::setAttribute( correlatesWithElement, "varID", correlationVarIdList_[i]);
    }

    DomFunctions::XmlNode correlationElement;
    for ( size_t i = 0; i < correlationVarIdList_.size(); i++) {
      correlationElement = DomFunctions::setChild( childElement, "correlation");
      DomFunctions::setAttribute( correlationElement, "varID",
          janus_->getVariableDef( correlation_[i].first).getVarID());
      DomFunctions::setAttribute( correlationElement, "corrCoef",
                                  aString("%").arg( correlation_[i].second));
    }
  }
  else { // Uniform PDF
    DomFunctions::XmlNode uniformPDFElement =
      DomFunctions::setChild( childElement, "uniformPDF");

    for ( size_t i = 0; i < bounds_.size(); i++) {
      bounds_[i].exportDefinition( uniformPDFElement);
    }
  }
}

void Uncertainty::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement )
{
  static const aString functionName( "Uncertainty::readDefinitionFromDom()");

  aString varId;
  aString numSigmasStr;
  aString corrCoef;
  size_t  pairIndex;
  aOptionalSizeT varIndex;

  switch ( elementType_) {
    case ELEMENT_UNIFORMPDF:
      elementType_ = ELEMENT_BOUNDS;
      try {
        DomFunctions::initialiseChildren( this,
                                          xmlElement,
                                          EMPTY_STRING,
                                          "bounds",
                                          true);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
        );
      }
      break;

    case ELEMENT_BOUNDS:
      bounds_.push_back( Bounds( janus_, xmlElement));
      break;
    
    case ELEMENT_NORMALPDF:
      numSigmasStr = DomFunctions::getAttribute( xmlElement, "numSigmas");
      numSigmas_ = numSigmasStr.toSize_T();

      if ( 0 == numSigmas_ ) {
        throw_message( range_error,
          setFunctionName( functionName)
          << "\n - normalPDF"
          << "\" has wrong \"numSigmas\" attribute."
        );
      }
    
      elementType_ = ELEMENT_BOUNDS;
      try {
        DomFunctions::initialiseChild( this,
                                       xmlElement,
                                       EMPTY_STRING,
                                       "bounds");
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
        );
      }
    
      elementType_ = ELEMENT_CORRELATESWITH;
      try { 
        DomFunctions::initialiseChildren( this,
                                          xmlElement,
                                          EMPTY_STRING,
                                          "correlatesWith",
                                          false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
        );
      }

      elementType_ = ELEMENT_CORRELATION;
      try { 
        DomFunctions::initialiseChildren( this,
                                          xmlElement,
                                          EMPTY_STRING,
                                          "correlation",
                                          false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
        );
      }
	    break;
	
    case ELEMENT_CORRELATESWITH:
    case ELEMENT_CORRELATION:
	    varId = DomFunctions::getAttribute( xmlElement, "varID");
	  
	    if ( !isInCorrelationList( varId, pairIndex)) {
	      varIndex = janus_->crossReferenceId( ELEMENT_VARIABLE, varId);

        if ( varIndex.isValid() ) {
		      if ( elementType_ == ELEMENT_CORRELATION) {
            corrCoef = DomFunctions::getAttribute( xmlElement, "corrCoef");
		        correlation_.push_back(
			        correlationPair( varIndex, dstomath::bound( corrCoef.toDouble(), -1.0, 1.0)));
		      }
		      else {
            correlation_.push_back( correlationPair( varIndex, 0.0));
		      }
	      }
		    else {
		      throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - correlation|correlationWith varID"
            << "\" is not available in variableDef list."
          );
		    }
		    correlationVarIdList_.push_back( varId);
	    }
	    else if (elementType_ == ELEMENT_CORRELATION) {
        corrCoef = DomFunctions::getAttribute( xmlElement, "corrCoef");
	      correlation_[pairIndex].second = dstomath::bound( corrCoef.toDouble(), -1.0, 1.0);
	    }
      break;
	  
    default:
      break;
  }
}

//------------------------------------------------------------------------//

void Uncertainty::resetJanus( Janus *janus)
{
  janus_ = janus;

  /*
   * Reset the Janus pointer in the Bounds class
   */
  for ( size_t i = 0; i < bounds_.size(); i++) {
    bounds_[i].resetJanus( janus);
  }
}

//------------------------------------------------------------------------//

bool Uncertainty::isInCorrelationList(
  const aString& elementName,
  size_t &pairIndex)
{
  size_t listSize = correlationVarIdList_.size();

//  for ( size_t i = 0 ; i <= listSize ; i++ ) {
  for ( size_t i = 0 ; i < listSize ; i++ ) {
//    cout << "Here i :" << i << endl;
    if ( correlationVarIdList_[ i ] == elementName ) {
	  pairIndex = i;
      return true;
    }
  }

  pairIndex = listSize;
  return false;
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const Uncertainty &uncertainty)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display Uncertainty contents:" << endl
     << "-----------------------------------" << endl;

  os << "  effect             : " << uncertainty.getEffect() << endl
     << "  Pdf                : " << uncertainty.getPdf() << endl
     << "  isSet              : " << uncertainty.isSet() << endl
     << "  # Sigmas           : " << uncertainty.getNumSigmas() << endl
     << endl;

  /*
   * Data associated with Class
   */
  const vector<Bounds>& bounds = uncertainty.getBounds();
  for ( size_t i = 0; i < bounds.size(); i++) {
    os << "  bound " << i << endl;
    os << (Bounds&)bounds[i] << endl;
  }

  aStringList correlationVarList = uncertainty.getCorrelationVarList();
  for ( size_t i = 0; i < correlationVarList.size(); i++) {
    os << "  correlation Var ID " << i << " = " << correlationVarList[i] << endl;
  }

  const vector<correlationPair>& correlation = uncertainty.getCorrelation();
  for ( size_t i = 0; i < correlation.size(); i++) {
    os << "  correlation " << i << endl;
    os << "    with  : " << correlation[i].first;
    os << "    coeff : " << correlation[i].second;
  }

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus

