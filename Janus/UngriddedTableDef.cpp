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
// Title:      Janus/UngriddedTableDef
// Class:      UngriddedTableDef
// Module:     UngriddedTableDef.cpp
// First Date: 2009-05-08
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file UngriddedTableDef.cpp
 *
 * A UngriddedTableDef instance holds in its allocated memory alphanumeric data
 * derived from a \em ungriddedTableDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes points that are
 * not in an orthogonal grid pattern; thus, the independent variable
 * coordinates are specified for each dependent variable value.  The table data
 * point values are specified as comma-separated values in floating-point
 * notation.  Associated alphanumeric identification and cross-reference data
 * are also included in the instance.
 *
 * The UngriddedTableDef class is only used within the janus namespace,
 * and should only be referenced through the Janus class.
 */

/*
 * Author: D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes 
#include <cstdio>

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aOptional.h>

#include "Janus.h"
#include "DomFunctions.h"
#include "UngriddedTableDef.h"
#include "BreakpointDef.h"

#ifndef HAVE_QHULL5
  #ifndef HAVE_QHULL6
    #define HAVE_QHULL7
  #endif
#endif

#if defined( HAVE_QHULL7)
  #include <libqhullcpp/QhullError.h>
  #include <libqhullcpp/QhullQh.h>
  #include <libqhullcpp/QhullFacet.h>
  #include <libqhullcpp/QhullFacetList.h>
  #include <libqhullcpp/QhullFacetSet.h>
  #include <libqhullcpp/QhullLinkedList.h>
  #include <libqhullcpp/QhullVertex.h>
  #include <libqhullcpp/QhullVertexSet.h>
  #include <libqhullcpp/QhullPoint.h>
  #include <libqhullcpp/QhullPoints.h>
  #include <libqhullcpp/QhullPointSet.h>
  #include <libqhullcpp/Qhull.h>
  using namespace orgQhull;

#else

  #if defined(__cplusplus)
  extern "C"
  {
  #endif
  #if defined( HAVE_QHULL6)
    //   qh_QHpointer: access global data with pointer or static structure (see libqhull user.h)
    //
    //   qh_QHpointer  = 1     access globals via a pointer to allocated memory
    //                         enables qh_saveqhull() and qh_restoreqhull()
    //                         [2010, gcc] costs about 4% in time and 4% in space
    //                         [2003, msvc] costs about 8% in time and 2% in space
    //
    //                 = 0     qh_qh and qh_qhstat are static data structures
    //                         only one instance of qhull() can be active at a time
    //                         default value
    #define qh_QHpointer 1 // MUST BE SET TO 1 (SDH 29/03/2012)
    #include <libqhull/libqhull.h>
    #include <libqhull/mem.h>
    #include <libqhull/qset.h>
    #include <libqhull/geom.h>
    #include <libqhull/merge.h>
    #include <libqhull/poly.h>
    #include <libqhull/io.h>
    #include <libqhull/stat.h>

  #elif defined( HAVE_QHULL5)
    #include <qhull/qhull.h>
    #include <qhull/mem.h>
    #include <qhull/qset.h>
    #include <qhull/geom.h>
    #include <qhull/merge.h>
    #include <qhull/poly.h>
    #include <qhull/io.h>
    #include <qhull/stat.h>

  #endif

  #if defined(__cplusplus)
  }
  #endif

#endif

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

UngriddedTableDef::UngriddedTableDef( )
  :
  XmlElementDefinition(),
  janus_( 0),
  elementType_( ELEMENT_NOTSET),
  independentVarCount_( 0),
  numberDataTableColumns_( 0),
  hasProvenance_( false ),
  hasUncertainty_( false ),
  lastUsedSimplex_( 0)
{
}


UngriddedTableDef::UngriddedTableDef(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
  :
  XmlElementDefinition(),
  janus_( janus),
  elementType_( ELEMENT_NOTSET),
  independentVarCount_( 0),
  numberDataTableColumns_( 0),
  hasProvenance_( false ),
  hasUncertainty_( false ),
  lastUsedSimplex_( 0)
{
  initialiseDefinition( janus, elementDefinition);
}

//------------------------------------------------------------------------//

void UngriddedTableDef::initialiseDefinition(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
{
  static const aString functionName( "UngriddedTableDef::initialiseDefinition()");

  janus_ = janus;
  domElement_ = elementDefinition;
  
  /*
   * Retrieve the element attributes
   */
  name_  = DomFunctions::getAttribute( elementDefinition, "name");
  units_ = DomFunctions::getAttribute( elementDefinition, "units");
  utID_  = DomFunctions::getAttribute( elementDefinition, "utID");
  independentVarCount_ = DomFunctions::getAttribute( elementDefinition,
                                                     "independentVarCount").toSize_T();
  
  /*
   * The following has been included to support pre Version 2.0 DAVE-ML
   * files, where the utID was not compulsory. A utID is assign at random
   * to streamline internal Janus processes. It is first set and then
   * re-read from the DOM.
   */
  if ( 0 == utID_.length()) {
    aString randomUtID;
    DomFunctions::setAttribute( (DomFunctions::XmlNode&)elementDefinition, "utID",
                                randomUtID.random( 20));
    utID_ = DomFunctions::getAttribute( elementDefinition, "utID");
  }

  /*
   * Retrieve the description associated with the variable
   */
  description_ = DomFunctions::getChildValue( elementDefinition,
                                              "description" );
   
  /*
   * Retrieve the optional Provenance associated with the element
   */
  elementType_ = ELEMENT_PROVENANCE;
  try {
    DomFunctions::initialiseChildOrRef( this,
                                        elementDefinition,
                                        EMPTY_STRING,
                                        "provenance",
                                        "provenanceRef",
                                        "provID",
                                        false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - for function \"" << name_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Uncertainty is processed in Janus, since it requires cross-referencing.
   * Just set the uncertainty flag if it is present for this ungriddedTableDef.
   */
   hasUncertainty_ = DomFunctions::isChildInNode( elementDefinition,
                                                  "uncertainty");

  /*
   * Retrieve the data for the ungridded table
   */  
  elementType_ = ELEMENT_DATAPOINTS;
  try {
    DomFunctions::initialiseChildren( this,
                                      elementDefinition,
                                      utID_,
                                      "dataPoint",
                                      true);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - for function \"" << name_ << "\"\n - "
      << excep.what()
    );
  }
  
  if ( 0 < dataPoint_.size()) {
    numberDataTableColumns_ = dataPoint_[0].size();
  }

  /*
   * Check that independentVarCount is set to a value > 0
   * If equal to 0 then set to # data for each row - 1
   */
  if ( 0 == independentVarCount_ &&
       0 < numberDataTableColumns_) {
    independentVarCount_ = numberDataTableColumns_ - 1;
  }

  /*
   * Extract the dependent and independent data from the dataPoint_ array
   * and store as columns/matrix of data.
   * This should speed up the ungridded table interpolation calculations.
   */
  setupDependentDataColumns();

  setupIndependentDataColumns();

  /*
   * Delaunay Triangulation
   */
  triangulateData();
}

