#ifndef _VARIABLEDEF_H_
#define _VARIABLEDEF_H_

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
// Title:      Janus/VariableDef
// Class:      VariableDef
// Module:     VariableDef.h
// First Date: 2009-06-05
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file VariableDef.h
 *
 * A VariableDef instance holds in its allocated memory alphanumeric data
 * derived from a \em variableDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
 * alphanumeric identification and cross-reference data, and may include
 * a calculation process tree for variables computed through MathML.
 * The variable definition can include statistical information regarding
 * the uncertainty of the values that it might take on, when measured
 * after any calculation is performed. This class sets up a structure
 * that manages the \em variableDef content.
 *
 * The VariableDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes
#include <utility>

// Ute Includes
#include <Ute/aMatrix.h>
#include <Ute/aUnits.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "DimensionDef.h"
#include "Array.h"
#include "Model.h"
#include "Provenance.h"
#include "Uncertainty.h"
#include "Function.h"
#include "MathMLDataClass.h"

namespace janus {

  class Janus;

  /**
   * A #VariableDef instance holds in its allocated memory alphanumeric data
   * derived from a \em variableDef element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
   * alphanumeric identification and cross-reference data, and may include
   * a calculation process tree for variables computed through MathML.
   *
   * The #VariableDef class is only used within the janus namespace, and should
   * only be referenced through the Janus class.
   *
   * To determine the characteristics of a dataset's variables, typical usage is:
  \code
    Janus test( xmlFileName );
    vector<VariableDef> variableDef = test.getVariableDef();
    for ( size_t i = 0 ; i < variableDef.size() ; i++ ) {
      cout << "  Variable " << i << " : \n"
           << "   ID           : " << variableDef.at( i ).getVarID() << "\n"
           << "   Name         : " << variableDef.at( i ).getName() << "\n"
           << "   Description  : " << variableDef.at( i ).getDescription() << "\n"
           << "   Units        : " << variableDef.at( i ).getUnits() << "\n"
           << "   Type         : " << variableDef.at( i ).getTypeString() << "\n"
           << "   Method       : " << variableDef.at( i ).getMethodString() << "\n"
           << "   Axis System  : " << variableDef.at( i ).getAxisSystem() << "\n"
           << "   Initial Value: " << variableDef.at( i ).getInitialValue() << "\n" << endl;
    }
  \endcode
   */
  
  class VariableDef : public XmlElementDefinition
  {
   public:

    enum ScriptType {
      NO_SCRIPT,
      EXPRTK_SCRIPT,
      LUA_SCRIPT
    };

    /**
     * The empty constructor can be used to instance the #VariableDef class
     * without supplying the DOM \em variableDef element from which the instance
     * is constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing an
     * \em variableDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    VariableDef( );

    /**
     * The constructor, when called with an argument pointing to a \em variableDef
     * element within a DOM, instantiates the #VariableDef class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em variableDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    VariableDef( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    virtual ~VariableDef();

    /**
     * An uninitialised instance of #VariableDef is filled with data from a
     * particular \em variableDef element within a DOM by this function.  If
     * another \em variableDef element pointer is supplied to an
     * instance that has already been initialised, data corruption may occur.
     *
     * \param elementDefinition is an address of a \em variableDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em DOMElement node associated
     * with the instance of the \em variableDef component. The function is
     * used internally within Janus while initialising a DAVE-ML compliant \
     * XML dataset source file.
     *
     * \return The \em DOMElement node in a DOM for the \em variableDef
     * component is returned as a pointer.
     */
    const DomFunctions::XmlNode& getDOMElement() { return domElement_;}

    /**
     * This function provides access to the \em name attribute of the
     * \em variableDef element represented by this #VariableDef instance.
     * A variable's \em name attribute is a string of arbitrary
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
     * This function provides access to the \em varID attribute
     * of the \em variableDef element represented by this #VariableDef instance.
     * A variable's \em varID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * variable.  It is used for indexing variables within an XML dataset,
     * and provides underlying cross-references for most of the Janus library
     * functionality.  If the instance has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em varID string is returned by reference.
     */
    const dstoute::aString& getVarID( ) const { return varID_; }

    /**
     * This function provides access to the \em units attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em units attribute is a string of arbitrary
     * length, but normally short, and complying with the format requirements
     * \latexonly chosen by \ac{DST} \ac{AD} \ac{APS} \cite{brian1} in accordance with
     * \ac{SI} \endlatexonly \htmlonly of SI \endhtmlonly and other systems.
     *  If the instance has not been initialised from a DOM, an empty string is
     * returned.
     *
     * \return The \em units string is returned by reference.
     */
    const dstoute::aString& getUnits( ) const { return units_; }

    /**
     * This function provides access to the optional \em axisSystem attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em axisSystem attribute is a string of arbitrary
     * length, but normally short, and complying with certain format requirements
     * \latexonly chosen by \ac{DST} \ac{AD} \ac{APS} \endlatexonly .
     * Typical values include "Body" or "Intermediate".  If no axis system
     * is specified in the XML dataset, or the #VariableDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em axisSystem string is returned by reference.
     */
    const dstoute::aString& getAxisSystem( ) const { return axisSystem_; }

