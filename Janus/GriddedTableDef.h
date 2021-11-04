#ifndef _GRIDDEDTABLEDEF_H_
#define _GRIDDEDTABLEDEF_H_

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
// Title:      Janus/GriddedTableDef
// Class:      GriddedTableDef
// Module:     GriddedTableDef.h
// First Date: 2011-11-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file GriddedTableDef.h
 *
 * A GriddedTableDef instance holds in its allocated memory alphanumeric data
 * derived from a \em griddedTableDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes points arranged in
 * an orthogonal, multi-dimensional array, where the independent variable ranges
 * are defined by separate breakpoint vectors.  The table data point values are
 * specified as comma-separated values in floating-point notation (0.93638E-06)
 * in a single long sequence as if the table had been unravelled with the
 * last-specified dimension changing most rapidly.  Gridded tables in DAVE-ML
 * and Janus are stored in row-major order, as in C/C++ (Fortran, Matlab and
 * Octave use column-major order).  Line breaks and comments in the XML are
 * ignored.  Associated alphanumeric identification and cross-reference data
 * are also included in the instance.
 * 
 * NOTE: The \em confidenceBound entry of the \em griddedTable element is
 * not supported, as it is expected to be deprecated in future version of
 * the DAVE-ML syntax language document type definition.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Ute Includes
#include <Ute/aList.h>
#include <Ute/aString.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "Provenance.h"
#include "Uncertainty.h"

namespace janus
{

  class Janus;

  /**
   * A #GriddedTableDef instance holds in its allocated memory alphanumeric data
   * derived from a \em griddedTableDef element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  It includes points arranged in
   * an orthogonal, multi-dimensional array, where the independent variable ranges
   * are defined by separate breakpoint vectors.  The table data point values are
   * specified as comma-separated values in floating-point notation (0.93638E-06)
   * in a single long sequence as if the table had been unravelled with the
   * last-specified dimension changing most rapidly.  Gridded tables in DAVE-ML
   * and Janus are stored in row-major order, as in C/C++ (Fortran, Matlab and
   * Octave use column-major order).  Line breaks and comments in the XML are
   * ignored.  Associated alphanumeric identification and cross-reference data
   * are also included in the instance.
   *
   * The #GriddedTableDef class is only used
   * within the janus namespace, and should only be referenced through the
   * Janus class.
   *
   * Janus exists to abstract data form and handling from a modelling process.
   * Therefore, in normal computational usage, it is unnecessary and undesirable
   * for a calling program to even be aware of the existence of this class.
   * However, functions do exist to access #GriddedTableDef contents directly,
   * which may be useful during dataset development.  A possible usage might be:
    \code
      Janus test( xmlFileName );
      const vector<GriddedTableDef>& griddedTableDef = test.getGriddedTableDef();
      for ( int i = 0 ; i < griddedTableDef.size() ; i++ ) {
        cout << " Gridded table " << i << " :\n"
             << "   name        = " << griddedTableDef.at( i ).getName() << "\n"
             << "   gtID        = " << griddedTableDef.at( i ).getGtID() << "\n"
             << "   units       = " << griddedTableDef.at( i ).getUnits() << "\n"
             << "   description = " << griddedTableDef.at( i ).getDescription()
             << "\n";
        const vector<int>& breakpointRef =
          griddedTableDef.at( i ).getBreakpointRef();
        for ( int j = 0 ; j < breakpointRef.size() ; j++ ) {
          cout << " Breakpoint " << j << " bpID = "
               << test.getBreakpointDef().at( breakpointRef.at( j ) ).getBpID()
               << "\n";
        }
        const vector<double>& dataTable = griddedTableDef.at( i ).getData();
        if ( 0 == dataTable.size() ) {
          const vector<string>& stringDataTable =
            griddedTableDef.at( i ).getStringData();
          for ( int j = 0 ; j < stringDataTable.size() ; j++ ) {
            cout << stringDataTable.at( j ) << "\n";
          }
        }
      }
    \endcode
   */
  class GriddedTableDef: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #GriddedTableDef class
     * without supplying the DOM \em griddedTableDef element from which the
     * instance is constructed, but in this state is not useful for any class
     * functions.
     * It is necessary to populate the class from a DOM containing a
     * \em griddedTableDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    GriddedTableDef();

    /**
     * The constructor, when called with an argument pointing to a
     * \em griddedTableDef element within a DOM, instantiates the
     * #GriddedTableDef class and fills it with alphanumeric data from the DOM.
     * String-based numeric data are converted to double-precision linear
     * vectors.
     *
     * \param elementDefinition is an address of a \em griddedTableDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    GriddedTableDef( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    //  GriddedTableDef& operator=( const GriddedTableDef& rhs);

    /**
     * An uninitialised instance of #GriddedTableDef is filled with data from a
     * particular \em griddedTableDef element within a DOM by this function.  If
     * another \em griddedTableDef element pointer is supplied to an
     * instance that has already been initialised, data corruption will occur
     * and the entire Janus instance will become unusable.  This function
     * can also be used with the deprecated \em griddedTable element.  For
     * backwards compatibility, Janus converts a \em griddedTable to the
     * equivalent \em griddedTableDef within this function.  Where a
     * \em griddedTableDef or \em griddedTable lacks a \em gtID attribute,
     * this function will generate a random \em gtID string for indexing within
     * the Janus class.
     *
     * \param elementDefinition is an address of a \em griddedTableDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em name attribute of a
     * \em griddedTableDef.  The \em name attribute is optional.  If
     * the gridded table has no name attribute or has not been initialised from a
     * DOM, an empty string is returned.
     *
     * \return The \em name string is returned by reference.
     */
    const dstoute::aString& getName() const
    {
      return name_;
    }

