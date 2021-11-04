#ifndef _BOUNDS_H_
#define _BOUNDS_H_

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
// Title:      Janus/Bounds
// Class:      Bounds
// Module:     Bounds.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file Bounds.h
 *
 * A Bounds instance holds in its allocated memory alphanumeric data
 * derived from a \em bounds element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The element contains some
 * description of the statistical limits to the values the citing parameter
 * element might take on. This can be in the form of a scalar value, a
 * \em variableDef that provides a functional definition of the bound, a
 * \em variableRef that refers to such a functional definition, or a
 * private table whose elements correlate with those of a tabular function
 * defining the citing parameter.  The class also
 * provides the functions that allow a calling Janus instance to access
 * these data elements.
 *
 * The Bounds class is only used within the janus namespace, and should
 * only be referenced indirectly through the Uncertainty class or through
 * the variable functions within the Janus class.
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
  class VariableDef;

  /**
   * A #Bounds instance holds in its allocated memory alphanumeric data
   * derived from a \em bounds element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  The element contains some
   * description of the statistical limits to the values the citing parameter
   * element might take on. This can be in the form of a scalar value, a
   * \em variableDef that provides a functional definition of the bound, a
   * \em variableRef that refers to such a functional definition, or a
   * private table whose elements correlate with those of a tabular function
   * defining the citing parameter.  The class also
   * provides the functions that allow a calling Janus instance to access
   * these data elements.
   *
   * The #Bounds class is only used within the janus namespace, and should
   * only be referenced indirectly through the Uncertainty class or through
   * the variable functions within the Janus class.
   *
   * One possible usage of the #Bounds class might be:
    \code
      Janus test( xmlFileName );

      for ( int i = 0 ; i < test.getNumberOfVariables() ; i++ ) {
        Uncertainty::UncertaintyPdf pdf = test.getVariableDef().at( i ).
          getUncertainty().getPdf( );
        if ( Uncertainty::NORMAL_PDF == pdf ) {
          double bound = test.getVariableDef().at( i ).getUncertainty().
            getBounds().getBound();
          cout << " Gaussian bound = " << bound << "\n";
        }
        else if ( Uncertainty::UNIFORM_PDF == pdf ) {
          const vector<Bounds>& bounds = test.getVariableDef().at( i ).
            getUncertainty().getBounds();
          if ( 1 == bounds.size() ) {
            double symmetricBound = bounds.at( 0 ).getBound();
            cout << " Uniform symmetric bound = " << symmetricBound << "\n";
          }
          else {
            double lowerBound = bounds.at( 0 ).getBound();
            double upperBound = bounds.at( 1 ).getBound();
            cout << " Uniform bounds range = [ " << lowerBound << " to "
                 << upperBound << " ]\n";
          }
        }
      }
    \endcode
   */
  class Bounds: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #Bounds class without
     * supplying the DOM \em bounds element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em bounds element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Bounds();

    /**
     * The constructor, when called with an argument pointing to a \em bounds
     * element within a DOM, instantiates the #Bounds class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of an \em bounds
     * component node within the DOM.
     *
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to evaluate bounds with a functional
     * dependence on the instance state.  It must be passed as a void pointer
     * to avoid circularity of dependencies at build time.
     */
    Bounds( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * Destructor required to free the locally allocated memory of the variableDef
     */
    virtual ~Bounds();

    /**
     * An uninitialised instance of #Bounds is filled with data from a
     * particular \em bounds element within a DOM by this function.  If another
     * \em bounds element pointer is supplied to an instance that has already
     * been initialised, the instance is re-initialised with the new data.
     * However, this is not a recommended procedure, since optional elements
     * may not be replaced.
     *
     * \param elementDefinition is an address of an \em bounds
     * component node within the DOM.
     *
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to evaluate bounds with a functional
     * dependence on the instance state.  It must be passed as a void pointer
     * to avoid circularity of dependencies at build time.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * If the bound is expressed in terms of a \em variableDef or
     * \em variableRef, this function allows the \em varID attribute of the
     * bound's variable to be determined.  If the instance has not been
     * populated, or if the bound is not expressed in terms of a
     * \em variableDef of \em variableRef, an empty string will be returned.
     *
     * \return The \em varID attribute string of a \em variableDef that
     * expresses this bound functionally is passed by reference.
     */
    const dstoute::aString& getVarID() const
    {
      return varID_;
    }

    /**
     * This function returns the current value of the bound defined by this
     * Bounds instance, based if necessary on the current state of all
     * variables within the parent Janus instance.  It will perform
     * whatever computations are required to determine the bound.  If the
     * bound can not be determined for any reason, a NaN will be returned.
     *
     * \param functionIndex is an optional argument, only necessary for tabular bounds
     * included in either GriddedTableDef or UngriddedTableDef instances.  It
     * refers to the Function instance making use of the table.
     *
     * \return A double precision representation of the current \em bound
     * value.
     */
    double getBound( 
      const dstoute::aOptionalSizeT&functionIndex = dstoute::aOptionalSizeT::invalidValue()) const;

    /**
     * This function is used to export the \em bound data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // @TODO :: Add set parameter functions

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os, const Bounds &bounds);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
      const dstoute::aString& elementID,
      const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

  private:

    Janus* janus_;

    /************************************************************************
     * These are the bounds elements, set up during instantiation.
     * The bounds can be expressed in terms of a scalar, a varID reference, or
     * a simple dataTable.
     *
     * The simplest bound is a scalar value.  If this bound is not defined
     * in terms of a scalar, scalar_ will be initialised to NaN.
     *
     * If the bound is expressed as a variableDef to be evaluated, either
     * directly or by reference, varIndex_ will be the index of that
     * variable.  If this bound is not defined in terms of a variableDef,
     * this value will be initialised to be an invalid index.
     *
     * If the bound is expressed as a simple data table, dataTable_ will
     * point to an array of that data in the same sequence as that in the
     * XML file.  If it is a gridded table, dataTable_ should be used as a
     * replacement for the function.griddedDataTable_.  If it is an
     * ungridded table, dataTable_ should be used as a replacement for the
     * final column of the data points.
     */
    ElementDefinitionEnum elementType_;

    bool isDataTable_;
    bool isVariableDef_;
    bool isVariableRef_;

    double scalar_;
    dstoute::aOptionalSizeT varIndex_;
    dstoute::aString varID_;
    VariableDef* variableDef_;
    std::vector< double> dataTable_;

  };

  typedef dstoute::aList< Bounds> BoundsList;

}

#endif /* _BOUNDS_H_ */
