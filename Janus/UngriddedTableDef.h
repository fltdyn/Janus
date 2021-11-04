#ifndef _UNGRIDDEDTABLEDEF_H_
#define _UNGRIDDEDTABLEDEF_H_

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
// Module:     UngriddedTableDef.h
// First Date: 2009-05-27
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file UngriddedTableDef.h
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
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes 
#include <utility>

// Ute Includes
#include <Ute/aOptional.h>

// Local Includes
#include "Provenance.h"
#include "Uncertainty.h"
#include <Ute/aMatrix.h>
#include <Ute/aString.h>

namespace janus {

  class Janus;

//typedef std::pair<size_t, std::vector<double>> dataPointPair;

  /**
   * A #UngriddedTableDef instance holds in its allocated memory alphanumeric data
   * derived from a \em ungriddedTableDef element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  It includes points that are
   * not in an orthogonal grid pattern; thus, the independent variable
   * coordinates are specified for each dependent variable value.  The table data
   * point values are specified as comma-separated values in floating-point
   * notation.  Associated alphanumeric identification and cross-reference data
   * are also included in the instance.
   *
   * The #UngriddedTableDef class is only
   * used within the janus namespace, and should only be referenced through the
   * Janus class.
   *
   * Janus exists to abstract data form and handling from a modelling process.
   * Therefore, in normal computational usage, it is unnecessary and undesirable
   * for a calling program to even be aware of the existence of this class.
   * However, functions do exist to access #UngriddedTableDef contents directly,
   * which may be useful during dataset development.  A possible usage might be:
  \code
    Janus test( xmlFileName );
    const vector<UngriddedTableDef>& ungriddedTableDef =
      test.getUngriddedTableDef();
    for ( int i = 0 ; i < ungriddedTableDef.size() ; i++ ) {
      cout << " Ungridded table " << i << " :\n"
           << "   name        = " << ungriddedTableDef.at( i ).getName() << "\n"
           << "   gtID        = " << ungriddedTableDef.at( i ).getGtID() << "\n"
           << "   units       = " << ungriddedTableDef.at( i ).getUnits() << "\n"
           << "   description = " << ungriddedTableDef.at( i ).getDescription()
           << "\n";
    }
  \endcode
   */
  class UngriddedTableDef : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #UngriddedTableDef class
     * without supplying the DOM \em ungriddedTableDef element from which the
     * instance is constructed, but in this state is not useful for any class
     * functions.
     * It is necessary to populate the class from a DOM containing a
     * \em ungriddedTableDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    UngriddedTableDef( );

    /**
     * The constructor, when called with an argument pointing to a
     * \em ungriddedTableDef element within a DOM, instantiates the
     * #UngriddedTableDef class and fills it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of an \em ungriddedTableDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    UngriddedTableDef( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #UngriddedTableDef is filled with data from a
     * particular \em ungriddedTableDef element within a DOM by this function.
     * If another \em ungriddedTableDef element pointer is supplied
     * to an instance that has already been initialised, data corruption will
     * occur and the entire Janus instance will become unusable.  This
     * function can also be used with the deprecated \em ungriddedTable element.
     * For backwards compatibility, Janus converts an \em ungriddedTable to the
     * equivalent \em ungriddedTableDef within this function.  Where an
     * \em ungriddedTableDef or \em ungriddedTable lacks a \em utID attribute,
     * this function will generate a random \em utID string for indexing within
     * the Janus class.
     *
     * \param elementDefinition is an address of an \em ungriddedTableDef
     * component within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the document object model
     * element, as a pointer, associated with this instance of the
     * #UngriddedTableDef. It is used internally within Janus when
     * instantiating a DAVE-ML compliant XML file that contains ungridded
     * tables. It should not be used by external applications as the
     * pointer will be invalidated once a file has been successfully
     * instantiated, and therefore may cause the external application
     * to fail.
     *
     * \return A pointer (DomFunctions::XmlNode) to the DOM element associated with
     * this instance of the #UngriddedTableDef.
     */
    const DomFunctions::XmlNode& getDOMElement() { return domElement_;}

    /**
     * This function provides access to the \em name attribute of a
     * \em ungriddedTableDef.  The \em name attribute is optional.
     * If the ungridded table has no name attribute or has not been initialised
     * from a DOM, an empty string is returned.
     *
     * \return The \em name string is returned by reference.
     */
    const dstoute::aString& getName( ) const { return name_; }

