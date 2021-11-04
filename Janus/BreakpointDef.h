#ifndef _BREAKPOINTDEF_H_
#define _BREAKPOINTDEF_H_

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
// Title:      Janus/BreakpointDef
// Class:      BreakpointDef
// Module:     BreakpointDef.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file BreakpointDef.h
 *
 * A BreakpointDef instance holds in its allocated memory alphanumeric data
 * derived from a \em breakpointDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes numeric
 * break points for gridded tables, and associated alphanumeric identification
 * data.
 *
 * A breakpointDef is where gridded table breakpoints are defined; that is,
 * a set of independent variable values associated with one dimension of a
 * gridded table of data. An example would be the Mach or angle-of-attack
 * values that define the coordinates of each data point in a
 * two-dimensional coefficient value table. These are separate from function
 * data, and thus they may be reused. The \em independentVarPts element used
 * within some DAVE-ML \em functionDefn elements is equivalent to a
 * \em breakpointDef element, and is also represented as a BreakpointDef
 * within Janus.
 *
 * The BreakpointDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 *
 * Janus exists to handle data for a modelling process.
 * Therefore, in normal computational usage it is unnecessary (and undesirable)
 * for a calling program to be aware of the existence of this class.
 * However, functions do exist to access BreakpointDef contents directly,
 * which may be useful during dataset development.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes
#include <vector>

// Local Includes
#include "XmlElementDefinition.h"

namespace janus
{

  /**
   * A #BreakpointDef instance holds in its allocated memory alphanumeric data
   * derived from a \em breakpointDef element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  It includes numeric
   * break points for gridded tables, and associated alphanumeric identification
   * data.
   *
   * A \em breakpointDef is where gridded table breakpoints are defined; that is,
   * a set of independent variable values associated with one dimension of a
   * gridded table of data. An example would be the Mach or angle-of-attack
   * values that define the coordinates of each data point in a
   * two-dimensional coefficient value table. These are separate from function
   * data, and thus they may be reused. The \em independentVarPts element used
   * within some DAVE-ML \em functionDefn elements is equivalent to a
   * \em breakpointDef element, and is also represented as a #BreakpointDef
   * within Janus.
   *
   * The #BreakpointDef class is only used within the janus namespace, and should
   * only be referenced through the Janus class.
   *
   * Janus exists to handle data for a modelling process.
   * Therefore, in normal computational usage it is unnecessary (and undesirable)
   * for a calling program to be aware of the existence of this class.
   * However, functions do exist to access #BreakpointDef contents directly,
   * which may be useful during dataset development.  A possible usage might be:
    \code
      Janus test( xmlFileName );
      const vector<BreakpointDef>& breakpointDef = test.getBreakpointDef();
      for ( int i = 0 ; i < breakpointDef.size() ; i++ ) {
        cout << " bpID = " << breakpointDef.at( i ).getBpID()
             << ", units = " << breakpointDef.at( i ).getUnits() << "\n";
        cout << " values = [ " << breakpointDef.at( i ).getBpVals() << " ]\n";
      }
    \endcode
   */
  class BreakpointDef: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #BreakpointDef class
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
    BreakpointDef();

