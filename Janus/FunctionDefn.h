#ifndef _FUNCTIONDEFN_H_
#define _FUNCTIONDEFN_H_

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
// Title:      Janus/FunctionDefn
// Class:      FunctionDefn
// Module:     FunctionDefn.h
// First Date: 2011-11-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file FunctionDefn.h
 *
 * A FunctionDefn instance holds in its allocated memory alphanumeric data
 * derived from a \em functionDefn element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  Each function stores
 * function data elements.

 * The FunctionDefn class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Ute Includes
#include <Ute/aList.h>
#include <Ute/aOptional.h>

// Local Includes
#include "XmlElementDefinition.h"

namespace janus
{

  class Janus;

  /**
   * A #FunctionDefn instance holds in its allocated memory alphanumeric data
   * derived from a \em functionDefn element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  Each function stores
   * function data elements.

   * The #FunctionDefn class is only used within the janus namespace, and should
   * only be referenced through the Janus class.
   *
   * Janus exists to abstract data form and handling from a modelling process.
   * Therefore, in normal computational usage, it is unnecessary and undesirable
   * for a calling program to even be aware of the existence of this class.
   * However, functions do exist to access #FunctionDefn contents directly, which
   * may be useful during dataset development.  A possible usage might be:
    \code
      Janus test( xmlFileName );
      const vector<Function>& function = test.getFunction();
      for ( int i = 0 ; i < function.size() ; i++ ) {
        cout << " Function " << i << " :\n"
             << "   name        = " << function.at( i ).getName() << "\n"
             << "   description = " << function.at( i ).getDescription() << "\n";
        cout << "   Number of independent variables = "
             << function.at( i ).getIndependentVarCount() << "\n";
        for ( int j = 0 ; j < function.at( i ).getIndependentVarCount() ; j++ ) {
          cout << "   Input variable " << j << " varID = "
               << test.getVariableDef().
                    at( function.at( i ).getIndependentVarRef( j ) ).getVarID()
               << "\n";
        }
      }
    \endcode
   */
  class FunctionDefn: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #FunctionDefn class without
     * supplying the DOM \em function element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em functionDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    FunctionDefn();

    /**
     * The constructor, when called with an argument pointing to a \em functionDefn
     * element within a DOM, instantiates the #FunctionDefn class and fills
     * it with alphanumeric data from the DOM.  String-based cross-references
     * as implemented in the XML dataset are converted to index-based
     * cross-references to improve computational performance.
     *
     * \param elementDefinition is an address of a \em functionDefn
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    FunctionDefn( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This functionDefn populates a #FunctionDefn instance based on the corresponding
     * \em functionDefn element of the DOM, defines the cross-references from the
     * Function to variables and breakpoints, and sets up arrays
     * that will later be used in run-time function evaluation.  If another
     * \em functionElement pointer is supplied to an instance that has already
     * been initialised, data corruption will occur and the entire Janus
     * instance will become unusable.
     *
     * \param elementDefinition is an address of a \em function
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to \em name attribute of
     * a \em functionDefn. If the functionDefn has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em name string is returned by reference.
     */
    const dstoute::aString& getName() const
    {
      return name_;
    }

    /**
     * This function returns the column number associated with the
     * dependent data of an ungridded table, that has been defined
     * for the \em functionDefn instance using an ungridded table
     * reference. This parameter may be non-zero if the ungridded
     * table has multiple dependent data columns.
     *
     * \return The column index (size_t) of the particular dependent data
     * parameter within the the list of dependent data of an
     * ungridded table associated with the \em functionDefn instance.
     */
    const size_t& getDependentDataColumnNumber() const
    {
      return dependentDataColumnNumber_;
    }

    /**
     * This function returns an enumeration defining the type of
     * data table associated with the \em functionDefn instance. The
     * enumeration will differentiate the data table as being
     * either gridded or ungridded.
     *
     * \return An enumeration defining the form of the data table.
     */
    const ElementDefinitionEnum& getTableType() const
    {
      return tableType_;
    }

    /**
     * This function is used to set the form of the data table associated
     * with the \em functionDefn instance using a enumeration. This
     * function is used internally within Janus while instantiating a
     * DAVE-ML compliant XML dataset source file.
     *
     * \param tableType is a enumeration defining the form of the data table
     * associated with the \em functionDefn instance.
     */
    void setTableType(
      const ElementDefinitionEnum& tableType)
    {
      tableType_ = tableType;
    }

    /**
     * This function returns a reference identifier for the table forming
     * the basis of the \em functionDefn instance. The reference is the table
     * identifier, being either the gtID attribute of a gridded table, or
     * the utID attribute of an ungridded table. This reference is used
     * internally within Janus to identify the particular table within the
     * list of tables encoded within the Janus instance.
     *
     * \return A string representing the tables identifier.
     */
    const dstoute::aString& getTableReference() const
    {
      return tableReference_;
    }

    /**
     * This function returns an index to the table forming the basis of
     * the \em functionDefn instance. This index identifies the particular
     * table within the list of tables encoded within the Janus instance.
     * This function is used internally within Janus when evaluating a
     * Function instance.
     *
     * \return An integer index to the table used to evaluate a Function instance.
     */
    dstoute::aOptionalSizeT getTableIndex() const
    {
      return tableIndex_;
    }

    /**
     * This function is used to set the reference identifier of the table forming
     * the basis of the \em functionDefn instance. The reference is the table
     * identifier, being either the gtID attribute if a gridded table, or
     * the utID attribute of an ungridded table.  This function is used
     * internally within Janus while instantiating a DAVE-ML compliant XML
     * dataset source file.
     *
     * \param xReference is the reference identifier of the table associated
     * with the \em functionDefn instance.
     */
    void setTableReference(
      const dstoute::aString &xReference)
    {
      tableReference_ = xReference;
    }

    /**
     * This function is used to set the index identifying the table from
     * within the list of tables encoded in the Janus instance. This function
     * is used internally within Janus while instantiating a DAVE-ML compliant XML
     * dataset source file.
     *
     * \param tableIndex is the integer index from the list of tables encoded
     * wthin the Janus instance of the table associated with the \em functionDefn
     * instance
     */
    void setTableIndex( dstoute::aOptionalSizeT tableIndex)
    {
      tableIndex_ = tableIndex;
    }

    /**
     * This function is used to export the \em functionDefn data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition(
      DomFunctions::XmlNode& documentElement);

    // @TODO :: Add set parameter functions

    // ---- Display functions. ----
    // This function displays the contents of the class
    //  friend std::ostream& operator<< ( std::ostream &os, const FunctionDefn &functionDefn);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom(
      const DomFunctions::XmlNode& elementDefinition);
    bool compareElementID(
      const DomFunctions::XmlNode& xmlElement,
      const dstoute::aString& elementID,
      const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus(
      Janus* janus)
    {
      janus_ = janus;
    }

  protected:

  private:

    /************************************************************************
     * These are the function elements, set up during instantiation.
     */
    Janus* janus_;
    ElementDefinitionEnum elementType_;

    dstoute::aString name_;

    ElementDefinitionEnum tableType_;
    dstoute::aString tableReference_;
    dstoute::aOptionalSizeT tableIndex_;
    size_t dependentDataColumnNumber_;
  };

  typedef dstoute::aList< FunctionDefn> FunctionDefnList;

}

#endif /* _FUNCTIONDEFN_H_ */
