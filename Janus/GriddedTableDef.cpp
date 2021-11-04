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
// Module:     GriddedTableDef.cpp
// First Date: 2011-12-05
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file GriddedTableDef.cpp
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
#include <Ute/aMessageStream.h>

// Local Includes
#include "Janus.h"
#include "JanusUtilities.h"
#include "DomFunctions.h"
#include "GriddedTableDef.h"
#include "BreakpointDef.h"

using namespace std;
using namespace dstoute;

namespace janus
{

//------------------------------------------------------------------------//

  GriddedTableDef::GriddedTableDef() :
      XmlElementDefinition(),
      janus_( 0),
      elementType_( ELEMENT_GRIDDEDTABLE),
      hasProvenance_( false),
      hasUncertainty_( false)
  {
  }

  GriddedTableDef::GriddedTableDef(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition) :
      XmlElementDefinition(),
      janus_( janus),
      elementType_( ELEMENT_GRIDDEDTABLE),
      hasProvenance_( false),
      hasUncertainty_( false)
  {
    initialiseDefinition( janus, elementDefinition);
  }

//GriddedTableDef& GriddedTableDef::operator=( const GriddedTableDef &rhs )
//{
//  if ( this != &rhs) {
//  }
//
//  return *this;
//}
//
//------------------------------------------------------------------------//

  void GriddedTableDef::initialiseDefinition(
    Janus* janus,
    const DomFunctions::XmlNode& elementDefinition)
  {
    static const aString functionName( "GriddedTableDef::initialiseDefinition()");
    janus_ = janus;
    domElement_ = elementDefinition;

    /*
     * Retrieve the element attributes
     */
    name_ = DomFunctions::getAttribute( elementDefinition, "name");
    units_ = DomFunctions::getAttribute( elementDefinition, "units");
    gtID_ = DomFunctions::getAttribute( elementDefinition, "gtID");

    /*
     * The following has been included to support pre Version 2.0 DAVE-ML
     * files, where the gtID was not compulsory. A gtID is assign at random
     * to streamline internal Janus processes. It is first set and then
     * re-read from the DOM.
     */
    if ( gtID_.length() == 0) {
      aString randomGtID;
      DomFunctions::setAttribute( ( DomFunctions::XmlNode&) elementDefinition, "gtID", randomGtID.random( 20));
      gtID_ = DomFunctions::getAttribute( elementDefinition, "gtID");
    }

    /*
     * Retrieve the description associated with the variable
     */
    description_ = DomFunctions::getChildValue( elementDefinition, "description");

    /*
     * Retrieve the optional Provenance associated with the element
     */
    elementType_ = ELEMENT_PROVENANCE;
    try {
      DomFunctions::initialiseChildOrRef( this, elementDefinition, EMPTY_STRING, "provenance", "provenanceRef",
        "provID", false);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve the breakpoint Reference element
     */
    elementType_ = ELEMENT_BREAKPOINTS;
    try {
      DomFunctions::initialiseChildrenOrRefs( this, elementDefinition, gtID_, "breakpointDef", "breakpointRefs",
        "bpRef", "bpID", true);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
        << excep.what()
      );
    }

    /*
     * Retrieve the data table element
     */
    elementType_ = ELEMENT_DATATABLE;
    try {
      DomFunctions::initialiseChild( this, elementDefinition, gtID_, "dataTable", false);
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
        << excep.what()
      );
    }

    /*
     * Once the data table has been read need to check the
     * dimension size against number of breakpoints points.
     * If not consistent then trigger an error command.
     * If consistent need to convert the data table to
     * numeric representations, set up ancestry and
     * descendant linkages.
     */
    try {
      instantiateDataTable();
    }
    catch ( exception &excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName) << "\n - for function \"" << name_ << "\"\n - "
        << excep.what()
      );
    }

    /*
     * Uncertainty is processed in Janus, since it requires cross-referencing.
     * Just set the uncertainty flag if it is present for this griddedTableDef.
     */
    hasUncertainty_ = DomFunctions::isChildInNode( elementDefinition, "uncertainty");
  }