    /**
     * This function provides access to the optional \em sign attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em sign attribute is a string of arbitrary
     * length, but normally short, describing the sign convention for the
     * signal represented by the variable.
     * Typical values include "TED +ve" or "LWD +ve".  If no sign convention
     * is specified in the XML dataset, or the #VariableDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em sign string is returned by reference.
     */
    const dstoute::aString& getSign( ) const { return sign_; }

    /**
     * This function provides access to the optional \em alias attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em alias attribute is a string of arbitrary
     * length, but normally short, providing an alternative name (possibly
     * facility specific) for the signal represented by the variable.  If no
     * alias is specified in the XML dataset, or the #VariableDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em alias string is returned by reference.
     */
    const dstoute::aString& getAlias( ) const { return alias_; }

    /**
     * This function provides access to the optional \em symbol attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em symbol attribute contains a Unicode
     * representation of the symbol associated with a signal represented by
     * the variable.  A typical example might be \latexonly $ \alpha $
     * \endlatexonly
     * \htmlonly <var>&alpha;</var> \endhtmlonly associated with angle of attack.
     * If no symbol is specified in the XML dataset, or the #VariableDef has not
     * been initialised from the DOM, a blank Unicode character is returned.
     *
     * \return The \em symbol Unicode character is returned by reference.
     */
    const dstoute::aString& getSymbol( ) const { return symbol_; }

    /**
     * This function provides access to the optional \em initialValue attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em initialValue attribute specifies an initial
     * value for the signal represented by the variable. It is also used to
     * store the value of a signal that remains constant.
     * If the initial value is not specified in the XML dataset, or the #VariableDef
     * has not been initialised from the DOM, the a NaN value is returned.
     *
     * \return A double precision floating point number is returned.
     */
    const double& getInitialValue( ) const { return initialValue_; }
    const dstoute::aString& getInitialValueString( ) const { return initialValueString_; }

    /**
     * This function provides access to the optional \em minValue attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em minValue attribute provides a minimum boundary value
     * that may be returned when evaluating the \em variableDef.
     *
     * \return A double precision floating point number is returned.
     */
    const double& getMinValue( ) const { return minValue_; }

    /**
     * This function provides access to the optional \em maxValue attribute
     * of the \em variableDef represented by this #VariableDef instance.
     * A variable's \em maxValue attribute provides a maximum boundary value
     * that may be returned when evaluating the \em variableDef.
     *
     * \return A double precision floating point number is returned.
     */
    const double& getMaxValue( ) const { return maxValue_; }

    /**
     * This function provides access to the optional \em description of the
     * \em variableDef element represented by this #VariableDef instance.
     * A \em variableDef's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #VariableDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function provides access to the dynamic system model instance
     * associated with a #VariableDef instance.  There may be zero or one
     * of these elements for each variable definition in a valid dataset.
     *
     * \return The Model instance is returned by reference.
     *
     * \sa Model
     */
    const Model& getModel() const { return model_;}

    /**
     * This function indicates whether a \em variableDef element of a
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
     * associated with a #VariableDef instance.  There may be zero or one
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
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset has been formally designated as an input to the
     * represented model, using the \em isInput child element.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as a model input.
     */
    const bool& isInput( ) const { return isInput_; }

    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset has been formally designated as a control for the
     * represented model, using the \em isControl child element.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as a model control.
     */
    const bool& isControl( ) const { return isControl_; }

    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset has been formally designated as a disturbance to the
     * represented model, using the \em isDisturbance child element.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as a model disturbance.
     */
    const bool& isDisturbance( ) const { return isDisturbance_; }

    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset has been formally designated as an output of the
     * represented model, using the \em isOutput child element.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as a model output.
     */
    const bool& isOutput( ) const { return isOutput_; }

    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset has been formally designated as a state (i.e. the
     * output of an integrator or a discrete time step computation) of the
     * represented model, using the \em isState child element.  Model states
     * need not be formally designated as such.
     *
     * \return A boolean variable, 'true' if a variable definition includes
     * formal designation as a model state.
     */
    const bool& isState( ) const { return isState_; }

    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset has been fomally designated as a state derivative of
     * the represented model, using the \em isStateDeriv child element.
     * State derivatives need not be formally designated as such.
     *
     * \return A boolean variable, 'true' if a variable definition includes formal
     * designation as a model state derivative.
     */
    const bool& isStateDeriv( ) const { return isStateDeriv_; }

    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset has been formally designated, using the \em isStdAIAA
     * child element, as complying with the AIAA standard naming convention
     * "Standard Simulation Variable Names", Annex 1 of "Standards for the
     * Exchange of Simulation Data".  The name should be recognizable exterior
     * to this class by code complying with the standard.
     * Variable names need be neither compliant with the AIAA convention
     * nor formally designated as such.
     *
     * \return A boolean variable, 'true' if a variable definition includes formal
     * designation as compliant with the AIAA standard naming convention.
     */
    const bool& isStdAIAA( ) const { return isStdAIAA_; }

    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset includes an \em uncertainty child element.  A variable
     * described by a \em variableDef without an \em uncertainty element may
     * still have uncertainty, if it is dependent on other variables or tables
     * with defined uncertainty.
     *
     * \return A boolean variable, 'true' if a variable definition includes an
     * \em uncertainty child element.
     *
     * \sa Uncertainty
     */
    const bool& hasUncertainty( ) const { return hasUncertainty_; }

