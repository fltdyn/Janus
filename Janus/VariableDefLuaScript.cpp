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
 * VariableDefLuaScript.cpp
 *
 *  Created on: Dec 7, 2016
 *      Author: hills
 */

// Local Includes
#include "Janus.h"
#include "VariableDef.h"

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aOptional.h>

// Lua/Kaguya Script Includes
#ifdef HAVE_KAGUYA
  #ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
  #endif
  #include <kaguya/kaguya.hpp>
  #ifdef __GNUC__
    #pragma GCC diagnostic warning "-Wunused-parameter"
    #pragma GCC diagnostic warning "-Wunused-but-set-parameter"
  #endif
  #include <Ute/aKaguyaUte.h>
#endif

using namespace std;
using namespace dstoute;
using namespace dstomath;

namespace janus {

#ifdef HAVE_KAGUYA

  const aStringList luaReservedWordList = ( aStringList()
    << "and"
    << "break"
    << "do"
    << "else"
    << "elseif"
    << "end"
    << "false"
    << "for"
    << "function"
    << "if"
    << "in"
    << "local"
    << "math"
    << "nil"
    << "not"
    << "or"
    << "repeat"
    << "return"
    << "then"
    << "true"
    << "until"
    << "while"
  );

  //------------------------------------------------------------------------//

  void Janus::deleteLuaState()
  {
    delete static_cast<kaguya::State*>(kState_);
    kState_ = 0;
  }

  //------------------------------------------------------------------------//

  void VariableDef::deleteLuaFunction()
  {
    if ( thisVarDef_ == this) {
    }
    kFunc_ = 0;
  }

  //------------------------------------------------------------------------//

  void VariableDef::initialiseLuaScript()
  {
    // Script separators.
    aString     delimiters = " \t\n\r=+-*/^%<>(){}[],;!";
    aStringList localVarList;
    aStringList indepVarList;
    aString scriptList = script_;
    while ( scriptList.hasStringTokens()) {
      // Strip off leading . or : as in "mach:rows()". Just need to test name "mach".
      aString token = scriptList.getStringToken( delimiters, true).getStringToken( ".:");
      if ( token == "local") {
        localVarList << scriptList.getStringToken( delimiters, true);
      }
      else if ( !luaReservedWordList.contains( token) &&
                !localVarList.contains( token) &&
                !indepVarList.contains( token) &&
                !token.isNumeric()) {
        aOptionalSizeT indepVarIndex = janus_->crossReferenceId( ELEMENT_VARIABLE, token);
        if ( indepVarIndex.isValid()) {
          independentVarRef_.push_back( size_t( indepVarIndex));
          indepVarList << token;
        }
      }
    }
  }

  //------------------------------------------------------------------------//

  void VariableDef::initLuaState() const
  {
    // Create a Lua virtual machine.
    janus_->kState_ = new kaguya::State;

    // Register aMatrix with Lua.
    Kaguya_DMatrix( *static_cast<kaguya::State*>(janus_->kState_));
  }

  //------------------------------------------------------------------------//

  void VariableDef::initLuaFunction() const
  {
    kaguya::LuaFunction *kFunc  = new kaguya::LuaFunction;
    *kFunc = static_cast<kaguya::State*>(janus_->kState_)->loadstring( script_);
    kFunc_ = kFunc;

    // Update dependent matrix references into Lua script.
    for ( size_t i = 0; i < independentVarRef_.size(); ++i) {
      VariableDef& indepVariableDef = janus_->variableDef_[ independentVarRef_[ i]];
      if ( indepVariableDef.isMatrix_) {
        (*static_cast<kaguya::State*>(janus_->kState_))[ indepVariableDef.varID_] = &indepVariableDef.matrix_;
      }
    }

    thisVarDef_ = this;
  }

  //------------------------------------------------------------------------//

  void VariableDef::solveLuaScript() const
  {
    static const aString functionName( "VariableDef::solveLuaValue()");

    // Initialise the Lua state engine once per Janus instance.
    if ( !janus_->kState_) {
      initLuaState();
    }

    // Pre-load the Lua script once per VariableDef instance.
    if ( !kFunc_ || thisVarDef_ != this) {
      initLuaFunction();
    }

    // Update dependent values into Lua script.
    for ( size_t i = 0; i < independentVarRef_.size(); ++i) {
      VariableDef& indepVariableDef = janus_->variableDef_[ independentVarRef_[ i]];
      if ( !indepVariableDef.isMatrix_) {
        (*static_cast<kaguya::State*>(janus_->kState_))[ indepVariableDef.varID_] = indepVariableDef.value_;
      }
      else {
        (*static_cast<kaguya::State*>(janus_->kState_))[ indepVariableDef.varID_] = indepVariableDef.matrix_;
      }
    }

    // Evaluate Lua script function.
    if ( isMatrix_) {
      matrix_ = (*static_cast<kaguya::LuaFunction*>(kFunc_))().get<DMatrix>();
    }
    else {
      value_ = (*static_cast<kaguya::LuaFunction*>(kFunc_))();
    }
  }

#else

  //------------------------------------------------------------------------//

  void Janus::deleteLuaState() {}
  void VariableDef::deleteLuaFunction() {}
  void VariableDef::initialiseLuaScript() {}
  void VariableDef::initLuaState() const {}
  void VariableDef::initLuaFunction() const {}
  void VariableDef::solveLuaScript() const {}

  //------------------------------------------------------------------------//

#endif
}