//------------------------------------------------------------------------//

void UngriddedTableDef::setupDependentDataColumns()
{
  /*
   * Dependent data is extracted from the dataPoint_ array and stored
   * as vectors of column data. Each vector represents a dependent data
   * variable, original stored as a column in the larger data array.
   */
  size_t dataPointSize = dataPoint_.size();
  size_t dependentColumnCount = numberDataTableColumns_ - independentVarCount_;
  size_t columnNumber;
  vector<double> dataColumn;

  for ( size_t i = 0; i < dependentColumnCount; i++) {
    columnNumber = i + independentVarCount_;
    dataColumn.clear();
    for ( size_t j = 0; j < dataPointSize; j++) {
      dataColumn.push_back( dataPoint_[j][columnNumber]);
    }
    dependentDataColumns_.push_back( dataColumn);
  }
}

//------------------------------------------------------------------------//

void UngriddedTableDef::setupIndependentDataColumns()
{
  /*
   * Independent data is extracted from the dataPoint_ array and stored
   * as vectors of column data. Each vector represents an independent data
   * variable, original stored as a column in the larger data array.
   */
  size_t dataPointSize = dataPoint_.size();

  independentDataColumns_.resize( dataPointSize, independentVarCount_);

  for ( size_t i = 0; i < independentVarCount_; i++) {
    for ( size_t j = 0; j < dataPointSize; j++) {
      independentDataColumns_( j, i) = dataPoint_[j][i];
    }
  }
}

//------------------------------------------------------------------------//