//------------------------------------------------------------------------//
  void GriddedTableDef::exportDefinition(
    DomFunctions::XmlNode& documentElement)
  {
    /*
     * Create a child node in the DOM for the GriddedTableDef element
     */
    DomFunctions::XmlNode childElement = DomFunctions::setChild( documentElement, "griddedTableDef");

    /*
     * Add attributes to the GriddedTableDef child
     */
    if ( !name_.empty()) {
      DomFunctions::setAttribute( childElement, "name", name_);
    }

    DomFunctions::setAttribute( childElement, "gtID", gtID_);

    if ( !units_.empty()) {
      DomFunctions::setAttribute( childElement, "units", units_);
    }

    /*
     * Add description element
     */
    if ( !description_.empty()) {
      DomFunctions::setChild( childElement, "description", description_);
    }

    /*
     * Add the optional provenance entry to the GriddedTableDef child
     */
    if ( hasProvenance_) {
      provenance_.exportDefinition( childElement);
    }

    /*
     * Add the breakpoint reference list to the GriddedTableDef child
     */
    DomFunctions::XmlNode breakpointRefElement = DomFunctions::setChild( childElement, "breakpointRefs");

    DomFunctions::XmlNode breakpointRefEntry;
    vector< BreakpointDef>& breakPointDefList = janus_->getBreakpointDef();

    for ( size_t i = 0; i < breakpointRef_.size(); i++) {
      breakpointRefEntry = DomFunctions::setChild( breakpointRefElement, "bpRef");
      DomFunctions::setAttribute( breakpointRefEntry, "bpID", breakPointDefList[ breakpointRef_[ i]].getBpID());
    }

    /*
     * Add the optional uncertainty entry to the GriddedTableDef child
     */
    if ( hasUncertainty_) {
      uncertainty_.exportDefinition( childElement);
    }

    /*
     * Add the data table values
     */
    aString dataTableStr;
    size_t numberBreakPoints = breakPointDefList[ breakpointRef_.back()].getNumberOfBpVals();
    size_t j = 0;

    for ( size_t i = 0; i < tableData_.size(); i++) {
      dataTableStr += aString( "%").arg( tableData_[ i], 16);
      dataTableStr += ",";
      j++;
      if ( j == numberBreakPoints) {
        dataTableStr += "\n";
        j = 0;
      }
    }
    DomFunctions::setChild( childElement, "dataTable", dataTableStr);
  }

  void GriddedTableDef::readDefinitionFromDom(
    const DomFunctions::XmlNode& xmlElement)
  {
    switch ( elementType_) {
    case ELEMENT_PROVENANCE:
      provenance_.initialiseDefinition( xmlElement);
      hasProvenance_ = true;
      break;

    case ELEMENT_DATATABLE:
      /*
       * Initially read the data table as a string table.
       * This will be converted to a numeric table after its size has been
       * checked for correctness and non-numeric entries have been evaluated.
       */
    {
      tableCData_ = DomFunctions::getCData( xmlElement);
    }
      break;

    default:
      break;
    }

    return;
  }

//------------------------------------------------------------------------//

  bool GriddedTableDef::compareElementID(
    const DomFunctions::XmlNode& xmlElement,
    const aString& elementID,
    const size_t& documentElementReferenceIndex)
  {

    switch ( elementType_) {
    case ELEMENT_PROVENANCE:
      if ( DomFunctions::getAttribute( xmlElement, "provID") != elementID) {
        return false;
      }
      readDefinitionFromDom( xmlElement);
      break;

    case ELEMENT_BREAKPOINTS:
      if ( DomFunctions::getAttribute( xmlElement, "bpID") != elementID) {
        return false;
      }
      breakpointRef_.push_back( documentElementReferenceIndex);
      break;

    default:
      return false;
      break;
    }

    return true;
  }

//------------------------------------------------------------------------//

  void GriddedTableDef::resetJanus(
    Janus *janus)
  {
    janus_ = janus;

    /*
     * Reset the Janus pointer in the Uncertainty class
     */
    uncertainty_.resetJanus( janus);
  }