    /**
     * This function provides access to the Uncertainty instance
     * associated with a #VariableDef instance.  There may be zero or one
     * \em uncertainty element for each \em variableDef in a valid dataset.
     * For \em variableDefs without \em uncertainty, the corresponding
     * #VariableDef instance includes an empty Uncertainty instance.
     *
     * \return The Uncertainty instance is returned by reference.
     *
     * \sa Uncertainty
     */
    Uncertainty& getUncertainty( ) { return uncertainty_; }

    /**
     * This enum lists the types of variables that may be included in a
     * DAVE-ML compliant XML dataset, based on the manner in which the variable
     * value is determined.  Calling programs may use it to determine whether a
     * variable is an input, or an output of various types.  Each #VariableDef
     * instance contains one of these enums, set during instantiation, to
     * indicate the source of the associated variable.
     */
    enum VariableType{
      /**
          This \em variableDef has none of the possible output attributes
          and should be treated as internal. */
      TYPE_INTERNAL,

      /**
          This \em variableDef is explicitly defined as an input by its
          child node, \b and is not the product of either a tabulated
          function or MathML evaluation. */
      TYPE_INPUT,

      /**
          This \em variableDef is explicitly defined as an output by its
          child node, \b and is not the product of either a tabulated
          function or MathML evaluation. */
      TYPE_OUTPUT
    };

    /**
     * This enum lists the methos used to calculate a variable that may be included in a
     * DAVE-ML compliant XML dataset, based on the manner in which the variable
     * value is determined.  Calling programs may use it to determine how a
     * variable is to be evaluated. Each #VariableDef
     * instance contains one of these enums, set during instantiation, to
     * indicate the source of the associated variable.
     */
    enum VariableMethod {
      /**
        This \em variableDef is defined as a constant.
       */
      METHOD_PLAIN_VARIABLE,
      /**
          This \em variableDef is referenced as a dependent variable by
          a \em function definition, and its value will therefore be
          determined by a function evaluation, using either gridded or ungridded
          data.  It will be available as a Janus output. */
      METHOD_FUNCTION,
      /**
          This \em variableDef includes a \em calculation child element,
          and its value will therefore be determined by a MathML
          function evaluation. It will be available as a Janus output.*/
      METHOD_MATHML,
      /**
          This \em variableDef includes a \em calculation child element,
          and its value will therefore be determined by a script
          function evaluation. It will be available as a Janus output.*/
      METHOD_SCRIPT,
      /**
        This \em variableDef is defined as either a vector or a matrix.
       */
      METHOD_ARRAY,
      /**
          This \em variableDef is defined as a dynamic system model.
       */
      METHOD_MODEL
    };

    /**
     * A variable that is specified as an output, a
     * function evaluation result, or a MathML function should
     * not normally have its value set directly by the calling program.
     * This function allows the caller to determine a variable's status
     * in this regard.
     *
     * \return The #VariableType is returned on successful completion.
     */
    VariableType getType( ) const { return variableType_; }
    const dstoute::aString& getTypeString( ) const;

    /**
     * A variable that is specified as an output, a
     * function evaluation result, or a MathML function should
     * not normally have its value set directly by the calling program.
     * This function allows the caller to determine a variable's status
     * in this regard.
     *
     * \return The #VariableMethod is returned on successful completion.
     */
    VariableMethod getMethod( ) const { return variableMethod_; }
    const dstoute::aString& getMethodString( ) const;

    /**
     * This function is provided for use with the Carna store modelling
     * library, which requires the capability to reset a #VariableDef type
     * attribute.
     *
     * \param variableType the type enumeration defined for this variable definition
     */
    void setType( const VariableType& variableType )
    { variableType_ = variableType; }

    /**
     * This function is provided for use with the Carna store modelling
     * library, which requires the capability to reset a #VariableDef type
     * attribute.
     *
     * \param variableType the type enumeration defined for this variable definition
     */
    void setMethod( const VariableMethod& variableMethod )
    { variableMethod_ = variableMethod; }

    /**
     * This enum is deprecated, and is only used by deprecated functions.  In
     * new programs, the variable characteristics it describes should be
     * determined by direct interrogation of the related #VariableDef instance.
     * It is used by calling programs to indicate the characteristics of a
     * variable relative to its use in equations of motion.  One of these
     * enums may be accessed for each #VariableDef to describe the associated
     * variable.
     */
    enum VariableFlag{
      /**
          This \em variableDef represents a state variable in a dynamic model,
          either the output of an integrator (for continuous models) or a
          discretely updated state (for discrete models).
      */
      ISSTATE,
      /**
          This \em variableDef represents a state derivative variable in a
          dynamic model, for continuous models only.
      */
      ISSTATEDERIV,
      /**
          This \em variableDef name is in accordance with "Standard Simulation
          Variable Names", Annex 1 of "Standards for the Exchange of Simulation
          Data", a draft AIAA standard.  The name should be recognizable
          exterior to this class by code complying with the standard.
      */
      ISSTDAIAA,
      /**
          This \em variableDef represents a state variable, and is named in
          accordance with "Standard Simulation Variable Names".
      */
      ISSTATE_STDAIAA,
      /**
          This \em variableDef represents a state derivative variable, and is
          named in accordance with "Standard Simulation Variable Names".
      */
      ISSTATEDERIV_STDAIAA,
      /**
          Used as a flag, indicates function or variable index out
          of range.
      */
      ISERRORFLAG };