    /**
     * The constructor, when called with an argument pointing to a
     * \em breakpointDef element within a DOM, instantiates the #BreakpointDef
     * class and fills it with alphanumeric data from the DOM.  The string
     * content of the \em bpVals element is converted to a double precision
     * numeric vector within the instance.
     *
     * \param elementDefinition is an address of an \em breakpointDef
     * component node within the DOM.
     */
    BreakpointDef( const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #BreakpointDef is filled with data from a
     * particular \em breakpointDef element within a DOM by this function.  The
     * string content of the \em bpVals element is converted to a double precision
     * numeric vector within the instance.  If another \em breakpointDef element
     * pointer is supplied to an instance that has already been initialised, the
     * instance will be re-initialised with the new data.  However, this is not
     * a recommended procedure, since optional elements may not be replaced.
     *
     * \param elementDefinition is an address of an \em breakpointDef
     * component node within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em name attribute of the
     * \em breakpointDef element represented by this #BreakpointDef instance.
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
     * This function provides access to the \em bpID attribute of a
     * \em breakpointDef.  This attribute is used for indexing breakpoints
     * within an XML dataset.  If the instance has not been initialised from
     * a DOM, an empty string is returned.
     *
     * \return The \em bpID string is passed by reference.
     */
    const dstoute::aString& getBpID() const
    {
      return bpID_;
    }

    /**
     * This function provides access to the \em units attribute
     * of a \em breakpointDef represented by this #BreakpointDef instance.
     * A breakpoint array's \em units attribute is a string of arbitrary
     * length, but normally short, and complying with the format requirements
     * \latexonly chosen by \ac{AD} \ac{APS} \cite{brian1} in accordance with
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
     * This function provides access to the \em description child of the
     * \em breakpointDef element represented by this #BreakpointDef instance.
     * A \em breakpointDef's optional \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #BreakpointDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is passed by reference.
     */
    const dstoute::aString& getDescription() const
    {
      return description_;
    }

    /**
     * This function provides access to the breakpoint values within a
     * #BreakpointDef instance.  The breakpoints are a vector of monotonically
     * increasing values used as the independent terms in function based on a
     * gridded table.  The function is not generally accessed directly by users,
     * but is employed by the Janus class in performing function evaluations.
     *
     * \return A reference to the \em bpVals vector from this #BreakpointDef
     * instance.
     */
    const std::vector< double>& getBpVals() const
    {
      return bpVals_;
    }

    /**
     * This function provides the number of breakpoint values within a
     * #BreakpointDef instance.  The breakpoints are a vector of monotonically
     * increasing values used as the independent terms in function based on a
     * gridded table.  The function is not generally accessed directly by users,
     * but is employed by the Janus class in performing function evaluations.
     *
     * \return The number of breakpoint values stored in the \em bpVals vector
     * from this #BreakpointDef instance.
     */
    size_t getNumberOfBpVals() const
    {
      return bpVals_.size();
    }

    /**
     * This function permits the \em name attribute of the
     * \em breakpointDef element to be reset for this #BreakpointDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param name The \em name string.
     *
     */
    void setName( const dstoute::aString& name)
    {
      name_ = name;
    }

    /**
     * This function permits the \em bpID attribute of the
     * \em breakpointDef element to be reset for this #BreakpointDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param bpID The breakpoint identifier \em bpID string.
     *
     */
    void setBpID( const dstoute::aString& bpID)
    {
      bpID_ = bpID;
    }

    /**
     * This function permits the \em units attribute of the
     * \em breakpointDef element to be reset for this #BreakpointDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param units The \em units identifier string.
     *
     */
    void setUnits( const dstoute::aString& units)
    {
      units_ = units;
    }

    /**
     * This function permits the \em description element of the
     * \em breakpointDef element to be reset for this #BreakpointDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param description The \em description string.
     *
     */
    void setDescription( const dstoute::aString& description)
    {
      description_ = description;
    }

    /**
     * This function permits the breakpoint values vector (\em bpVals) element of the
     * \em breakpointDef element to be reset for this #BreakpointDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param bpVals The vector of numerical breakpoint values.
     *
     */
    void setBpVals( const std::vector< double> bpVals)
    {
      bpVals_.clear();
      bpVals_ = bpVals;
    }

    /**
     * This function is used to export the \em breakpointDef data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os,
      const BreakpointDef &breakpointDef);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);

  private:

    /************************************************************************
     * These are the breakpointDef elements, set up during instantiation.
     * Each breakpointDef includes a single vector of numeric values.
     */
    dstoute::aString name_;
    dstoute::aString bpID_;
    dstoute::aString units_;
    dstoute::aString description_;
    std::vector< double> bpVals_;

  };

  typedef dstoute::aList< BreakpointDef> BreakpointDefList;

}

#endif /* _BREAKPOINTDEF_H_ */