    /**
     * This function provides access to the \em units attribute
     * of the \em ungriddedTableDef represented by this #UngriddedTableDef
     * instance.  An ungridded table's \em units attribute is a string of
     * arbitrary length, but normally short, and complying with the format
     * requirements \latexonly chosen by \ac{DST} \ac{AD} \ac{APS} \cite{brian1} in
     * accordance with \ac{SI} \endlatexonly \htmlonly of SI \endhtmlonly and
     * other systems.
     * If the instance has not been initialised from a DOM, or if no \em units
     * attribute is present, an empty string is returned.
     *
     * \return The \em units string is returned by reference.
     */
    const dstoute::aString& getUnits( ) const { return units_; }

    /**
     * This function provides access to the \em utID attribute of an
     * \em ungriddedTableDef.  This attribute is used for indexing ungridded
     * tables within an XML dataset.  Where an \em ungriddedTableDef within the
     * DOM does not contain a \em utID attribute, or where an \em ungriddedTable
     * has been placed in the #UngriddedTableDef structure,
     * a \em utID string is generated and inserted in the DOM at initialisation
     * time.  If the instance has not been initialised from
     * a DOM, an empty string is returned.
     *
     * \return The \em utID string is returned by reference.
     */
    const dstoute::aString& getUtID( ) const { return utID_; }

    /**
     * This function provides access to the \em independentVarCount attribute
     * of the \em ungriddedTableDef represented by this #UngriddedTableDef
     * instance.  An ungridded table's \em independentVarCount attribute
     * is a count of the number of independent data variables defined in
     * the \em dataPoint elements.
     * If the instance has not been initialised from a DOM, or if no
     * \em independentVarCount attribute is present, a zero value is returned.
     *
     * \return The \em independentVarCount value is returned by reference.
     */
    const size_t& getIndependentVarCount( ) const { return independentVarCount_; }

    /**
     * This function provides access to the \em description child of the
     * \em ungriddedTableDef element represented by this #UngriddedTableDef
     * instance.  An \em ungriddedTableDef's optional \em description child
     * element consists of a string of arbitrary length, which can include tabs
     * and new lines as well as alphanumeric data.  This means text formatting
     * embedded in the XML source will also appear in the returned description.
     * If no description is specified in the XML dataset, or the #UngriddedTableDef
     * has not been initialised from the DOM, an empty string is returned.
     *
     * \return The \em description string is returned by reference.
     */
    const dstoute::aString& getDescription( ) const { return description_; }

    /**
     * This function indicates whether an \em ungriddedTableDef element of a
     * DAVE-ML dataset includes either \em provenance or \em provenanceRef.
     *
     * \return A boolean variable, 'true' if the \em ungriddedTableDef includes a
     * provenance, defined either directly or by reference.
     *
     * \sa Provenance
     */
    const bool& hasProvenance( ) const { return hasProvenance_; }

    /**
     * This function provides access to the Provenance instance
     * associated with a #UngriddedTableDef instance.  There may be zero or one
     * of these elements for each ungridded table in a valid dataset.
     * \return The Provenence class instance is returned by reference.
     * \sa Provenance
     */
    const Provenance& getProvenance( ) const { return provenance_; }

    /**
     * This function indicates whether a \em ungriddedTableDef element of a
     * DAVE-ML dataset includes an \em uncertainty child element.  A variable
     * described by a \em ungriddedTableDef without an \em uncertainty element may
     * still have uncertainty, if it is dependent on other variables or tables
     * with defined uncertainty.
     *
     * \return A boolean variable, 'true' if a ungriddedTableDef definition
     *  includes an \em uncertainty child element.
     *
     * \sa Uncertainty
     */
    const bool& hasUncertainty( ) const { return hasUncertainty_; }

    /**
     * This function provides access to the Uncertainty instance
     * associated with a #UngriddedTableDef instance.  There may be zero or one
     * of these elements for each \em ungriddedTableDef in a valid dataset.
     * For \em ungriddedTableDefs without \em uncertainty, and for
     * \em ungriddedTables, the corresponding #UngriddedTableDef instance
     * includes an empty Uncertainty instance.
     *
     * \return The Uncertainty instance is returned by reference.
     *
     * \sa Uncertainty
     */
    Uncertainty& getUncertainty( ) { return uncertainty_; }

    /**
     * This function provides access to the vector of data points associated
     * with an #UngriddedTableDef instance.  Each element of the vector is
     * itself a vector of double precision values representing the contents
     * of a \em dataPoint element from the DOM.
     *
     * \return A vector of \em dataPoint vectors is returned by reference.
     */
    const std::vector< std::vector<double> >& getData( ) const { return dataPoint_; }

