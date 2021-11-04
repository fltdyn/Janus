#ifndef _ElementDefinitionEnum_H_
#define _ElementDefinitionEnum_H_

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
// Title:      Janus/ElementDefinitionEnum
// Class:      -
// Module:     ElementDefinitionEnum.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file ElementDefinitionEnum.h
 *
 * This file contains enumeration parameters that are used when
 * instantiating a DAVE-ML compliant XML dataset source file
 * from a Document Object Model (DOM).
 *
 * Author   :  G. J. Brian
 *
 */

namespace janus
{

  enum ElementDefinitionEnum
  {
    ELEMENT_NOTSET,
    ELEMENT_FILEHEADER,
    ELEMENT_ARRAY,
    ELEMENT_TIMEDOMAIN,
    ELEMENT_FREQDOMAIN,
    ELEMENT_DATATABLE,
    ELEMENT_SIGNAL,
    ELEMENT_DIMENSION,
    ELEMENT_DIMRECORD,
    ELEMENT_PROVENANCE,
    ELEMENT_CALCULATION,
    ELEMENT_MATH,
    ELEMENT_MODEL,
    ELEMENT_SCRIPT,
    ELEMENT_AUTHOR,
    ELEMENT_REFERENCE,
    ELEMENT_MODIFICATION,
    ELEMENT_ADDRESS,
    ELEMENT_CONTACTINFO,
    ELEMENT_UNIFORMPDF,
    ELEMENT_BOUNDS,
    ELEMENT_NORMALPDF,
    ELEMENT_CORRELATESWITH,
    ELEMENT_CORRELATION,
    ELEMENT_VARIABLE,
    ELEMENT_VARIABLE_OUTPUT,
    ELEMENT_STATICSHOT,
    ELEMENT_BREAKPOINTS,
    ELEMENT_DATAPOINTS,
    ELEMENT_DEPENDENTVARPTS,
    ELEMENT_DEPENDENTVARREF,
    ELEMENT_INDEPENDENTVARPTS,
    ELEMENT_INDEPENDENTVARREF,
    ELEMENT_FUNCTION,
    ELEMENT_FUNCTIONDEFN,
    ELEMENT_GRIDDEDTABLE,
    ELEMENT_UNGRIDDEDTABLE,
    ELEMENT_CHECKDATA,
    ELEMENT_PROPERTY,
    ELEMENT_TRANSFERFN,
    ELEMENT_STATESPACEFN,
    ELEMENT_NUMERATOR,
    ELEMENT_DENOMINATOR,
    ELEMENT_PERTURBATION
  };

  enum SignalTypeEnum
  {
    SIGNAL_CHECKINPUTS, SIGNAL_CHECKOUTPUTS, SIGNAL_INTERNALVALUES, SIGNAL_NOSET
  };

  /*
   * The optional \em extrapolate attribute of \em independentVarRef and
   * \em independentVarPts child nodes of a \em function in the XML
   * dataset governs the treatment of the function's independent variables
   * when their requested input value exceeds the data range available.
   * Each input variable has a limited data range, which is determined by the
   * extremities of the list of points for single-variable, directly-defined
   * functions, and by the extremities of the breakpoints for functions
   * defined by reference.
   * Note that the same input variable can have different extrapolation
   * treatments for different functions.
   *
   * This enum takes its value from the \em extrapolate attribute, and
   * may be used to determine what extrapolation is allowable for each input
   * variable used in a function.  Its allowable values in this usage are
   * NEITHER, MINEX, MAXEX, or BOTH (see #getIndependentVarExtrapolate()).
   *
   * Variable references for functions defined by reference can
   * also specify minimum and maximum values, which do not necessarily
   * coincide with the extremities of the breakpoints.  The \em extrapolate
   * attribute does not allow for exceedance of any defined minimum and
   * Maximum values.
   *
   * The enum may also indicate activation of data range and extrapolation
   * constraints during a Janus function evaluation.  Its allowable values in
   * this usage are NEITHER, MINEX, MAXEX, XMIN, or XMAX (see
   * #getIndependentVarExtrapolateFlag(), #getIndependentVarMin(),
   * #getIndependentVarMax()).
   */
  enum ExtrapolateMethod
  {
    /* No extrapolation allowed (When used as a flag, indicates
     no extrapolation was required during computation). */
    EXTRAPOLATE_NEITHER,
    /* Extrapolation below data range minimum allowed (When used
     as a flag, indicates specified minimum value constraint
     was activated during computation). */
    EXTRAPOLATE_MINEX,
    /* Extrapolation above data range maximum allowed (When used
     as a flag, indicates specified maximum value constraint
     was activated during computation). */
    EXTRAPOLATE_MAXEX,
    /* Extrapolation above or below data range limits allowed. */
    EXTRAPOLATE_BOTH,
    /* Used as a flag, indicates input value was below data range
     minimum. */
    EXTRAPOLATE_XMIN,
    /* Used as a flag, indicates input value was above data range
     maximum */
    EXTRAPOLATE_XMAX
  };

