#ifndef _UNCERTAINTY_H_
#define _UNCERTAINTY_H_

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
// Title:      Janus/Uncertainty
// Class:      Uncertainty
// Module:     Uncertainty.h
// First Date: 2008-07-07
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Uncertainty.h
 *
 * A Uncertainty instance holds in its allocated memory alphanumeric data
 * derived from a \em uncertainty element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The element is used in
 * function and parameter definitions to describe statistical variance in the
 * possible value of that function or parameter value. Only Gaussian
 * (normal) or uniform distributions of continuous random variable
 * distribution functions are supported.  The class also provides the functions
 * that allow a calling Janus instance to access these data elements.
 *
 * The Uncertainty class is only used within the janus namespace, and should
 * only be referenced indirectly through the Janus class.
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
#include <Ute/aBiMap.h>
#include <Ute/aList.h>
#include <Ute/aString.h>

// Local Includes
#include "XmlElementDefinition.h"
#include "Bounds.h"

namespace janus {

  class Janus;

  typedef std::pair<size_t, double> correlationPair;
  typedef dstoute::aList< correlationPair> correlationPairList;

  //  class Janus;
  
  /**
   * A #Uncertainty instance holds in its allocated memory alphanumeric data
   * derived from a \em uncertainty element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  The element is used in
   * function and parameter definitions to describe statistical variance in the
   * possible value of that function or parameter value. Only Gaussian
   * (normal) or uniform distributions of continuous random variable
   * distribution functions are supported.  The class also provides the functions
   * that allow a calling Janus instance to access these data elements.
   *
   * The #Uncertainty class is only used within the janus namespace, and should
   * only be referenced indirectly through the Janus class.
   *
   * a possible usage is:
  \code
    Janus test( xmlFileName );
    int nv = test.getNumberOfVariables();
    Uncertainty::UncertaintyPdf uncertaintyPdf;
    for ( int i = 0 ; i < nv ; i++ ) {
      VariableDef& variableDef = test.getVariableDef( i );
      cout << " Variable        : "
           << test.getVariableID( i ) << endl
           << "        Value              = "
           << test.getVariableByIndex( i ) << endl
           << "        uncertaintyPdf     = ";
      Uncertainty::UncertaintyPdf& uncertaintyPdf =
        variableDef.getUncertainty().getPdf( );
      switch( uncertaintyPdf ) {
      case Uncertainty::UNIFORM_PDF:
        cout << "UNIFORM_PDF";
        break;
      case Uncertainty::NORMAL_PDF:
        cout << "NORMAL_PDF";
        break;
      case Uncertainty::UNKNOWN_PDF:
        cout << "UNKNOWN_PDF";
        break;
      case Uncertainty::ERROR_PDF:
        cout << "ERROR_PDF";
        break;
      default:
        break;
      }
    }
  \endcode
   */
  class Uncertainty : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #Uncertainty class
     * without supplying the DOM \em uncertainty element from which the instance
     * is constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em uncertainty element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Uncertainty( );