    /**
     * This function provides access to the \em gtID attribute of a
     * \em griddedTableDef.  This attribute is used for indexing gridded tables
     * within an XML dataset.  Where a \em griddedTableDef within the DOM does
     * not contain a \em gtID attribute, or where a \em griddedTable or
     * \em dependentVarPoints have been placed in the #GriddedTableDef structure,
     * a \em gtID string is generated and inserted in the DOM at initialisation
     * time.  If the instance has not been initialised from
     * a DOM, an empty string is returned.
     *
     * \return The \em gtID string is returned by reference.
     */
    const dstoute::aString& getGtID() const
    {
      return gtID_;
    }

    /**
     * This function provides access to the \em description child of the
     * \em griddedTableDef element represented by this #GriddedTableDef instance.
     * A \em griddedTableDef's optional \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means text formatting embedded in the
     * XML source will also appear in the returned description. If no description
     * is specified in the XML dataset, or the #GriddedTableDef has not been
     * initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription() const
    {
      return description_;
    }

    /**
     * This function provides access to the \em units attribute
     * of the \em griddedTableDef represented by this #GriddedTableDef instance.
     * A gridded table's \em units attribute is a string of arbitrary
     * length, but normally short, and complying with the format requirements
     * \latexonly chosen by \ac{AD} \ac{APS} \cite{brian1} in accordance with
     * \ac{SI} \endlatexonly \htmlonly of SI \endhtmlonly and other systems.
     * If the instance has not been initialised from a DOM, or if no \em units
     * attribute is present, an empty string is returned.
     *
     * \return The \em units string is returned by reference.
     */
    const dstoute::aString& getUnits() const
    {
      return units_;
    }

    /**
     * This function indicates whether a \em griddedTableDef element of a
     * DAVE-ML dataset includes either \em provenance or \em provenanceRef element.
     *
     * \return A boolean variable, 'true' if the \em griddedTableDef includes a
     * provenance, defined either directly or by reference.
     *
     * \sa Provenance
     */
    const bool& hasProvenance() const
    {
      return hasProvenance_;
    }

    /**
     * This function provides access to the Provenance instance
     * associated with a #GriddedTableDef instance.  There may be zero or one
     * of these elements for each gridded table in a valid dataset, defined
     * either directly or by reference.
     *
     * \return The Provenance instance is returned by reference.
     *
     * \sa Provenance
     */
    const Provenance& getProvenance() const
    {
      return provenance_;
    }

    /**
     * This function indicates whether a \em griddedTableDef element of a
     * DAVE-ML dataset includes an \em uncertainty child element.  A variable
     * described by a \em griddedTableDef without an \em uncertainty element may
     * still have uncertainty, if it is dependent on other variables or tables
     * with defined uncertainty.
     *
     * \return A boolean variable, 'true' if a griddedTableDef definition
     *  includes an \em uncertainty child element.
     *
     * \sa Uncertainty
     */
    const bool& hasUncertainty() const
    {
      return hasUncertainty_;
    }

    /**
     * This function provides access to the Uncertainty instance
     * associated with a #GriddedTableDef instance.  There may be zero or one
     * \em uncertainty element for each \em griddedTableDef in a valid dataset.
     * For \em griddedTableDefs without \em uncertainty, for \em griddedTables,
     * and for \em dependentVarPts, the corresponding #GriddedTableDef instance
     * includes an empty Uncertainty instance.
     *
     * \return The Uncertainty instance is returned by reference.
     *
     * \sa Uncertainty
     */
    Uncertainty& getUncertainty()
    {
      return uncertainty_;
    }

    /**
     * This function provides access to the vector of breakpoint indices
     * associated with a #GriddedTableDef instance.  The vector contains
     * one integer for each relevant \em breakpointDef, representing the
     * position of the relevant \em breakpointDef in the vector of
     * BreakpointDefs within a Janus instance.
     *
     * \return The vector of breakpoint indices is returned by reference.
     *
     * \sa BreakpointDef
     */
    const std::vector< size_t>& getBreakpointRef() const
    {
      return breakpointRef_;
    }

    /**
     * This function provides access to a vector of numeric data
     * stored in a #GriddedTableDef instance.  This vector contains
     * the double precision variables in the same sequence as they were
     * presented in the \em dataTable of the corresponding XML dataset.
     *
     * \return The vector of double-precision numeric content of the
     * #GriddedTableDef instance is returned by reference.
     */
    const std::vector< double>& getData() const
    {
      return tableData_;
    }

