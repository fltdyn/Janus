#ifndef _MODEL_H_
#define _MODEL_H_

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
// First Date: 2015-11-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Model.h
 *
 * An Model instance holds in its allocated memory alphanumeric data
 * derived from a dynamic system \em model element of a DOM corresponding to
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

// C++ Includes

// Ute Includes
#include <Ute/aString.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "TransferFn.h"
#include "StatespaceFn.h"

namespace janus
{

  class Janus;

  /**
   * An #Model instance holds in its allocated memory alphanumeric data
   * derived from a dynamic system \em model element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.
   *
   * It includes entries arranged as follows:
   *  An element defining if the dynamic system model represents a continuous or
   *  discrete system
   *  An element defining the domain of the integration when solving the model,
   *  being either time or frequency.
   *  An element defining the integration method to be used when solving the model
   *  for time domain analysis
   *  ...
   *
   * The #Model class is only used within the janus namespace, and should only
   * be referenced through the Janus class.
   */
  class Model: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #Model class
     * without supplying the DOM dynamic system \em model element from which the
     * instance is constructed, but in this state it is not useful for
     * any class functions.  It is necessary to populated the class from
     * a DOM containing a \em model element before any further use of
     * the instanced class.
     *
     * This form of the constructor is principally for use within
     * higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Model();

