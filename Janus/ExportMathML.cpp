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
// Title:      Janus/ExportMathML : DSTO Math-ML functions Class
// Class:      ExportMathML
// Module:     ExportMathML.cpp
// First Date: 2014
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file ExportMathML.cpp
 *
 * This class contains functions for exporting mathematics procedures
 * defined using the MathML syntax to a DOM. Data detailing each MathML operation
 * and is stored in a MathMLDataClass structure. This includes the
 * sub-elements to which the operator is to be applied. Functions to
 * process both scalar and matrix data are included.
 */

/*
 * Author: Geoff Brian
 *
 * Modified :  S. Hill
 *
 */

// C++ Includes
#include <iostream>

#include <Ute/aString.h>

#include "ElementDefinitionEnum.h"
#include "ExportMathML.h"
#include "VariableDef.h"
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace dstomathml
{
  namespace exportmathml
  {

    //------------------------------------------------------------------------//
    /*
     * Functions to export mathematics procedures defined using the MathML syntax.
     *
     * exportMathMl : A function to export the MathML syntax.
     */

    void exportMathMl(
      DomFunctions::XmlNode& documentElement,
      dstomathml::MathMLData& t)
    {
      exportMathMLMap[ t.functionTag_]( documentElement, t);
    }

    void exportChild(
      DomFunctions::XmlNode& documentElement,
      dstomathml::MathMLData& t)
    {
      DomFunctions::XmlNode childElement = DomFunctions::setChild( documentElement, t.functionTag_);
      exportMathMl( childElement, t.mathChildren_.front());
    }

    void exportChildren(
      DomFunctions::XmlNode& documentElement,
      dstomathml::MathMLData& t)
    {
      DomFunctions::XmlNode childElement = DomFunctions::setChild( documentElement, t.functionTag_);

      for ( size_t i = 0; i < t.mathChildren_.size(); i++) {
        exportMathMl( childElement, t.mathChildren_[ i]);
      }
    }

    void exportSibling(
      DomFunctions::XmlNode& documentElement,
      dstomathml::MathMLData& t)
    {
      DomFunctions::setChild( documentElement, t.functionTag_);
      exportMathMl( documentElement, t.mathChildren_.front());
    }

    void exportSiblings(
      DomFunctions::XmlNode& documentElement,
      dstomathml::MathMLData& t)
    {
      DomFunctions::setChild( documentElement, t.functionTag_);

      for ( size_t i = 0; i < t.mathChildren_.size(); i++) {
        exportMathMl( documentElement, t.mathChildren_[ i]);
      }
    }

    //------------------------------------------------------------------------//
    /*
     * Special case exports.
     *
     * cn
     * ci
     * apply
     * csymbol
     * selector
     */

    void cn(
      DomFunctions::XmlNode& documentElement,
      MathMLData& t)
    {
      DomFunctions::setChild( documentElement, t.functionTag_, aString( "%").arg( t.cnValue_));
    }

    void ci(
      DomFunctions::XmlNode& documentElement,
      MathMLData& t)
    {
      DomFunctions::setChild( documentElement, t.functionTag_, t.variableDef_->getVarID());
    }

    void csymbol(
      DomFunctions::XmlNode& documentElement,
      MathMLData& t)
    {
      DomFunctions::XmlNode childElement =
        DomFunctions::setChild( documentElement, "csymbol", t.functionTag_);

      if ( !t.attribute_.empty()) {
        if ( t.functionTag_ != "mask") {
          DomFunctions::setAttribute( childElement, "cd", t.attribute_);
        }
        else {
          DomFunctions::setAttribute( childElement, "type", t.attribute_);
        }
      }

      for ( size_t i = 0; i < t.mathChildren_.size(); i++) {
        exportMathMl( documentElement, t.mathChildren_[ i]);
      }
    }

    void mask(
      DomFunctions::XmlNode& documentElement,
      MathMLData& t)
    {
      DomFunctions::XmlNode childElement =
        DomFunctions::setChild( documentElement, "csymbol", "mask");
      DomFunctions::setAttribute( childElement, "type", t.attribute_);
      for ( size_t i = 0; i < t.mathChildren_.size(); i++) {
        exportMathMl( documentElement, t.mathChildren_[ i]);
      }
    }

    void selector(
      DomFunctions::XmlNode& documentElement,
      MathMLData& t)
    {
      DomFunctions::XmlNode childElement =
        DomFunctions::setChild( documentElement, "selector");
      DomFunctions::setAttribute( childElement, "other", t.attribute_);
      for ( size_t i = 0; i < t.mathChildren_.size(); i++) {
        exportMathMl( documentElement, t.mathChildren_[ i]);
      }
    }

    //------------------------------------------------------------------------//

    ExportMathMLMap exportMathMLMap = ( ExportMathMLMap()
      << ExportMathMLMap::pair( "cn",               cn)
      << ExportMathMLMap::pair( "ci",               ci)
      << ExportMathMLMap::pair( "apply",            exportChild)
      << ExportMathMLMap::pair( "csymbol",          csymbol)
      << ExportMathMLMap::pair( "eq",               exportSiblings)
      << ExportMathMLMap::pair( "neq",              exportSiblings)
      << ExportMathMLMap::pair( "gt",               exportSiblings)
      << ExportMathMLMap::pair( "geq",              exportSiblings)
      << ExportMathMLMap::pair( "lt",               exportSiblings)
      << ExportMathMLMap::pair( "leq",              exportSiblings)
      << ExportMathMLMap::pair( "quotient",         exportSiblings)
      << ExportMathMLMap::pair( "factorial",        exportSiblings)
      << ExportMathMLMap::pair( "divide",           exportSiblings)
      << ExportMathMLMap::pair( "max",              exportSiblings)
      << ExportMathMLMap::pair( "min",              exportSiblings)
      << ExportMathMLMap::pair( "minus",            exportSiblings)
      << ExportMathMLMap::pair( "plus",             exportSiblings)
      << ExportMathMLMap::pair( "power",            exportSiblings)
      << ExportMathMLMap::pair( "rem",              exportSiblings)
      << ExportMathMLMap::pair( "times",            exportSiblings)
      << ExportMathMLMap::pair( "root",             exportSiblings)
      << ExportMathMLMap::pair( "degree",           exportChild)
      << ExportMathMLMap::pair( "and",              exportSiblings)
      << ExportMathMLMap::pair( "or",               exportSiblings)
      << ExportMathMLMap::pair( "xor",              exportSiblings)
      << ExportMathMLMap::pair( "not",              exportSiblings)
      << ExportMathMLMap::pair( "abs",              exportSiblings)
      << ExportMathMLMap::pair( "floor",            exportSiblings)
      << ExportMathMLMap::pair( "ceiling",          exportSiblings)
      << ExportMathMLMap::pair( "piecewise",        exportChildren)
      << ExportMathMLMap::pair( "piece",            exportChildren)
      << ExportMathMLMap::pair( "otherwise",        exportChild)
      << ExportMathMLMap::pair( "sin",              exportSiblings)
      << ExportMathMLMap::pair( "cos",              exportSiblings)
      << ExportMathMLMap::pair( "tan",              exportSiblings)
      << ExportMathMLMap::pair( "sec",              exportSiblings)
      << ExportMathMLMap::pair( "csc",              exportSiblings)
      << ExportMathMLMap::pair( "cot",              exportSiblings)
      << ExportMathMLMap::pair( "arcsin",           exportSiblings)
      << ExportMathMLMap::pair( "arccos",           exportSiblings)
      << ExportMathMLMap::pair( "arctan",           exportSiblings)
      << ExportMathMLMap::pair( "arcsec",           exportSiblings)
      << ExportMathMLMap::pair( "arccsc",           exportSiblings)
      << ExportMathMLMap::pair( "arccot",           exportSiblings)
      << ExportMathMLMap::pair( "exp",              exportSiblings)
      << ExportMathMLMap::pair( "ln",               exportSiblings)
      << ExportMathMLMap::pair( "log",              exportSiblings)
      << ExportMathMLMap::pair( "logbase",          exportChild)
      << ExportMathMLMap::pair( "exponentiale",     exportSiblings)
      << ExportMathMLMap::pair( "notanumber",       exportSiblings)
      << ExportMathMLMap::pair( "pi",               exportSiblings)
      << ExportMathMLMap::pair( "eulergamma",       exportSiblings)
      << ExportMathMLMap::pair( "infinity",         exportSiblings)
      << ExportMathMLMap::pair( "noop",             exportSiblings)

      // Matrix specific ops.
      << ExportMathMLMap::pair( "determinant",      exportSibling)
      << ExportMathMLMap::pair( "transpose",        exportSibling)
      << ExportMathMLMap::pair( "inverse",          exportSibling)
      << ExportMathMLMap::pair( "selector_element", selector)
      << ExportMathMLMap::pair( "selector_row",     selector)
      << ExportMathMLMap::pair( "selector_column",  selector)
      << ExportMathMLMap::pair( "selector_diag",    selector)
      << ExportMathMLMap::pair( "selector_mslice",  selector)
      << ExportMathMLMap::pair( "vectorproduct",    exportSiblings)
      << ExportMathMLMap::pair( "scalarproduct",    exportSiblings)
      << ExportMathMLMap::pair( "outerproduct",     exportSiblings)

      // csymbol definitions
      << ExportMathMLMap::pair( "fmod",             csymbol)
      << ExportMathMLMap::pair( "sign",             csymbol)
      << ExportMathMLMap::pair( "bound",            csymbol)
      << ExportMathMLMap::pair( "nearbyint",        csymbol)
      << ExportMathMLMap::pair( "sind",             csymbol)
      << ExportMathMLMap::pair( "cosd",             csymbol)
      << ExportMathMLMap::pair( "tand",             csymbol)
      << ExportMathMLMap::pair( "secd",             csymbol)
      << ExportMathMLMap::pair( "cscd",             csymbol)
      << ExportMathMLMap::pair( "cotd",             csymbol)
      << ExportMathMLMap::pair( "arcsind",          csymbol)
      << ExportMathMLMap::pair( "arccosd",          csymbol)
      << ExportMathMLMap::pair( "arctand",          csymbol)
      << ExportMathMLMap::pair( "arcsecd",          csymbol)
      << ExportMathMLMap::pair( "arccscd",          csymbol)
      << ExportMathMLMap::pair( "arccotd",          csymbol)
      << ExportMathMLMap::pair( "atan2",            csymbol)
      << ExportMathMLMap::pair( "atan2d",           csymbol)

      << ExportMathMLMap::pair( "unitmatrix",       csymbol)
      << ExportMathMLMap::pair( "eulertransform",   csymbol)
      << ExportMathMLMap::pair( "eulertransformd",  csymbol)
      << ExportMathMLMap::pair( "cross",            csymbol)
      << ExportMathMLMap::pair( "mask_plus",        mask)
      << ExportMathMLMap::pair( "mask_minus",       mask)
      << ExportMathMLMap::pair( "mask_times",       mask)
      << ExportMathMLMap::pair( "mask_divide",      mask));

  } // end namespace exportmathml

} // end namespace dstomathml

