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
 * VariableDefExprTkScript.cpp
 *
 *  Created on: Dec 7, 2016
 *      Author: hills
 */

// C++ Includes
#include <cctype>
#include <sstream>
#include <iomanip>
#include <set>
#include <unordered_set>

// Local Includes
#include "Janus.h"
#include "VariableDef.h"

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aMath.h>
#include <Ute/aOptional.h>

// ExprTk Script Includes
#if defined(WIN32) || defined(WIN64)
  #if defined(__GNUC__)
    // Disabled under MinGW due to long, long, long compile times.
    #define exprtk_disable_enhanced_features
//    #define exprtk_disable_string_capabilities
  #endif
#endif
#define exprtk_disable_return_statement
#define exprtk_disable_caseinsensitivity
#include <Ute/exprtk_matrix.hpp>

typedef exprtk::symbol_table<double> exprtk_symboltable;
typedef exprtk::expression<double>   exprtk_script;
typedef exprtk::parser<double>       exprtk_parser;
typedef exprtk::parser_error::type   exprtk_parser_error;
typedef exprtk_parser::dependent_entity_collector::symbol_t exprtk_symbol;

#define EFUNC_ static_cast<exprtk_script*>( eFunc_)

exprtk::polynomial<double, 1>        exprtk_poly01;
exprtk::polynomial<double, 2>        exprtk_poly02;
exprtk::polynomial<double, 3>        exprtk_poly03;
exprtk::polynomial<double, 4>        exprtk_poly04;
exprtk::polynomial<double, 5>        exprtk_poly05;
exprtk::polynomial<double, 6>        exprtk_poly06;
exprtk::polynomial<double, 7>        exprtk_poly07;
exprtk::polynomial<double, 8>        exprtk_poly08;
exprtk::polynomial<double, 9>        exprtk_poly09;
exprtk::polynomial<double,10>        exprtk_poly10;
exprtk::polynomial<double,11>        exprtk_poly11;
exprtk::polynomial<double,12>        exprtk_poly12;
exprtk::rtl::io::package<double>     exprtk_io_package;
exprtk::rtl::vecops::package<double> exprtk_vecops_package;
exprtk::rtl::matops::package<double> exprtk_matops_package;

using namespace std;
using namespace dstoute;

namespace janus {

  //------------------------------------------------------------------------//

  namespace local_exprtk_extras {

    double exit( double err)
    {
      ::exit( int( err));
    }

    double quotient( double a, double b)
    {
      double quot;
      ::modf( a / b, &quot);
      return quot;
    }

    double copysign( double a, double b)
    {
      return dstomath::copysign( a, b);
    }

    double bound( double val, double minVal, double maxVal)
    {
      return dstomath::bound( val, minVal, maxVal);
    }

    double fact( double a)
    {
      return dstomath::fact( a);
    }

    double linterp( double x, double xl, double xh, double yl, double yh)
    {
      if ( xl < xh) {
        if ( x <= xl) return yl;
        if ( x >= xh) return yh;
      }
      else if ( xl > xh) {
        if ( x >= xl) return yl;
        if ( x <= xh) return yh;
      }
      return ( yh - yl) / ( xh - xl) * ( x - xl) + yl;
    }

