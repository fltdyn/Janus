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
// Title:      Janus/Model
// Class:      Model
// Module:     Model.h
// First Date: 2015-11-18
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Model.cpp
 *
 * An Model instance holds in its allocated memory alphanumeric data
 * derived from a dynamic systems \em model element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.
 *
 * It includes entries arranged as follows:
 *  An element defining if the dynamic systems model represents a continuous or
 *  discrete system
 *  An element defining the domain of the integration when solving the model,
 *  being either time or frequency.
 *  An element defining the integration method to be used when solving the model
 *  for time domain analysis
 *  ...
 *
 * The Model class is only used within the janus namespace, and should only
 * be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 */

// Ute Includes
#include <Ute/aBiMap.h>
#include <Ute/aString.h>
#include <Ute/aMessageStream.h>

// Local Includes
#include "DomFunctions.h"
#include "JanusConstants.h"
#include "Model.h"

using namespace std;
using namespace dstoute;

namespace janus
{

typedef aBiMap< aString, int > EnumStringBiMap;
const EnumStringBiMap modelBasisAttributes = ( EnumStringBiMap()
  << EnumStringBiMap::pair( "discrete", MODEL_DISCRETE)
  << EnumStringBiMap::pair( "continuous", MODEL_CONTINUOUS)
);

const EnumStringBiMap integrationTypeAttributes = ( EnumStringBiMap()
  << EnumStringBiMap::pair( "freq", INTEGRATION_FREQ)
  << EnumStringBiMap::pair( "time", INTEGRATION_TIME)
);

const EnumStringBiMap integrationMethodAttributes = ( EnumStringBiMap()
  << EnumStringBiMap::pair( "EULER",          INTEGRATION_EULER)
  << EnumStringBiMap::pair( "RUNGE_KUTTA_2",  INTEGRATION_RUNGE_KUTTA_2)
  << EnumStringBiMap::pair( "RUNGA_KUTTA_4",  INTEGRATION_RUNGE_KUTTA_4)
  << EnumStringBiMap::pair( "RUNGE_KUTTA_45", INTEGRATION_RUNGE_KUTTA_45)
  << EnumStringBiMap::pair( "ADAM_BASHFORD",  INTEGRATION_ADAM_BASHFORD)
);

  //------------------------------------------------------------------------//

  Model::Model() :
    XmlElementDefinition(),
    elementType_( ELEMENT_NOTSET),
    integrationStep_( 0.0),
    modelMethod_( MODEL_CONTINUOUS),
    integrationMethod_( INTEGRATION_EULER),
    integrationDomain_( INTEGRATION_TIME),
    isProvenanceRef_( false),
    hasProvenance_( false),
    isModelTransferFn_( false),
    isTransferFnRef_( false),
    isStatespaceFnRef_( false)
  {
  }

  Model::Model(
    const DomFunctions::XmlNode& modelElement) :
    XmlElementDefinition(),
    elementType_( ELEMENT_NOTSET),
    integrationStep_( 0.0),
    modelMethod_( MODEL_CONTINUOUS),
    integrationMethod_( INTEGRATION_EULER),
    integrationDomain_( INTEGRATION_TIME),
    isProvenanceRef_( false),
    hasProvenance_( false),
    isModelTransferFn_( false),
    isTransferFnRef_( false),
    isStatespaceFnRef_( false)
  {
    initialiseDefinition( modelElement);
  }

  //------------------------------------------------------------------------//

  void Model::initialiseDefinition(
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "Model::initialiseDefinition()");

    /*
     * Retrieve attributes for the element's Definition
     */
    name_    = DomFunctions::getAttribute( elementDefinition, "name");
    modelID_ = DomFunctions::getAttribute( elementDefinition, "modelID");

