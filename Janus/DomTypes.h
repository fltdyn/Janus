#ifndef _DOMTYPES_H_
#define _DOMTYPES_H_

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
// Title:      Janus/DomTypes
// Namespace:  DomFunctions
// Module:     DomTypes.h
// First Date: 2014-08-13
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file DomTypes.h
 *
 * This class contains common types for interacting with a
 * Document Object Model (DOM) containing data from a DAVE-ML
 * compliant XML dataset source file.
 *
 * Author   :  S. D. Hill
 *
 */

// Ute Includes
#include <Ute/aList.h>

// Pugi Includes
#include "pugixml.hpp"

namespace DomFunctions
{
  typedef pugi::xml_document XmlDoc;
  typedef pugi::xml_node XmlNode;
  typedef pugi::xml_parse_result XmlResult;

  typedef dstoute::aList< XmlNode> XmlNodeList;
}

#endif /* _DOMTYPES_H_ */
