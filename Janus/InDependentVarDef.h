#ifndef _INDEPENDENTVARDEF_H_
#define _INDEPENDENTVARDEF_H_

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
// Title:      Janus/InDependentVarDef
// Class:      InDependentVarDef
// Module:     InDependentVarDef.h
// First Date: 2011-11-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file InDependentVarDef.h
 *
 * This code is used during initialisation of the Janus class, and
 * provides access to the InDependent variable definitions contained in a DOM that
 * complies with the DAVE-ML DTD.
 *
 * A breakpointDef is where gridded table breakpoints are given.
 * Since these are separate from function data, they may be reused.
 *
 * bpVals is a set of breakpoints; that is, a set of independent
 * variable values associated with one dimension of a gridded table
 * of data. An example would be the Mach or angle-of-attack values
 * that define the coordinates of each data point in a
 * two-dimensional coefficient value table.
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

  /**
   * This code is used during initialisation of the Janus class, and
   * provides access to the In-Dependent variable definitions contained in a DOM that
   * complies with the DAVE-ML DTD.
   *
   * A breakpointDef is where gridded table breakpoints are given.
   * Since these are separate from function data, they may be reused.
   *
   * bpVals is a set of breakpoints; that is, a set of independent
   * variable values associated with one dimension of a gridded table
   * of data. An example would be the Mach or angle-of-attack values
   * that define the coordinates of each data point in a
   * two-dimensional coefficient value table.
   */

  class InDependentVarDef: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the BreakpointDef class
     * without supplying the DOM \em breakpointDef element from which the
     * instance is constructed, but in this state is not useful for any class
     * functions. It is necessary to populate the class from a DOM containing a
     * \em breakpointDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    InDependentVarDef();

    /**
     * The constructor, when called with an argument pointing to a
     * \em breakpointDef element within a DOM, instantiates the BreakpointDef
     * class and fills it with alphanumeric data from the DOM.  The string
     * content of the \em bpVals element is converted to a double precision
     * numeric vector within the instance.
     *
     * \param elementDefinition is an address of an \em independentVarElement component
     * within the DOM.
     */
    InDependentVarDef( const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #InDependentVarDef is filled with data from a
     * particular \em InDependentVarDef element within a DOM by this function.  The
     * string content of the \em bpVals element is converted to a double precision
     * numeric vector within the instance.  If another \em InDependentVarDef element
     * pointer is supplied to an instance that has already been initialised, the
     * instance will be re-initialised with the new data.  However, this is not
     * a recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address of a \em breakpointDef component
     * within the DOM.
     * \param isIndependentVarDef is a boolean indicating whether the definition
     * represents an independent (TRUE) or a dependent variable (FALSE).
     */
    void initialiseDefinition(
      const DomFunctions::XmlNode& elementDefinition,
      const bool& isIndependentVarDef = true);

    /*
     * Janus tabular function evaluations rely on the equivalency of
     * \em independentVarPts and \em breakpointDef elements.
     * This function allows simply-defined functions to used the same
     * Janus internal structure as more complex functions defined by
     * reference.
     * An uninitialised instance of #BreakpointDef is filled with data from a
     * particular \em independentVarPts element within a DOM by this function.
     * If another \em independentVarPts element pointer is
     * supplied to an instance that has already been initialised by this
     * function or by #setBreakpointDefFromDom, the instance will be
     * re-initialised with the new data.  However, this is not
     * a recommended procedure, since optional elements may not be replaced.
     * \param independentVarElement is a pointer to a \em independentVarPts
     * component within the DOM.
     * \return A string containing a \em bpID generated for this #BreakpointDef
     * by using the independent variable \em varID string is returned by
     * reference.
     * \sa GriddedTableDef::setGriddedTableDefFromDependentVarPts
     */
    //  const dstoute::aString& initialiseDefinitionFromIndependentVarElement( const
    //       DomFunctions::XmlNode& independentVarElement );
    /**
     * This function provides access to the \em name attribute of the
     * \em breakpointDef element represented by this BreakpointDef instance.
     * The \em name attribute is optional.  If the instance has not been
     * initialised from a DOM, or if no \em name attribute is present, an
     * empty string is returned.
     *
     * \return The \em name string is passed by reference.
     */
    const dstoute::aString& getName() const
    {
      return name_;
    }

    /**
     * This function provides access to the \em varID attribute of a
     * \em InDependentVarDef.  This attribute is used for indexing variableDefs
     * within an XML dataset.  If the instance has not been initialised from
     * a DOM, an empty string is returned.
     *
     * \return The \em varID string is passed by reference.
     */
    const dstoute::aString& getVarID() const
    {
      return varID_;
    }

    /**
     * This function provides access to the \em units attribute
     * of a \em InDependentVarDef represented by this #InDependentVarDef instance.
     * A breakpoint array's \em units attribute is a string of arbitrary
     * length, but normally short, and complying with the format requirements
     * \latexonly chosen by \ac{AD} \ac{FS} \cite{brian1} in accordance with
     * \ac{SI} \endlatexonly \htmlonly of SI \endhtmlonly and other systems.
     * The \em units attribute is optional.  If the instance has not been
     * initialised from a DOM, or if no \em units attribute is present, an
     * empty string is returned.
     *
     * \return The \em units string is passed by reference.
     */
    const dstoute::aString& getUnits() const
    {
      return units_;
    }

    /**
     * This function provides access to the \em sign attribute of a
     * \em independentVarPts.  The data for the independentVarPts is
     * stored as breakpoints, and hence uses the \em breakpointDef
     * construct. The \em sign attribute is optional.  If the independentVarPts
     * has no sign attribute or has not been initialised from a
     * DOM, an empty string is returned.
     *
     * \return The \em sign string is returned by reference.
     */
    const dstoute::aString& getSign() const
    {
      return sign_;
    }

    /**
     * This function provides access to the \em extrapolate attribute of a
     * \em independentVarPts.  The data for the independentVarPts is
     * stored as breakpoints, and hence uses the \em breakpointDef
     * construct. The \em extrapolate attribute is optional.  If the independentVarPts
     * has no extrapolate attribute or has not been initialised from a
     * DOM, a 'neither' string is returned.
     *
     * \return An \em Extrapolation enum containing the extrapolation technique applicable to the independent
variable selected.
     */
    ExtrapolateMethod getExtrapolationMethod() const
    {
      return extrapolateMethod_;
    }

    /**
     * This function provides access to the \em interpolate attribute of a
     * \em independentVarPts.  The data for the independentVarPts is
     * stored as breakpoints, and hence uses the \em breakpointDef
     * construct. The \em interpolate attribute is optional.  If the independentVarPts
     * has no interpolate attribute or has not been initialised from a
     * DOM, a 'linear' string is returned.
     *
     * \return An \em Interpolation enum containing the interpolation technique applicable to the independent
variable selected.
     */
    InterpolateMethod getInterpolationMethod() const
    {
      return interpolateMethod_;
    }

    /**
     * This function provides access to the \em min attribute of a
     * \em independentVarPts element. This is used to bound the interpolation
     * or extrapolation of breakpoint data when evaluating a Function element.
     *
     * \return The \em min bound condition is returned by reference.
     */
    const double& getMin() const
    {
      return min_;
    }

    /**
     * This function provides access to the \em max attribute of a
     * \em independentVarPts element. This is used to bound the interpolation
     * or extrapolation of breakpoint data when evaluating a Function element.
     *
     * \return The \em max bound condition is returned by reference.
     */
    const double& getMax() const
    {
      return max_;
    }

    /**
     * This function provides access to the array of data values
     * stored within this instance of the InDependentVarDef class.
     *
     * \return a vector of numeric values, representing the data for
     * this InDependentVarDef instance, is returned by reference.
     */
    const std::vector< double>& getData() const
    {
      return dataPoint_;
    }

    /**
     * This function provides access to the variableDef reference for this
     * instance of the InDependentVarDef class. This is the index of the
     * variableDef entry within the list of variableDef elements managed by
     * the base Janus instance.
     *
     * \return the index of the variableDef associated with this instance of
     * the InDependentVarDef class.
     */
    dstoute::aOptionalSizeT getVariableReference() const
    {
      return varRef_;
    }

    /**
     * This function is used to set the index of the variableDef associated
     * with this instance of the InDependentVarDef class. This is the index of the
     * variableDef entry within the list of variableDef elements managed by
     * the base Janus instance. This function is called when instantiating gridded
     * and ungridded table elements.
     *
     * \param varRef a index of the variableDef associated with this instance
     *  of the InDependentVarDef class
     */
    void setVariableReference(
      dstoute::aOptionalSizeT varRef)
    {
      varRef_ = varRef;
    }

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em InDependentVarDef data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     * \param asPts a boolean indicating whether data is exported as points
     *              or the entry is exported as a reference to predefined data.
     */
    void exportDefinition(
      DomFunctions::XmlNode& documentElement,
      const bool& asPts = true);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<(
      std::ostream &os,
      const InDependentVarDef &griddedTableDef);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom(
      const DomFunctions::XmlNode& elementDefinition);

  private:
    /************************************************************************
     * These are the InDependentVarDef elements, set up during instantiation.
     * Each InDependentVarDef includes a single vector of numeric values.
     */
    bool isIndependentVarDef_;
    dstoute::aString name_;
    dstoute::aString varID_;
    dstoute::aString units_;
    dstoute::aString sign_;
    ExtrapolateMethod extrapolateMethod_;
    InterpolateMethod interpolateMethod_;
    std::vector< double> dataPoint_;
    double min_;
    double max_;
    dstoute::aOptionalSizeT varRef_;
  };

  typedef dstoute::aList< InDependentVarDef> InDependentVarDefList;

}

#endif /* _INDEPENDENTVARPTS_H_ */