    /*
     * Retrieve the Model Basis method attribute
     */
    aString attributeStr = DomFunctions::getAttribute( elementDefinition, "basis");
    if ( attributeStr.empty()) {
      attributeStr = modelBasisAttributes[ MODEL_CONTINUOUS];
    }
    if ( modelBasisAttributes.find( attributeStr) == modelBasisAttributes.end()) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - invalid \"basis\" attribute \"" << attributeStr << "\"."
      );
    }
    modelMethod_ = ModelMethod( modelBasisAttributes[ attributeStr]);

    /*
     * Retrieve the Integration type attribute
     */
    attributeStr = DomFunctions::getAttribute( elementDefinition, "type");
    if ( attributeStr.empty()) {
      attributeStr = integrationTypeAttributes[ INTEGRATION_TIME];
    }
    if ( integrationTypeAttributes.find( attributeStr) == integrationTypeAttributes.end()) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - invalid \"integration type \" attribute \"" << attributeStr << "\"."
      );
    }
    integrationDomain_ = IntegrationDomain( integrationTypeAttributes[ attributeStr]);

    /*
     * Retrieve the Integration method attribute
     */
    attributeStr = DomFunctions::getAttribute( elementDefinition, "integrator");
    if ( attributeStr.empty()) {
      attributeStr = integrationMethodAttributes[ INTEGRATION_EULER];
    }
    if ( integrationMethodAttributes.find( attributeStr) == integrationMethodAttributes.end()) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - invalid \"integration method \" attribute \"" << attributeStr << "\"."
      );
    }
    integrationMethod_ = IntegrationMethod( integrationMethodAttributes[ attributeStr]);

    aString tempString;
    tempString = DomFunctions::getAttribute( elementDefinition, "step");
    if ( tempString.isNumeric()) {
      integrationStep_ = tempString.toDouble();
    }

    /*
     * Retrieve the description associated with the element
     */
    description_ = DomFunctions::getChildValue( elementDefinition, "description");

    /*
     * Retrieve the references to the input, state, state derivative and
     * disturbance variableDefs
     */
    elementType_ = ELEMENT_VARIABLE;
    try {
      DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING,
        "variableDef", "variableRef", "varID", false);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - Error initialising the \"variableRef\" element for the model input.\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve the model form - transferFn, transferFnRef, statespaceFn, statespaceFnRef
     */
    bool isTransferFn      = DomFunctions::isChildInNode( elementDefinition, "transferFn");
    bool isTransferFnRef   = DomFunctions::isChildInNode( elementDefinition, "transferFnRef");
    bool isStatespaceFn    = DomFunctions::isChildInNode( elementDefinition, "statespaceFn");
    bool isStatespaceFnRef = DomFunctions::isChildInNode( elementDefinition, "statespaceFnRef");

    if ( !isTransferFn && !isTransferFnRef && !isStatespaceFn && !isStatespaceFnRef) {
      throw_message( invalid_argument,
        setFunctionName( functionName) << "Model element"
        << "\" does not have a valid model form element.\n"
      );
    }

    /*
     * Retrieve transfer function - Def or Ref
     */
    elementType_ = ELEMENT_TRANSFERFN;
    if ( isTransferFn || isTransferFnRef) {
      try {
        DomFunctions::initialiseChildOrRef( this, elementDefinition, name_,
          "transferFn", "transferFnRef", "transFnID", false);
        isModelTransferFn_ = true;
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName) << "\n - for model \"" << name_ << "\"\n - "
          << excep.what()
        );
      }
    }

    /*
     * Retrieve transfer function - Def or Ref
     */
    elementType_ = ELEMENT_STATESPACEFN;
    if ( isStatespaceFn || isStatespaceFnRef) {
      try {
        DomFunctions::initialiseChildOrRef( this, elementDefinition, name_,
          "statespaceFn", "statespaceFnRef", "sspaceFnID", false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName) << "\n - for model \"" << name_ << "\"\n - "
          << excep.what()
        );
      }
    }

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
        setFunctionName( functionName) << "\n - for model \"" << name_ << "\"\n - "
        << excep.what()
      );
    }
  }

  //------------------------------------------------------------------------//

  void Model::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the Model element
     */
    DomFunctions::XmlNode childElement =
      DomFunctions::setChild( documentElement, "model");

    /*
     * Add attributes to the VariableDef child
     */
    DomFunctions::setAttribute( childElement, "name",  name_);
    DomFunctions::setAttribute( childElement, "modelID", modelID_);

    DomFunctions::setAttribute( childElement, "basis",
      modelBasisAttributes[ modelMethod_]);
    DomFunctions::setAttribute( childElement, "type",
      integrationTypeAttributes[ integrationDomain_]);
    DomFunctions::setAttribute( childElement, "integrator",
      integrationMethodAttributes[ integrationMethod_]);
    if ( !dstomath::isnan( integrationStep_)) {
      DomFunctions::setAttribute( childElement, "step", aString("%").arg( integrationStep_));
    }

    /*
     * Add description element
     */
    if ( !description_.empty()) {
      DomFunctions::setChild( childElement, "description", description_);
    }

    /*
     * Add the references to the input, state, state derivative and
     * disturbance variableDefs
     */
    if ( !inputID_.empty()) {
        DomFunctions::XmlNode variableRefElement;
        variableRefElement = DomFunctions::setChild( childElement, "variableRef");
        DomFunctions::setAttribute( variableRefElement, "varID", inputID_);
    }

    /*
     * Retrieve the model form - transferFn, transferFnRef, statespaceFn, statespaceFnRef
     */
    if ( isModelTransferFn_) {
      transferFn_.exportDefinition( childElement, isTransferFnRef_);
    }
    else {
      statespaceFn_.exportDefinition( childElement, isStatespaceFnRef_);
    }

    /*
     * Add the optional provenance entry to the VariableDef child
     */
    if ( hasProvenance_) {
      provenance_.exportDefinition( childElement, isProvenanceRef_);
    }
  }

  void Model::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    static const aString functionName(
      "ModelFn::readDefinitionFromDom()");

    switch ( elementType_) {
      case ELEMENT_TRANSFERFN:
        transferFn_.initialiseDefinition( xmlElement);
        break;

      case ELEMENT_STATESPACEFN:
        statespaceFn_.initialiseDefinition( xmlElement);
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

  bool Model::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& /*documentElementReferenceIndex*/)
  {
    aString crossRefElementId;

    switch ( elementType_) {
    case ELEMENT_TRANSFERFN:
      if ( DomFunctions::getAttribute( xmlElement, "transFnID") != elementID) {
        return false;
      }
      isTransferFnRef_ = true;
      break;

    case ELEMENT_STATESPACEFN:
      if ( DomFunctions::getAttribute( xmlElement, "sspaceFnID") != elementID) {
        return false;
      }
      isStatespaceFnRef_ = true;
      break;

    case ELEMENT_PROVENANCE:
      if ( DomFunctions::getAttribute( xmlElement, "provID") != elementID) {
        return false;
      }
      isProvenanceRef_ = true;
      break;

    case ELEMENT_VARIABLE:
      if ( DomFunctions::getAttribute( xmlElement, "varID") != elementID) {
        return false;
      }
      inputID_ = elementID;
      break;

    default:
      return false;
      break;
    }

    readDefinitionFromDom( xmlElement);
    return true;
  }

  //------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const Model &model)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display Model contents:" << endl << "-----------------------------------" << endl;

    /*
     * Data associated with Class
     */
    os << "  name                   : " << model.getName() << endl
       << "  modelID                : " << model.getModelID() << endl
       << "  basis                  : " << model.getModelMethod() << endl
       << "  type                   : " << model.getIntegrationDomain() << endl
       << "  integrator             : " << model.getIntegrationMethod() << endl
       << "  step                   : " << model.getIntegrationStep() << endl
       << "  description            : " << model.getDescription() << endl
       << "  inputRef (varID)       : " << model.getInputID() << endl
       << endl;

    /*
     * Numerator and Denominator data for the Class
     */
    os << "Transfer Function" << endl;
    os << model.getTransferFn() << endl << endl;

    os << "Statespace Function" << endl;
    os << model.getStatespaceFn() << endl;

    /*
     * Provenance data for the Class
     */
    if ( model.hasProvenance()) {
      os << model.getProvenance() << endl;
    }

    return os;
  }

} // namespace janus