    /**
     * This function provides access to the matrix of delaunay simplex
     * vertices associated with an #UngriddedTableDef instance.
     *
     * \return A vector of vectors of simplex vertex indices is returned
     * by reference.
     */
    const std::vector< std::vector<size_t> >& getDelaunay( ) const { return delaunay_; }

    /**
     * This function provides access to the list of modification record indices associated
     * with each of the data point records defined for this \em ungriddedTable instance.
     *
     * \return A vector of indices (int) listing the modification record indices of the
     * ungridded table dataPoint records is returned by reference.
     */
    const std::vector<dstoute::aOptionalSizeT>& getDataPointModID( ) const 
    { return dataPointModID_; }

    /**
     * This function provides returns the count of columns making up the ungridded table.
     *
     * \return The count (size_t) of data columns is returned by reference.
     */
    const size_t& getDataTableColumnCount() const { return numberDataTableColumns_;}

    /**
     * This function provides access to the list of dependent data for a nominated
     * dependent data column of this ungridded table.
     *
     * \param dataColumn the index of the dependent data column starting from 0.
     * A default column entry of 0 is used if this parameter is not provided.
     *
     * \return A vector of data (double) listing the dependent values for the
     * ungridded table dependent data column of interest is returned by reference.
     */
    const std::vector<double>& getDependentData( const size_t& dataColumn = 0) const
    { return dependentDataColumns_.at( dataColumn);}

    /**
     * This function provides access to the independent data for this ungridded table.
     *
     * \return A matrix of data (double) listing the independent values for the
     * ungridded table is returned by reference.
     */
    const dstomath::DMatrix& getIndependentData() const
    { return independentDataColumns_;}

    // ---- Functions used internal to Janus ----
    // The following functions are used by the ungridded interpolation procedure
    // defined in the Janus::getUngriddedInterpolation() function.
    // They do not provide useful information to external applications and thus
    // their descriptions are not included in the Janus reference manual.
    const std::vector< std::vector<size_t> >& getConnect( ) const { return connect_; }
    const dstomath::DMatrix& getCentroid() const { return centroid_;}

    dstomath::DVector& getXIVal() { return xival_;}
    dstomath::DVector& getWeight() { return weight_;}
    dstomath::DMatrix& getAMatrix() { return A_;}

    size_t lastUsedSimplex() const { return lastUsedSimplex_;}
    void lastUsedSimplex( const size_t& lastUsedSimplexArg)
    { lastUsedSimplex_ = lastUsedSimplexArg;}

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em UngriddedTableDef data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os,
                                      const UngriddedTableDef &ungriddedTableDef);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
                           const dstoute::aString& elementID,
                           const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

   protected:

    /*
     * This is an internal function that uses the Qhull library to determine
     * the Delaunay simplex indices for ungridded data.  It is called from
     * #setUngriddedTableDefFromDom.  The resultant simplex indices are stored
     * with the other ungridded table data, and are reused for each ungridded
     * data computation.
     * \return The number of simplices computed is returned.
     */
    size_t getDelaunaySimplex( );

   private:
    void setupDependentDataColumns();
    void setupIndependentDataColumns();
    void triangulateData();

    /************************************************************************
     * These are the ungriddedTableDef elements, set up during instantiation.
     */
    Janus*                               janus_;
    DomFunctions::XmlNode                domElement_;
    ElementDefinitionEnum                elementType_;

    dstoute::aString                     name_;
    dstoute::aString                     utID_;
    dstoute::aString                     units_;
    dstoute::aString                     description_;

    size_t                               independentVarCount_;
    std::vector<dstoute::aOptionalSizeT> dataPointModID_;
    std::vector<std::vector<double> >    dataPoint_;
    size_t                               numberDataTableColumns_;
    std::vector<std::vector<double> >    dependentDataColumns_;
    dstomath::DMatrix                    independentDataColumns_;

    bool                                 hasProvenance_;
    Provenance                           provenance_;

    bool                                 hasUncertainty_;
    Uncertainty                          uncertainty_;

    std::vector<std::vector<size_t> >    delaunay_;
    dstomath::DMatrix                    centroid_;
    std::vector<std::vector<size_t> >    connect_;
  
    size_t lastUsedSimplex_;
  
    /*
     * these are workspaces, sized once at instantiation to speed up
     * ungridded interpolation
     */
    dstomath::DVector                 xival_;
    dstomath::DVector                 weight_;
    dstomath::DMatrix                 A_;
  };
  
  typedef std::vector<UngriddedTableDef> UngriddedTableDefList;
  
}

#endif /* _UNGRIDDEDTABLEDEF_H_ */