void UngriddedTableDef::triangulateData()
{
  static const aString functionName( "UngriddedTableDef::triangulateData()");
  /*
   * data is now complete in a series of arrays.  
   * 
   * ungridTableDataLength = dataPoint_.size();
   * ungridTableDataColumn = dataPoint_[0].second.size();
   *
   * Input data is in:
   *
   * dataPoint_[j = 0 ; j < ungridTableDataLength ; j++]
   *           [k = 0 ; k < independentVarCount_ ; k++]
   *
   * and output values are in:
   *
   * dataPoint_[j = 0 ; j < ungridTableDataLength ; j++]
   *           [k = independentVarCount_; k < ungridTableDataColumns; k++]
   *
   * ready for Delaunay triangulation
   */
  
  size_t nDel;
  try {  
    nDel = getDelaunaySimplex( );
    lastUsedSimplex_ = nDel;
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }

  /*
   * The triangulation is now contained in "delaunay_", with one row for
   * each simplex and one column for each vertex of the simplex.
   *
   * set up the workspaces - this uses more memory, but saves run time
   * during ungridded interpolation.  Then set up an index of all simplices 
   * which connect to each data point, which will be used for distances from a 
   * requested point, to speed up interpolation searches.  
   *
   */
  size_t iVer;
  size_t nVer = delaunay_[ 0 ].size();
  size_t nCol = independentVarCount_;
  
  centroid_.resize( nDel, nCol );
  weight_.resize( nVer );
  xival_.resize( nVer );
  A_.resize( nVer, nVer );
  
  centroid_.zero();
  for ( size_t i = 0 ; i < nDel ; i++ ) {
    for ( size_t j = 0 ; j < nCol ; j++ ) {
      for ( size_t k = 0 ; k < nVer ; k++ ) {
        iVer = delaunay_[ i ][ k ];
        centroid_( i, j ) += independentDataColumns_( iVer, j );
      }
      centroid_( i, j ) = centroid_( i, j ) / nVer;
    }
  }

  /*
   * Create the connection data array, 'connect'.
   */
  size_t nRows = independentDataColumns_.rows();
  connect_.resize( nRows );
  vector<size_t> connectRow;
  for ( size_t i = 0 ; i < nRows ; i++ ) {
    connectRow.clear();
    for ( size_t j = 0 ; j < nDel ; j++ ) {
      for ( size_t k = 0 ; k < nVer ; k++ ) {
        if ( i == delaunay_[ j ][ k ] ) {
          connectRow.push_back( j );
        }
      }
    }
    connect_[ i ] = connectRow;
  }

  return;
}

//------------------------------------------------------------------------//

#if defined( HAVE_QHULL7)

size_t UngriddedTableDef::getDelaunaySimplex()
{
  static const aString functionName( "UngriddedTableDef::getDelaunaySimplex() for Qhull-7.x up");

  //
  // This function IS thread safe since Qhull-7.x.
  // The Qhull-6.x versions MAY BE thread safe.
  // The Qhull-5.x versions ARE NOT thread safe.
  //

  /*
   * Normalise the data before triangulation
   */
  bool applyNormalisation = true;

  dstomath::DMatrix triData = independentDataColumns_;
  if ( applyNormalisation) {
    dstomath::DMatrix columnData( independentDataColumns_.rows(), 1);

    for ( size_t i = 0; i < independentVarCount_; i++) {
      columnData = independentDataColumns_(i);
      triData(i) = columnData(0).normalise();
    }
  }

  /*
   * option "Qt" makes all simplices have nCols+1 vertices, even if
   * some are degenerate.  This simplifies the interpolation.
   * "QJ" achieves the same result by different means. "QJ" seems slightly
   * more robust on our problems, but may give a slightly less accurate
   * interpolation.  If this looks problematic, swap "QJ" for "Qt"
   */
//  const char flags[] = "d Qbb Qx Qc Qs QJ";
  const char flags[] = "d Qbb Qx Qc Qs Qt"; // Use Qt by SDH 22/06/2017
  size_t nRows = independentDataColumns_.rows();
  size_t nCols = independentVarCount_;

  delaunay_.clear();
  Qhull qhull( "Janus Ungridded Table Qhull", static_cast<int>( nCols), static_cast<int>( nRows), &triData.matrixData()[0], flags);
  vector<QhullFacet> facet = qhull.facetList().toStdVector();
  for ( size_t i = 0; i < facet.size(); ++i) {
    if ( !facet[ i].isUpperDelaunay()) {
      delaunay_.push_back( vector<size_t>());
      vector<QhullVertex> vertex = facet[ i].vertices().toStdVector();
      for ( size_t j = 0; j < vertex.size(); ++j) {
        delaunay_.back().push_back( vertex[ j].point().id());
      }
    }
  }

  return delaunay_.size();
}