    /**
     * This function is deprecated.  In new programs, the variable
     * characteristics it describes should be determined by direct interrogation
     * of the related #VariableDef instance.  The function allows the caller to
     * determine a variable's status in respect of the flags specified
     * in #VariableFlag.
     *
     * \return A copy of the #VariableFlag enum.
     *
     * \sa VariableFlag
     */
    VariableFlag getVariableFlag( ) const;

    /**
     * This function returns the number of independent variables that directly
     * contribute to computation of the value of this variable.  If the
     * instance has not been populated from a DOM, zero is returned.  In all
     * other cases, there must be zero or more independent variables.
     *
     * \return An integer number, zero or more in a populated instance.
     */
    size_t getIndependentVarCount( ) const
    { return independentVarRef_.size(); }

    /**
     * This function provides access to the indices within the parent Janus
     * instance of those independent variables that directly
     * contribute to computation of the value of this variable.
     *
     * \return The vector of directly contributing independent variable indices
     * is returned by reference.
     */
    const std::vector<size_t>& getIndependentVarRef( ) const
    { return independentVarRef_; }

    /**
     * This function returns true if any input variables ultimately
     * contribute to computation of the value of this variable.  If the
     * instance has not been populated from a DOM, false is returned.
     *
     * \return A boolean variable of, 'true' is returned if the \em variableDef
     * has ancestors.
     */
    bool hasAncestorsRef( )
    { return descendantsRef_.size() > 0;}

    /**
     * This function returns the number of input variables that ultimately
     * contribute to computation of the value of this variable.  If the
     * instance has not been populated from a DOM, zero is returned.  In all
     * other cases, there must be zero or more independent variables.
     *
     * \return An integer number, zero or more in a populated instance.
     */
    size_t getAncestorCount( ) const
    { return ancestorsRef_.size(); }

    /**
     * This function provides access to the indices within the parent Janus
     * instance of those input variables that ultimately
     * contribute to computation of the value of this variable.
     * These are the variables that must be set using #setValue() before
     * a valid result can be computed using #getValue.
     *
     * \return The vector of ultimate independent variable indices
     * is returned by reference.
     */
    const std::vector<size_t>& getAncestorsRef( ) const
    { return ancestorsRef_; }


    /**
     * This function returns true if any variables depend on this variable as
     * input. If the instance has not been populated from a DOM, false is
     * returned.
     *
     * \return A boolean variable of, 'true' is returned if the \em variableDef
     * has desendants.
     */
    bool hasDescendantsRef( )
    { return descendantsRef_.size() > 0;}

    /**
     * This function returns the number of variables that ultimately
     * depend on this variable as input. If the instance has not been
     * populated from a DOM, zero is returned.  In all other cases,
     * there must be zero or more independent variables.
     *
     * \return An integer number, zero or more in a populated instance.
     */
    size_t getDescendantCount( ) const
    { return descendantsRef_.size(); }

    /**
     * This function provides access to the indices within the parent Janus
     * instance of those variables that ultimately depend on this variable
     * as input.
     *
     * \return The vector of ultimate dependent variable indices is returned
     * by reference.
     */
    const std::vector<size_t>& getDescendantsRef( ) const
    { return descendantsRef_; }


    /**
     * This function indicates whether a \em variableDef element of a
     * DAVE-ML dataset includes either \em dimensionDef or \em dimensionRef.
     *
     * \return A boolean variable, 'true' if the \em function includes a
     * dimensionDef, defined either directly or by reference.
     *
     * \sa DimensionDef
     */
    const bool& hasDimension( ) const { return hasDimensionDef_; }

    /**
     * This function provides access to the DimensionDef instance
     * associated with a #VariableDef instance.  There may be zero or one
     * of these elements for each variable definition in a valid dataset,
     * either provided directly or cross-referenced through a
     * \em dimensionRef element.
     *
     * \return The DimensionDef instance is returned by reference.
     *
     * \sa DimensionDef
     */
    const DimensionDef& getDimension( ) const { return dimensionDef_; }

    /**
     * This function fulfils the basic purpose of the Janus class.  It is used
     * during run-time to evaluate the variable associated with this
     * #VariableDef.  It returns a value based on
     * the current state of the Janus instance, irrespective of variable type.
     * It provides the major functionality of the Janus library.
     * As well as returning the requested value, it sets contributing values
     * within the parent Janus instance and flags them as valid.
     *
     * \return A double precision value containing the value of the variable
     * after all relevant computations based on the current input state of
     * the parent Janus instance.
     */
    double getValue() const;

    /**
     * This function is an alternative to #getValue.  It is particularly
     * useful for ensuring that all variables used by a calling program are
     * in consistent units.
     *
     * \return A double precision value containing the value of the variable
     * expressed in SI units after all relevant computations based on the current
     * input state of the parent Janus instance.
     */
    double getValueSI() const;

    /**
     * This function is an alternative to #getValue.  It is particularly
     * useful for ensuring that all variables used by a calling program are
     * in consistent units.
     *
     * With getValueMetric(), fluid volumes will be returned in litres and not m3.
     *
     * \return A double precision value containing the value of the variable
     * expressed in Metric units after all relevant computations based on the current
     * input state of the parent Janus instance.
     *
     */
    double getValueMetric() const;

