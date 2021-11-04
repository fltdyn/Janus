#ifndef _SIGNAL_H_
#define _SIGNAL_H_

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
// Title:      Janus/Signal
// Class:      Signal
// Module:     Signal.h
// First Date: 2009-05-07
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Signal.h
 *
 * A Signal instance holds in its allocated memory alphanumeric data
 * derived from a \em signal element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance may describe
 * inputs, internal values of a computation, or outputs.  The class also
 * provides the functions that allow a calling StaticShot instance to access
 * these data elements. It is used to document the name, ID, value, tolerance,
 * and units of measure for checkcases.
 *
 * A \em signal must have \em signalName and \em signalUnits if it is
 * a child of \em checkInputs or \em checkOutputs.  Alternatively, if it is a
 * child of \em internalValues, it must have a \em varID (\em signalID is
 * deprecated).  When used in a \em checkOutputs vector, the \em tol element
 * must be present. Tolerance is specified as a maximum absolute difference
 * between the expected and actual value. This class accepts whichever of these
 * children it finds in the XML dataset, and leaves applicability to its parents
 * to sort out.
 *
 * The Signal class is only used within the janus namespace, and should
 * only be referenced indirectly through the StaticShot, CheckInputs,
 * InternalValues and CheckOutputs classes.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes 
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif

// Ute Includes
#include <Ute/aList.h>

// Local Includes
#include "XmlElementDefinition.h"

namespace janus {

  /**
   * A #Signal instance holds in its allocated memory alphanumeric data
   * derived from a \em signal element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  The instance may describe
   * inputs, internal values of a computation, or outputs.  The class also
   * provides the functions that allow a calling StaticShot instance to access
   * these data elements.
   * A \em signal must have \em signalName and \em signalUnits if it is
   * a child of \em checkInputs or \em checkOutputs.  Alternatively, if it is a
   * child of \em internalValues, it must have a \em varID (\em signalID is
   * deprecated).  This class accepts whichever of these children it finds in the
   * XML dataset, and leaves applicability to its parents to sort out.
   *
   * The #Signal class is only used within the janus namespace, and should
   * only be referenced indirectly through the StaticShot, CheckInputs,
   * InternalValues and CheckOutputs classes.
   *
   * Typical usage:
  \code
    Janus test( xmlFileName );
    CheckData checkData = test.getCheckData();
    size_t nss = checkData.getStaticShotCount();
    for ( size_t j = 0 ; j < nss ; j++ ) {
      StaticShot staticShot = checkData.getStaticShot( j );
      CheckOutputs checkOutputs = staticShot.getCheckOutputs();
      size_t ncout = checkOutputs.getSignalCount();
      cout << " staticShot[" << j << "] : " << endl
           << "      Name                      = "
           << staticShot.getName( ) << endl
           << "      Number of check outputs   = " << ncout << endl;
      for ( size_t k = 0 ; k < ncout ; k++ ) {
        cout << "  checkOutputs[" << k << "] : " << endl
             << "    signalName                = "
             << checkOutputs.getName( k ) << endl
             << "    signalUnits               = "
             << checkOutputs.getUnits( k ) << endl
             << "    signalValue               = "
             << checkOutputs.getValue( k ) << endl
             << "    signalTol                 = "
             << checkOutputs.getTolerance( k ) << endl
             << endl;
      }
    }
  \endcode
   */
  class Signal : public XmlElementDefinition
  {
   public:

    /**
     * The empty constructor can be used to instance the #Signal class without
     * supplying the DOM \em signal element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em signal element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Signal( );

    /**
     * The constructor, when called with an argument pointing to a \em signal
     * element within a DOM, instantiates the #Signal class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address of a \em signal component
     * within the DOM.
     * \param signalType is a enumeration identifying the signal as either an
     * input, and output, or an internal value.
     */
    Signal( const DomFunctions::XmlNode& elementDefinition,
            const SignalTypeEnum &signalType);

    /**
     * An uninitialised instance of #Signal is filled with data from a
     * particular \em signal element within a DOM by this function.  If another
     * \em signal element pointer is supplied to an instance that has already
     * been initialised, the instance will be re-initialised with the new data.
     * However, this is not a recommended procedure, since optional elements may
     * not be replaced.
     *
     * \param elementDefinition is an address of a \em signal component
     * within the DOM.
     * \param signalType is a enumeration identifying the signal as either an
     * input, and output, or an internal value.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
                               const SignalTypeEnum &signalType);

    /**
     * This function returns the content of the signal's \em signalName child
     * element.  If the instance has not been initialised from a DOM, an
     * empty string is returned.
     *
     * \return The \em signalName content string is returned by reference.
     */
    const dstoute::aString& getName( ) const { return name_; }

    /**
     * This function returns the content of the signal's \em signalUnits child
     * element.  The \em signalUnits content is a
     * string of arbitrary length, but normally short, and complying with the
     * format requirements chosen by \htmlonly AD APS\endhtmlonly
     * \latexonly \ac{AD} \ac{APS} \cite{brian1}\endlatexonly in accordance with
     * \htmlonly SI\endhtmlonly \latexonly\ac{SI} \endlatexonly and other systems.
     * If the #Signal has not been initialised from a DOM, an
     * empty string is returned.
     *
     * \return The \em signalUnits content string is returned by reference.
     */
    const dstoute::aString& getUnits( ) const { return units_; }

   /**
     * This function returns the content of the signal's \em varID child
     * element.  The \em varID is a unique (per list of check case elements),
     * short string not including whitespace that indicates the
     * VariableDef the signal corresponds with, and is
     * used for signal indexing.  If the \em signal element owns a
     * (deprecated alternative) \em signalID child element, that will be
     * returned by this function.  If the #Signal has not been initialised from
     * a DOM, an empty string is returned.
     *
     * \return The \em varID or \em signalID content string is returned by
     * reference.
     */
    const dstoute::aString& getVarID( ) const { return varID_; }

    /**
     * This function returns the content of the signal's \em signalValue child
     * element.  It represent the numeric value that a particular variable from
     * the XML dataset should return for the check case that forms the parent of
     * this signal.  If the #Signal has not been populated from a DOM
     * element, NaN is returned.
     *
     * \return A double precision variable containing the \em signal
     * value is returned.
     */
    const double& getValue( ) const { return value_; }

    /**
     * This function returns the content of a signal's \em tol child
     * element, if the \em signal is part of either an
     * \em internalValues or a \em checkOutputs element.  If the #Signal has not
     * been populated from a DOM, NaN will be returned.  If the \em signal is
     * part of a \em checkInputs element, or a tolerance is not specified for
     * the \em signal within the XML dataset, this function will return zero.
     *
     * \return A double precision variable containing the tolerance on the
     * \em signal value is returned.
     */
    const double& getTolerance( ) const { return tol_; }

    // @TODO :: Add set parameter functions

    /**
     * This function is used to export the \em Signal data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<< ( std::ostream &os, const Signal &signal);

    // ---- Internally reference functions. ----

    void          setActualValue( const double& value) const { actualValue_ = value;}
    const double& getActualValue() const                     { return actualValue_;}
   private:

    /************************************************************************
     * These are the signal elements, set up during instantiation.
     */

    dstoute::aString name_;
    dstoute::aString units_;
    dstoute::aString varID_;
    double           value_;
    double           tol_;
    mutable double   actualValue_;
  };

  typedef dstoute::aList<Signal> aListSignals;
  
}

#endif /* _SIGNAL_H_ */