#else

size_t UngriddedTableDef::getDelaunaySimplex( )
{
  static const aString functionName( "UngriddedTableDef::getDelaunaySimplex() for Qhull-6.x and Qhull-5.x up");

  size_t nRows   = independentDataColumns_.rows();
  size_t nCols   = independentVarCount_;
  boolT ismalloc = false;
  char flags[]   = "qhull d Qbb Qx Qc Qs QJ ";
  /*
   * option "Qt" makes all simplices have nCols+1 vertices, even if
   * some are degenerate.  This simplifies the interpolation.
   * "QJ" achieves the same result by different means. "QJ" seems slightly
   * more robust on our problems, but may give a slightly less accurate
   * interpolation.  If this looks problematic, swap "QJ" for "Qt"
   */
  FILE* errfile = stderr;

  /*
   * Normalise the data before triangulation
   */
  bool  applyNormalisation = true;

  dstomath::DMatrix triData = independentDataColumns_;
  if ( applyNormalisation) {
    dstomath::DMatrix columnData(nRows,1);

    for ( size_t i = 0; i < independentVarCount_; i++) {
      columnData = independentDataColumns_(i);
      triData(i) = columnData(0).normalise();
    }
  }

  int nRet = qh_new_qhull( nCols, nRows, &triData.matrixData()[0], ismalloc,
                           flags, NULL, errfile );
  if ( 0 != nRet ) {
    throw_message( logic_error,
      setFunctionName( functionName)
      << "\n - Problem with qh_new_qhull."
    );
  }
  qh_triangulate();
  qh_checkpolygon (qh facet_list);

  facetT *facet;
  vertexT *vertex, **vertexp;

  /*
   * fill the array with the vertices of each facet
   */
  delaunay_.clear();
  vector<size_t> rowVec;
  FORALLfacets {
    if ( !facet->upperdelaunay ) {
      rowVec.clear();
      FOREACHvertex_ ( facet->vertices ) {
        rowVec.push_back( qh_pointid( vertex->point ) );
      }
      delaunay_.push_back( rowVec );
    }
  }

  qh_freeqhull( !qh_ALL );
  //free long memory

  int curlong, totlong;
  qh_memfreeshort ( &curlong, &totlong );
  //free short memory and memory allocator

  if (curlong || totlong) {
    throw_message( logic_error,
      setFunctionName( functionName)
      << "\n - Delaunay did not free long memory."
    );
  }

  return delaunay_.size();
}

#endif
//------------------------------------------------------------------------//

void UngriddedTableDef::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the UngriddedTableDef element
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "ungriddedTableDef");

  /*
   * Add attributes to the UngriddedTableDef child
   */
  DomFunctions::setAttribute( childElement, "name",  name_);
  DomFunctions::setAttribute( childElement, "utID",  utID_);
  DomFunctions::setAttribute( childElement, "units", units_);
  DomFunctions::setAttribute( childElement, "independentVarCount",
                                   aString("%").arg( independentVarCount_));

  /*
   * Add description element
   */
  DomFunctions::setChild( childElement, "description", description_);

  /*
   * Add the optional provenance entry to the UngriddedTableDef child
   */
  if ( hasProvenance_) {
    provenance_.exportDefinition( childElement);
  }

  /*
   * Add the optional uncertainty entry to the UngriddedTableDef child
   */
  if ( hasUncertainty_) {
    uncertainty_.exportDefinition( childElement);
  }

  /*
   * Add the data points
   */
  size_t numberColumns = dataPoint_[0].size();
  size_t numberRows = dataPoint_.size();

  DomFunctions::XmlNode dataPointElement;
  aString dataPointStr;
  for ( size_t i = 0; i < numberRows; i++) {
    dataPointStr.clear();

    for ( size_t j = 0; j < numberColumns; j++) {
      dataPointStr += aString("%").arg( dataPoint_[i][j], 16);
      dataPointStr += ",";
    }

    dataPointElement = DomFunctions::setChild( childElement,
                                               "dataPoint", dataPointStr);

    if ( dataPointModID_[i].isValid()) {
      DomFunctions::setAttribute(
          dataPointElement, "modID",
          janus_->getFileHeader().getModification( dataPointModID_[i]).getModID());
    }
  }
}

