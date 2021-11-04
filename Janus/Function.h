#ifndef _FUNCTION_H_
#define _FUNCTION_H_

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
// Title:      Janus/Function
// Class:      Function
// Module:     Function.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Function.h
 *
 * A Function instance holds in its allocated memory alphanumeric data
 * derived from a \em function element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  Each function has optional
 * description, optional provenance, and either a simple input/output values
 * or references to more complete (possible multiple) input, output, and
 * function data elements.
 *
 * The Function class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 */

// Ute Includes
#include <Ute/aList.h>
#include <Ute/aOptional.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "Provenance.h"
#include "GriddedTableDef.h"
#include "BreakpointDef.h"
#include "FunctionDefn.h"
#include "InDependentVarDef.h"

namespace janus
{

  class Janus;
  class Function;

  typedef dstoute::aList< Function> FunctionList;

  /**
   * A #Function instance holds in its allocated memory alphanumeric data
   * derived from a \em function element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  Each function has optional
   * description, optional provenance, and either a simple input/output values
   * or references to more complete (possible multiple) input, output, and
   * function data elements.

   * The #Function class is only used within the janus namespace, and should
   * only be referenced through the Janus class.
   *
   * Where a \em function is defined
   * directly using \em dependentVarPts and \em independentVarPts, these are
   * converted during initialisation to externally-defined gridded tables and
   * breakpoints respectively.  Any data tables defined within the
   * \em functionDefn are also converted to external tables.  Because of these
   * processes, a #Function instance never contains primary data, only references
   * to external tables, breakpoints and variables.  However, because it is
   * possible (but heavily discouraged) to apply output scale factors to
   * tabulated data with Janus, a copy of the relevant external table with
   * current scale factors applied is maintained within each Janus #Function
   * instance.
   *
   * Janus exists to abstract data form and handling from a modelling process.
   * Therefore, in normal computational usage, it is unnecessary and undesirable
   * for a calling program to even be aware of the existence of this class.
   * However, functions do exist to access #Function contents directly, which
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
  class Function : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #Function class without
     * supplying the DOM \em function element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em function element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Function( );

    /**
     * The constructor, when called with an argument pointing to a \em function
     * element within a DOM, instantiates the #Function class and fills
     * it with alphanumeric data from the DOM.  String-based cross-references
     * as implemented in the XML dataset are converted to index-based
     * cross-references to improve computational performance.
     *
     * \param elementDefinition is an address of a \em function
     * component within the DOM.
     *
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.  It must be passed as a void pointer to avoid circularity of
     * dependencies at build time.
     */
    Function( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function populates a #Function instance based on the corresponding
     * \em function element of the DOM, defines the cross-references from the
     * #Function to variables and breakpoints, and sets up arrays
     * which will later be used in run-time function evaluation.  If another
     * \em functionElement pointer is supplied to an instance that has already
     * been initialised, data corruption will occur and the entire Janus
     * instance will become unusable.
     *
     * \param elementDefinition is an address of a \em function
     * component within the DOM.
     *
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.  It must be passed as a void pointer to avoid circularity of
     * dependencies at build time.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to \em name attribute of a \em function.  If
     * the function has not been initialised from a DOM, an empty string is
     * returned.
     *
     * \return The \em name string is returned by reference.
     */
    const dstoute::aString& getName( ) const { return name_; }

    /**
     * This function provides access to the optional \em description of the
     * \em function element represented by this #Function instance.
     * A \em function's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #Function has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function indicates whether a \em function element of a
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
     * associated with a #Function instance.  There may be zero or one
     * of these elements for each function in a valid dataset, defined either
     * directly or by reference.
     *
     * \return The Provenance instance is returned by reference.
     *
     * \sa Provenance
     */
    const Provenance& getProvenance( ) const { return provenance_; }

    /**
     * This function provides access to the optional \em name attribute of the
     * \em functionDefn that is a child of a \em function.  If the function
     * definition has no name attribute or has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em functionDefn \em name string is returned by reference.
     */
    const dstoute::aString& getDefnName( ) const { return functionDefn_.getName(); }

    /**
     * Each #Function instance involves one dependent variable and one or
     * more independent variables.  Within the #Function, the dependent variable
     * is referenced by an index into the vector of VariableDef instances within
     * the encompassing Janus instance.
     *
     * \return An integer index to the dependent variable of the
     * referenced #Function within the encompassing Janus instance.
     */
    dstoute::aOptionalSizeT getDependentVarRef() const;

    /**
     * This function provides access to the independent variable definitions
     * instances that have been defined for the function instance.
     * An empty vector will be returned if the #Function instance has not
     * been populated from a DOM. In all other cases, the vector will contain
     * at least one independent variable instance.
     *
     * \return A list of independent variable definitions instances.
     *
     * \sa InDependentVarDef
     */
    const InDependentVarDefList& getInDependentVarDef() const
    { return independentVarElement_;}

    /**
     * This function returns the number of \em independentVarRef or
     * \em independentVarPts elements used in a \em function.  If the
     * instance has not been populated from a DOM, zero is returned.  In all
     * other cases, there must be one or more independent variables.
     *
     * \return An integer number, one or more in a populated instance.
     */
    size_t getIndependentVarCount( ) const
    { return independentVarElement_.size(); }

    /**
     * This function provides access to the \em independentVarRef or
     * \em independentVarPts elements used in a \em function.  Within
     * the #Function, these variables are referenced by indices into the
     * vector of VariableDef instances within the encompassing Janus instance.
     *
     * \param index is an integer in the range from 0 through
     * ( #getIndependentVarCount() - 1 ), and selects the required independent
     * variable.  Attempting to access an independent variable outside the
     * available range will throw a standard out_of_range exception.
     *
     * \return An integer index to the selected independent variable of the
     * referenced #Function within the encompassing Janus instance.
     */
    dstoute::aOptionalSizeT getIndependentVarRef( 
      const dstoute::aOptionalSizeT& index ) const;

    /**
     * The \em min attribute of a \em function's independent variable describes
     * a lower limit for the independent variable's value during computation of
     * the output.  This function makes that limit available to the calling
     * program.
     * The \em min attribute is optional for all degrees of freedom for
     * a \em function, and if it is not set for any particular
     * degree of freedom then the data may be extrapolated downwards without
     * limit in that degree of freedom unless the \em extrapolate attribute
     * indicates otherwise.
     *
     * Note that a variable may be an independent input for multiple
     * \em functions, and may have a different \em min in each such \em function.
     * Also, the \em min need not coincide with the minimum
     * \em independentVarPts or breakpoint (\latexonly$x_\textrm{min}$
     * \endlatexonly\htmlonly<var>x</var><sub>min</sub>\endhtmlonly) for its
     * degree of freedom.
     *
     * The value (\latexonly$x$\endlatexonly \htmlonly<var>x</var>\endhtmlonly) of
     * an independent variable used for evaluation of a
     * function is never less than \em min, no matter what the input value is
     * or what other constraints are applied.  Within this constraint,
     * the \em min attribute interacts with both the lowest available value
     * for its variable and the variable's \em extrapolate attribute (see
     * #getIndependentVarExtrapolate()), to define the input value
     * used in a function evaluation.  Whenever a constraint is activated during
     * a function evaluation, the extrapolation flag for that degree of freedom
     * is changed, and can be checked by the calling program (see
     * getIndependentVarExtrapolateFlag()).  The various possible combinations
     * of constraining attributes and data limits are:

     \htmlonly

     <center><table style="border-width: 1px 1px 1px 1px; border-spacing: 2px;
     border-style: none none none none; border-color: black black black black;
     border-collapse: collapse">
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><b><i>extrapolate</i></b></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><var>x</var> <b>relative values</b></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><var>x</var> <b>used in</b></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><b>extrapolation flag</b></td></tr>
     <tr>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid"><b> attribute</b>        </td>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid">                         </td>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid"><b> computation</b>      </td>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid"><b>after computation</b> </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid">any value                </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><var>x</var><sub>min</sub> &lt; <i>min</i> &lt; <var>x</var>  </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><var>x</var>             </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid">NEITHER                  </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">                         </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><i>min</i> &lt; <var>x</var><sub>min</sub> &lt; <var>x</var> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var>             </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">NEITHER                  </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid">neither / max            </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><var>x</var><sub>min</sub> &lt; <var>x</var> &lt; <i>min</i> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><i>min</i>               </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid">MINEX                    </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                        </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var> &lt; <var>x</var><sub>min</sub> &lt; <i>min</i> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><i>min</i>              </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">MINEX                   </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                        </td>
       <td><i>min</i> &lt; <var>x</var> &lt; <var>x</var><sub>min</sub> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var><sub>min</sub></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">XMIN                    </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">                         </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var> &lt; <i>min</i> &lt; <var>x</var><sub>min</sub> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var><sub>min</sub></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">XMIN                     </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid">min / both               </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><var>x</var><sub>min</sub> &lt; <var>x</var> &lt; <i>min</i> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><i>min</i>               </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid">MINEX                    </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                        </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var> &lt; <var>x</var><sub>min</sub> &lt; <i>min</i> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><i>min</i>              </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">MINEX                   </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                        </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><i>min</i> &lt; <var>x</var> &lt; <var>x</var><sub>min</sub> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var>            </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">XMIN                    </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">                         </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var> &lt; <i>min</i> &lt; <var>x</var><sub>min</sub> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><i>min</i>               </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">MINEX                    </td></tr>
     </table></center>

     \endhtmlonly

     \latexonly

       \begin{center}\begin{tabular}[H]{|l|l|l|l|}\hline
       \textit{extrapolate} & $x$ relative values            & $x$ used in
                                                       & extrapolation flag \\
       attribute            &                                & computation
                                                       & after computation  \\
       \hline\hline
       any value            & $x_\textrm{min}$ < \textit{min} < $x$
                                               & $x$ & NEITHER\\
                            & \textit{min} < $x_\textrm{min}$ < $x$
                                               & $x$ & NEITHER\\
       \hline
       neither / max        & $x_\textrm{min}$ < $x$ < \textit{min}
                                               & \textit{min}  & MINEX  \\
                            & $x$ < $x_\textrm{min}$ < \textit{min}
                                               & \textit{min}  & MINEX  \\
                            & \textit{min} < $x$ < $x_\textrm{min}$
                                               & $x_\textrm{min}$ & XMIN \\
                            & $x$ < \textit{min} < $x_\textrm{min}$
                                               & $x_\textrm{min}$& XMIN \\
       \hline
       min / both           & $x_\textrm{min}$ < $x$ < \textit{min}
                                               & \textit{min}  & MINEX  \\
                            & $x$ < $x_\textrm{min}$ < \textit{min}
                                               & \textit{min}  & MINEX  \\
                            & \textit{min} < $x$ < $x_\textrm{min}$
                                               & $x$ & XMIN \\
                            & $x$ < \textit{min} < $x_\textrm{min}$
                                               & \textit{min}& MINEX \\
       \hline
       \end{tabular}\end{center}

     \endlatexonly

     * If a \em min limit has not been specified for a variable, this function
     * returns -DBL_MAX.
     *
     * \param index is an integer in the range from 0 through
     * ( #getIndependentVarCount() - 1 ), and selects the required independent
     * variable.  Attempting to access an independent variable outside the
     * available range will throw a standard out_of_range exception.
     *
     * \return A double precision value for the selected variable's minimum
     * limit.
     */
    const double& getIndependentVarMin( const size_t& index ) const
    { return independentVarElement_.at( index ).getMin(); }

    /**
     * The \em max attribute of a \em function's independent variable describes
     * an upper limit for the independent variable's value during computation of
     * the output.  This function makes that limit available to the calling
     * program.
     * The \em max attribute is optional for all degrees of freedom for a
     * \em function, and if it is not set for any particular
     * degree of freedom then the data may be extrapolated upwards without
     * limit in that degree of freedom unless the \em extrapolate attribute
     * indicates otherwise.
     *
     * Note that a variable may be an independent input for multiple
     * \em functions, and may have a different \em max in each such \em function.
     * Also, the \em max need not coincide with the maximum \em independentVarPts
     * or breakpoint (\latexonly$x_\textrm{max}$\endlatexonly
     * \htmlonly<var>x</var><sub>max</sub>\endhtmlonly) for its degree of freedom.
     *
     * The value (\latexonly$x$\endlatexonly \htmlonly<var>x</var>\endhtmlonly) of
     * an independent variable used for evaluation of a
     * function is never greater than \em max, no matter what the input value is
     * or what other constraints are applied.  Within this constraint,
     * the \em max attribute interacts with both the highest available value
     * for its variable and the variable's \em extrapolate attribute (see
     * #getIndependentVarExtrapolate()), to define the input value
     * used in a function evaluation.  Whenever a constraint is activated during
     * a function evaluation, the extrapolation flag for that degree of freedom
     * is changed, and can be checked by the calling program (see
     * getIndependentVarExtrapolateFlag()).  The various possible combinations
     * of constraining attributes and data limits are:

     \htmlonly

     <center><table style="border-width: 1px 1px 1px 1px; border-spacing: 2px;
     border-style: none none none none; border-color: black black black black;
     border-collapse: collapse">
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><b><i>extrapolate</i></b> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><var>x</var> <b>relative values</b></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><var>x</var> <b>used in</b></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid"><b>extrapolation flag</b></td></tr>
     <tr>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid"><b>attribute</b>         </td>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid">                         </td>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid"><b>computation</b>       </td>
       <td style="border-width: 1px 1px 2px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid"><b>after computation</b> </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid">any value                </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><var>x</var> &lt; <i>max</i> &lt; <var>x</var><sub>max</sub> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><var>x</var>             </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"> NEITHER                 </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">                         </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var> &lt; <var>x</var><sub>max</sub> &lt; <i>max</i> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var>                                                 </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"> NEITHER                                                     </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"> neither / min            </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><i>max</i> &lt; <var>x</var> &lt; <var>x</var><sub>max</sub> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><i>max</i>                                                   </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"> MAXEX                                                       </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                        </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><i>max</i> &lt; <var>x</var><sub>max</sub> &lt; <var>x</var> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><i>max</i>                                                   </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"> MAXEX                                                       </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                          </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var><sub>max</sub> &lt; <var>x</var> &lt; <i>max</i> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var><sub>max</sub>                                   </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"> XMAX                                                        </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">                                                             </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var><sub>max</sub> &lt; <i>max</i> &lt; <var>x</var> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var><sub>max</sub>                                   </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"> XMAX                                                        </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"> max / both               </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><i>max</i> &lt; <var>x</var> &lt; <var>x</var><sub>max</sub> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"><i>max</i>                                                   </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid hidden solid"> MAXEX                                                       </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                          </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><i>max</i> &lt; <var>x</var><sub>max</sub> &lt; <var>x</var> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><i>max</i>                                                   </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"> MAXEX                                                       </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid">                          </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var><sub>max</sub> &lt; <var>x</var> &lt; <i>max</i> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"><var>x</var>                                                 </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid hidden solid"> XMAX                                                        </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid">                                                             </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><var>x</var><sub>max</sub> &lt; <i>max</i> &lt; <var>x</var> </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"><i>max</i>                                                   </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: hidden solid solid solid"> MAXEX                                                       </td></tr>
     </table></center>

     \endhtmlonly

     \latexonly

       \begin{center}\begin{tabular}[H]{|l|l|l|l|}\hline
       \textit{extrapolate} & $x$ relative values            & $x$ used in
                                                       & extrapolation flag \\
       attribute            &                                & computation
                                                       & after computation  \\
       \hline\hline
       any value            & $x$ < \textit{max} < $x_\textrm{max}$
                                               & $x$ & NEITHER\\
                            & $x$ < $x_\textrm{max}$ < \textit{max}
                                               & $x$ & NEITHER\\
       \hline
       neither / min        & \textit{max} < $x$ < $x_\textrm{max}$
                                               & \textit{max}  & MAXEX  \\
                            & \textit{max} < $x_\textrm{max}$ < $x$
                                               & \textit{max}  & MAXEX  \\
                            & $x_\textrm{max}$ < $x$ < \textit{max}
                                               & $x_\textrm{max}$ & XMAX \\
                            & $x_\textrm{max}$ < \textit{max} < $x$
                                               & $x_\textrm{max}$& XMAX \\
       \hline
       max / both           & \textit{max} < $x$ < $x_\textrm{max}$
                                               & \textit{max}  & MAXEX  \\
                            & \textit{max} < $x_\textrm{max}$ < $x$
                                               & \textit{max}  & MAXEX  \\
                            & $x_\textrm{max}$ < $x$ < \textit{max}
                                               & $x$ & XMAX \\
                            & $x_\textrm{max}$ < \textit{max} < $x$
                                               & \textit{max}& MAXEX \\
       \hline
       \end{tabular}\end{center}

     \endlatexonly
  
     * If a \em max limit has not been specified for a variable, this function
     * returns DBL_MAX.
     *
     * \param index is an integer in the range from 0 through
     * ( #getIndependentVarCount() - 1 ), and selects the required independent
     * variable.  Attempting to access an independent variable outside the
     * available range will throw a standard out_of_range exception.
     *
     * \return A double precision value for the selected variable's maximum
     * limit.
     */
    const double& getIndependentVarMax( const size_t& index ) const
    { return independentVarElement_.at( index ).getMax(); }

    /**
     * The \em extrapolate attribute of an independent variable describes any
     * allowable extrapolation in the independent variable's degree of freedom
     * beyond a function's tabulated data range.
     * The \em extrapolate attribute is optional for all degrees of freedom
     * for any \em function within the XML dataset, and if it is not set for any
     * particular degree of freedom then the enum representing its value
     * within the #Function instance defaults to NEITHER.
     *
     * When the returned value is NEITHER, MINEX, or MAXEX, constraining the
     * independent variable at both ends, the maximum, or the minimum
     * respectively, the constrained independent variable value used for the
     * \em function evaluation will be the more limiting of:

     \htmlonly

     <center><table style="border-width: 1px 1px 1px 1px; border-spacing: 2px;
     border-style: none none none none; border-color: black black black black;
     border-collapse: collapse">
     <tr>
       <td style="border-width: 1px 1px 2px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid solid solid"><b>Min Constraints</b>                  </td>
       <td style="border-width: 1px 1px 2px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid solid solid"><b>Max Constraints</b>                  </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid">lowest independentVarPts <em>or</em></td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 2px 4px;
       border-style: solid solid hidden solid">highest independentVarPts <em>or</em></td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid">lowest breakpoint            </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 2px 4px 4px 4px;
       border-style: hidden solid solid solid">highest breakpoint           </td></tr>
     <tr>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid solid solid"><em> min</em>  attribute               </td>
       <td style="border-width: 1px 1px 1px 1px; padding: 4px 4px 4px 4px;
       border-style: solid solid solid solid"><em> max</em>  attribute               </td></tr>
       </table></center>

       \endhtmlonly

       \latexonly

       \begin{center}\begin{tabular}[H]{|l|l|}\hline
       Min Constraints & Max Constraints \\\hline\hline
       lowest independentVarPts  \emph{or} & highest independentVarPts
                                                           \emph{or}\\
       lowest breakpoint                   & highest breakpoint     \\\hline
       \emph{min} attribute                & \emph{max} attribute    \\
       \hline\end{tabular}\end{center}

       \endlatexonly
     *
     * \param index has a range from 0 to (#getIndependentVarCount() - 1),
     * and selects the required independent variable.  Attempting to access an
     * independent variable outside the available range will throw a standard
     * out_of_range exception.
     *
     * \return An Extrapolation enum containing the extrapolation constraint
     * on the independent variable selected, determined as tabulated above.
     *
     * \sa getIndependentVarExtrapolateFlag
     */
    ExtrapolateMethod getIndependentVarExtrapolate( const size_t& index ) const
    { return independentVarElement_.at( index ).getExtrapolationMethod(); }

    /**
     * The \em interpolate attribute of an independent variable describes the
     * form of interpolation applicable to that variable's degree of freedom
     * within the range of the tabulated dataset.
     * The \em extrapolate attribute is optional for all degrees of freedom
     * for any \em function within the XML dataset, and if it is not set for any
     * particular degree of freedom then the enum representing its value
     * within the #Function instance defaults to LINEAR.
     *
     * \param index is an integer in the range from 0 through
     * ( #getIndependentVarCount() - 1 ), and selects the required independent
     * variable.  Attempting to access an independent variable outside the
     * available range will throw a standard out_of_range exception.
     *
     * \return An Interpolation enum containing the interpolation technique
     * applicable to the independent variable selected.
     */
    InterpolateMethod getIndependentVarInterpolate( const size_t& index ) const
    { return independentVarElement_.at( index ).getInterpolationMethod(); }

    /**
     * This function indicates whether the referenced #Function instance
     * requires linear or lower order interpolation in all independent
     * variables.  It is a convenience function, saving checking and speeding
     * up the interpolation process in the most common case.
     *
     * \return A boolean variable, "true" if linear interpolation is required
     * in all degrees of freedom.
     */
    const bool& isAllInterpolationLinear( ) const { return isAllInterpolationLinear_; }

    /**
     * This function returns the type of the table that is associated with
     * the #Function instance, being either a \em gridded table or an
     * \em ungridded table. This functionality is used internally when
     * instantiating a Janus instance and returning data from a Function
     * instance.
     *
     * \return An enumeration defining the table type associated with
     * the Function instance.
     */
    const ElementDefinitionEnum& getTableType() const
    { return functionDefn_.getTableType(); }


    /**
     * This function provides access to a table forming the basis for
     * evaluation of a function. Within the #Function, the table
     * is referenced by an index into the vector of table definition instances
     * encompassed within the Janus instance.
     *
     * \return An integer index to the table used by the
     * referenced #Function encompassed within the Janus instance.
     */
    dstoute::aOptionalSizeT getTableRef() const;

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
    { return functionDefn_.getDependentDataColumnNumber(); }

    /**
     * This function returns the tabular data for the table associated
     * with a #Function instance. For a gridded table this represents
     * all the data as a continuous sequence, while for an ungridded
     * table this represents the dependent data column of the table.
     *
     * \return The data for the table is returned as a reference to
     * a vector of double numeric values.
     */
    const std::vector<double>& getData() const;

    /**
     * This function is used to export the \em Function data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // @TODO :: Add set parameter functions

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const Function &function);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

   protected:

   private:

    /************************************************************************
     * These are the function elements, set up during instantiation.
     */
    Janus*                    janus_;
    ElementDefinitionEnum     elementType_;

    dstoute::aString          name_;
    dstoute::aString          description_;

    bool                      hasProvenance_;
    Provenance                provenance_;

    bool                      isSimpleIO_;
    bool                      isCompleteIO_;
    bool                      isAllInterpolationLinear_;

    mutable FunctionDefn      functionDefn_;

    std::vector<size_t>       breakpointRef_;
    InDependentVarDef         dependentVarElement_;
    InDependentVarDefList     independentVarElement_;

    DomFunctions::XmlNodeList independentVarRefList_;
    DomFunctions::XmlNode dependentVarRef_;
  };

}

#endif /* _FUNCTION_H_ */