    double linterpe( double x, double xl, double xh, double yl, double yh)
    {
      return ( yh - yl) / ( xh - xl) * ( x - xl) + yl;
    }

#define _setVarDef_SetUp                                                        \
  VariableDef* callingVarDef = static_cast<VariableDef*>( a);                   \
  if ( callingVarDef->getInEvaluation())                                        \
    return 0.0;                                                                 \
  Janus* janus = callingVarDef->getJanusInstance();                             \
  callingVarDef->setInEvaluation( true);

#define _setVarDef_SetValue( i, j)                                              \
  const size_t idx##i = static_cast<size_t>( v##i);                             \
  janus->getVariableDef( idx##i).setValue( v##j, true);

#define _setVarDef_UpdateDependencies                                           \
  const std::vector<size_t>& indVarIdx = callingVarDef->getIndependentVarRef(); \
  for ( const auto& idx : indVarIdx) janus->getVariableDef( idx).getValue();

#define _setVarDef_CleanUp                                                      \
  callingVarDef->setInEvaluation( false);                                       \
  return 0.0;

    double setVarDefInternal02( void* a, double v00, double v01)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal04( void* a, double v00, double v01, 
                                         double v02, double v03)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal06( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal08( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05,
                                         double v06, double v07)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05) _setVarDef_SetValue( 06, 07)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal10( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05,
                                         double v06, double v07,
                                         double v08, double v09)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05) _setVarDef_SetValue( 06, 07)
      _setVarDef_SetValue( 08, 09)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal12( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05,
                                         double v06, double v07,
                                         double v08, double v09,
                                         double v10, double v11)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05) _setVarDef_SetValue( 06, 07)
      _setVarDef_SetValue( 08, 09) _setVarDef_SetValue( 10, 11)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal14( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05,
                                         double v06, double v07,
                                         double v08, double v09,
                                         double v10, double v11,
                                         double v12, double v13)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05) _setVarDef_SetValue( 06, 07)
      _setVarDef_SetValue( 08, 09) _setVarDef_SetValue( 10, 11)
      _setVarDef_SetValue( 12, 13)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal16( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05,
                                         double v06, double v07,
                                         double v08, double v09,
                                         double v10, double v11,
                                         double v12, double v13,
                                         double v14, double v15)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05) _setVarDef_SetValue( 06, 07)
      _setVarDef_SetValue( 08, 09) _setVarDef_SetValue( 10, 11)
      _setVarDef_SetValue( 12, 13) _setVarDef_SetValue( 14, 15)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal18( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05,
                                         double v06, double v07,
                                         double v08, double v09,
                                         double v10, double v11,
                                         double v12, double v13,
                                         double v14, double v15,
                                         double v16, double v17)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05) _setVarDef_SetValue( 06, 07)
      _setVarDef_SetValue( 08, 09) _setVarDef_SetValue( 10, 11)
      _setVarDef_SetValue( 12, 13) _setVarDef_SetValue( 14, 15)
      _setVarDef_SetValue( 16, 17)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

    double setVarDefInternal20( void* a, double v00, double v01, 
                                         double v02, double v03,
                                         double v04, double v05,
                                         double v06, double v07,
                                         double v08, double v09,
                                         double v10, double v11,
                                         double v12, double v13,
                                         double v14, double v15,
                                         double v16, double v17,
                                         double v18, double v19)
    {
      _setVarDef_SetUp
      _setVarDef_SetValue( 00, 01) _setVarDef_SetValue( 02, 03)
      _setVarDef_SetValue( 04, 05) _setVarDef_SetValue( 06, 07)
      _setVarDef_SetValue( 08, 09) _setVarDef_SetValue( 10, 11)
      _setVarDef_SetValue( 12, 13) _setVarDef_SetValue( 14, 15)
      _setVarDef_SetValue( 16, 17) _setVarDef_SetValue( 18, 19)
      _setVarDef_UpdateDependencies
      _setVarDef_CleanUp
    }

#undef _setVarDef_SetUp
#undef _setVarDef_SetValue
#undef _setVarDef_CleanUp

    double getJanusValueQuietly( void* a, double idx)
    {
      VariableDef* callingVarDef = static_cast<VariableDef*>( a);
      Janus* janus = callingVarDef->getJanusInstance();
      return janus->getVariableDef( static_cast<size_t>( idx)).getValue();
    }

    void initExprTkSymbolTable( exprtk_symboltable &eSymbolTable)
    {
      eSymbolTable.add_constants();
      eSymbolTable.add_function( "exit",     local_exprtk_extras::exit);
      eSymbolTable.add_constant( "nan",      dstomath::nan());
      eSymbolTable.add_function( "quot",     local_exprtk_extras::quotient);
      eSymbolTable.add_function( "copysign", local_exprtk_extras::copysign);
      eSymbolTable.add_function( "bound",    local_exprtk_extras::bound);
      eSymbolTable.add_function( "fact",     local_exprtk_extras::fact);
      eSymbolTable.add_function( "linterp",  local_exprtk_extras::linterp);
      eSymbolTable.add_function( "linterpe", local_exprtk_extras::linterpe);
      eSymbolTable.add_function( "fmod",     ::fmod);
      eSymbolTable.add_function( "poly01",   exprtk_poly01);
      eSymbolTable.add_function( "poly02",   exprtk_poly02);
      eSymbolTable.add_function( "poly03",   exprtk_poly03);
      eSymbolTable.add_function( "poly04",   exprtk_poly04);
      eSymbolTable.add_function( "poly05",   exprtk_poly05);
      eSymbolTable.add_function( "poly06",   exprtk_poly06);
      eSymbolTable.add_function( "poly07",   exprtk_poly07);
      eSymbolTable.add_function( "poly08",   exprtk_poly08);
      eSymbolTable.add_function( "poly09",   exprtk_poly09);
      eSymbolTable.add_function( "poly10",   exprtk_poly10);
      eSymbolTable.add_function( "poly11",   exprtk_poly11);
      eSymbolTable.add_function( "poly12",   exprtk_poly12);
      eSymbolTable.add_package(              exprtk_io_package);
      eSymbolTable.add_package(              exprtk_vecops_package);
      #ifndef exprtk_disable_matrix_capabilities
      eSymbolTable.add_package(              exprtk_matops_package);
      #endif
    }

