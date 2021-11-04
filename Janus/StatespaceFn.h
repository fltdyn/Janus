#ifndef _STATESPACEFN_H_
#define _STATESPACEFN_H_

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
// Title:      Janus/StatespaceFn
// Class:      StatespaceFn
// Module:     StatespaceFn.h
// First Date: 2015-11-18
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file StatespaceFn.h
 *
 * An StatespaceFn instance holds in its allocated memory alphanumeric data
 * derived from a dynamic systems model \em statespaceFn element of a DOM
 * corresponding to a DAVE-ML compliant XML dataset source file.
 *
 * It includes entries arranged as follows:
 * ...
 *
 * The StatespaceFn class is only used within the janus namespace, and should only
 * be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 */

// Ute Includes
#include <Ute/aList.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "Provenance.h"

namespace janus
{

  class Janus;

  /**
   * An #StatespaceFn instance holds in its allocated memory alphanumeric data
   * derived from a dynamic systems model \em statespaceFn element of a DOM
   * corresponding to a DAVE-ML compliant XML dataset source file.
   *
   * It includes entries arranged as follows:
   * .....
   *
   * The #StatespaceFn class is only used within the janus namespace, and should only
   * be referenced through the Janus class.
   */

  class StatespaceFn: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #StatespaceFn class
     * without supplying the dynamic system model DOM \em statespaceFn element
     * from which the instance is constructed, but in this state it is not useful for
     * any class functions.  It is necessary to populated the class from
     * a DOM containing an \em statesspaceFn element before any further use of
     * the instanced class.
     *
     * This form of the constructor is principally for use within
     * higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    StatespaceFn();