    /**
     * This function indicates whether the numeric table associated with the
     * \em griddedTableDef element of a DAVE-ML dataset contains data or is
     * empty.
     *
     * \return A boolean variable, 'true' if the numeric table is empty.
     *
     */
    bool isDataTableEmpty() const
    {
      return tableData_.empty();
    }

    /**
     * This function provides access to a vector of alphanumeric data
     * stored in a #GriddedTableDef instance.  This vector contains
     * the data strings in the same sequence as they were
     * presented in the \em dataTable of the corresponding XML dataset.
     *
     * \return The list of strings containing alphanumeric content of the #GriddedTableDef
     * instance is returned by reference.
     */
    const dstoute::aStringList& getStringData() const
    {
      return stringTableData_;
    }

    /**
     * This function indicates whether the alphanumeric table associated with the
     * \em griddedTableDef element of a DAVE-ML dataset contains data or is
     * empty.
     *
     * \return A boolean variable, 'true' if the alphanumeric table is empty.
     *
     */
    bool isStringDataTableEmpty() const
    {
      return stringTableData_.empty();
    }

    /**
     * This function permits the pointer to the base Janus class to be
     * set manually. This function is used internally within a Janus instance
     * by the Function class when it is instantiating a locally defined
     * gridded table.
     *
     * \param janus a pointer to the base Janus instance
     */
    void setJanus(
      Janus* janus)
    {
      janus_ = janus;
    }

    /**
     * This function permits the \em name attribute of the \em griddedTableDef
     * element to be reset for this GriddedTableDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param name a string detailing the \em name attribute.
     */
    void setName(
      const dstoute::aString& name)
    {
      name_ = name;
    }

    /**
     * This function permits the \em gtID index attribute of the \em griddedTableDef
     * element to be reset for this GriddedTableDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param gtID a string detailing the \em gtID index attribute.
     */
    void setGtID(
      const dstoute::aString& gtID)
    {
      gtID_ = gtID;
    }

    /**
     * This function permits the \em units attribute of the \em griddedTableDef
     * element to be reset for this GriddedTableDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param units a string detailing the \em units attribute.
     */
    void setUnits(
      const dstoute::aString& units)
    {
      units_ = units;
    }

    /**
     * This function permits the optional \em description of the
     * \em griddedTableDef element to be reset for this GriddedTableDef instance.
     * A \em griddedTableDef's \em description child element consists
     * of a string of arbitrary length, which can include tabs and new lines as
     * well as alphanumeric data.  This means pretty formatting of the XML source
     * will also appear in the returned description.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param description a string representation of the description.
     */
    void setDescription(
      const dstoute::aString& description)
    {
      description_ = description;
    }

    /**
     * This function permits a vector of \em breakpointRef's to be manually
     * set for the \em griddedTableDef element of this GriddedTableDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param breakpointRef a vector if breakpointRef indices (size_t) representing
     * the position of the breakpoint identifier in the breakpoint list managed by
     * the Janus instance.
     */
    void setBreakpointRefs(
      const std::vector< size_t> breakpointRef)
    {
      breakpointRef_.clear();
      breakpointRef_ = breakpointRef;
    }

    /**
     * This function permits a vector of data points to be manually
     * set for the \em griddedTableDef element of this GriddedTableDef instance.
     * The data points are interpreted as the numeric data table associated with
     * a gridded table.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param dataPoints a vector if numeric data values representing
     * the data table for the \em griddedTableDef.
     */
    void setTableData(
      const std::vector< double> dataPoints)
    {
      tableData_.clear();
      tableData_ = dataPoints;
    }

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em GriddedTableDef data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address pointer to the parent DOM node/element.
     */
    void exportDefinition(
      DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<(
      std::ostream &os,
      const GriddedTableDef &griddedTableDef);

    // ---- Internally reference functions. ----
    const DomFunctions::XmlNode& getDOMElement() { return domElement_;}

    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom(
      const DomFunctions::XmlNode& elementDefinition);
    bool compareElementID(
      const DomFunctions::XmlNode& xmlElement,
      const dstoute::aString& elementID,
      const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus(
      Janus* janus);

  private:

    void instantiateDataTable(
      const bool& checkBreakPointSize = true);

    /************************************************************************
     * These are the griddedTableDef elements, set up during instantiation.
     */
    Janus* janus_;
    DomFunctions::XmlNode domElement_;
    ElementDefinitionEnum elementType_;

    dstoute::aString name_;
    dstoute::aString gtID_;
    dstoute::aString units_;
    dstoute::aString description_;

    std::vector< size_t> breakpointRef_;
    std::vector< double> tableData_;

    bool hasProvenance_;
    Provenance provenance_;

    bool hasUncertainty_;
    Uncertainty uncertainty_;

    dstoute::aStringList stringTableData_;

    // used internally for instantiating data tables
    dstoute::aString tableCData_;
  };

  typedef dstoute::aList< GriddedTableDef> GriddedTableDefList;

}

#endif /* _GRIDDEDTABLEDEF_H_ */