#define _initExprTkSymbolTable( name)                                                                               \
  local_exprtk_extras::initExprTkSymbolTable( name);                                                                \
  name.add_function( "setVarDefInternal02",  local_exprtk_extras::setVarDefInternal02,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal04",  local_exprtk_extras::setVarDefInternal04,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal06",  local_exprtk_extras::setVarDefInternal06,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal08",  local_exprtk_extras::setVarDefInternal08,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal10",  local_exprtk_extras::setVarDefInternal10,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal12",  local_exprtk_extras::setVarDefInternal12,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal14",  local_exprtk_extras::setVarDefInternal14,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal16",  local_exprtk_extras::setVarDefInternal16,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal18",  local_exprtk_extras::setVarDefInternal18,  static_cast<void*>( this)); \
  name.add_function( "setVarDefInternal20",  local_exprtk_extras::setVarDefInternal20,  static_cast<void*>( this)); \
  name.add_function( "getJanusValueQuietly", local_exprtk_extras::getJanusValueQuietly, static_cast<void*>( this));

  }

  //------------------------------------------------------------------------//

  void VariableDef::deleteExprTkFunction()
  {
    if ( thisVarDef_ == this) {
      delete EFUNC_;
    }
    eFunc_ = 0;
  }

  //------------------------------------------------------------------------//

  void VariableDef::initialiseExprTkScript()
  {
    static const aString functionName( "VariableDef::initialiseExprTkScript()");

    preExprtkParser( true);

    // Use ExprTk to return a list of variables used by a script.
    // Then we don't need to use our own parser like we do for Lua scripts.
    exprtk_symboltable eSymbolTable;
    exprtk_script      eFunc;
    exprtk_parser      eParser;

    _initExprTkSymbolTable( eSymbolTable);
    eFunc.register_symbol_table( eSymbolTable);

    eParser.enable_unknown_symbol_resolver();
    eParser.dec().collect_variables() = true;
    exprtk::details::disable_type_checking( eParser);

    independentVarRef_.clear();
    if ( !eParser.compile( script_, eFunc)) {
      exprtk_parser_error eError = eParser.get_error( 0);
      size_t  eLine  = aString( script_.substr( 0, eError.token.position)).count( "\n");
      aString eLines = script_;
      ErrorStream eout;
      eout << setFunctionName( functionName)
           << "\n - File \"" << janus_->getXmlFileName() << "\""
           << "\n - varID \"" << varID_ << "\""
           << "\n - ExprTk script error"
           << "\n - " << exprtk::parser_error::to_str( eError.mode)
           << ":" << eLine << ": " << eError.diagnostic
           << "\n\n";
      for ( size_t i = 0; eLines.hasStringTokens(); ++i) {
        eout << (( i == eLine) ? "->" : "  ") << eLines.getStringToken( "\n", false) << "\n";
      }
      for ( vector<size_t>::const_iterator indRefIdx = independentVarRef_.begin();
        indRefIdx != independentVarRef_.end(); ++indRefIdx) {
        VariableDef& indepVariableDef = janus_->variableDef_[ *indRefIdx];
        eout << "var = " << indepVariableDef.varID_ << '\n';
      }
      throw invalid_argument( eout.throwStr());
    }

    aString thisVar = varID_;
    deque<exprtk_symbol> eVariables;
    eParser.dec().symbols( eVariables);

    VariableDefList &variableDefs = janus_->getVariableDef();
    for ( size_t i = 0; i < eVariables.size(); ++i) {
      aString var = aString( eVariables[i].first);
      if ( var == thisVar) {
        scriptContainsThisVarDef_ = true;
      }
      else {
        for ( size_t j = 0; j < variableDefs.size(); ++j) {
          if ( variableDefs[ j].getVarID() == var) {
            independentVarRef_.push_back( j);
          }
        }
      }
    }
  }

  //------------------------------------------------------------------------//

  void VariableDef::compileExprTkFunction()
  {
    static const aString functionName( "VariableDef::compileExprTkValue()");

    preExprtkParser( false);

    exprtk_symboltable eSymbolTable;
    _initExprTkSymbolTable( eSymbolTable);

    auto putVarIntoSymbolTable = [this, &eSymbolTable]( const VariableDef& indepVariableDef) -> void
    {
      if ( indepVariableDef.isMatrix_) {
        if ( indepVariableDef.matrix_.rows() == 1 || indepVariableDef.matrix_.cols() == 1) {
          eSymbolTable.add_vector( indepVariableDef.varID_, &indepVariableDef.matrix_.matrixData()[0], indepVariableDef.matrix_.size());
        }
        else {
          #ifndef exprtk_disable_matrix_capabilities
          eSymbolTable.add_matrix( indepVariableDef.varID_, &indepVariableDef.matrix_.matrixData()[0], indepVariableDef.matrix_.rows(), indepVariableDef.matrix_.cols());
          #else
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - varID \"" << varID_ << "\" - indepVar \"" << indepVariableDef.varID_ << "\" is matrix."
            << "\n - ExprTk scripts matrix support has been disabled. Try recompiling the library."
          );
          #endif
        }
      }
      else {
        eSymbolTable.add_variable( indepVariableDef.varID_, indepVariableDef.value_);
      }
    };

    // Update dependent variable references into ExprTk script.
    for ( size_t i = 0; i < independentVarRef_.size(); ++i) {
      putVarIntoSymbolTable( janus_->variableDef_[ independentVarRef_[ i]]);
    }
    if ( scriptContainsThisVarDef_ && mathCalculation_.mathChildren_.size() == 0) {
      putVarIntoSymbolTable( *this);
    }

    eFunc_ = new exprtk_script;
    EFUNC_->register_symbol_table( eSymbolTable);

    exprtk_parser eParser;
    if ( !eParser.compile( script_, *EFUNC_)) {
      exprtk_parser_error eError = eParser.get_error( 0);
      size_t  eLine  = aString( script_.substr( 0, eError.token.position)).count( "\n");
      aString eLines = script_;
      ErrorStream eout;
      eout << setFunctionName( functionName)
           << "\n - File \"" << janus_->getXmlFileName() << "\""
           << "\n - varID \"" << varID_ << "\""
           << "\n - ExprTk script error"
           << "\n - " << exprtk::parser_error::to_str( eError.mode)
           << ":" << eLine << ": " << eError.diagnostic
           << "\n\n";
      for ( size_t i = 0; eLines.hasStringTokens(); ++i) {
        eout << (( i == eLine) ? "->" : "  ") << eLines.getStringToken( "\n", false) << "\n";
      }
      for ( vector<size_t>::const_iterator indRefIdx = independentVarRef_.begin();
        indRefIdx != independentVarRef_.end(); ++indRefIdx) {
        VariableDef& indepVariableDef = janus_->variableDef_[ *indRefIdx];
        eout << "var = " << indepVariableDef.varID_ << '\n';
      }
      throw invalid_argument( eout.throwStr());
    }

    // Check for use of return statements.
    EFUNC_->value();
    if ( EFUNC_->results().count()) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - varID \"" << varID_ << "\""
        << "\n - Janus ExprTk scripts don't support \"return []\" statements. Too slow."
        << "\n\n" << script_
      );
    }

    thisVarDef_ = this;
  }

  //------------------------------------------------------------------------//

  void VariableDef::solveExprTkScript() const
  {
    if ( !eFunc_ || thisVarDef_ != this) {
      const_cast<VariableDef*>( this)->compileExprTkFunction();
    }

    double value = EFUNC_->value();
    if ( !scriptContainsThisVarDef_) {
      value_ = value;
    }
  }

  //------------------------------------------------------------------------//

  void VariableDef::preExprtkParser( const bool firstPass)
  {
    static const aString functionName( "VariableDef::compileExprTkValue()");

    static const aString setVarDef               ( "setVarDef");
    static const aString setVarDefSticky         ( "setVarDefSticky");
    static const aString setVarDefInternal       ( "setVarDefInternal");
    static const aString getJanusValueQuietly    ( "getJanusValueQuietly");
    static const aString tempStoreOfNonStickyVals( "tempStoreOfNonStickyVals");

    /** Treats all opening and closing bracket types as the same, 
     * and does not check for zero bracket count at the end
     * as any bracket mismatches will be detected and thrown by 
     * exprtk itself.
     */
    auto bracketCountingSplit = []( const aString& string) -> aStringList {
      aStringList tokens;
      int bcount = 0;
      size_t lastDelimiter = 0;
      for ( size_t i = 0; i < string.size(); ++i) {
        switch ( string[i]) {
        case ',':
          if ( bcount == 0) {
            tokens.push_back( string.substr( lastDelimiter, i - lastDelimiter));
            lastDelimiter = i + 1;
          }
          break;

        case '(': /* fallthrough */
        case '{': /* fallthrough */
        case '[': ++bcount; break;

        case ')': /* fallthrough */
        case '}': /* fallthrough */
        case ']': --bcount; break;

        default : break;
        }
      }
      tokens.push_back( string.substr( lastDelimiter, string.size() - lastDelimiter));
      return tokens;
    };

    auto isValidEndOfFunction = []( const char c) -> bool {
      return iswspace( c) != 0 || c == '(';
    };

    /** If firstPass:
     * -> Replace any instance of setVarDef( variable0, expression0, ..., variableN, expressionN) 
     * or setVarDefSticky( variable0, expression0, ..., variableN, expressionN)
     * with setVarDefInternal[2*N]( / *variable0* /0, expression0, ..., / *variableN* /0, expressionN).
     * -> Keep track of the variables set using setVarDef (non-sticky).
     * If !firstPass:
     * -> Replace any instance of getJanusValueQuietly( / *variable0* /0) 
     * setVarDefInternal[2*N]( / *variable0* /0, expression0, ..., / *variableN* /0, expressionN) with
     * setVarDefInternal[2*N]( / *variable0* /janus_->getVariableIndex( "variable0"), expression0, ...).
     */
    size_t firstPos = script_.find( setVarDef, 0);
    isCurrentable_ = ( firstPos == string::npos);
    set<pair<int, string>> nonStickyIds;

    while ( firstPos != string::npos) {

      bool isSetVarDefSticky   = false;
      bool isSetVarDefInternal = false;

      // Are we looking at setVarDef, setVarDefSticky or setVarDefInternal ?
      // setVarDefSticky
      if ( firstPos + setVarDefSticky.size() + 1 < script_.size() 
        && script_.substr( firstPos, setVarDefSticky.size()) == setVarDefSticky
        && isValidEndOfFunction( script_[ firstPos + setVarDefSticky.size()]))
      {
        isSetVarDefSticky = true;
      }

      // setVarDefInternal (only handled on the second pass)
      else if ( !firstPass
             && firstPos + setVarDefInternal.size() + 3 < script_.size()
             && script_.substr( firstPos, setVarDefInternal.size()) == setVarDefInternal
             && isdigit( script_[ firstPos + setVarDefInternal.size()])
             && isdigit( script_[ firstPos + setVarDefInternal.size() + 1])
             && isValidEndOfFunction( script_[ firstPos + setVarDefInternal.size() + 2]))
      {
        isSetVarDefInternal = true;
      }

      // !setVarDef (look for the next one if we aren't any of setVarDef etc. - ie. could be a variable called 'setVarDefTmp' etc.)
      else if ( !( firstPos + setVarDef.size() + 1 < script_.size()
                && isValidEndOfFunction( script_[ firstPos + setVarDef.size()])))
      {
        firstPos = script_.find( setVarDef, firstPos + 1);
        continue;
      }

      // Extract setVarDef arguments
      const size_t lastPos = script_.find( ";", firstPos);
      if ( lastPos == string::npos) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - varID \"" << varID_ << "\""
          << "\n - \"" << setVarDef << "\" requires a matching \";\"."
          << "\n\n" << script_
        );
      }

      const aString setVarDefArgs = aString( script_.substr( firstPos, lastPos - firstPos)).trimFirst( "(").trimLast( ")").trim();
      if ( setVarDefArgs.empty()) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - varID \"" << varID_ << "\""
          << "\n - function \"" << setVarDef << "\" cannot be called with no arguments."
          << "\n\n" << script_
        );
      }

      const aStringList tokens = bracketCountingSplit( setVarDefArgs);
      if ( tokens.size() % 2 != 0) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - varID \"" << varID_ << "\""
          << "\n - function \"" << setVarDef << "\" must have an even number of arguments."
          << "\n\n" << script_
        );
      }

      // Process arguments
      ostringstream newString;
      newString << setVarDefInternal << setfill('0') << setw( 2) << tokens.size() << "(";

      // setVarDef or setVarDefSticky
      if ( !isSetVarDefInternal) {

        if ( !firstPass) {
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - varID \"" << varID_ << "\""
            << "\n - Internal error - contact RJP."
            << "\n\n" << script_
          );
        }

        for ( size_t i = 0; i < tokens.size(); i += 2) {
          newString <<  "/*" <<  tokens[i].trim() << "*/0," << tokens[i+1] << ( i + 2 < tokens.size() ? "," : ")");

          if ( !isSetVarDefSticky) {
            nonStickyIds.insert( {0, tokens[i].trim()});
          }
        }
      }

      // setVarDefInternal
      else {

        for ( size_t i = 0; i < tokens.size(); i += 2) {

          const aString        varId  = tokens[i].trimFirst( "/*").trimLast( "*/").trim();
          const aOptionalSizeT varIdx = janus_->getVariableIndex( varId); 
          if ( !varIdx.isValid()) {
            throw_message( invalid_argument,
              setFunctionName( functionName)
              << "\n - varID \"" << varID_ << "\""
              << "\n - \"" << varId << "\" is not defined."
              << "\n\n" << script_
            );
          }

          const VariableDef& varDef = janus_->getVariableDef( varIdx);
          if ( varDef.isInput()) {
            throw_message( invalid_argument,
              setFunctionName( functionName)
              << "\n - varID \"" << varID_ << "\""
              << "\n - input variable \"" << varId << "\" cannot be set."
              << "\n - function \"" << setVarDef << "\" cannot be used to modify input variables."
              << "\n\n" << script_
            );
          }

          newString <<  "/*" << varId << "*/" << varIdx << "," << tokens[i+1] << ( i + 2 < tokens.size() ? "," : ")");
        }
      }

      script_.replace( firstPos, lastPos - firstPos, newString.str());
      firstPos = script_.find( setVarDef, firstPos + newString.str().length());
    }

    /** If !firstPass replace any instances of getJanusValueQuietly( / *[variable name]* /[variable index]) 
     * with getJanusValueQuietly( / *[variable name]* /janus_->getVariableIndex( "[variable name]"))
     */
    if ( !firstPass) {
      firstPos = script_.find( getJanusValueQuietly, 0);
      while ( firstPos != string::npos) {
      
        // Ensure we're looking at getJanusValueQuietly
        if ( !( firstPos + getJanusValueQuietly.size() + 1 < script_.size()
             && isValidEndOfFunction( script_[ firstPos + getJanusValueQuietly.size()])))
        {
          firstPos = script_.find( getJanusValueQuietly, firstPos + 1);
          continue;
        }

        // Extract argument
        const size_t lastPos = script_.find( ";", firstPos);
        if ( lastPos == string::npos) {
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - varID \"" << varID_ << "\""
            << "\n - \"" << getJanusValueQuietly << "\" requires a matching \";\"."
            << "\n\n" << script_
          );
        }

        const aString varId = aString( script_.substr( firstPos, lastPos - firstPos)).trimFirst( "/*").trimLast( "*/").trim();
        if ( varId.empty()) {
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - varID \"" << varID_ << "\""
            << "\n - error in \"" << getJanusValueQuietly << "\"."
            << "\n\n" << script_
          );
        }

        const aOptionalSizeT varIdx = janus_->getVariableIndex( varId); 
        if ( !varIdx.isValid()) {
          throw_message( invalid_argument,
            setFunctionName( functionName)
            << "\n - varID \"" << varID_ << "\" is not defined."
            << "\n\n" << script_
          );
        }

        ostringstream newString;
        newString << getJanusValueQuietly <<"(/*" << varId << "*/" << varIdx << ")";
        script_.replace( firstPos, lastPos - firstPos, newString.str());
        firstPos = script_.find( getJanusValueQuietly, firstPos + newString.str().length());
      }
    }

    /** If firstPass we haven't parsed the script yet to determine if scriptContainsThisVarDef_, 
     * so it needs to be done here as part of undoing any setVarDefs.
     */
    auto scriptContainsThisAssignment = [this]( const aString& token) -> bool {
      const size_t tokenLen  = token.size();
      const size_t scriptLen = script_.size();
      size_t idx = script_.find( token, 0);
      while ( idx != string::npos) {
        
        if ( idx == 0 || iswspace( script_[idx-1]) || script_[idx-1] == ';') {
          idx += tokenLen;
          while ( idx < scriptLen - 1 && iswspace( script_[idx])) ++idx;
          if ( idx == scriptLen - 1) return false;

          if ( script_[idx] == ':' && script_[idx+1] == '=') {
            return true;
          }
        }

        idx = script_.find( token, idx);
      }
      return false;
    };

    /** To reset any set variable at the end of a script:
     * The follow script:
     *
     * [code]
     * setVarDef( varA, expr, varB, expr, ...);
     * [code]
     *
     * Is modified to become:
     *
     * var tempStoreOfNonStickyVals[counter]     := getJanusValueQuietly( / *varA* /janus_->getVariableIndex( "varA"));
     * var tempStoreOfNonStickyVals[counter + 1] := getJanusValueQuietly( / *varB* /janus_->getVariableIndex( "varB"));
     * [original code (minus last line of script if !scriptContainsThisVarDef_)]
     * scriptContainsThisVarDef_ ? "" : thisVarDef := former last line of script;
     * setVarDefInternal04( / *varA* /janus_->getVariableIndex( "varA"), setVarDefInitial[counter], ...);
     *
     * scriptContainsThisVarDef_ will be true if the script is modified.
     */
    if ( nonStickyIds.size() > 0) {
      script_ = script_.trimRight();
      if ( script_.back() != ';') {
        script_ += ";";
      }

      ostringstream newString;
      if ( !scriptContainsThisAssignment( varID_)) {
        scriptContainsThisVarDef_ = true;
        const size_t secondLastSC = script_.rfind( ";", script_.size() - 2);
        newString << varID_ << ":=";
        script_.insert( secondLastSC + 1, newString.str());
        newString.str( "");
      }

      size_t tempStoreCounter = 0;
      for ( auto& p : nonStickyIds) {
        newString << "var " << tempStoreOfNonStickyVals << tempStoreCounter++ << ":=" << getJanusValueQuietly << "(/*" << p.second << "*/" << p.first << ");\n";
      }
      script_.insert( 0, newString.str());

      size_t toInsert  = 0; 
      size_t inserted  = 0;
      tempStoreCounter = 0;
      for ( auto& p : nonStickyIds) {
        if ( inserted == toInsert) {
          if ( toInsert != 0) {
            newString << ");";
            script_ += newString.str();
          }
          toInsert = min<size_t>( nonStickyIds.size() - tempStoreCounter, 10);
          inserted = 0;
          newString.str( "");
          if ( toInsert > 0) {
            newString << setVarDefInternal << setfill('0') << setw( 2) << 2 * toInsert << "(";
          }
        }
        ++inserted;
        newString << "/*" << p.second << "*/"<< p.first << "," << tempStoreOfNonStickyVals << tempStoreCounter++ << ( inserted < toInsert ? "," : "");
      }
      newString << ");\n";
      script_ += newString.str();
    }
  }

  //------------------------------------------------------------------------//

} // namespace janus