    /**
     * The constructor, when called with an argument pointing to a \em uncertainty
     * element within a DOM, instantiates the #Uncertainty class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of an \em uncertainty component
     * within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to evaluate bounds with a functional
     * dependence on the instance state.
     */
    Uncertainty( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #Uncertainty is filled with data from a
     * particular \em uncertainty element within a DOM by this function.  If
     * another \em uncertainty element pointer is supplied to an
     * instance that has already been initialised, the instance will be
     * re-initialised with the new data. However, this is not
     * a recommended procedure.
     *
     * \param elementDefinition is an address of an \em uncertainty component
     * within the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to evaluate bounds with a functional
     * dependence on the instance state.
     */
    void initialiseDefinition( Janus* janus, const DomFunctions::XmlNode& elementDefinition);

  //  /*
  //   * This enum specifies the effects of an uncertainty bound on a variable.
  //   * All effects are computationally permissible for both types of probability
  //   * density functions.  However, some don't make much sense in some
  //   * applications.
  //   */
  //  enum UncertaintyEffect{
  //    /* Indicates that the bound[s] supplied are expressed as a real number
  //     * to be added to or subtracted from the variable value to which they
  //     * relate.
  //     */
  //    ADDITIVE_UNCERTAINTY,
  //    /* Indicates that the bound[s] supplied are expressed as a proportion of
  //     * the variable value to which they relate.
  //     */
  //    MULTIPLICATIVE_UNCERTAINTY,
  //    /* Indicates that the bound[s] supplied are expressed as a percentage of
  //     * the variable value to which they relate.
  //     */
  //    PERCENTAGE_UNCERTAINTY,
  //    /* Normally only applicable to the uniform PDF, indicates that bounds
  //     * supplied are real numbers, which must straddle the value of the
  //     * variable to which they apply.
  //     */
  //    ABSOLUTE_UNCERTAINTY,
  //    /* Used during instantiation, indicates that the effect of an
  //     * uncertainty entry has not yet been specified.
  //     */
  //    UNKNOWN_UNCERTAINTY
  //  };

    /**
     * This function returns the \em effect of the referenced \em uncertainty
     * element.  It indicates how bounds should be interpreted (e.g. additive,
     * multiplicative, percentage or absolute uncertainty).
     *
     * \return An UncertaintyEffect enum.  Where the #Uncertainty instance has
     * not been initialised, UNKNOWN_UNCERTAINTY is returned.
     */
    const UncertaintyEffect& getEffect( ) const { return effect_; }


    /**
     * This enum defines the probability distribution functions that may
     * be found in a DAVE-ML compliant dataset.
     */
    enum UncertaintyPdf{
      /** A normal or Gaussian probability distribution, defined in terms of its
          mean and standard deviation. */
      NORMAL_PDF,
      /** A uniform or constant probability distribution, defined in terms of
          the bounds of the interval over which it applies. */
      UNIFORM_PDF,
      /** A probability distribution that has not been specified in terms of
          the previous two allowable distributions. */
      UNKNOWN_PDF,
      /** Error flag, generally associated with incompatible combinations of PDFs
          within the XML dataset. */
      ERROR_PDF
    };

    /**
     * This function indicates whether the referenced #Uncertainty instance
     * describes Gaussian, uniform or unknown uncertainty.  In the case of a
     * variable that has its uncertainty defined directly through an
     * \em uncertainty child element (see hasUncertainty()), this function
     * returns that definition's uncertainty type.  For a \em variableDef
     * that does not include an \em uncertainty child element, this function
     * returns an uncertainty type determined by considering all variables
     * and functions on which this variable depends.
     *
     * \return An #UncertaintyPdf enum.  Where the #Uncertainty instance has
     * not been initialised,  #UNKNOWN_PDF is returned.
     */
    const UncertaintyPdf& getPdf( ) const { return pdf_; }

    /**
     * This function is used to determine whether a PDF has been explicitly
     * applied to a variable at the output stage.  This is in contrast to a
     * variable whose PDF is propagated through the computations from its
     * independent variable values.
     *
     * \return A boolean value, 'true' if the referenced variable has an
     * explicitly-specified PDF.
     */
    const bool& isSet( ) const { return isSet_; }

    /**
     * This function provides access to the \em bounds vector of the
     * #Uncertainty instance.  A Gaussian PDF will have a vector length
     * of one, and a uniform PDF will have a vector length of one or two,
     * depending on the bounds symmetry or asymmetry about the variable
     * value.
     *
     * \return A list of double precision values, containing one or two
     * elements, is returned by reference.
     */
    const BoundsList& getBounds( ) const { return bounds_; }

    /**
     * This function applies only to Gaussian PDF uncertainties.  It indicates
     * how many standard deviations are represented by the corresponding
     * bounds magnitude.
     *
     * \return The \em numSigmas attribute of the \em uncertainty instance is
     * returned as an integer.
     */
    const size_t& getNumSigmas( ) const { return numSigmas_; }

  //  /*
  //   * This function applies only to Gaussian PDF uncertainties.  It allows
  //   * access to an array of the indices of variables
  //   * whose Gaussian uncertainties are correlated with the uncertainty of
  //   * the variable associated with this #Uncertainty instance.
  //   *
  //   * \return The \em correlatesWith integer array is returned by reference.
  //   * \sa getCorrelation
  //   */
  //  const std::vector<int>& getCorrelatesWith( ) const { return correlatesWith_; }

  //  /*
  //   * This function applies only to Gaussian PDF uncertainties.  It allows
  //   * access to an array of correlation coefficients, which indicate the degree
  //   * of non-randomness in the relationship between the variable associated
  //   * with this #Uncertainty instance and those whose VariableDef indices within
  //   * the Janus instance are stored in the corresponding elements of the vector
  //   * returned by #getCorrelatesWith().
  //   * \return The \em correlation array is returned by reference.
  //   * \sa getCorrelatesWith
  //   */
  //
  //  const std::vector<double>& getCorrelation( ) const { return correlation_; }

    /**
     * This function applies only to Gaussian PDF uncertainties. It allows access
     * to an array of indices and coefficients for variables whose Gaussian
     * uncertainties are correlated with the uncertainty of the variable associated
     * with this #Uncertainty instance.
     *
     * The correlation coefficients, which indicate the degree of non-randomness in
     * the relationship between the variable associated with this #Uncertainty instance
     * and other variableDefs instances, are stored together with the index to the
     * variableDef from the global list as a pair; that is, the variableDef index and
     * the associated correlation coefficient are combined as a \em correlationPair
     * having the form \em pair<size_t, double> with the first element being the
     * index and the second the coefficient. This approach replaces the \em correlation
     * and \em correlatesWith arrays that contained the same information separately.
     *
     * \return The list of correlation pairs for this #Uncertainty instance is returned
     * as a reference to a vector of correlation pairs.
     */
    const correlationPairList& getCorrelation( ) const
    { return correlation_; }

    /**
     * This function applies only to Gaussian PDF uncertainties.  It allows
     * access to a list of the identifiers, \em varIDs, for variables that
     * are correlated with the variable associated with this #Uncertainty
     * instance. It is used internally within the class when initialising
     * the correlation pairs - variable index and coefficient.
     * This function permits external applications to retrieve this data for
     * information.
     *
     * \return The correlation varID list is returned by reference as a list
     *  of strings.
     */
    const dstoute::aStringList& getCorrelationVarList( ) const
    { return correlationVarIdList_; }

    /**
     * This function is used by Janus during initialisation, when setting up
     * uncertainty dependencies.  It should not be used by other classes or
     * external programs.
     *
     * \param pdf describes the type of probability associated with this
     * Uncertainty instance, as derived from its antecedent variables during
     * the Janus initialization process.
     */
    void setPdf( const UncertaintyPdf &pdf )
    { pdf_ = pdf; }

    /**
     * This function is used by Janus during initialisation, when setting up
     * uncertainty dependencies.  It should not be used by other classes or
     * external programs.
     *
     * \param n sets the length of the vector required for a Bounds instance.
     * Possible values are 1, for symmetric bounds, or 2, for asymmetric
     * bounds.
     */
    void setBoundsSize( const size_t n )
    { bounds_.resize( n ); }

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em Uncertainty data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const Uncertainty &uncertainty);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

    static const dstoute::aBiMap< dstoute::aString, int> uncertaintyAttributesMap;

   protected:

   private:
    bool isInCorrelationList( const dstoute::aString& elementName,
                              size_t &pairIndex);

    Janus* janus_;

    /************************************************************************
     * These are the uncertainty elements, set up during instantiation.
     * Gaussian has only one bound, uniform has two, so need a vector of
     * Bounds to cater for both possible cases
     */
    ElementDefinitionEnum elementType_;

    UncertaintyEffect   effect_;
    UncertaintyPdf      pdf_;
    bool                isSet_;
    BoundsList          bounds_;

    /*
     * the following are only required for Gaussian pdf
     */
    size_t              numSigmas_;
  //  std::vector<int>    correlatesWith_;
  //  std::vector<double> correlation_;
    dstoute::aStringList correlationVarIdList_;
    correlationPairList  correlation_;

  };

}
#endif /* _UNCERTAINTY_H_ */