    /**
     * This function fulfils the basic purpose of the Janus class.  It is used
     * during run-time to evaluate the variable associated with this
     * #VariableDef.  It returns a data matrix based on
     * the current state of the Janus instance, irrespective of variable type.
     * It provides the major functionality of the Janus library.
     * As well as returning the requested data values, it sets contributing values
     * within the parent Janus instance and flags them as valid.
     *
     * \return A double precision vector (DVector) containing the values of the variable
     * after all relevant computations based on the current input state of
     * the parent Janus instance.
     */
    const dstomath::DVector& getVector();

    /**
     * This function fulfils the basic purpose of the Janus class.  It is used
     * during run-time to evaluate the variable associated with this
     * #VariableDef.  It returns a data matrix based on
     * the current state of the Janus instance, irrespective of variable type.
     * It provides the major functionality of the Janus library.
     * As well as returning the requested data values, it sets contributing values
     * within the parent Janus instance and flags them as valid.
     *
     * \return A double precision matrix (DMatrix) containing the values of the variable
     * after all relevant computations based on the current input state of
     * the parent Janus instance.
     */
    const dstomath::DMatrix& getMatrix();

    /**
     * Returns true if the VariableDef is a single double value.
     */
    bool isValue();

    /**
     * Returns true if the VariableDef is a vector.
     */
    bool isVector();

    /**
     * Returns true if the VariableDef is a matrix or vector.
     */
    bool isMatrix();

    /**
     * This function is used during run-time to evaluate the Gaussian uncertainty
     * of the variable associated with this #VariableDef.  It returns a value
     * based on the current state of the Janus instance.
     * It supplements the major functionality of the Janus library.
     * As well as returning the requested value, it sets contributing values
     * within the parent Janus instance and flags them as valid.
     * A variable with uniform
     * uncertainty set, either directly or indirectly, will return a Gaussian
     * uncertainty of zero.  A variable with no uncertainty
     * set, either directly or indirectly, will also return zero.
     *
     * \return A double precision value containing the value of the Gaussian
     * uncertainty after all relevant computations based on the current input
     * state of the parent Janus instance.
     */
    double getUncertaintyValue( const size_t& numSigmas )
    { return sqrt( getVariance() ) * (double)numSigmas; }

    /**
     * This function is used during run-time to evaluate the uniform uncertainty
     * of the variable associated with this #VariableDef.  It returns a value
     * based on the current state of the Janus instance.
     * It supplements the major functionality of the Janus library.
     * As well as returning the requested value, it sets contributing values
     * within the parent Janus instance and flags them as valid.
     * A variable with Gaussian
     * uncertainty set, either directly or indirectly, will return a uniform
     * bound of zero.  A variable with no uncertainty
     * set, either directly or indirectly, will also return zero.
     *
     * \return A double precision value containing the value of the uniform
     * uncertainty after all relevant computations based on the current input
     * state of the parent Janus instance.
     */
    double getUncertaintyValue( const bool& isUpperBound )
    { return getAdditiveBounds( isUpperBound ) + getValue(); }

    /**
     * This function provides access to the level of correlation between the
     * Gaussian uncertainty of the variable associated with this #VariableDef and
     * the Gaussian uncertainty of any other variable in the XML dataset.
     * \param index has a range from 0 to (Janus#getNumberOfVariables() - 1), and
     * selects the other variable to be addressed from the #VariableDef vector
     * within the parent Janus instance.
     *
     * \return The correlation coefficient relating the two variables'
     * uncertainties is returned as a double.  Where correlation has not
     * been specified in the XML dataset, the coefficient is returned as zero.
     */
    double getCorrelationCoefficient( const size_t& index );

    /**
     * This function provides the means to set the current value of the
     * variable associated with this #VariableDef.  It is the basic means
     * permitting a calling program to pass an independent variable's state to a
     * Janus instance prior to function evaluation.
     * This function will throw a standard exception if an attempt is made to
     * modify the value of a variable that is specified in the XML dataset as
     * being evaluated by computation.
     *
     * \param x is the double precision scalar value, vector or matrix to which
     * the current value of the indexed variable will be set.
     * \param isForced a flag indicating whether to force the variable to be set
     * to the value and thereby override other flags detailing the state of a variable
     */
    void setValue( const double &x, bool isForced = false);

    /**
     * This function provides the means to set the current value of the
     * variable associated with this #VariableDef.  It is the basic means
     * permitting a calling program to pass an independent variable's state to a
     * Janus instance prior to function evaluation.
     * This function will throw a standard exception if an attempt is made to
     * modify the value of a variable that is specified in the XML dataset as
     * being evaluated by computation.
     *
     * \param x is a vector of double precision values (DVector), to which
     * this variable will be set.
     * \param isForced a flag indicating whether to force the variable to be set
     * to the value and thereby override other flags detailing the state of a variable
     */
    void setValue( const dstomath::DVector &x, bool isForced = false);

    /**
     * This function provides the means to set the current value of the
     * variable associated with this #VariableDef.  It is the basic means
     * permitting a calling program to pass an independent variable's state to a
     * Janus instance prior to function evaluation.
     * This function will throw a standard exception if an attempt is made to
     * modify the value of a variable that is specified in the XML dataset as
     * being evaluated by computation.
     *
     * \param x is a matrix of double precision values (DMatrix), to which
     * this variable will be set.
     * \param isForced a flag indicating whether to force the variable to be set
     * to the value and thereby override other flags detailing the state of a variable
     */
    void setValue( const dstomath::DMatrix &x, bool isForced = false);