//------------------------------------------------------------------------//

  void GriddedTableDef::instantiateDataTable(
    const bool& checkBreakPointSize)
  {
    static const aString functionName( "GriddedTableDef::instantiateDataTable()");

    /*
     * Check whether the dataTable has numeric entries
     * It is assumed that if the first entry is non-numeric
     * then the table should be treated as a string data table.
     */
    if ( isNumericTable( tableCData_.c_str())) {
      tableData_ = tableCData_.toDoubleList( JANUS_DELIMITERS);
    }
    else {
      stringTableData_ = tableCData_.toStringList( JANUS_STRING_DELIMITERS);
    }
    tableCData_.clear();

    if ( checkBreakPointSize) {
      /*
       * Once the data table has been read need to check the
       * dimension size against number of breakpoints.
       * If not consistent then trigger an error command.
       * If consistent need to convert the data table to
       * numeric representations, set up ancestry and
       * descendant linkages.
       */

      /*
       * Check the number of entries in the dataTable is consistent
       * with the number of breakpoints
       */
      size_t numberOfBreakpoints = 1;
      size_t breakPointRefLength = breakpointRef_.size();
      vector< BreakpointDef> breakPointDefList = janus_->getBreakpointDef();

      for ( size_t i = 0; i < breakPointRefLength; i++) {
        numberOfBreakpoints *= breakPointDefList[ breakpointRef_[ i]].getNumberOfBpVals();
      }

      if ( ( numberOfBreakpoints != tableData_.size()) && ( numberOfBreakpoints != stringTableData_.size())) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName) << "\n - The Gridded Data Table \"" << gtID_
          << "\" has an incorrect number of entries" << "\" defined for the dataTable.\n" << "\" The table size \""
          << tableData_.size() << "\" does not match the number\"" << "\" of breakpoints \"" << numberOfBreakpoints
          << "\""
        );
      }
    }

    return;
  }

//------------------------------------------------------------------------//

  ostream& operator<<(
    ostream &os,
    const GriddedTableDef &griddedTableDef)
  {
    /*
     * General properties of the Class
     */
    os << endl << endl << "Display GriddedTableDef contents:" << endl << "-----------------------------------" << endl;

    os << "  name               : " << griddedTableDef.getName() << endl << "  gtID               : "
      << griddedTableDef.getGtID() << endl << "  units              : " << griddedTableDef.getUnits() << endl
      << "  description        : " << griddedTableDef.getDescription() << endl << "  hasProvenance      : "
      << griddedTableDef.hasProvenance() << endl << "  hasUncertainty     : " << griddedTableDef.hasUncertainty()
      << endl << endl;

    /*
     * Provenance data for the Class
     */
    if ( griddedTableDef.hasProvenance()) {
      os << griddedTableDef.getProvenance() << endl;
    }

    /*
     * Uncertainty data for the Class
     */
    if ( griddedTableDef.hasUncertainty()) {
      os << griddedTableDef.uncertainty_ << endl;
    }

    /*
     * Breakpoints associated with Class
     */
    Janus* janus = griddedTableDef.janus_;
    const vector< BreakpointDef>& breakpointDef = janus->getBreakpointDef();
    os << "Breakpoint Def Size : " << janus->getBreakpointDef().size() << endl;

    const vector< size_t>& bpRef = griddedTableDef.getBreakpointRef();
    for ( size_t i = 0; i < bpRef.size(); i++) {
      os << "Breakpoint Reference : " << bpRef[ i] << endl;
      os << breakpointDef[ bpRef[ i]] << endl;
    }

    /*
     * Data associated with Class
     */
    const vector< double>& dataTable = griddedTableDef.getData();
    for ( size_t i = 0; i < dataTable.size(); i++) {
      os << "  dataPoint " << i << " = " << dataTable[ i] << endl;
    }

    /*
     * String Data associated with Class
     */
    const vector< aString>& stringTableData = griddedTableDef.getStringData();
    for ( size_t i = 0; i < stringTableData.size(); i++) {
      os << "  string data " << i << " = " << stringTableData[ i] << endl;
    }

    return os;
  }

//------------------------------------------------------------------------//

}// namespace janus

