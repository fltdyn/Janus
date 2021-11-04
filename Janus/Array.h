#ifndef _ARRAY_H_
#define _ARRAY_H_

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
// Title:      Janus/Array
// Class:      Array
// Module:     Array.h
// First Date: 2009-07-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Array.h
 *
 * An Array instance holds in its allocated memory alphanumeric data
 * derived from an \em array element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.
 *
 * It includes entries arranged as follows:
 *  Entries for a vector represent the row entries of that vector.
 *  Entries for a matrix are specified such that the column entries
 *  of the first row are listed followed by column entries for subsequent rows
 *  until the base matrix is complete. This sequence is repeated for higher order
 *  matrix dimensions until all entries of the matrix are specified.
 *
 * The Array class is only used within the janus namespace, and should only
 * be referenced through the Janus class.
 */

/*
 * Author:  G. J. Brian
 *
 * Modified :  S. Hill
 *
 */

// C++ Includes

// Local Includes
#include "XmlElementDefinition.h"
#include <Ute/aString.h>

namespace janus
{

  class Janus;

  /**
   * An #Array instance holds in its allocated memory alphanumeric data
   * derived from an \em array element of a DOM corresponding to
   * a DAVE-ML compliant XML dataset source file.
   *
   * It includes entries arranged as follows:
   *  Entries for a vector represent the row entries of that vector.
   *  Entries for a matrix are specified such that the column entries
   *  of the first row are listed followed by column entries for subsequent rows
   *  until the base matrix is complete. This sequence is repeated for higher order
   *  matrix dimensions until all entries of the matrix are specified.
   *
   * The #Array class is only used within the janus namespace, and should only
   * be referenced through the Janus class.
   */
  class Array: public XmlElementDefinition
  {
  public:

    /**
     * The empty constructor can be used to instance the #Array class
     * without supplying the DOM \em array element from which the
     * instance is constructed, but in this state it is not useful for
     * any class functions.  It is necessary to populate the class from
     * a DOM containing an \em array element before any further use of
     * the instanced class.
     *
     * This form of the constructor is principally for use within
     * higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa initialiseDefinition
     */
    Array();

    /**
     * The constructor, when called with an argument pointing to an
     * \em array element within a DOM, instantiates the
     * #Array class and fills it with alphanumeric data from the DOM.
     * String-based numeric data are converted to double-precision linear
     * vectors.
     *
     * \param elementDefinition is an address of an \em array
     * component node within the DOM.
     */
    Array( const DomFunctions::XmlNode& elementDefinition);

    /**
     * An uninitialised instance of #Array is filled with data from a
     * particular \em array element within a DOM by this function.  If
     * another \em array element pointer is supplied to an
     * instance that has already been initialised, data corruption will occur
     * and the entire Janus instance will become unusable.
     *
     * \param elementDefinition is an address of an \em array
     * component node within the DOM.
     */
    void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition);

    /**
     * This function returns the number of entries stored in an array.
     *
     * \return The number of entries stored in an array.
     */
    size_t getArraySize() const
    {
      return stringTableData_.size();
    }

    /**
     * This function provides access to a vector of alphanumeric data
     * stored in an #Array instance.  This vector contains
     * the data strings in the same sequence as they were
     * presented in the \em dataTable of the corresponding XML dataset.
     *
     * \return The string list containing the alphanumeric content of the #Array
     * instance is returned by reference.
     */
    const dstoute::aStringList& getStringDataTable() const
    {
      return stringTableData_;
    }

    /**
     * This function permits the string data table of the
     * \em array element to be reset for this Array instance.
     *
     * If the instance has not been initialised from a DOM then this function
     * permits it to be set before being written to an output XML based file.
     *
     * \param stringDataTable a string list containing data table entries.
     */
    void setStringDataTable( const dstoute::aStringList stringDataTable)
    {
      stringTableData_ = stringDataTable;
    }

    /**
     * This function is used to export the \em array data to a DAVE-ML
     * compliant XML dataset file as defined by the DAVE-ML
     * document type definition (DTD).
     *
     * \param documentElement an address to the parent DOM node/element.
     * \param elementTag a string defining the XML element tag name
     */
    void exportDefinition( DomFunctions::XmlNode& documentElement,
                           const dstoute::aString& elementTag = "");

    // ---- Display functions. ----
    // This function displays the contents of the class
    friend std::ostream& operator<<( std::ostream &os, const Array &array);

    // ---- Internally reference functions. ----
    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition);

  private:
    /************************************************************************
     * These are the array elements, set up during instantiation.
     */
    dstoute::aStringList stringTableData_;

  };
}

#endif /* _ARRAY_H_ */