    /**
     * This function is an alternative to #setValue.  It will throw a standard
     * exception if an attempt is made to modify the value of a variable that
     * is specified in the XML dataset as being evaluated by computation.
     *
     * \param xSI is the double precision value, expressed in SI units, to which
     * the current value of the indexed variable will be set.
     */
    void setValueSI( const double &xSI );

    /**
     * This function is an alternative to #setValue.  It will throw a standard
     * exception if an attempt is made to modify the value of a variable that
     * is specified in the XML dataset as being evaluated by computation.
     *
     * With setValueMetric(), fluid volumes will be in litres and not m3.
     *
     * \param xSI is the double precision value, expressed in SI units, to which
     * the current value of the indexed variable will be set.
     */
    void setValueMetric( const double &xSI );

    /**
     * This function allows a calling program to determine the \em function, if any,
     * which a \em variableDef's value is based.  It should not be required during
     * normal computations, but is used internally within the Janus instance and
     * may be used by other programs during XML dataset development.
     *
     * \return The index to Function instance, within the top-level Janus instance,
     * upon which this #VariableDef instance depends is returned.  If the
     * #VariableDef is not based on a tabular function, an invalid index is returned.
     */
    dstoute::aOptionalSizeT getFunctionRef( ) const
    { return functionRef_; }

    /**
     * This function allows an external application to set the \em functionDef
     * for the #VariableDef instance.  It is used internally within the
     * Janus instance and may be used by other programs during XML dataset development.
     *
     * \param functionRef The index within the top-level Janus instance of a \em function
     * instance upon which this #VariableDef instance depends.
     */
    void setFunctionRef( const dstoute::aOptionalSizeT& functionRef);

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance during computation, changing the output
     * scale factor within a VariableDef instance so that values that result
     * from MathML computations can be appropriately scaled.
     *
     * Care should be taken when using this function as use by an external program
     * could result in corruption of data.
     *
     * \param factor is a multiplicative constant to be applied to the computed
     * value of this variable during the computation process.
     */
    void setOutputScaleFactor( const double& factor )
    { outputScaleFactor_ = factor; hasOutputScaleFactor_ = true;}

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance during computation, accessing the output
     * scale factor within a VariableDef instance so that values that result
     * from MathML computations can be appropriately scaled.  The whole concept
     * of output scale factors is fraught with problems, and they should not be
     * used unless absolutely necessary.
     *
     * \return The current multiplicative constant to be applied to this variable
     * during computation of its value is returned as a double precision number.
     */
    const double& getOutputScaleFactor( ) const
    { return outputScaleFactor_; }

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance during initialisation, setting up
     * the self-reference for each variable.  This is necessary for Gaussian
     * correlation determination.  Use in other circumstances will result in
     * data corruption.
     *
     * \param index is the offset of this #VariableDef instance within the Janus
     * instance, set during initialisation and passed to each #VariableDef instance
     * for use during computation.
     */
    void setVarIndex( const dstoute::aOptionalSizeT& index ) { varIndex_ = index; }

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance during initialisation, setting up
     * cross-references of ultimately dependent variables.  Use in other
     * circumstances may result in data corruption.
     *
     * \param descendantsRef is a vector of \em variableDef indices within the
     * Janus instance, computed by Janus during the final stages of initialisation
     * and passed to each #VariableDef instance for use during computation.
     */
    void setDescendantsRef( const std::vector<size_t>& descendantsRef )
    { descendantsRef_ = descendantsRef; }

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance during initialisation, setting up
     * cross-references of ultimately contributing variables.  Use in other
     * circumstances may result in data corruption.
     *
     * \param ancestorsRef is a vector of VariableDef indices within the Janus
     * instance, computed by Janus during the final stages of initialisation
     * and passed to each VariableDef for use during computation.
     */
    void setAncestorsRef( const std::vector<size_t>& ancestorsRef )
    { ancestorsRef_ = ancestorsRef; }

    /**
     * The function should not be used by external programs. It is designed for
     * use within a Janus instance during initialisation, setting up \em MathML
     * cross-references to \em variable elements defined using the \em 'ci' tag.
     */
    void setMathMLDependencies();

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance during initialisation, setting up
     * a varaible's Uncertainty based on the XML dataset content.  Use in other
     * circumstances may result in data corruption.
     *
     * \param hasUncertaintyArg is a Boolean indication of the presence of an
     * \em uncertainty element associated with a \em variableDef in an XML dataset.
     */
    void setHasUncertainty( const bool& hasUncertaintyArg )
    { hasUncertainty_ = hasUncertaintyArg; }

    /**
     * This function indicates to the calling function is the variable has
     * been evaluated and its value is current, or whether the variable needs
     * to be re-evaluated.
     */
    const bool& isCurrent( )
    { return isCurrent_;}

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance, maintaining consistency between variable
     * condition flags as different input variables are set and different output
     * variables are computed.  Use in other circumstances may result in data
     * corruption.
     */
    void setNotCurrent()
    { isCurrent_ = false; isCurrentVariance_ = false; isCurrentBound_ = false;}

    /**
     * This function should not be used by external programs.  It is designed for
     * use within a Janus instance, maintaining consistency between variable
     * condition flags as different input variables are set and different output
     * variables are computed.  Use in other circumstances may result in data
     * corruption.
     */
    void setForced( bool isForced)
    { isForced_ |= isForced;}