void UngriddedTableDef::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  switch ( elementType_) {
    case ELEMENT_PROVENANCE:
      provenance_.initialiseDefinition( xmlElement);
      hasProvenance_ = true;
      break;
	  
	case ELEMENT_DATAPOINTS:
	  {
	    // Determine the index for the modID
	    aString modIDAttribute = DomFunctions::getAttribute( xmlElement, "modID");
	    aOptionalSizeT modID = janus_->crossReferenceId( ELEMENT_MODIFICATION, modIDAttribute);
	    dataPointModID_.push_back( modID);

	    // Read the data and convert to numerical values
	    dataPoint_.push_back( DomFunctions::getCData( xmlElement).toDoubleList( JANUS_DELIMITERS));
  	};
	  break;
	  
    default:
      break;
  }

  return;
}

bool UngriddedTableDef::compareElementID(
  const DomFunctions::XmlNode& xmlElement,
  const aString& elementID,
  const size_t& /*documentElementReferenceIndex*/)
{
  if ( DomFunctions::getAttribute( xmlElement, "provID") != elementID) {
    return false;
  }
  readDefinitionFromDom( xmlElement);  
  
  return true;
}


//------------------------------------------------------------------------//

void UngriddedTableDef::resetJanus( Janus *janus)
{
  janus_ = janus;

  /*
   * Reset the Janus pointer in the Uncertainty class
   */
  uncertainty_.resetJanus( janus);
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const UngriddedTableDef &ungriddedTableDef)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display UngriddedTableDef contents:" << endl
     << "-----------------------------------" << endl;

  os << "  name                : " << ungriddedTableDef.getName() << endl
     << "  utID                : " << ungriddedTableDef.getUtID() << endl
     << "  units               : " << ungriddedTableDef.getUnits() << endl
     << "  independentVarCount : " << ungriddedTableDef.getIndependentVarCount() << endl
     << "  description         : " << ungriddedTableDef.getDescription() << endl
     << "  hasProvenance       : " << ungriddedTableDef.hasProvenance() << endl
     << "  hasUncertainty      : " << ungriddedTableDef.hasUncertainty() << endl
     << endl;

  /*
   * Provenance data for the Class
   */
  if ( ungriddedTableDef.hasProvenance()) {
    os << ungriddedTableDef.getProvenance() << endl;
  }

  /*
   * Uncertainty data for the Class
   */
  if ( ungriddedTableDef.hasUncertainty()) {
    os << ungriddedTableDef.uncertainty_ << endl;
  }

  /*
   * Data associated with Class
   */
  const vector< vector<double> >& dataTable = ungriddedTableDef.getData();
  const vector<aOptionalSizeT>& dataPointModID = ungriddedTableDef.getDataPointModID();

  for ( size_t i = 0; i < dataTable.size(); i++) {
    os << "  dataPoint Row " << i << " :";
    os << " ModID : " << dataPointModID[i] << " :: ";

    for ( size_t j = 0; j < dataTable[i].size(); j++) {
      os << dataTable[i][j] << ", ";
    }
    os << endl;
  }

  /*
   * Delaunay triangulation data
   */
  const vector< vector<size_t> >& delaunay = ungriddedTableDef.getDelaunay();

  os << "Delaunay Data" << endl;
  for ( size_t i = 0; i < delaunay.size(); i++) {
    os << "  row " << i << " ::";

    for ( size_t j = 0; j < delaunay[i].size(); j++) {
      os << delaunay[i][j] << ", ";
    }
    os << endl;
  }

  const vector< vector<size_t> >& connect = ungriddedTableDef.getConnect();

  os << "Connect Data" << endl;
  for ( size_t i = 0; i < connect.size(); i++) {
    os << "  row " << i << " ::";

    for ( size_t j = 0; j < connect[i].size(); j++) {
      os << connect[i][j] << ", ";
    }
    os << endl;
  }

  const dstomath::DMatrix& centroid = ungriddedTableDef.getCentroid();
  os << "Centroid Data" << endl;
  os << centroid;
  os << endl;

  const dstomath::DVector& xiVal  = ungriddedTableDef.xival_;
  os << "xiVal Data" << endl;
  for ( size_t j = 0; j < xiVal.size(); j++) {
    os << xiVal[j] << ", ";
  }
  os << endl;

  const dstomath::DVector& weight  = ungriddedTableDef.weight_;
  os << "Weight Data" << endl;
  for ( size_t j = 0; j < weight.size(); j++) {
    os << weight[j] << ", ";
  }
  os << endl;

  const dstomath::DMatrix& A = ungriddedTableDef.A_;
  os << "A Data" << endl;
  os << A;
  os << endl;

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus

