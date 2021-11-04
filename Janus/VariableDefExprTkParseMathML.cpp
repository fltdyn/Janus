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

/*
 * VariableDefExprTkParseMathML.cpp
 *
 *  Created on: Dec 9, 2016
 *      Author: hills
 */

// C++ Includes
#include <algorithm>
#include <cctype>

// Local Includes
#include "Janus.h"
#include "VariableDef.h"

using namespace std;
using namespace dstoute;

namespace janus {


  //------------------------------------------------------------------------//

  enum ExprTkMathMLOp {
    OP_NONE    = -1,
    OP_APPLY   = -2,
    OP_CDATA   = -3,
    OP_CSYMBOL = -4,
    OP_ZERO    = -5,
    OP_PIECE   = -6,
    OP_MINUS   = -7,
    OP_ROOT    = -8,
    OP_LOG     = -9
  };

  struct ExprTkSyntaxDef
  {
    ExprTkSyntaxDef()
     : ops_( OP_NONE), hasChild_( false) {}
    ExprTkSyntaxDef( int ops)
     : ops_( ops), hasChild_( false) {}
    ExprTkSyntaxDef( int ops, const aString& pre, const aString& post, const aString& sep, bool hasChild = false)
     : ops_( ops), pre_( pre), post_( post), sep_( sep), hasChild_( hasChild) {}
    int     ops_;
    aString pre_;
    aString post_;
    aString sep_;
    bool    hasChild_;
  };

  typedef dstoute::aMap< dstoute::aString, ExprTkSyntaxDef > ExprTkSyntaxMap;