  /*
   * The optional \em interpolate attribute of \em independentVarRef
   * and \em independentVarPts child elements of a \em function in the XML
   * dataset governs the form of interpolation to be used in that
   * variable's degree of freedom when evaluating the \em function between
   * gridded data points.
   * Note that the same input variable can have different interpolation
   * treatments in different functions.
   *
   * This enum can take its value from the \em interpolate attribute,
   * and may be used to determine the required form of interpolation for
   * each input variable used in each Function.  Its default value in this
   * usage is LINEAR (see #getIndependentVarInterpolate()).
   */
  enum InterpolateMethod
  {
    /* No interpolation.  The function takes on the value at the breakpoint
     nearest to the input, with exact midpoint inputs being rounded in the
     positive direction. */
    INTERPOLATE_DISCRETE,
    /* The function holds the value associated with each breakpoint until the
     next (numerically) higher breakpoint value is reached by the
     independent argument */
    INTERPOLATE_FLOOR,
    /* The function takes on the value associated with the next (numerically)
     higher breakpoint as soon as the input exceeds the previous
     breakpoint */
    INTERPOLATE_CEILING,
    /* Interpolation in this degree of freedom is linear, maintaining
     continuity of data, but with derivatives discontinuous across
     breakpoints. */
    INTERPOLATE_LINEAR,
    /* Interpolation in this degree of freedom is by quadratic spline,
     maintaining continuity of the data and its first derivative. */
    INTERPOLATE_QSPLINE,
    /* Interpolation in this degree of freedom is by cubic spline,
     maintaining continuity of the data, its first derivative, and its
     second derivative. */
    INTERPOLATE_CSPLINE,
    /* This is not part of the DAVE-ML standard, but covers DSTO legacy
     datasets.  Interpolation in this degree of freedom is polynomial, of
     order 2 as specified by \em interpolationOrder, maintaining
     continuity of data for this degree of freedom.  Derivatives are
     continuous if there are 3 breakpoints, not otherwise. */
    INTERPOLATE_POLY2,
    /* This is not part of the DAVE-ML standard, but covers DSTO legacy
     datasets.  Interpolation in this degree of freedom is polynomial, of
     order 3 as specified by \em interpolationOrder, maintaining
     continuity of data for this degree of freedom.  Derivatives are
     continuous if there are 4 breakpoints, not otherwise. */
    INTERPOLATE_POLY3
  };

  /*
   * This enum specifies the effects of an uncertainty bounds on a variable.
   * All effects are computationally permissible for both types of probability
   * density functions.  However, some don't make much sense in some
   * applications.
   */
  enum UncertaintyEffect
  {
    /* Indicates that the bound[s] supplied are expressed as a real number
     * to be added to or subtracted from the variable value to which they
     * relate.
     */
    ADDITIVE_UNCERTAINTY,
    /* Indicates that the bound[s] supplied are expressed as a proportion of
     * the variable value to which they relate.
     */
    MULTIPLICATIVE_UNCERTAINTY,
    /* Indicates that the bound[s] supplied are expressed as a percentage of
     * the variable value to which they relate.
     */
    PERCENTAGE_UNCERTAINTY,
    /* Normally only applicable to the uniform PDF, indicates that bounds
     * supplied are real numbers, which must straddle the value of the
     * variable to which they apply.
     */
    ABSOLUTE_UNCERTAINTY,
    /* Used during instantiation, indicates that the effect of an
     * uncertainty entry has not yet been specified.
     */
    UNKNOWN_UNCERTAINTY
  };

  /*
   * This enum specifies the valid forms for dynamic system models that
   * may be encoded within a dataset for a \em variableDef. The options are:
   */
  enum ModelMethod
  {
    /*
     * CONTINUOUS: indicating that the model is encapsulated as a continuous
     *             dynamic system model
     */
    MODEL_CONTINUOUS,
    /*
     * DISCRETE: indicating that the model is encapsulated as a discrete
     *           dynamic system model
     */
    MODEL_DISCRETE
  };

  /*
   * This enum specifies the integration domain for evaluating dynamic system
   * models encoded within a dataset for a \em variableDef. The options are:
   */
  enum IntegrationDomain
  {
    /*
     * FREQ: indicating that the dynamic system model is to evaluated using a
     *       frequency based integration technique
     */
    INTEGRATION_FREQ,
    /*
     * TIME: indicating that the dynamic system model is to evaluated using a
     *       time based integration technique
     */
    INTEGRATION_TIME
  };

  /*
   * This enum specifies the integration method for evaluating dynamic system
   * models encoded within a dataset for a \em variableDef. The options are:
   */
  enum IntegrationMethod
  {
    INTEGRATION_EULER,
    INTEGRATION_RUNGE_KUTTA_2,
    INTEGRATION_RUNGE_KUTTA_4,
    INTEGRATION_RUNGE_KUTTA_45,
    INTEGRATION_ADAM_BASHFORD
  };

} // End namespace janus

#endif /* _ElementDefinitionEnum_H_ */