    /**
     * The constructor, when called with an argument pointing to a dynamic system
     * model \em statespaceFn element within a DOM, instantiates the
     * #StatespaceFn class and fills it with alphanumeric data from the DOM.
     * String-based numeric data are converted to double-precision linear
     * vectors.
     *
     * \param elementDefinition is an address of an \em statespaceFn
     * component node within the DOM.
     */
    StatespaceFn( const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #StatespaceFn is filled with data from a
     * particular dynamic systems model \em statespaceFn element within a DOM
     * by this function.  If another \em statespaceFn element pointer is supplied
     * to an instance that has already been initialised, data corruption will
     * occur and the entire Janus instance will become unusable.
     *
     * \param elementDefinition is an address of an \em statespaceFn
     * component node within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em name attribute of the
     * \em statespaceFn element represented by this #StatespaceFn instance.
     * A statespaceFn's \em name attribute is a string of arbitrary
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
     * This function provides access to the \em ssID attribute
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * A statespaceFn's \em ssID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * property.  It is used for indexing variables within an XML dataset,
     * and provides underlying cross-references for most of the Janus library
     * functionality.  If the instance has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em ssID string is returned by reference.
     */
    const dstoute::aString& getSSID( ) const { return ssID_; }

    /**
     * This function provides access to the 'x' state vector entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element that
     * contains the values of the state derivative equation state vector.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The state vector \em varID string is returned by reference.
     */
    const dstoute::aString& getStateVectorID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_STATE_VECTOR) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_STATE_VECTOR);
    }

    /**
     * This function provides access to the 'x_dot' state derivative vector entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the values from evaluating the state derivative equation.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The state derivative vector \em varID string is returned by reference.
     */
    const dstoute::aString& getStateDerivVectorID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_STATEDERIV_VECTOR) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_STATEDERIV_VECTOR);
    }

    /**
     * This function provides access to the 'w' disturbance vector entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the values of the state derivative and output equation
     * disturbance vector.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The disturbance vector \em varID string is returned by reference.
     */
    const dstoute::aString& getDisturbanceVectorID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_DISTURBANCE_VECTOR) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_DISTURBANCE_VECTOR);
    }

    /**
     * This function provides access to the 'A' state Matrix entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the coefficients (constant or varying) of the
     * state derivative equation state plant matrix.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The state matrix \em varID string is returned by reference.
     */
    const dstoute::aString& getStateMatrixID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_STATE_MATRIX) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_STATE_MATRIX);
    }

    /**
     * This function provides access to the 'B' input control Matrix entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the coefficients (constant or varying) of the
     * state derivative equation input control matrix.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The input matrix \em varID string is returned by reference.
     */
    const dstoute::aString& getInputMatrixID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_INPUT_MATRIX) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_INPUT_MATRIX);
    }

    /**
     * This function provides access to the state derivative disturbance Matrix 'F' entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the coefficients (constant or varying) of the
     * state derivative equation disturbance matrix.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The disturbance matrix \em varID string is returned by reference.
     */
    const dstoute::aString& getStateDerivDisturbanceMatrixID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_SD_DISTURBANCE_MATRIX) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_SD_DISTURBANCE_MATRIX);
    }

    /**
     * This function provides access to the 'C' output Matrix entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the coefficients (constant or varying) of the
     * output equation state plant matrix.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The state matrix \em varID string is returned by reference.
     */
    const dstoute::aString& getOutputMatrixID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_OUTPUT_MATRIX) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_OUTPUT_MATRIX);
    }

    /**
     * This function provides access to the 'D' direct (feed through) Matrix entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the coefficients (constant or varying) of the
     * output equation direct (feed through) matrix.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The direct matrix \em varID string is returned by reference.
     */
    const dstoute::aString& getDirectMatrixID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_DIRECT_MATRIX) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_DIRECT_MATRIX);
    }

    /**
     * This function provides access to the output disturbance Matrix 'H' entry
     * of the \em statespaceFn element represented by this #StatespaceFn instance.
     * The identifier is a \em varID to an existing \em variableDef element
     * that contains the coefficients (constant or varying) of the
     * output equation disturbance matrix.
     *
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The disturbance matrix \em varID string is returned by reference.
     */
    const dstoute::aString& getOutputDisturbanceMatrixID( ) const
    {
      if ( statespaceVarRefs_.size() <= SS_OUTPUT_DISTURBANCE_MATRIX) {
        return EMPTY_STRING;
      }
      return statespaceVarRefs_.at( SS_OUTPUT_DISTURBANCE_MATRIX);
    }

    /**
     * This function provides access to the optional \em description of the
     * \em statespaceFn element represented by this #StatespaceFn instance.
     * A \em statespaceFn's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #StatespaceFn has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function indicates whether a \em statespaceFn element of a
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
     * associated with a #StatespaceFn instance.  There may be zero or one
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
     * This function permits the \em name attribute
     * of the \em statespaceFn element to be reset for this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param name a string representation of the \em name attribute.
     */
    void setName( const dstoute::aString& name) { name_ = name; }

    /**
     * This function permits the \em ssID attribute
     * of the \em statespaceFn element to be reset for this #StatespaceFn instance.
     * A \em ssID attribute is normally a short
     * string without whitespace, such as "MACH02", that uniquely defines the
     * trransferFn.  It is used for indexing transfer functions within an XML dataset,
     * and provides underlying cross-references if required.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param ssID a string representation of the \em ssID identifier.
     */
    void setSSID( const dstoute::aString& ssID) { ssID_ = ssID; }

    /**
     * This function permits the \em varID to the 'x' state vector \em variableDef
     * to be set or reset for the \em statespaceFn element of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the state vector of the dynamic system model.
     */
    void setStateVectorID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty()) {
    	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_STATE_VECTOR).assign( varID);
    }

    /**
     * This function permits the \em varID to the 'x_dot' state derivative vector \em variableDef
     * to be set or reset for the \em statespaceFn element of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the state derivative vector of the dynamic system model.
     */
    void setStateDerivVectorID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty()) {
    	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_STATEDERIV_VECTOR).assign( varID);
    }

    /**
     * This function permits the \em varID to the 'w' disturbance vector \em variableDef
     * to be set or reset for the \em statespaceFn element of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the disturbance vector of the dynamic system model.
     */
    void setDisturbanceVectorID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty() || statespaceVarRefs_.size() <= SS_DISTURBANCE_VECTOR) {
    	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_DISTURBANCE_VECTOR).assign( varID);
    }

    /**
     * This function permits the \em varID to the 'A' state Matrix \em variableDef
     * to be set or reset for the \em statespaceFn element of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the state matrix of the dynamic system model.
     */
    void setStateMatrixID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty()) {
    	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_STATE_MATRIX).assign( varID);
    }

    /**
     * This function permits the \em varID to the 'B' input control Matrix \em variableDef
     * to be set or reset for the \em statespaceFn element of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an state derivative XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the input matrix of the dynamic system model.
     */
    void setInputMatrixID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty()) {
      	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_INPUT_MATRIX).assign( varID);
    }

    /**
     * This function permits the \em varID of the state derivative disturbance Matrix 'F'
     * \em variableDef to be set or reset for the \em statespaceFn element of this
     *  #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the state derivative disturbance matrix of the dynamic
     * system model.
     */
    void setStateDerivDisturbanceMatrixID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty() || statespaceVarRefs_.size() <= SS_DISTURBANCE_VECTOR) {
    	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_SD_DISTURBANCE_MATRIX).assign( varID);
    }

    /**
     * This function permits the \em varID to the 'C' output Matrix \em variableDef
     * to be set or reset for the \em statespaceFn element of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the output matrix of the dynamic system model.
     */
    void setOutputMatrixID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty()) {
      	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_OUTPUT_MATRIX).assign( varID);
    }

    /**
     * This function permits the \em varID to the 'D' direct control Matrix \em variableDef
     * to be set or reset for the \em statespaceFn element of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the direct (feed through) matrix of the dynamic system model.
     */
    void setDirectMatrixID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty()) {
      	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_DIRECT_MATRIX).assign( varID);
    }

    /**
     * This function permits the \em varID of the output disturbance Matrix 'H'
     * \em variableDef to be set or reset for the \em statespaceFn element
     * of this #StatespaceFn instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param varID a string identifier cross-referencing the \em variableDef
     * associated with the output disturbance matrix of the dynamic
     * system model.
     */
    void setOutputDisturbanceMatrixID( const dstoute::aString& varID)
    {
      if ( statespaceVarRefs_.empty() || statespaceVarRefs_.size() <= SS_DISTURBANCE_VECTOR) {
    	statespaceVarRefs_.resize( SS_OUTPUT_DISTURBANCE_MATRIX + 1);
      }
      statespaceVarRefs_.at( SS_OUTPUT_DISTURBANCE_MATRIX).assign( varID);
    }

    /**
     * This function permits the optional \em description of the
     * \em statespaceFn element to be reset for this #StatespaceFn instance.
     * A \em statespaceFn's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means pretty formatting of the XML source
     * will also appear in the returned description.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param description a string representation of the description.
     */
    void setDescription( const dstoute::aString& description)
    { description_ = description; }

    /**
     * This function is used to export the \em statespaceFn data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     * \param isReference a flag indicating if the exprt refers to a full
     * Statespace definition or a reference to a Statespace definition.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement,
                           const bool &isReference);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os, const StatespaceFn &statespaceFn);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

  private:
    enum SS_MODEL
    {
      SS_STATE_VECTOR,
      SS_STATEDERIV_VECTOR,
      SS_STATE_MATRIX,
      SS_INPUT_MATRIX,
      SS_OUTPUT_MATRIX,
      SS_DIRECT_MATRIX,
      SS_DISTURBANCE_VECTOR,
      SS_SD_DISTURBANCE_MATRIX,
      SS_OUTPUT_DISTURBANCE_MATRIX
    };

    /************************************************************************
     * These are the statespaceFn elements, set up during instantiation.
     */
    ElementDefinitionEnum elementType_;

    dstoute::aString name_;
    dstoute::aString ssID_;
    dstoute::aString description_;

    /*
     * The order of the variable referenc elements in this list is:
     *   x     : the state vector
     *   x_dot : the state derivative vector
     *   [A]   : the state matrix
     *   [B]   : the input matrix (control / stimulus) ??
     *   [C]   : the output matrix
     *   [D]   : the direct matrix (feed through)
     *   w     : the disturbance vector (optional)
     *   [F]   : the state derivative equation disturbance matrix (optional)
     *   [H]   : the output equation disturbance matrix (optional)
     *
     */
    dstoute::aStringList statespaceVarRefs_;
//    dstoute::aString stateMatrixID_;  // State Matrix
//    dstoute::aString outputMatrixID_; // Output Matrix
//    dstoute::aString inputMatrixID_;  // Input Matrix
//    dstoute::aString directMatrixID_; // Direct Matrix
//    dstoute::aString stateDerivDisturbanceMatrixID_; // State Derivative Disturbance Matrix
//    dstoute::aString outputDisturbanceMatrixID_;     // Output Disturbance Matr??

    bool             isProvenanceRef_;
    bool             hasProvenance_;
    Provenance       provenance_;
  };
}

#endif /* _STATESPACEFN_H_ */
