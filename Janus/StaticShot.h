#ifndef _STATICSHOT_H_
#define _STATICSHOT_H_

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
// Title:      Janus/StaticShot
// Class:      StaticShot
// Module:     StaticShot.h
// First Date: 2009-05-08
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file StaticShot.h
 *
 * A StaticShot instance holds in its allocated memory alphanumeric data
 * derived from a \em staticShot element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance describes
 * the inputs and outputs, and possibly internal values, of a DAVE-ML model
 * at a particular instant in time.  The class also provides the functions
 * that allow a calling Janus instance to access these data elements.
 *
 * The StaticShot class is only used within the janus namespace, and should
 * only be referenced indirectly through the CheckData class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Ute Includes
#include <Ute/aList.h>

//local includes
#include "XmlElementDefinition.h"
#include "Provenance.h"
#include "CheckInputs.h"
#include "InternalValues.h"
#include "CheckOutputs.h"
#include "VariableDef.h"

namespace janus {

//typedef std::pair< int, double> InputPair;
typedef std::pair< dstoute::aOptionalSizeT, VariableDef> InputPair;

  class Janus;

  /**
   * XML datset content verification - static input / output correlation.
   * A #StaticShot instance holds in its allocated memory alphanumeric data
   * derived from a \em staticShot element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  The instance describes
   * the inputs and outputs, and possibly internal values, of a DAVE-ML model
   * at a particular instant of time.  The class also provides the functions
   * that allow a calling Janus instance to access these data elements.
   *
   * The #StaticShot class is only used within the janus namespace, and should
   * only be referenced indirectly through the CheckData class.
   *
   * Units used in the \em staticShot element need not be identical to those
   * used in the remainder of the dataset.
   *
   * Typical usage:
  \code
    Janus test( xmlFileName );
    CheckData checkData = test.getCheckData();
    size_t nss = checkData.getStaticShotCount( );
    for ( size_t j = 0 ; j < nss ; j++ ) {
      StaticShot staticShot = checkData.getStaticShot( j );
      size_t nInvalid = staticShot.getInvalidVariableCount();
      if ( 0 < nInvalid ) {
        for ( size_t k = 0 ; k < nInvalid ; k++ ) {
          string failVarID = staticShot.getInvalidVariable(  k );
          cout << "  Problem at varID : " << failVarID << endl;
        }
      }
      else {
        cout << "  No problems from static shot " << j << " ... " << endl;
      }
    }
  \endcode
   */
  class StaticShot : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #StaticShot class without
     * supplying the DOM \em staticShot element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em staticShot element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    StaticShot( );

    /**
     * The constructor, when called with an argument pointing to a \em staticShot
     * element within a DOM, instantiates the #StaticShot class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em staticShot component within
     * the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    StaticShot( const DomFunctions::XmlNode& elementDefinition,
                Janus* janus);

    /**
     * An uninitialised instance of #StaticShot is filled with data from a
     * particular \em staticShot element within a DOM by this function.  If
     * another \em staticShot element pointer is supplied to an
     * instance that has already been initialised, the
     * instance will be re-initialised with the new data.  However, this is not
     * a recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address of a \em staticShot component within
     * the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
                               Janus* janus);

    /**
     * This function returns the \em name attribute of a \em staticShot.  If
     * the static shot has not been initialised from a DOM, an
     * empty string is returned.
     *
     * \return The \em name string is returned by reference.
     */
    const dstoute::aString& getName( ) const { return name_; }

    /**
     * The \em refID attribute is an optional document reference for a
     * \em staticShot.
     * This function returns the \em refID of a \em staticShot
     * element, if one has been supplied in the XML dataset.  If not, or if the
     * StaticShot has not been initialised from a DOM, it
     * returns an empty string.
     *
     * \return The \em refID string is returned by reference.
     */
    const dstoute::aString& getRefID( ) const { return refID_; }

    /**
     * This function returns the \em description of the referenced \em staticShot
     * element, if one has been supplied in the XML dataset.  If not, or if the
     * StaticShot has not been initialised from a DOM, it
     * returns an empty string.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function indicates whether a \em staticShot element of a
     * DAVE-ML dataset includes either \em provenance or \em provenanceRef.
     *
     * \return A boolean variable, 'true' if the \em staticShot includes a
     * provenance, defined either directly or by reference.
     *
     * \sa Provenance
     */
    const bool& hasProvenance( ) const { return hasProvenance_; }

    /**
     * This function provides access to the Provenance instance
     * associated with a #StaticShot instance.  There may be zero or one
     * of these elements for each static shot in a valid dataset, defined
     * either directly or by reference.
     *
     * \return The Provenence instance is returned by reference.
     *
     * \sa Provenance
     */
    const Provenance& getProvenance( ) const { return provenance_; }

