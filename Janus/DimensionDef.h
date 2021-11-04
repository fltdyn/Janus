#ifndef _DIMENSIONDEF_H_
#define _DIMENSIONDEF_H_

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
// Title:      Janus/DimensionDef
// Class:      DimensionDef
// Module:     DimensionDef.h
// First Date: 2010-07-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file DimensionDef.h
 *
 * A DimensionDef instance holds in its allocated memory alphanumeric data
 * derived from a \em dimensionDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
 * alphanumeric identification and cross-reference data.
 *
 * The DimensionDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 * Modified :  S. Hill
 */

// C++ Includes
#include <vector>

// Local Includes
#include "XmlElementDefinition.h"
#include "DomFunctions.h"

namespace janus
{

  /**
   * A #DimensionDef instance holds in its allocated memory alphanumeric data
   * derived from a \em dimensionDef element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
   * alphanumeric identification and cross-reference data.
   *
   * The #DimensionDef class is only used within the janus namespace, and should
   * only be referenced through the Janus class.
   */
  class DimensionDef: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #DimensionDef class
     * without supplying the DOM \em dimensionDef element from which the instance
     * is constructed, but in this state it not useful for any class functions.
     * It is necessary to populate the class from a DOM containing a
     * \em dimensionDef element before any further use of the instanced class.
     *
     * This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    DimensionDef();

    /**
     * The constructor, when called with an argument pointing to a \em dimensionDef
     * element within a DOM, instantiates the #DimensionDef class and fills
     * it with alphanumeric data from the DOM.
     *
     * \param elementDefinition is an addressto a \em dimensionDef
     * component node within the DOM.
     */
    DimensionDef( const DomFunctions::XmlNode& elementDefinition);

    virtual ~DimensionDef()
    {
    }

    /**
     * An uninitialised instance of #DimensionDef is filled with data from a
     * particular \em dimensionDef element within a DOM by this function.  If
     * another \em dimensionDef element pointer is supplied to an
     * instance that has already been initialised, data corruption may occur.
     *
     * \param elementDefinition is an address to a \em dimensionDef
     * component node within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function provides access to the \em dimID attribute
     * of the \em dimensionDef element represented by this #DimensionDef instance.
     * An dimensionDef's \em dimID attribute is normally a short
     * string without whitespace, such as "matrix_3x3", which uniquely defines the
     * dimensionDef.  It is used for indexing dimension tables within an XML dataset,
     * and provides underlying cross-references.
     * If the instance has not been initialised from a DOM, an empty string is returned.
     *
     * \return The \em dimID string is returned by reference.
     */
    const dstoute::aString& getDimID() const
    {
      return dimID_;
    }

    /**
     * This function returns the number of dimension records listed in a
     * #DimensionDef.  If the instance has not been populated from a DOM
     * element, zero is returned.
     *
     * \return An unsigned integer (size_t) number, one or more in a populated instance.
     */
    size_t getDimCount() const
    {
      return dimRecords_.size();
    }

    /**
     * This function returns a reference to the selected dimension record within
     * the #DimensionDef instance.
     *
     * \param index has a range from zero to ( #getDimCount() - 1 ),
     * and selects the required dimension record.  An attempt to access
     * a non-existent \em dimension record will throw a standard out_of_range exception.
     *
     * \return The requested dimension record is returned by reference.
     */
    size_t getDim( const size_t &index) const
    {
      return dimRecords_.at( index);
    }

    /**
     * This function returns the combined total of the dimensions defined for
     * the #DimensionDef instance. This is the multiple of each of the elements.
     *
     * \return An unsigned integer (size_t) number, multiplying each dimension
     * value in a populated instance.
     */
    size_t getDimTotal() const
    {
      return getTotal( dimRecords_);
    }

    /**
     * This function permits the dimension identifier \em dimID of the
     * \em dimensionDef element to be reset for this DimensionDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param dimID a string identifier representing the instance of
     * the \em dimensionDef.
     */
    void setDimID( const dstoute::aString& dimID)
    {
      dimID_ = dimID;
    }

    /**
     * This function permits the dimension records of the
     * \em dimensionDef element to be reset for this DimensionDef instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits them to be set before being written to an output XML based file.
     *
     * \param dimRecords a vector of unsigned integers (size_t) representing
     * the sizes of each dimension defined for a vector or matrix array.
     */
    void setDimRecords( const std::vector< size_t> &dimRecords)
    {
      dimRecords_ = dimRecords;
    }

    /**
     * This function is used to export the \em dimensionDef data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     * \param isReference a boolean flag indicating the dimensionDef element
     *         should be treated as a reference.
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement, const bool &isReference = false);

    // @TODO :: Add set parameter functions

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os, const DimensionDef &dimensionDef);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.

    // Other internally referenced functions
    //   This function should not be used by external programs.  It is designed for
    //   use within a Janus instance during initialisation, setting up
    //   the self-reference for each dimension. Use in other circumstances will
    //   result in data corruption.
    //
    //   'index' is the offset of this DimensionDef instance within the Janus
    //   instance, set during initialisation and passed to each DimensionDef for use
    //   during computation.
    //  void setIndex( const size_t &index ) { index_ = index; }

    // This function should not be used by external programs.  It is designed for
    // use within a Janus instance, maintaining consistency between variable
    // condition flags as different input variables are set and different output
    // variables are computed.  Use in other circumstances may result in data
    // corruption.
    void setNotCurrent()
    {
      isCurrent_ = false;
    }

  protected:

  private:
    size_t getTotal( const std::vector< size_t> &records) const;

    /************************************************************************
     * These are the dimensionDef elements, set up during instantiation.
     */
    dstoute::aString dimID_;
    std::vector< size_t> dimRecords_;

    /*
     * value and status elements - changeable during run time
     */
    bool isCurrent_;
  };

}

#endif /* _DIMENSIONDEF_H_ */
