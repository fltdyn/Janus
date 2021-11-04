#ifndef _CHECKDATA_H_
#define _CHECKDATA_H_

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
// Title:      Janus/CheckData
// Class:      CheckData
// Module:     CheckData.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file CheckData.h
 *
 * Check data is used for XML dataset content verification.
 * A CheckData instance holds in its allocated memory alphanumeric data
 * derived from a \em checkData element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It will include static
 * check cases, trim shots, and dynamic check case information.  At present
 * only static check cases are implemented, using \em staticShot children of the
 * top-level \em checkData element.  The functions within this class provide
 * access to the raw check data, as well as actually performing whatever
 * checks may be done on the dataset using the \em checkData.
 *
 * The CheckData class is only used within the janus namespace, and should
 * normally only be referenced through the Janus class.
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
#include "Provenance.h"
#include "StaticShot.h"

namespace janus
{
  class Janus;

  /**
   * Check data is used for XML dataset content verification.
   * A CheckData instance holds in its allocated memory alphanumeric data
   * derived from a \em checkData element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  It will include static
   * check cases, trim shots, and dynamic check case information.  At present
   * only static check cases are implemented, using \em staticShot children of the
   * top-level \em checkData element.  The functions within this class provide
   * access to the raw check data, as well as actually performing whatever
   * checks may be done on the dataset using the \em checkData.
   * The provenance sub-element is now deprecated and has been moved
   * to individual staticShots; it is allowed here for backwards
   * compatibility.
   *
   * The CheckData class is only used within the janus namespace, and should
   * normally only be referenced through the Janus class.
   *
   * Typical usage of the checking functions:
    \code
      Janus test( xmlFileName );
      CheckData checkData = test.getCheckData();
      int nss = checkData.getStaticShotCount( );
      cout << " Number of static shots = " << nss << endl;
      for ( int j = 0 ; j < nss ; j++ ) {
        StaticShot staticShot = checkData.getStaticShot( j );
        int nInvalid = staticShot.getInvalidVariableCount();
        if ( 0 < nInvalid ) {
          for ( int k = 0 ; k < nInvalid ; k++ ) {
            string failVarID =
              staticShot.getInvalidVariable(  k );
            cout << "  Problem at varID : " << failVarID << endl;
          }
        } else {
          cout << "  No problems from static shot " << j << " ... " << endl;
        }
      }
    \endcode
   */
  class CheckData: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the CheckData class without
     * supplying the DOM \em checkData element from which the instance is
     * constructed, but in this state is not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em checkData element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    CheckData();

    /**
     * The constructor, when called with an argument pointing to a \em checkData
     * element within a DOM, instantiates the CheckData class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an address to a \em checkData component node within
     * the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    CheckData( const DomFunctions::XmlNode& elementDefinition,
               Janus* janus);

    /**
     * An uninitialised instance of CheckData is filled with data from a
     * particular \em checkData element within a DOM by this function.  If
     * another \em checkData element pointer is supplied to an instance that
     * has already been initialised, the instance will be re-initialised with
     * the new data.  However, this is not a recommended procedure, since
     * optional elements may not be replaced.
     *
     * \param elementDefinition is an address to a \em checkData component node within
     * the DOM.
     * \param janus is a pointer to the owning Janus instance, used
     * within this class to set up cross-references depending on the instance
     * state.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
                               Janus* janus);

    /**
     * This function indicates whether the \em checkData element of a
     * DAVE-ML dataset includes either \em provenance or \em provenanceRef
     * children.  For DAVE-ML version 2.0RC3 and subsequent releases, the
     * use of \em provenance or \em provenanceRef at the \em checkData level is
     * deprecated.
     *
     * \return A boolean indication of the presence of \em checkData's
     * provenance.
     */
    const bool& hasProvenance() const
    {
      return hasProvenance_;
    }

    /**
     * This function provides access to the \em provenance or \em provenanceRef
     * element contained in a DAVE-ML \em checkData element.  The element is
     * deprecated in this location; however, access through this function is retained
     * for compatibility with older datasets.  There may be zero or one
     * of these elements attached to the \em checkData element in a valid
     * dataset.  If the instance has not been initialised or the \em checkData
     * element has no provenance, an empty Provenance instance is returned.
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
     * This function allows the number of \em staticShot elements referenced by a
     * \em checkData element to be determined.  If the CheckData instance has not
     * been populated from a DOM, zero is returned.
     * Because future \em checkData may include
     * other cases than static shots, a \em checkData element without any
     * \em staticShot components may still be valid.
     *
     * \return An integer number, zero or more in a populated instance.
     *
     * \sa StaticShot
     */
    size_t getStaticShotCount() const
    {
      return staticShot_.size();
    }

    /**
     * This function provides access to the \em staticShot elements
     * referenced by a DAVE-ML \em checkData element.  There may be zero, one
     * or many \em staticShot elements within the \em checkData component of a
     * valid XML dataset.
     *
     * \return A list containing the StaticShot instances is returned by reference.
     *
     * \sa StaticShot
     */
    StaticShotList& getStaticShot()
    {
      return staticShot_;
    }
    const StaticShotList& getStaticShot() const
    {
      return staticShot_;
    }

    /**
     * This function provides access to a \em staticShot element
     * referenced by a DAVE-ML \em checkData element.  There may be zero, one
     * or many \em staticShot elements within the \em checkData component of a
     * valid XML dataset.
     *
     * \param index has a range from zero to ( #getStaticShotCount() - 1 ),
     * and selects the required StaticShot instance.  Attempting to access a
     * StaticShot outside the available range will throw a standard out_of_range
     * exception.
     *
     * \return The requested StaticShot instance is returned by reference.
     *
     * \sa StaticShot
     */
    const StaticShot& getStaticShot( const size_t &index) const
    {
      return staticShot_.at( index);
    }

    /**
     * This function permits the staticShot instance vector of the
     * \em checkData element to be reset for this #CheckData instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param staticShot The list of staticShot instances.
     *
     */
    void setStaticShot( const StaticShotList staticShot)
    {
      staticShot_.clear();
      staticShot_ = staticShot;
    }

    /**
     * This function is used to export the \em checkData data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement);

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os,
      const CheckData &checkData);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);
    bool compareElementID( const DomFunctions::XmlNode& xmlElement,
      const dstoute::aString& elementID,
      const size_t &documentElementReferenceIndex = 0);

    // Function to reset the Janus pointer in the lower level classes
    void resetJanus( Janus* janus);

  private:

    /************************************************************************
     * These are pointers to the checkData elements, set up during
     * instantiation.  There may be multiple static shots within the
     * CheckData instance.
     */
    Janus* janus_;
    ElementDefinitionEnum elementType_;

    bool hasProvenance_;
    Provenance provenance_;
    StaticShotList staticShot_;

  };

}

#endif /* _CHECKDATA_H_ */