    /**
     * This function provides access to the \em checkInputs element
     * contained in a DAVE-ML \em staticShot element.  There must be one
     * of these elements for each static shot in a valid dataset.
     *
     * \return The CheckInputs instance is returned by reference.
     */
    const CheckInputs& getCheckInputs( ) const { return checkInputs_; }

    /**
     * This function indicates whether a \em staticShot element of a
     * DAVE-ML dataset includes \em internalValues.
     *
     * \return A boolean variable, 'true' if the \em staticShot includes
     * internal values.
     */
    bool hasInternalValues( ) const
    { return !internalValuesValid_.empty();}

    /**
     * This function provides access to the \em internalValues element
     * contained in a DAVE-ML \em staticShot element.  There may be zero or one
     * of these elements for each static shot in a valid dataset.
     *
     * \return The InternalValues instance is returned by reference.
     */
    const InternalValues& getInternalValues( ) const { return internalValues_; }

    /**
     * This function provides access to the \em checkOutputs element
     * contained in a DAVE-ML \em staticShot element.  There must be one
     * of these elements for each static shot in a valid dataset.
     *
     * \return The CheckOutputs instance is returned by reference.
     */
    const CheckOutputs& getCheckOutputs( ) const { return checkOutputs_; }

    /**
     * This function uses the contents of a \em staticShot element within
     * a DAVE-ML compliant dataset to verify the functional relationships
     * within the remainder of the dataset.  It is called as part of the
     * Janus instantiation process to flag problems in the dataset content.
     * It should not normally be called directly by other classes.
     * The results of the tests performed are placed in internal arrays for
     * access by other Janus functions.
     *
     * \sa getInvalidVariableCount
     * \sa getInvalidVariable
     */
    void verifyStaticShot( );

    /**
     * This function uses the results computed by setStaticShotVerification() to
     * indicate how many internal or output values computed in accordance with a
     * DAVE-ML compliant dataset are incompatible with the values contained in
     * this \em staticShot element.
     *
     * \return An positive integer value is returned, being the total number of internal
     * or output \em variableDefs in the dataset whose values are not compatible,
     * within the specified tolerences, with the specified \em staticShot values.
     *
     * \sa getInvalidVariable
     * \sa setStaticShotVerification
     */
    size_t getInvalidVariableCount( ) const;

    /**
     * This function uses the results computed by setStaticShotVerification() to
     * indicate which internal or output values, computed in accordance with a
     * DAVE-ML compliant dataset, are incompatible with the values contained in
     * this \em staticShot element.  It is used in conjunction with
     * #getInvalidVariableCount and other CheckData functions.
     *
     * \param index has a range from 0 to ( #getInvalidVariableCount() - 1 ), and
     * selects the invalid variable whose identity is required.
     *
     * \return A string is returned by reference, containing either the
     * \em name or \em varID attribute, as applicable, of the selected invalid
     * variable.
     */
    dstoute::aString getInvalidVariableMessage( int index) const;

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em StaticShot data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const StaticShot &staticShot);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

   private:
    void evaluateSignals();

    void processSignals( const SignalTypeEnum& signalType,
                         aListSignals& sgnls);

    void processSignalList( const SignalList& signalList);
    void processSignalList( const SignalTypeEnum& signalType);

    dstoute::aString invalidMessageSignals( const aListSignals &sgnls,
                                            const SignalTypeEnum& signalType,
                                            const size_t sIndex) const;

    dstoute::aString invalidMessageSignalList( const SignalTypeEnum& signalType,
                                               const size_t sIndex) const;

    void copyCheckInputVariableDefs();
    void resetCheckInputVariableDefs() const;
    void checkSignalConsistency();
    void checkVectorMatrixConsistency( const SignalList& signalList);


    /************************************************************************
     * These are pointers to the staticShot elements, set up during
     * instantiation. */
    Janus* janus_;
    ElementDefinitionEnum  elementType_;
    SignalTypeEnum         signalType_;
  
    dstoute::aString       name_;
    dstoute::aString       refID_;
    dstoute::aString       description_;
    bool                   hasProvenance_;
    Provenance             provenance_;
    CheckInputs            checkInputs_;
    InternalValues         internalValues_;
    CheckOutputs           checkOutputs_;
    dstoute::aList<bool>   internalValuesValid_;
    dstoute::aList<bool>   checkOutputsValid_;

    std::vector< InputPair > copyInputSignal_;
  };
  
  typedef dstoute::aList<StaticShot> StaticShotList;

}

#endif /* _STATICSHOT_H_ */