    /**
     * The constructor, when called with an argument pointing to a dynamic system
     * \em model element within a DOM, instantiates the
     * #Model class and fills it with alphanumeric data from the DOM.
     * String-based numeric data are converted to double-precision.
     *
     * \param elementDefinition is an address of an \em model
     * component node within the DOM.
     */
    Model( const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #Model is filled with data from a
     * particular \em model element within a DOM by this function.  If
     * another \em model element pointer is supplied to an
     * instance that has already been initialised, data corruption will occur
     * and the entire Janus instance will become unusable.
     *
     * \param elementDefinition is an address of an \em array
     * component node within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em name attribute of the
     * \em model element represented by this #Model instance.
     * A model's \em name attribute is a string of arbitrary
     * length, but normally short.  It is not used for indexing, and therefore
     * need not be unique (although uniqueness may aid both programmer and
     * user), but should comply with the ANSI/AIAA S-119-2011 standard\latexonly
     * \cite{aiaa_data}\endlatexonly .  If the instance has not been
     * initialised from a DOM, an empty string is returned.
     *
     * \return The \em name string is returned by reference.
     */
    const dstoute::aString& getName( ) const { return name_; }

    /**
     * This function provides access to the \em modelID attribute
     * of the \em model element represented by this #Model instance.
     * A model's \em modelID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * model.  It is used for indexing models within an XML dataset,
     * and provides underlying cross-references for most of the Janus library
     * functionality.  If the instance has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em modelID string is returned by reference.
     */
    const dstoute::aString& getModelID( ) const { return modelID_; }

    /**
     * This function provides access to the optional \em description of the
     * \em model element represented by this #Model instance.
     * A \em model's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #Model has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function indicates whether a \em model element of a
     * DAVE-ML dataset includes either \em provenance or \em provenanceRef.
     *
     * \return A boolean variable, 'true' if the \em function includes a
     * provenance, defined either directly or by reference.
     *
     * \sa Provenance
     */
    const bool& hasProvenance( ) const { return hasProvenance_; }

    /**
     * This function provides access to the Provenance instance
     * associated with a #Model instance.  There may be zero or one
     * of these elements for each variable definition in a valid dataset,
     * either provided directly or cross-referenced through a
     * \em provenanceRef element.
     *
     * \return The Provenance instance is returned by reference.
     *
     * \sa Provenance
     */
    const Provenance& getProvenance( ) const { return provenance_; }

    /**
     * This function indicates if a \em model element of a DAVE-ML
     * dataset is defined as either a \em continuous dynamic system model
     * or a \em discrete dynamic system model.
     *
     * \return A ModelMethod enumeration is returned indicating the model
     * type, being: MODEL_CONTINUOUS or MODEL_DISCRETE.
     */
    const ModelMethod& getModelMethod() const { return modelMethod_;}

    /**
     * This function indicates the integration domain that is to be used
     * for evaluating a dynamic system \em model element of a DAVE-ML
     * dataset. The integration domain is defined as being either \em time
     * or \em frequency.
     *
     * \return An IntegrationDomain enumeration is returned indicating the
     * domain as being either: INTEGRATION_TIME or INTEGRATION_FREQ.
     */
    const IntegrationDomain& getIntegrationDomain() const { return integrationDomain_;}

    /**
     * This function indicates the integration method that is to be used
     * for evaluating a dynamic system \em model element of a DAVE-ML
     * dataset. The options for integration method currently supported by
     * DAVE-ML are: Euler; Runge Kutta 2, 4, and 4-5; and Adam Bashford.
     * Alternative integration methods may be included in future version of
     * DAVE-ML.
     *
     * \return An IntegrationMethod enumeration is returned indicating the
     * type as being either: INTEGRATION_EULER, INTEGRATION_RUNGE_KUTTA_2,
     * INTEGRATION_RUNGE_KUTTA_4,INTEGRATION_RUNGE_KUTTA_45, or
     * INTEGRATION_ADAM_BASHFORD
     */
    const IntegrationMethod& getIntegrationMethod() const { return integrationMethod_;}

    /**
     * This function returns the integration step to be used for evaluating
     * a dynamic system \em model element of a DAVE-ML dataset. This
     * represents either a constant step size for the integration, or
     * the initial step size for a variable step integrator.
     *
     * \return The integration step is returned as a double value.
     *
     */
    const double& getIntegrationStep() const { return integrationStep_;}

    /**
     * This function returns an identifier to an input (control)
     * used to evaluate a dynamic system \em model element of
     * a DAVE-ML dataset. The identifier is the \em varID for a
     * \em variableDef element that defines the contents of the
     * input (control). It is used for both transfer function
     * and state-space dynamic system models.
     *
     * \return The input \em varID string is returned by reference.
     */
    const dstoute::aString& getInputID() const { return inputID_;}

    /**
     * This function provides access to the \em transferFn element of the
     * dynamic system model. The \em transferFn elements can be
     * accessed through the \em transferFn class.
     *
     * \return The address of the transfer function element.
     */
    const TransferFn& getTransferFn() const { return transferFn_;}

    /**
     * This function provides access to the \em statespaceFn element of the
     * dynamic system model. The \em statespaceFn elements can be
     * accessed through the \em statespaceFn class.
     *
     * \return The address of the statespace function element.
     */
    const StatespaceFn& getStatespaceFn() const { return statespaceFn_;}

    /**
     * This function provides the means to set the content of the name
     * element within this #Model instance.
     *
     * \param name is a string containing the content to set for the
     * name element of this #Model instance.
     */
    void setName( const dstoute::aString& name)
    { name_ = name;}

    /**
     * This function permits the model identifier \em modelID of the
     * \em model element to be reset for this Model instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param modelID a string identifier representing the instance of
     * the \em model.
     */
    void setModelID( const dstoute::aString& modelID)
    {
      modelID_ = modelID;
    }

    /**
     * This function provides the means to set the content of the description
     * element within this #Model instance.
     *
     * \param description is a string containing the content to set for the
     * description element of this #Model instance.
     */
    void setDescription( const dstoute::aString& description)
    { description_ = description;}

    /**
     * This function permits a \em model element of a DAVE-ML
     * dataset, defined as either a \em continuous dynamic system model
     * or a \em discrete dynamic system model, to be reset.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param modelMethod a ModelMethod enumeration indicating the model
     * type, being either: MODEL_CONTINUOUS or MODEL_DISCRETE.
     */
    void setModelMethod( const ModelMethod& modelMethod)
    { modelMethod_ = modelMethod;}

    /**
     * This function permits the integration domain used for evaluating
     * a dynamic system \em model element to be reset.
     * The integration domain is defined as being either \em time
     * or \em frequency.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param integrationDomain an IntegrationDomain enumeration indicating the
     * domain, being either: INTEGRATION_TIME or INTEGRATION_FREQ.
     */
    void setIntegrationDomain( const IntegrationDomain& integrationDomain)
    { integrationDomain_ = integrationDomain;}

    /**
     * This function permits the integration method used for evaluating
     * a dynamic system \em model element to be reset.
     * The options for integration method currently supported by
     * DAVE-ML are: Euler; Runge Kutta 2, 4, and 4-5; and Adam Bashford.
     * Alternative integration methods may be included in future version of
     * DAVE-ML.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param integrationMethod an IntegrationMethod enumeration indicating the
     * type, being either: INTEGRATION_EULER, INTEGRATION_RUNGE_KUTTA_2,
     * INTEGRATION_RUNGE_KUTTA_4,INTEGRATION_RUNGE_KUTTA_45, or
     * INTEGRATION_ADAM_BASHFORD
     */
    void setIntegrationMethod( const IntegrationMethod& integrationMethod)
    { integrationMethod_ = integrationMethod;}

    /**
     * This function permits the integration step used for evaluating
     * a dynamic system \em model element to be reset. The integration
     * step represents either a constant step size for the integration,
     * or the initial step size for a variable step integrator.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param step a value (of type double) representing the integration step.
     */
    void setIntegrationStep( const double& step)
    { integrationStep_ = step;}

    /**
     * This function permits the \em varID associated with input data element in
     * this \em model instance to be reset.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the input data of the dynamic system model.
     */
    void setInputVectorID( const dstoute::aString& varID)
    {
      inputID_ = varID;
    }

    /**
     * This function is used to export the dynamic system \em model data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os, const Model &model);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

  private:
  /************************************************************************
   * These are the dynamic system model elements, set up during instantiation.
   */

    ElementDefinitionEnum elementType_;

    dstoute::aString  name_;
    dstoute::aString  modelID_;

    double            integrationStep_;

    ModelMethod       modelMethod_;
    IntegrationMethod integrationMethod_;
    IntegrationDomain integrationDomain_;

    dstoute::aString  description_;
    bool              isProvenanceRef_;
    bool              hasProvenance_;
    Provenance        provenance_;

    dstoute::aString  inputID_;

    bool              isModelTransferFn_;
    bool              isTransferFnRef_;
    bool              isStatespaceFnRef_;
    TransferFn        transferFn_;
    StatespaceFn      statespaceFn_;

  };
}

#endif /* _ARRAY_H_ */