    /**
     * This is used by external programs / classes to set that a VariablDef is being used.
     * This is used to suppress warnings about input variables not being used.
     */
    void setReferencedExternally( bool isReferencedExternally)
    { isReferencedExternally_ |= isReferencedExternally;}

    bool isReferencedExternally() const { return isReferencedExternally_;}

    /**
     * As an extension to the normal behaviour of a DAVE-ML gridded table,
     * support has been included for managing a table of strings in a similar
     * manner to numeric tabular data. The strings are accessed in the same way
     * as a numeric tabular function. The array of strings may be
     * multi-dimensionsional, and its breakpoints in each dimension should be
     * monotonic sequences of integers (\latexonly $ 1, 2, 3, \ldots n $\endlatexonly
     * \htmlonly 1, 2, 3, ..., <var>n</var>\endhtmlonly is a good choice),
     * where the product of the breakpoint array lengths equals the number
     * of strings. The independent variables must lie within the ranges of their
     * corresponding breakpoints, and must be set to require ``discrete''
     * interpolation.
     *
     * The strings can be delimited by any of: tab, newline, comma, semicolon.
     * DO NOT start or end the strings with excess whitespace.
     *
     * Janus detects a string table by looking for non-numeric characters, so a table
     * consisting entirely of numeric data will never be detected as a string.
     *
     * \return The required row of the string table, selected based on the
     * current input state of the parent Janus instance, is returned by
     * reference.
     */
    const dstoute::aString& getStringValue();

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em variableDef data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream& os,
                                      const VariableDef& variableDef);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

    /**
     * This is used for speed test purposes only. DO NOT USE.
     */
    void setForceUseOfMatrixCode( bool useMatrixOps = true) { hasMatrixOps_ = useMatrixOps;}

    // Functions to support Uncertainty calculations - FOR INTERNAL USE ONLY
    void setValueForUncertainty( const double &x);

    /*
     * Script functions.
     */
    const dstoute::aString& getScript() const  { return script_;}
    ScriptType getScriptType() const           { return scriptType_;}
    void testForMathML_to_ExprTkScript();
    Janus* getJanusInstance()                  { return janus_;}

    /*
     * Script and perturbation functions.
     */
    void   setInEvaluation( bool inEvaluation) { inEvaluation_ = inEvaluation;}
    bool   getInEvaluation( ) const            { return inEvaluation_;}

    /*
     * Perturbation functions.
     */
    void setPerturbation( UncertaintyEffect uncertaintyEffect, const double& value);

    // If target of perturbation
    void setPerturbationVarIndex( const size_t index);
    bool hasPerturbation() const                                  { return hasPerturbation_;}
    dstoute::aOptionalSizeT getPerturbationVarIndex() const       { return associatedPerturbationVarIndex_;}

    // If source of perturbation
    dstoute::aOptionalSizeT getPerturbationTargetVarIndex() const { return perturbationTargetVarIndex_;}

   protected:
    // ---- Internally reference functions. ----

    void initialiseBaseVariableDef( const DomFunctions::XmlNode& variableDefElement);
    void instantiateDataTable();
    void evaluateDataTable() const;
    double evaluateDataEntry( const dstoute::aString &dataEntry);

  // private:
    // ---- Internally reference functions. ----

    // This function is called during run-time evaluations of MathML-based
    // variables. The function traverses the linked list of representations
    // of functions set up by #setMath and performs indicated numeric
    // computations in sequence.  This function will frequently be called
    // recursively.
    void solveValue() const;
    void solveMath() const;

    void initialiseExprTkScript();
    void deleteExprTkFunction();
    void compileExprTkFunction();
    void solveExprTkScript() const;
    void preExprtkParser( const bool firstPass);
    void exportScript( DomFunctions::XmlNode& documentElement);

    void initialiseLuaScript();
    void deleteLuaFunction();
    void initLuaState() const;
    void initLuaFunction() const;
    void solveLuaScript() const;

    bool hasMatrixOps() const;
    bool hasMatrixOps( const dstomathml::MathMLData &t) const;
    void initialiseCalculation( const DomFunctions::XmlNode& xmlElement);
    void initialiseScript( const DomFunctions::XmlNode& xmlElement);
    void initialiseArray( const DomFunctions::XmlNode& xmlElement);
    void initialiseModel( const DomFunctions::XmlNode& xmlElement);
    void initialiseMath( const DomFunctions::XmlNode& xmlElement);
    void exportMath( DomFunctions::XmlNode& documentElement);

    // This is an internal function that, for a variable having Gaussian
    // uncertainty either directly or by propagation through functions, returns
    // the variable's variance based on the current state of the parent Janus
    // instance.  As well as returning the requested value, it sets the relevant
    // uncertainty values within the parent Janus instance and flags them
    // as valid.
    // A variable with uniform uncertainty set, either directly or indirectly,
    // will return a Gaussian variance of zero.  A variable with no uncertainty
    // set, either directly or indirectly, will return a variance of zero.
    //
    // \return A double precision value containing the value of the variance
    // after all relevant computations based on the current input state.
    const double& getVariance();
    double computeVarianceForNormalPdf();

