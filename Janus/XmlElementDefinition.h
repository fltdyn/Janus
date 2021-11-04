#ifndef _XMLELEMENTDEFINITION_H_
#define _XMLELEMENTDEFINITION_H_

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
// Title:      Janus/XmlElementDefinition
// Class:      XmlElementDefinition
// Module:     XmlElementDefinition.h
// First Date: 2011-11-23
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file XmlElementDefinition.h
 *
 * This file contains definitions of virtual functions that are used
 * when instantiating a DAVE-ML compliant XML file using Janus. The
 * \em XmlElementDefinition class is inherited by base element classes,
 * such as \em FileHeader and \em VariableDef, which have specific versions
 * of the virtual functions. These function calls are accessed internally
 * within Janus through the \em DomFunctions class and permit abstraction of
 * the process of interacting with the DOM. They do not provide a capability
 * to external applications to interact with the XML encoded data file or
 * the associated DOM.
 * 
 * Author   :  G. J. Brian
 *
 */

// C++ Includes
// Ute Includes
#include <Ute/aString.h>

// Local Includes
#include "ElementDefinitionEnum.h"
#include "DomTypes.h"

namespace janus
{

  /**
   * This file contains definitions of virtual functions that are used
   * when instantiating a DAVE-ML compliant XML file using Janus. The
   * \em XmlElementDefinition class is inherited by base element classes,
   * such as \em FileHeader and \em VariableDef, which have specific versions
   * of the virtual functions. These function calls are accessed internally
   * within Janus through the \em DomFunctions class and permit abstraction of
   * the process of interacting with the DOM. They do not provide a capability
   * to external applications to interact with the XML encoded data file or
   * the associated DOM.
   */
  class XmlElementDefinition
  {
  public:

    XmlElementDefinition()
      : elementType_( ELEMENT_NOTSET)
    {
    }
    virtual ~XmlElementDefinition()
    {
    }

    //  virtual void initialiseDefinition( const DomFunctions::XmlNode& elementDefinition,
    //                                     void* moduleVoid) = 0;
    virtual void readDefinitionFromDom(
      const DomFunctions::XmlNode& /*elementDefinition*/)
    {
    }
    virtual bool compareElementID(
      const DomFunctions::XmlNode& /*elementDefinition*/,
      const dstoute::aString& /*elementID*/,
      const size_t& /*documentElementReferenceIndex = 0*/)
    {
      return false;
    }

  protected:

    ElementDefinitionEnum elementType_;
  };

}

#endif /* _XMLELEMENTDEFINITION_H_ */