  ExprTkSyntaxMap exprTkMathMLMap = ( ExprTkSyntaxMap()
  << ExprTkSyntaxMap::pair( "cn",        ExprTkSyntaxDef( OP_CDATA))
  << ExprTkSyntaxMap::pair( "ci",        ExprTkSyntaxDef( OP_CDATA))
  << ExprTkSyntaxMap::pair( "csymbol",   ExprTkSyntaxDef( OP_CSYMBOL, "","",  "" ))

  << ExprTkSyntaxMap::pair( "apply",     ExprTkSyntaxDef( OP_APPLY, "( ",  ")",    "",    true ))
  << ExprTkSyntaxMap::pair( "plus",      ExprTkSyntaxDef( 0,  "",          "",     " + "  ))
  << ExprTkSyntaxMap::pair( "minus",     ExprTkSyntaxDef( OP_MINUS, "",    "",     " - "  ))
  << ExprTkSyntaxMap::pair( "times",     ExprTkSyntaxDef( 0,  "",          "",     " * "  ))
  << ExprTkSyntaxMap::pair( "divide",    ExprTkSyntaxDef( 2,  "",          "",     " / "  ))
  << ExprTkSyntaxMap::pair( "power",     ExprTkSyntaxDef( 2,  "",          "",     "^"    ))
  << ExprTkSyntaxMap::pair( "root",      ExprTkSyntaxDef( OP_ROOT,"root( ",")",    ", "   ))
  << ExprTkSyntaxMap::pair( "degree",    ExprTkSyntaxDef( 1,  "",          "",     "",    true )) // Do nothing, just pass through.
  << ExprTkSyntaxMap::pair( "logbase",   ExprTkSyntaxDef( 1,  "",          "",     "",    true )) // Do nothing, just pass through.

  << ExprTkSyntaxMap::pair( "min",       ExprTkSyntaxDef( 0,  "min( ",     ")",    ", "   ))
  << ExprTkSyntaxMap::pair( "max",       ExprTkSyntaxDef( 0,  "max( ",     ")",    ", "   ))

  << ExprTkSyntaxMap::pair( "rem",       ExprTkSyntaxDef( 2,  "frac( ",    ")",    " / "  )) // ExprTk does not like the word "rem"??
  << ExprTkSyntaxMap::pair( "quotient",  ExprTkSyntaxDef( 2,  "quot( ",    ")",    ", "   )) // ExprTk does not like the word "quotient"??

  << ExprTkSyntaxMap::pair( "abs",       ExprTkSyntaxDef( 1,  "abs( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "floor",     ExprTkSyntaxDef( 1,  "floor( ",   ")",    ""     ))
  << ExprTkSyntaxMap::pair( "ceiling",   ExprTkSyntaxDef( 1,  "ceil( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "nearbyint", ExprTkSyntaxDef( 1,  "round( ",   ")",    ""     ))
  << ExprTkSyntaxMap::pair( "sign",      ExprTkSyntaxDef( 1,  "sgn( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "exp",       ExprTkSyntaxDef( 1,  "exp( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "ln",        ExprTkSyntaxDef( 1,  "log( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "log",       ExprTkSyntaxDef( OP_LOG, "logn( ",")",    ", "   ))
  << ExprTkSyntaxMap::pair( "sin",       ExprTkSyntaxDef( 1,  "sin( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "cos",       ExprTkSyntaxDef( 1,  "cos( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "tan",       ExprTkSyntaxDef( 1,  "tan( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "sec",       ExprTkSyntaxDef( 1,  "sec( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "csc",       ExprTkSyntaxDef( 1,  "csc( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "cot",       ExprTkSyntaxDef( 1,  "cot( ",     ")",    ""     ))
  << ExprTkSyntaxMap::pair( "sinh",      ExprTkSyntaxDef( 1,  "sinh( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "cosh",      ExprTkSyntaxDef( 1,  "cosh( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "tanh",      ExprTkSyntaxDef( 1,  "tanh( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "sech",      ExprTkSyntaxDef( 1,  "sech( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "csch",      ExprTkSyntaxDef( 1,  "csch( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "coth",      ExprTkSyntaxDef( 1,  "coth( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "arcsin",    ExprTkSyntaxDef( 1,  "asin( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "arccos",    ExprTkSyntaxDef( 1,  "acos( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "arctan",    ExprTkSyntaxDef( 1,  "atan( ",    ")",    ""     ))
//  << ExprTkSyntaxMap::pair( "arcsec",    ExprTkSyntaxDef( 1,  "asec( ",    ")",    ""     ))
//  << ExprTkSyntaxMap::pair( "arccsc",    ExprTkSyntaxDef( 1,  "acsc( ",    ")",    ""     ))
//  << ExprTkSyntaxMap::pair( "arccot",    ExprTkSyntaxDef( 1,  "acot( ",    ")",    ""     ))
  << ExprTkSyntaxMap::pair( "arcsinh",   ExprTkSyntaxDef( 1,  "asinh( ",   ")",    ""     ))
  << ExprTkSyntaxMap::pair( "arccosh",   ExprTkSyntaxDef( 1,  "acosh( ",   ")",    ""     ))
  << ExprTkSyntaxMap::pair( "arctanh",   ExprTkSyntaxDef( 1,  "atanh( ",   ")",    ""     ))
//  << ExprTkSyntaxMap::pair( "arcsech",   ExprTkSyntaxDef( 1,  "asech( ",   ")",    ""     ))
//  << ExprTkSyntaxMap::pair( "arccsch",   ExprTkSyntaxDef( 1,  "acsch( ",   ")",    ""     ))
//  << ExprTkSyntaxMap::pair( "arccoth",   ExprTkSyntaxDef( 1,  "acoth( ",   ")",    ""     ))

  // Conditions
  << ExprTkSyntaxMap::pair( "eq",  ExprTkSyntaxDef( 2,  "", "", " == "  ))
  << ExprTkSyntaxMap::pair( "neq", ExprTkSyntaxDef( 2,  "", "", " != "  ))
  << ExprTkSyntaxMap::pair( "gt",  ExprTkSyntaxDef( 2,  "", "", " > "   ))
  << ExprTkSyntaxMap::pair( "geq", ExprTkSyntaxDef( 2,  "", "", " >= "  ))
  << ExprTkSyntaxMap::pair( "lt",  ExprTkSyntaxDef( 2,  "", "", " < "   ))
  << ExprTkSyntaxMap::pair( "leq", ExprTkSyntaxDef( 2,  "", "", " <= "  ))
  << ExprTkSyntaxMap::pair( "and", ExprTkSyntaxDef( 0,  "", "", " & "   ))
  << ExprTkSyntaxMap::pair( "or",  ExprTkSyntaxDef( 0,  "", "", " | "   ))
  << ExprTkSyntaxMap::pair( "xor", ExprTkSyntaxDef( 0,  "", "", " xor " ))
  << ExprTkSyntaxMap::pair( "not", ExprTkSyntaxDef( 1,  "", "", " not " ))

  // Use ExprTk "if" / "else if" / "else" instead.
  << ExprTkSyntaxMap::pair( "piecewise", ExprTkSyntaxDef( 0,        "",    "",     "\nelse ", true ))
  << ExprTkSyntaxMap::pair( "piece",     ExprTkSyntaxDef( OP_PIECE, "if ", ";",    " ",       true ))
  << ExprTkSyntaxMap::pair( "otherwise", ExprTkSyntaxDef( 1,        "",    ";\n",  "",        true ))

  // Constants.
  << ExprTkSyntaxMap::pair( "pi",           ExprTkSyntaxDef( OP_ZERO, " ", "", "3.1415926535897932384626433832795" ))
  << ExprTkSyntaxMap::pair( "exponentiale", ExprTkSyntaxDef( OP_ZERO, " ", "", "2.71828182845905"    ))
  << ExprTkSyntaxMap::pair( "eulergamma",   ExprTkSyntaxDef( OP_ZERO, " ", "", "0.57721566490153286" ))
  << ExprTkSyntaxMap::pair( "infinity",     ExprTkSyntaxDef( OP_ZERO, " ", "", "inf" ))
  << ExprTkSyntaxMap::pair( "notanumber",   ExprTkSyntaxDef( OP_ZERO, " ", "", "nan" ))

  // csymbol definitions (technically these are not defined in MathML)
  << ExprTkSyntaxMap::pair( "sind",      ExprTkSyntaxDef( 1,  "sin( deg2rad( ", "))",  ""   ))
  << ExprTkSyntaxMap::pair( "cosd",      ExprTkSyntaxDef( 1,  "cos( deg2rad( ", "))",  ""   ))
  << ExprTkSyntaxMap::pair( "tand",      ExprTkSyntaxDef( 1,  "tan( deg2rad( ", "))",  ""   ))
  << ExprTkSyntaxMap::pair( "secd",      ExprTkSyntaxDef( 1,  "sec( deg2rad( ", "))",  ""   ))
  << ExprTkSyntaxMap::pair( "cscd",      ExprTkSyntaxDef( 1,  "csc( deg2rad( ", "))",  ""   ))
  << ExprTkSyntaxMap::pair( "cotd",      ExprTkSyntaxDef( 1,  "cot( deg2rad( ", "))",  ""   ))
  << ExprTkSyntaxMap::pair( "arcsind",   ExprTkSyntaxDef( 1,  "rad2deg( asin( ", "))", ""   ))
  << ExprTkSyntaxMap::pair( "arccosd",   ExprTkSyntaxDef( 1,  "rad2deg( acos( ", "))", ""   ))
  << ExprTkSyntaxMap::pair( "arctand",   ExprTkSyntaxDef( 1,  "rad2deg( atan( ", "))", ""   ))
//  << ExprTkSyntaxMap::pair( "arcsecd",   ExprTkSyntaxDef( 1,  "rad2deg( asec( ", "))", ""   ))
//  << ExprTkSyntaxMap::pair( "arccscd",   ExprTkSyntaxDef( 1,  "rad2deg( acsc( ", "))", ""   ))
//  << ExprTkSyntaxMap::pair( "arccotd",   ExprTkSyntaxDef( 1,  "rad2deg( acot( ", "))", ""   ))
  << ExprTkSyntaxMap::pair( "atan2",     ExprTkSyntaxDef( 2,  "atan2( ",         ")",  ", " )) // This has been checked (1st div 2nd)
  << ExprTkSyntaxMap::pair( "atan2d",    ExprTkSyntaxDef( 2,  "rad2deg( atan2( ","))", ", " )) // This has been checked (1st div 2nd)
  << ExprTkSyntaxMap::pair( "fmod",      ExprTkSyntaxDef( 2,  "fmod( ",          ")",  ", " ))
  << ExprTkSyntaxMap::pair( "sign",      ExprTkSyntaxDef( 2,  "copysign( ",      ")",  ", " )) // ExprTk does not like the word "sign"??
  << ExprTkSyntaxMap::pair( "bound",     ExprTkSyntaxDef( 3,  "bound( ",         ")",  ", " ))
  << ExprTkSyntaxMap::pair( "factorial", ExprTkSyntaxDef( 1,  "fact( ",          ")",  ""   ))

  );

  //------------------------------------------------------------------------//

  aString parseMathML_to_ExprTkScript( const DomFunctions::XmlNode& xmlElement, const aString &tag, bool isFirstCall)
  {
    static const aString functionName( "parseMathML_to_ExprTkScript()");

    if ( tag.empty()) {
      return aString();
    }

    ExprTkSyntaxDef syntaxDef = exprTkMathMLMap[ tag];
    int thisOP = syntaxDef.ops_;

    // Special cases.
    switch ( thisOP) {
    case OP_NONE:
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - MathML \"" << tag << "\" is not supported."
      );
      break;

    case OP_APPLY:
      if ( isFirstCall) {
        // Remove brackets on first ever APPLY op.
        syntaxDef.pre_.clear();
        syntaxDef.post_.clear();
      }
      syntaxDef.ops_ = 0;
      break;

    case OP_MINUS:
    case OP_ROOT:
    case OP_LOG:
      syntaxDef.ops_ = 0;
      break;

    case OP_PIECE:
      syntaxDef.ops_ = 2;
      break;

    case OP_CDATA:
      return DomFunctions::getCData( xmlElement).trim();
      break;

    case OP_CSYMBOL:
      return parseMathML_to_ExprTkScript( xmlElement, DomFunctions::getCData( xmlElement).trim());
      break;

    case OP_ZERO:
      return syntaxDef.pre_ + syntaxDef.sep_ + syntaxDef.post_;
      break;

    default:
      break;
    }

    // Get children.
    DomFunctions::XmlNodeList childList;
    if ( syntaxDef.hasChild_) {
      childList = DomFunctions::getChildren( xmlElement, EMPTY_STRING);
    }
    else {
      childList = DomFunctions::getSiblings( xmlElement, EMPTY_STRING);
    }

    // APPLY op only gets one pass.
    size_t loopSize = childList.size();
    if ( thisOP == OP_APPLY) {
      loopSize = 1;
    }

    // Do we have enough arguments for this op.
    if ( syntaxDef.ops_ == 0) {
      if ( childList.size() == 0) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - MathML \"" << tag << "\" has no associate elements."
        );
      }
    }
    else if ( syntaxDef.ops_ != int( childList.size())) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - MathML \"" << tag << "\" has an incorrect number of associate elements."
      );
    }

    // Collect child ops.
    aStringList syntaxList;
    for ( size_t i = 0; i < loopSize; ++i) {
      syntaxList << parseMathML_to_ExprTkScript( childList[ i], DomFunctions::getChildName( childList[ i]).trim());
    }

    // Special cases for child ops.
    switch ( thisOP) {
    case OP_PIECE:
      reverse( syntaxList.begin(), syntaxList.end());
      break;

    case OP_MINUS:
      // Check for unary minus op.
      if ( loopSize == 1) {
        syntaxList[ 0] = "-" + syntaxList[ 0];
      }
      break;

    case OP_ROOT:
      reverse( syntaxList.begin(), syntaxList.end());
      if ( loopSize == 1) {
        // Default is root 2.
        syntaxList << "2";
        loopSize = 2;
      }
      break;

    case OP_LOG:
      reverse( syntaxList.begin(), syntaxList.end());
      if ( loopSize == 1) {
        // Default is log10.
        syntaxDef.pre_ = "log10( ";
      }
      break;

    default:
      break;
    }

    // Build ExprTk script elements and return.
    aString syntax = syntaxDef.pre_;
    for ( size_t i = 0; i < loopSize; ++i) {
      if ( i) {
        syntax += syntaxDef.sep_;
      }
      syntax += syntaxList[ i];
    }
    syntax += syntaxDef.post_;

    return syntax;
  }

  //------------------------------------------------------------------------//

  aString mathML_to_ExprTkScript( const DomFunctions::XmlNode& xmlElement, const aString &varID)
  {
    static const aString functionName( "mathML_to_ExprTkScript()");

    aString script;

    try {
      DomFunctions::XmlNodeList childList = DomFunctions::getChildren( xmlElement, EMPTY_STRING, varID);
      script = parseMathML_to_ExprTkScript( childList.front(),
        DomFunctions::getChildName( childList.front()).trim(), true);
      // Just some prettier formatting.
      if ( script.size() > 50) {
        script = "\n" + script;
      }
      if ( script.contains( "\n") && !script.endsWith( "\n")) {
        script += "\n";
      }
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - for ID \"" << varID << "\"\n - "
        << excep.what()
      );
    }

    return script;
  }

  //------------------------------------------------------------------------//

  bool isValidExprTkSymbol( const aString& symbol)
  {
     if ( symbol.empty()) {
       return false;
     }
     else if ( !isalpha(symbol[0])) {
       return false;
     }
     else if ( symbol.size() > 1) {
       for ( size_t i = 1; i < symbol.size(); ++i) {
         if ( !isalnum( symbol[i]) && '_' != symbol[i]) {
           if (( '.' == symbol[i]) && ( i < (symbol.size() - 1))) {
             continue;
           }
           else {
             return false;
           }
         }
       }
     }

     return true;
  }

  //------------------------------------------------------------------------//

  void VariableDef::testForMathML_to_ExprTkScript()
  {
    static const aString functionName( "VariableDef::testForMathML_to_ExprTkScript()");

    if ( variableMethod_ == METHOD_MATHML) {

      // We don't support matrix MathML to matrix ExprTk yet.
      // Mainly because gtest matrixMathMLTest switches scripts between scalar, vector and matrix
      // on the fly. We can't change variable states with ExprTk.
      //
      // Note the code below is written to support MathML to ExprTk matrix conversions.
      if ( hasMatrixOps_ || isMatrix_) {
        script_.clear();
        return;
      }

      // Initial script conversion may have failed.
      if ( script_.empty()) {
        warning_message(
          setFunctionName( functionName)
          << verbose
          << "\n - for file \"" << janus_->getXmlFileName()
          << "\n - Could not convert MathML script for \"" << varID_ << "\" to ExprTk script... using MathML."
        );
      }

      // Evaluate MathML
      // Check all input variables are valid ExprTk symbol names and are updated.
      aStringList badSymbolList;
      if ( !isValidExprTkSymbol( varID_)) badSymbolList << varID_;
      for ( vector<size_t>::const_iterator indRefIdx = independentVarRef_.begin();
        indRefIdx != independentVarRef_.end(); ++indRefIdx) {
        VariableDef& indepVariableDef = janus_->variableDef_[ *indRefIdx];
        if ( !isValidExprTkSymbol( indepVariableDef.getVarID())) badSymbolList << indepVariableDef.getVarID();
        if ( !indepVariableDef.isCurrent_) {
          indepVariableDef.solveValue();
        }
      }
      if ( badSymbolList.size()) {
        warning_message(
          setFunctionName( functionName)
          << verbose
          << "\n - for file \"" << janus_->getXmlFileName()
          << "\n - MathML script \"" << varID_ << "\" contains invalid variable names for use with ExprTk script... using MathML.\n"
          << badSymbolList
        );
        script_.clear();
        return;
      }

      solveMath();

      dstomath::DMatrix mathMLValue;
      if ( isMatrix_) {
        mathMLValue = matrix_;
      }
      else {
        mathMLValue.resize( 1, 1);
        mathMLValue = value_;
      }
      if ( dstomath::isnan( mathMLValue(0,0))) mathMLValue = -1;
      if ( dstomath::isinf( mathMLValue(0,0))) mathMLValue = -2;

      // Initialise & evaluate ExprTk script equivalent.
      std::vector<size_t> independentVarRefSave = independentVarRef_;
      initialiseExprTkScript();
      // Check all input variables are updated.
      for ( vector<size_t>::const_iterator indRefIdx = independentVarRef_.begin();
        indRefIdx != independentVarRef_.end(); ++indRefIdx) {
        VariableDef& indepVariableDef = janus_->variableDef_[ *indRefIdx];
        if ( !indepVariableDef.isCurrent_) {
          indepVariableDef.solveValue();
        }
      }

      // Evaluate ExprTk script.
      solveExprTkScript();

      dstomath::DMatrix scriptValue;
      if ( isMatrix_) {
        scriptValue = matrix_;
      }
      else {
        scriptValue.resize( 1, 1);
        scriptValue = value_;
      }
      if ( dstomath::isnan( scriptValue(0,0))) scriptValue = -1;
      if ( dstomath::isinf( scriptValue(0,0))) scriptValue = -2;

      bool isSameValue = ( scriptValue.size() == mathMLValue.size());
      for ( size_t i = 0; isSameValue && i < mathMLValue.size(); ++i) {
        if ( dstomath::isnan( scriptValue.matrixData()[ i]) ||
             dstomath::isnan( mathMLValue.matrixData()[ i])) {
          isSameValue = ( dstomath::isnan( scriptValue.matrixData()[ i]) &&
                          dstomath::isnan( mathMLValue.matrixData()[ i]));
        }
        else if ( dstomath::isNotZero( scriptValue.matrixData()[ i] - mathMLValue.matrixData()[ i])) {
          isSameValue = false;
        }
      }

      if ( !isSameValue) {
        // Reset MathML indep variables.
        independentVarRef_ = independentVarRefSave;
        warning_message(
          setFunctionName( functionName)
          << verbose
          << "\n - for file \"" << janus_->getXmlFileName()
          << "\n - MathML to ExprScript for \"" << varID_ << "\" failed value test... using MathML."
          << "\n - varID, MathML value, ExprTk value: " << varID_ << ", " << mathMLValue << ", " << scriptValue
          << "\n - script = \n"
          << script_
        );
      }
      else {
        scriptType_     = EXPRTK_SCRIPT;
        variableMethod_ = METHOD_SCRIPT;
      }
      if ( hasOutputScaleFactor_) {
        if ( isMatrix_) {
          matrix_ *= outputScaleFactor_;
        }
        else {
          value_ *= outputScaleFactor_;
        }
      }
      if ( hasPerturbation_) {
        applyPerturbation();
      }
    }
  }

  //------------------------------------------------------------------------//

} // End namespace janus