    // This is an internal function that, for a variable having uniform
    // uncertainty either directly or by propagation through functions, returns
    // the variable's additive bounds based on the current state of the parent
    // Janus instance.  As well as returning the requested value, it sets the
    // relevant function and uncertainty values within the parent Janus instance
    // and flags them as valid.
    // A variable with Gaussian uncertainty set, either directly or indirectly,
    // will return a uniform bound of zero.  A variable with no uncertainty
    // set, either directly or indirectly, will return a bound of zero.
    //
    // \param isUpper is a Boolean value indicating whether the upper or lower
    // bound of the selected variable is required.
    //
    // \return A double precision value containing the value of the requested
    // additive bound after all relevant computations based on the current
    // input state.
    const double& getAdditiveBounds( const bool& isUpper );
    void computeAdditiveBoundsForUniformPdf();

    // This internal function numerically computes the gradient of an
    // output variable with respect to an input variables.  It is
    // required as part of the normal PDF uncertainty computation, when
    // only input uncertainties are supplied and output uncertainty is
    // required.  It should only be called for input variables having
    // normal PDFs.
    //
    // \param index has a range from 0 to
    // (#getNumberOfVariables( ) - 1), and selects the independent input
    // variable to be addressed.  Selection of a variable that does not
    // form an input to the output will result in a zero Jacobian.
    //
    // \return The result is the
    // partial derivative of the output variable with respect to the
    // selected input variable.
    double getJacobianComponent( const dstoute::aOptionalSizeT& index );

    bool isCalculation() const;
    bool isInitialValueAllowed() const;

    /************************************************************************
     * These are the variableDef elements, set up during instantiation.
     */
    Janus* janus_;
    DomFunctions::XmlNode domElement_;
    ElementDefinitionEnum elementType_;

    dstoute::aOptionalSizeT varIndex_;
    dstoute::aString        name_;
    dstoute::aString        varID_;
    dstoute::aString        units_;
    dstoute::aString        axisSystem_;
    dstoute::aString        sign_;
    dstoute::aString        alias_;
    dstoute::aString        symbol_;
    dstoute::aString        initialValueString_;
    dstoute::aString        minValueString_;
    dstoute::aString        maxValueString_;
    double                  initialValue_;
    double                  minValue_;
    double                  maxValue_;

    dstoute::aString description_;
    bool             isProvenanceRef_;
    bool             hasProvenance_;
    Provenance       provenance_;

    bool isInput_;
    bool isControl_;
    bool isDisturbance_;
    bool isOutput_;
    bool isState_;
    bool isStateDeriv_;
    bool isStdAIAA_;
    bool hasUncertainty_;
    bool exportUncertainty_;

    Uncertainty    uncertainty_;
    VariableType   variableType_;
    VariableMethod variableMethod_;

    /*
     * cross-references
     */
    dstoute::aOptionalSizeT functionRef_;
    std::vector<size_t>     independentVarRef_;
    std::vector<size_t>     descendantsRef_;
    std::vector<size_t>     ancestorsRef_;
    double                  outputScaleFactor_;
    bool                    hasOutputScaleFactor_;

    //
    // Need more "const" functions. Nearly all Janus functions should be const,
    //   except for those functions that initialise Janus from DOMs and setValues().
    //
    // The mutable variables below are variables that internally change state when functions like getValue()
    // are called. getValue() is effectively a "const", but there are some house-keeping variables
    // that need updating.
    //

    /*
     * value and status elements - changeable during run time
     */
    mutable bool   isCurrent_;
    mutable double value_;
    mutable bool   isCurrentVariance_;
    double variance_;
    bool   isCurrentBound_;
    double lowerBound_;
    double upperBound_;
    bool   isForced_;
    bool   isReferencedExternally_;

    /*
     * These are the Matrix elements, set up during instantiation.
     */
    bool isDimensionRef_;
    bool hasDimensionDef_;
    bool hasVarIdEntries_;

    DimensionDef dimensionDef_;
    Array        array_;
    Model        model_;

    mutable bool isMatrix_;
    mutable bool hasMatrixOps_;

    typedef dstomath::aMatrix<dstoute::aOptionalSizeT> IdxMatrix;

    mutable dstomath::DMatrix matrix_;
    dstomath::DMatrix matrixScaleFactor_;
    IdxMatrix         matrixVarId_;

    mutable dstoute::aUnits xmlUnits_;

    /*
     * MathML elements
     */
    dstomathml::MathMLData mathCalculation_;

    /*
     * Script elements
     */
    ScriptType scriptType_;
    dstoute::aString script_;
    bool scriptContainsThisVarDef_;
    mutable const VariableDef *thisVarDef_;
    mutable void *eFunc_;
    mutable void *kFunc_;
    bool isCurrentable_;

    /*
     * Script and perturbation elements
     */
    bool inEvaluation_;

    /*
     * Perturbation elements 
     */
    // If target of perturbation
    bool                    hasPerturbation_;
    dstoute::aOptionalSizeT associatedPerturbationVarIndex_;
    void applyPerturbation() const;

    // If source of perturbation
    dstoute::aOptionalSizeT perturbationTargetVarIndex_;
    UncertaintyEffect       perturbationEffect_;
    void initialisePerturbation( const DomFunctions::XmlNode& xmlElement);

   private:
    bool inputWarningDone_;
    bool outputWarningDone_;
  };

  typedef std::vector<VariableDef> VariableDefList;

  dstoute::aString parseMathML_to_ExprTkScript(
    const DomFunctions::XmlNode& xmlElement, const dstoute::aString &tag, bool isFirstCall = false);
}

#endif /* _VARIABLEDEF_H_ */
